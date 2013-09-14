--####################################################################
--# Usage of this software requires acceptance of the SMASH-CMU License,
--# which can be found at the following URL:
--#
--# https://code.google.com/p/smash-cmu/wiki/License
--######################################################################

-- This reference point is chosen to get better latitudes. Now it is at CMU.
g_referencePoint = {}
g_referencePoint['latitude'] = 40.44108
g_referencePoint['longitude'] =  -79.947164

-- Real Earth measurements are required to transform between cartesian and lat/long positions.
g_degressInCircumference = 360
g_earthPolesPerimeter = 40008000
g_earthEquatorialPerimter = 40075160    

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Load the people's locations, so we are able to check when we find one.
--/////////////////////////////////////////////////////////////////////////////////////////////
function loadPeoplePositions()
	-- Load all of these in global variables.
	g_numPeople = simGetScriptSimulationParameter(sim_handle_main_script, 'numberOfPeople')
	g_personCoordsX = {}
    g_personCoordsY = {}
    
	local counter = 1
	for i=1, g_numPeople, 1 do
		if(i==1) then
			personHandle = simGetObjectHandle('Bill#')
		else
			personHandle = simGetObjectHandle('Bill#' .. (i-2))
		end

        local billposition = getObjectPositionInDegrees(personHandle, -1)
		g_personCoordsX[i] = billposition[1]
		g_personCoordsY[i] = billposition[2]
		--simAddStatusbarMessage('Person ' .. counter .. ' : ' .. g_personCoords[counter] .. ', ' .. counter+1 .. ' : '..g_personCoords[counter+1])
	end    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the name and position (as a x,y,x table) of a drone with a given suffix.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromSuffix(suffix)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (suffix -1), we have to add the suffix, which starts at 0.
    if(suffix ~= -1) then
        droneObjectName = droneObjectName .. suffix
    end
    
    -- Get the position from the drone object.
    local droneHandle = simGetObjectHandle(droneObjectName)
    local dronePosition = getObjectPositionInDegrees(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Returns the name and position (as a x,y,x table) of a drone with a given id (starting at 0).
--/////////////////////////////////////////////////////////////////////////////////////////////
function getDroneInfoFromId(id)
    local droneObjectName = 'Quadricopter#'

    -- For all drones but the first one (id 0), we have to add the suffix, which starts at 0 (id-1).
    if(id ~= 0) then
        droneObjectName = droneObjectName .. (id-1)
    end
    
    -- Get the position from the drone object.
    local droneHandle = simGetObjectHandle(droneObjectName)
    local dronePosition = getObjectPositionInDegrees(droneHandle, -1)    
    
    return droneObjectName, dronePosition    
end

--/////////////////////////////////////////////////////////////////////////////////////////////
--
--/////////////////////////////////////////////////////////////////////////////////////////////
function asRadians(degrees)
    return degrees * math.pi / 180
end

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- Returns the position of a given object in degrees.
--/////////////////////////////////////////////////////////////////////////////////////////////
function getObjectPositionInDegrees(objectHandle, relativeTo)
    -- Get the cartesian position first.
    local vrepPosition = simGetObjectPosition(objectHandle, relativeTo)
    --simAddStatusbarMessage('Pos original: ' .. vrepPosition[1] .. ',' .. vrepPosition[2] .. ',' .. vrepPosition[3])
    local cartesianPosition = {}
    cartesianPosition['x'] = vrepPosition[1]
    cartesianPosition['y'] = vrepPosition[2]

    -- Get the long and lat now.
    local latAndLong = getLatAndLong(cartesianPosition)
    
    -- Return the new coordinates. Note that the height was already in meters, and will be still in meters.
    local newPosition = {latAndLong['longitude'], latAndLong['latitude'], vrepPosition[3]}
    return newPosition
end

--/////////////////////////////////////////////////////////////////////////////////////////////    
-- Sets the real cartesian position of an object in the simulation given a latitude and longitude.
--/////////////////////////////////////////////////////////////////////////////////////////////
function setObjectPositionFromDegrees(objectHandle, relativeTo, latAndLongPosition)
    -- Turn into a named table.
    local degreePosition = {}
    degreePosition['longitude'] = latAndLongPosition[1]
    degreePosition['latitude'] = latAndLongPosition[2]

    -- Get the cartesian position.
    local cartesianPosition = getXYpos(degreePosition)
    
    -- Return the new coordinates. Note that the height was already in meters.
    local vrepPosition = {cartesianPosition['x'], cartesianPosition['y'], latAndLongPosition[3]}
    --simAddStatusbarMessage('Moving to pos in cart: ' .. vrepPosition[1] .. ',' .. vrepPosition[2] .. ',' .. vrepPosition[3])
    simSetObjectPosition(objectHandle, relativeTo, vrepPosition)
end    

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates X and Y distances in meters.
--/////////////////////////////////////////////////////////////////////////////////////////////    
function getXYpos(point, relativeNullPoint)
    -- Get the default reference point if none was provided.
    relativeNullPoint = relativeNullPoint or g_referencePoint

    --simAddStatusbarMessage("Delta between: " .. point['latitude'] .. "," .. point['longitude'] .. ' and ' .. relativeNullPoint['latitude'] .. "," .. relativeNullPoint['longitude'])
    local deltaLatitude = point['latitude'] - relativeNullPoint['latitude']
    local deltaLongitude = point['longitude'] - relativeNullPoint['longitude']
    --simAddStatusbarMessage("Delta is: " .. deltaLatitude .. "," .. deltaLongitude)
    
    -- Actually do the calculation.
    local latitudePerimeter =  g_earthEquatorialPerimter * math.cos(asRadians(relativeNullPoint['latitude']))
    local resultX = latitudePerimeter * deltaLongitude / g_degressInCircumference
    local resultY = g_earthPolesPerimeter * deltaLatitude / g_degressInCircumference
    
    -- Turn this into a table.
    local result = {}
    result['x'] = resultX
    result['y'] = resultY

    return result
end

--/////////////////////////////////////////////////////////////////////////////////////////////
-- Calculates latitude and longitude distances from meters.
--/////////////////////////////////////////////////////////////////////////////////////////////    
function getLatAndLong(point, relativeNullPoint)
    -- Get the default reference point if none was provided.
    relativeNullPoint = relativeNullPoint or g_referencePoint
    
    -- Get the deltas from reference point.
    local latitudePerimeter =  g_earthEquatorialPerimter * math.cos(asRadians(relativeNullPoint['latitude']))
    local deltaLongitude = point['x'] * g_degressInCircumference / latitudePerimeter
    local deltaLatitude = point['y'] * g_degressInCircumference / g_earthPolesPerimeter
    
    -- Get the lat and long.
    local latitude = deltaLatitude + relativeNullPoint['latitude']
    local longitude = deltaLongitude + relativeNullPoint['longitude']
    
    -- Turn this into a table.
    local result = {}
    result['latitude'] = latitude
    result['longitude'] = longitude

    return result
end 