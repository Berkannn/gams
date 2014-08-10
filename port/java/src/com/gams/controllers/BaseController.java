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
 * 3. The names �Carnegie Mellon University,� "SEI� and/or �Software
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
 *    INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON
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
package com.gams.controllers;

import com.gams.GamsJNI;
import com.madara.KnowledgeBase;
import com.madara.KnowledgeList;
import com.gams.algorithms.BaseAlgorithm;
import com.gams.platforms.BasePlatform;
import com.gams.platforms.DebuggerPlatform;
import com.gams.algorithms.DebuggerAlgorithm;

public class BaseController extends GamsJNI
{	
  private native long jni_BaseControllerFromKb(long cptr);
  private native long jni_BaseController(long cptr);
  private static native void jni_freeBaseController(long cptr);
  private native java.lang.String jni_toString(long cptr);
  private native long jni_analyze(long cptr);
  private native long jni_execute(long cptr);
  private native long jni_getPlatform(long cptr);
  private native long jni_getAlgorithm(long cptr);
  private native void jni_initAccent(long cptr, java.lang.String name, long[] args);
  private native void jni_initAlgorithm(long cptr, java.lang.String name, long[] args);
  private native void jni_initPlatform(long cptr, java.lang.String name, long[] args);
  private native void jni_initAlgorithm(long cptr, Object algorithm);
  private native void jni_initPlatform(long cptr, Object platform);
  private native void jni_initVars(long cptr, long id, long processes);
  private native void jni_initVarsAlgorithm(long cptr, long algorithm);
  private native void jni_initVarsPlatform(long cptr, long platform);
  private native long jni_monitor(long cptr);
  private native long jni_plan(long cptr);
  private native long jni_run(long cptr, double period, double max);
  private native long jni_systemAnalyze(long cptr);

  public BaseController(KnowledgeBase knowledge)
  {
    setCPtr(jni_BaseControllerFromKb(knowledge.getCPtr ()));
    
    initPlatform(new DebuggerPlatform ());
    initAlgorithm(new DebuggerAlgorithm ());
  }

  public BaseController(BaseController input)
  {
    setCPtr(jni_BaseController(input.getCPtr()));
  }

  /**
   * Analyzes the platform, algorithm and accents
   **/
  public long analyze ()
  {
    return jni_analyze(getCPtr());
  }
  
  /**
   * Executes the algorithm and accents
   **/
  public long execute ()
  {
    return jni_execute(getCPtr());
  }
  
  /**
   * Initialize an accent within the controller
   *
   * @param  name       name of the accent
   * @param  args       arguments to the accent initialization
   */
  public void initAccent(java.lang.String name, KnowledgeList args)
  {
    jni_initAccent(getCPtr(), name, args.toPointerArray());
  }

  /**
   * Initialize an algorithm within the controller
   *
   * @param  name       name of the algorithm
   * @param  args       arguments to the algorithm initialization
   */
  public void initAlgorithm(java.lang.String name, KnowledgeList args)
  {
    jni_initAlgorithm(getCPtr(), name, args.toPointerArray());
  }

  /**
   * Initialize a platform within the controller
   *
   * @param  name       name of the platform
   * @param  args       arguments to the platform initialization
   */
  public void initPlatform(java.lang.String name, KnowledgeList args)
  {
    jni_initPlatform(getCPtr(), name, args.toPointerArray());
  }

  /**
   * Initialize an algorithm within the controller
   *
   * @param  name       name of the algorithm
   * @param  args       arguments to the algorithm initialization
   */
  public void initAlgorithm(BaseAlgorithm algorithm)
  {
    jni_initPlatform(getCPtr(), algorithm);
    algorithm.assume(jni_getAlgorithm(getCPtr()));
    algorithm.init(this);
  }

  /**
   * Initialize a platform within the controller
   *
   * @param  platform  platform to add to the controller
   */
  public void initPlatform(BasePlatform platform)
  {
    jni_initPlatform(getCPtr(), platform);
    platform.assume(jni_getPlatform(getCPtr()));
    platform.init(this);
  }

  /**
   * Initialize the variables within the controller
   *
   * @param  id         the id of this process within the swarm
   * @param  processes  the number of processes participating in the swarm
   */
  public void initVars(long id, long processes)
  {
    jni_initVars(getCPtr(), id, processes);
  }

  /**
   * Initialize the variables for a platform. This function is useful
   * for user-defined platforms and essentially shares the controller's
   * KnowledgeBase, self-identifying variables, etc.
   *
   * @param  platform   the user-defined platform
   */
  public void initVars(BasePlatform platform)
  {
    jni_initVarsPlatform(getCPtr(), platform.getCPtr ());
  }

  /**
   * Initialize the variables for an algorithm. This function is useful
   * for user-defined algorithms and essentially shares the controller's
   * KnowledgeBase, self-identifying variables, etc.
   *
   * @param  algorithm   the user-defined algorithm
   */
  public void initVars(BaseAlgorithm algorithm)
  {
    jni_initVarsAlgorithm(getCPtr(), algorithm.getCPtr ());
  }

  /**
   * Monitors the platform's sensors
   **/
  public long monitor ()
  {
    return jni_monitor(getCPtr());
  }
  
  /**
   * Plans the algorithm and accents
   **/
  public long plan ()
  {
    return jni_plan(getCPtr());
  }
 
  /**
   * Runs the monitor, analyze, plan and execute loop with a
   * specific period and duration
   **/
  public long run (double period, double duration)
  {
    return jni_run(getCPtr(), period, duration);
  }
   
  /**
   * Analyzes the controller and system
   **/
  public long systemAnalyze ()
  {
    return jni_systemAnalyze(getCPtr());
  }
  
  /**
   * Converts the value to a string
   *
   * @return current string value
   */
  public java.lang.String toString()
  {
    return "BaseController";
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    jni_freeBaseController(getCPtr());
  }
}

