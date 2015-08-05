/*********************************************************************
 * Copyright (c) 2013-2015 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following acknowledgments and disclaimers.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 
 * 3. The names "Carnegie Mellon University," "SEI" and/or
 * "Software Engineering Institute" shall not be used to endorse or promote
 * products derived from this software without prior written permission. For
 * written permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 * appear in their names without prior written permission of
 * permission@sei.cmu.edu.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 * acknowledgment:
 *
 * This material is based upon work funded and supported by the Department of
 * Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon University
 * for the operation of the Software Engineering Institute, a federally funded
 * research and development center. Any opinions, findings and conclusions or
 * recommendations expressed in this material are those of the author(s) and
 * do not necessarily reflect the views of the United States Department of
 * Defense.
 * 
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE
 * MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND
 * WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 * This material has been approved for public release and unlimited
 * distribution.
 * 
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *********************************************************************/
package com.gams.platforms;

import com.gams.utility.Axes;
import com.gams.utility.Position;

/**
 * Represents quadrotor model in VREP
 **/
public class VrepUavPlatform extends VrepTargetPlatform
{
  private native int jni_getTargetHandle(int cid, int nid);

  /// platform move speed
  protected double moveSpeed;

  /**
   * Constructor
   **/
  public VrepUavPlatform()
  {
    super();
  }

  /**
   * Sets the targetID value
   **/
  protected void getTargetHandle()
  {
    targetID = jni_getTargetHandle(clientID, nodeID);
  }

  /**
   * Analyzes the platform. This should be
   * a non-blocking call.
   * @return   status information (@see PlatformStatusEnum)
   **/
  public int analyze ()
  {
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Returns the accuracy in meters
   * @return the accuracy of the platform
   **/
  public double getAccuracy ()
  {
    return 1.0;
  }

  /**
   * Returns the position accuracy in meters
   * @return position accuracy
   **/
  public double getPositionAccuracy ()
  {
    return getAccuracy();
  }

  /**
   * Returns the current position 
   * @return the current position of the device/agent
   **/
  public Position getPosition ()
  {
    return new Position(self.device.location);
  }

  /**
   * Returns to the home location. This should be
   * a non-blocking call.
   * @return   status information (@see PlatformReturnStatusEnum)
   **/
  public int home ()
  {
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Requests the platform to land. This should be
   * a non-blocking call.
   * @return   status information (@see PlatformReturnStatusEnum)
   **/
  public int land ()
  {
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Initializes a move to the target position. This should be
   * a non-blocking call.
   * @param   target     the new position to move to
   * @param   proximity  the minimum distance between current position
   *                   and target position that terminates the move.
   * @return  status information (@see PlatformReturnStatusEnum)
   **/
  public int move (Position target, double proximity)
  {
    setVrepPosition(targetID, target);
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Initializes a rotate along x, y, z axes. This should be
   * a non-blocking call and implements an extrinsic rotation.
   * @param   axes       parameters for rotation along x, y, z axes
   * @return  status information (@see PlatformReturnStatusEnum)
   **/
  public int rotate (Axes axes)
  {
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Get sensor radius
   * @return minimum radius of all available sensors for this platform
   */
  public double getMinSensorRange ()
  {
    return 1.0;
  }

  /**
   * Gets the movement speed
   * @return movement speed
   **/
  public double getMoveSpeed ()
  {
    return moveSpeed;
  }

  /**
   * Gets the unique id of the platform. This should be an alphanumeric
   * id that can be part of a MADARA variable name. Specifically, this
   * is used in the variable expansion of .platform.{yourid}.*
   * @return the id of the platform (alphanumeric only: no spaces!)
   **/
  public String getId ()
  {
    return "vrep_uav_java";
  }

  /**
   * Gets the name of the platform
   * @return the name of the platform
   **/
  public String getName ()
  {
    return "vrep_uav_java";
  }

  /**
   * Gets results from the platform's sensors. This should be
   * a non-blocking call.
   * @return  the status of the platform, @see PlatformStatusEnum
   **/
  public int sense ()
  {
    Position current = getVrepPosition(nodeID);
    current.toContainer(self.device.location);
    current.free();

    return PlatformStatusEnum.OK.value();
  }

  /**
   * Sets move speed
   * @param speed new speed in meters/second
   **/
  public void setMoveSpeed (double speed)
  {
    moveSpeed = speed;
  }

  /**
   * Takes off. This should be
   * a non-blocking call.
   * @return   status information (@see PlatformReturnStatusEnum)
   **/
  public int takeoff ()
  {
    return PlatformStatusEnum.OK.value();
  }

  /**
   * Stops moving
   **/
  public void stopMove ()
  {
    // dwell at current location
    move (getPosition(), 1.0);
  }
}
