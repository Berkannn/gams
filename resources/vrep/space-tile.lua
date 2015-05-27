if (simGetScriptExecutionCount()==0) then


  -- Put some initialization code here

  targetObj=simGetObjectHandle('Target')
  simSetObjectParent(targetObj, -1, true)

  selfObj=simGetObjectHandle('SpaceTile')
  MagFields = {
    simGetObjectHandle("MagFieldXP"),
    simGetObjectHandle("MagFieldXN"),
    simGetObjectHandle("MagFieldYP"),
    simGetObjectHandle("MagFieldYN"),
  }

  magnetsOn = false
  magSigName = "MagnetControl" .. tostring(selfObj)
end

simHandleChildScript(sim_handle_all_except_explicit)

m=simGetObjectMatrix(selfObj,-1)
m[4]=0
m[8]=0
m[12]=0

magForce = 0

magDat = simGetIntegerSignal(magSigName)

if magDat ~= nil then
  simClearStringSignal(magSigName)
  magnetsOn = (magDat == 1)
end

if magnetsOn then

  for key, field in ipairs(MagFields) do
    result, distance, detectedPoint, detObj, normal = simReadProximitySensor(field)
    if result == 1 then
      magForce = 0.00015 / (distance * distance)
      forceVec = {
        magForce * detectedPoint[1] / distance,
        magForce * detectedPoint[2] / distance,
        magForce * detectedPoint[3] / distance }
      magnetMat = simGetObjectMatrix(field, selfObj)
      magnetMat[4]=0
      magnetMat[8]=0  
      magnetMat[12]=0
      forceVec = simMultiplyVector(magnetMat,forceVec)
      magnetPos = simGetObjectPosition(field, selfObj)
      simAddForce(selfObj, magnetPos, forceVec)
    end
  end

end

pos = simGetObjectPosition(selfObj, -1)
orient = simGetObjectOrientation(selfObj, -1)
linVel, angVel = simGetObjectVelocity(selfObj)

targetPos = simGetObjectPosition(targetObj, -1)
targetOrient = simGetObjectOrientation(targetObj, -1)

mult=4

baseForce={0, 0, 0}

for i = 1, 3 do
  f = mult * (targetPos[i] - pos[i])
  v = mult * linVel[i]
  f = f - v
  baseForce[i] = f
end

if magForce == 0 then

  mult = .1

  rotQuat = simGetObjectQuaternion(targetObj, selfObj)
  rotTransMat = simBuildMatrixQ(pos, rotQuat)
  rotEuler = simGetEulerAnglesFromMatrix(rotTransMat)

  rotEuler = simMultiplyVector(m, rotEuler)

  torqA = mult * (rotEuler[1]) - mult * angVel[1]
  torqB = mult * (rotEuler[2]) - mult * angVel[2]
  torqC = mult * (rotEuler[3]) - mult * angVel[3]

  --simAddStatusbarMessage("torqA: " .. tostring(torqA))
  --simAddStatusbarMessage("torqB: " .. tostring(torqB))
  --simAddStatusbarMessage("torqC: " .. tostring(torqC))

  baseTorque={torqA , torqB, torqC}
else
  baseTorque={0 , 0, 0}
end

simAddForceAndTorque(selfObj, baseForce, baseTorque)



-- Put your main code here



-- For example:

--

-- position=simGetObjectPosition(handle,-1)

-- position[1]=position[1]+0.001

-- simSetObjectPosition(handle,-1,position)



if (simGetSimulationState()==sim_simulation_advancing_lastbeforestop) then



  -- Put some restoration code here



end
