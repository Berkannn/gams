/*********************************************************************
 * Usage of this software requires acceptance of the GAMS-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/gams-cmu/wiki/License
 *********************************************************************/
package com.gams.utility;

import com.gams.GamsJNI;

public class Position extends GamsJNI
{
  private native long jni_Position();
  private native long jni_Position(long cptr);
  private native long jni_Position(double inx, double iny, double inz);
  private static native void jni_freePosition(long cptr);
  private native java.lang.String jni_toString(long cptr);
  private native double jni_getX(long cptr);
  private native double jni_getY(long cptr);
  private native double jni_getZ(long cptr);
  private native void jni_setX(long cptr, double input);
  private native void jni_setY(long cptr, double input);
  private native void jni_setZ(long cptr, double input);

  private boolean manageMemory = true;

  public Position()
  {
    setCPtr(jni_Position());
  }

  public Position(double inx, double iny, double inz)                                   
  {
    setCPtr(jni_Position(inx,iny,inz));
  }

  public Position(Position input)
  {
    setCPtr(jni_Position(input.getCPtr()));
  }

  /**
   * Converts the position into a string
   * @return position as a string
   **/
  public java.lang.String toString()
  {
    String result = "";
    result += getX();
    result += ",";
    result += getY();
    result += ",";
    result += getZ();
    
    return result;
  }
  
  /**
   * Returns the latitude
   * @return latitude
   **/
  public double getX()
  {
    return jni_getX(getCPtr());
  }
  
  /**
   * Returns the longitude
   * @return longitude
   **/
  public double getY()
  {
    return jni_getY(getCPtr());
  }

  /**
   * Returns the altitude
   * @return altitude
   **/
  public double getZ()
  {
    return jni_getZ(getCPtr());
  }
  
  /**
   * Sets the latitude
   * @return latitude
   **/
  public void setX(double input)
  {
    jni_setX(getCPtr(),input);
  }
  
  /**
   * Sets the longitude
   * @return longitude
   **/
  public void setY(double input)
  {
    jni_setY(getCPtr(),input);
  }

  /**
   * Sets the altitude
   * @return altitude
   **/
  public void setZ(double input)
  {
    jni_setZ(getCPtr(),input);
  }
  
  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be
   * called before an instance of WaitSettings gets garbage collected
   */
  public void free()
  {
    if(manageMemory)
    {
      jni_freePosition(getCPtr());
    }
  }                
}

