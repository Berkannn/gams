/**
 * Copyright (c) 2013-2014 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 * 
 * 3. The names “Carnegie Mellon University,” "SEI” and/or “Software
 *  Engineering Institute" shall not be used to endorse or promote products
 *  derived from this software without prior written permission. For written
 *  permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *  appear in their names without prior written permission of
 *  permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *  acknowledgment:
 * 
 *    This material is based upon work funded and supported by the Department
 *    of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *    University for the operation of the Software Engineering Institute, a
 *    federally funded research and development center. Any opinions,
 *    findings and conclusions or recommendations expressed in this material
 *    are those of the author(s) and do not necessarily reflect the views of
 *    the United States Department of Defense.
 * 
 *    NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *    INSTITUTE MATERIAL IS FURNISHED ON AN “AS-IS” BASIS. CARNEGIE MELLON
 *    UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *    IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *    FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *    OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *    NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *    TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *    This material has been approved for public release and unlimited
 *    distribution.
 **/
package com.gams.variables;

import com.gams.GamsJNI;
import com.madara.KnowledgeBase;
import com.madara.Variables;
import com.madara.containers.Integer;

public class Platform extends GamsJNI
{	
  private native long jni_Platform();
  private native long jni_Platform(long cptr);
  private static native void jni_freePlatform(long cptr);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_init(long cptr, long type, long kb, java.lang.String name);
  private native java.lang.String jni_toString(long cptr);
  private native long jni_getCommunicationAvailable(long cptr);
  private native long jni_getDeadlocked(long cptr);
  private native long jni_getFailed(long cptr);
  private native long jni_getGpsSpoofed(long cptr);
  private native long jni_getMovementAvailable(long cptr);
  private native long jni_getMoving(long cptr);
  private native long jni_getOk(long cptr);
  private native long jni_getPausedMoving(long cptr);
  private native long jni_getReducedSensing(long cptr);
  private native long jni_getReducedMovement(long cptr);
  private native long jni_getSensorsAvailable(long cptr);
  private native long jni_getWaiting(long cptr);

  public Platform()
  {
    setCPtr(jni_Platform());
  }

  public Platform(Platform input)
  {
    setCPtr(jni_Platform(input.getCPtr()));
  }

  /**
   * Gets the name of the variable
   *
   * @param  name of the variable within the context
   */
  public java.lang.String getName()
  {
    return jni_getName(getCPtr());
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  kb      the knowledge base that contains the name
   * @param  name    the variable name
   */
  public void init(KnowledgeBase kb, java.lang.String name)
  {
    jni_init(getCPtr(), 0, kb.getCPtr (), name);
    
    command = new com.madara.containers.String.fromPointer (
      jni_getCommand (getCPtr ()));
    args = new com.madara.containers.Vector.fromPointer (
      jni_getArgs (getCPtr ()));
  }

  /**
   * Sets the name and knowledge base being referred to
   *
   * @param  vars    the variables facade that contains the name
   * @param  name    the variable name
   */
  public void init(Variables vars, java.lang.String name)
  {
    jni_init(getCPtr(), 1, vars.getCPtr (), name);
    
    command = new com.madara.containers.String.fromPointer (
      jni_getCommand (getCPtr ()));
    args = new com.madara.containers.Vector.fromPointer (
      jni_getArgs (getCPtr ()));
  }

  /**
   * Flag for whether the algorithm is deadlocked or not
   */
  public com.madara.containers.Integer communicationAvailable;

  /**
   * Flag for whether the algorithm is deadlocked or not
   */
  public com.madara.containers.Integer deadlocked;

  /**
   * Flag for whether the algorithm is failed or not
   */
  public com.madara.containers.Integer failed;

  /**
   * Flag for whether the algorithm is failed or not
   */
  public com.madara.containers.Integer gpsSpoofed;

  /**
   * Flag for whether the algorithm is failed or not
   */
  public com.madara.containers.Integer movementAvailable;

  /**
   * Flag for whether the algorithm is failed or not
   */
  public com.madara.containers.Integer moving;

  /**
   * Flag for whether the algorithm is ok or not
   */
  public com.madara.containers.Integer ok;

  /**
   * Flag for whether the algorithm is paused or not
   */
  public com.madara.containers.Integer pausedMoving;

  /**
   * Flag for whether the algorithm is paused or not
   */
  public com.madara.containers.Integer reducedSensing;

  /**
   * Flag for whether the algorithm is paused or not
   */
  public com.madara.containers.Integer reducedMovement;

  /**
   * Flag for whether the algorithm is paused or not
   */
  public com.madara.containers.Integer sensorsAvailable;

  /**
   * Flag for whether the algorithm is in a waiting state or not
   */
  public com.madara.containers.Integer waiting;

  /**
   * Converts the value to a string
   *
   * @return current string value
   */
  public java.lang.String toString()
  {
    return jni_toString(getCPtr());
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    jni_freePlatform(getCPtr());
  }
}

