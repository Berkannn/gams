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
import com.gams.controllers.BaseController;

/**
 * Abstract class for defining a platform to interact with VREP
 **/
public abstract class VrepPlatform extends BasePlatform
{
  private native int jni_simStart(String host, int port);
  private native int jni_addModelToEnvironment(int cid, String file, boolean clientSide);
  private native long jni_getVrepPosition(int cid, int nid);
  private native void jni_setVrepPosition(int cID, int hid, long pos_cptr);

  /**
   * Client ID to interact with VREP
   **/
  int clientID;

  /**
   * Handle of the model in the environment
   **/
  int nodeID;

  /**
   * Constructor
   **/
  public VrepPlatform ()
  {
  }

  /**
   * Initialize the platform with controller variables. Use this
   * method to synchronize user-defined platforms with the controller.
   * @param  controller   the controller that will be running the platform loop
   * @param  host         ip address of VREP system
   * @param  port         port to connect to VREP on
   * @param  file         model file to load
   * @param  clientSide   true if model file is on client, false if on server
   * @param  pos          initial position of model
   **/
  public void init (BaseController controller, String host, int port, 
    String file, boolean clientSide, Position pos)
  {
    super.init (controller);
    clientID = jni_simStart(host, port);
    addModelToEnvironment(file, clientSide);
    setInitialPosition(pos);
  }

  /**
   * Add model to environment
   * @param file        model file to load
   * @param clientSide  true if file is on client side, false otherwise
   **/
  public void addModelToEnvironment(String file, boolean clientSide)
  {
    nodeID = jni_addModelToEnvironment(clientID, file, clientSide);
  }

  /**
   * Set initial position of the model
   * @param pos   set position of object
   **/
  protected void setInitialPosition(Position pos)
  {
    setVrepPosition(nodeID, pos);
  }

  /**
   * Get location of a handle
   * @param handle    handle to get position of
   * @return Position of the handle in VREP
   **/
  protected Position getVrepPosition(int handle)
  {
    return Position.fromPointer(jni_getVrepPosition(clientID, handle), true);
  }

  /**
   * Set location of a handle
   * @param handle    handle to set position of
   * @param pos       position to set for handle in VREP
   **/
  protected void setVrepPosition(int handle, Position pos)
  {
    jni_setVrepPosition(clientID, handle, pos.getCPtr());
  }
}
