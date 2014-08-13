/*********************************************************************
 * Usage of this software requires acceptance of the GAMS-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/gams-cmu/wiki/License
 *********************************************************************/
package com.gams.variables;

import com.gams.GamsJNI;
import com.madara.KnowledgeBase;
import com.madara.containers.String;
import com.madara.containers.Vector;

public class Sensor extends GamsJNI
{	
  private native long jni_Sensor();
  private native long jni_Sensor(long cptr);
  private static native void jni_freeSensor(long cptr);
  private native java.lang.String jni_getName(long cptr);
  private native void jni_init(long cptr, long type, long kb, java.lang.String name, double region);
  private native java.lang.String jni_toString(long cptr);

  private boolean manageMemory = true;

  public Sensor()
  {
    setCPtr(jni_Sensor());
  }

  public Sensor(Sensor input)
  {
    setCPtr(jni_Sensor(input.getCPtr()));
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Sensor fromPointer(long cptr)
  {
    Sensor ret = new Sensor();
    ret.manageMemory = true;
    ret.setCPtr(cptr);
    return ret;
  }

  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @param shouldManage  if true, manage the pointer
   * @return a new java instance of the underlying pointer
   */
  public static Sensor fromPointer(long cptr, boolean shouldManage)
  {
    Sensor ret = new Sensor();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
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
  public void init(KnowledgeBase kb, java.lang.String name, double region)
  {
    jni_init(getCPtr(), 0, kb.getCPtr (), name, region);
  }

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
    if (manageMemory)
    {
      jni_freeSensor(getCPtr());
    }
  }
}

