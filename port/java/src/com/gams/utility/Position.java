/*********************************************************************
 * Usage of this software requires acceptance of the GAMS-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/gams-cmu/wiki/License
 *********************************************************************/
package com.gams.utility;

import com.gams.GamsJNI;

/**
 * Position of a device/agent
 **/
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

  /**
   * Default constructor
   **/
  public Position()
  {
    setCPtr(jni_Position());
  }

  /**
   * Constructor for a provided x,y,z coordinate
   * @param inx the x coordinate
   * @param iny the y coordinate
   * @param inz the z coordinate
   **/
  public Position(double inx, double iny, double inz)                                   
  {
    setCPtr(jni_Position(inx,iny,inz));
  }

  /**
   * Copy constructor
   * @param input the position to copy
   **/
  public Position(Position input)
  {
    setCPtr(jni_Position(input.getCPtr()));
  }

  /**
   * Checks two instances for equality
   * @param other  other position to check against
   * @return true if equal, false otherwise
   **/
  public boolean equals (Position other)
  {
    return getX() == other.getX() &&
           getY() == other.getY() &&
           getZ() == other.getZ();
  }
  
  /**
   * Gets a unique hashcode for this instance
   * @return hashcode for this object instance
   **/
  @Override
  public int hashCode()
  {
    return (int)getCPtr();
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
   * Sets the latitude/x coord
   * @param  input  the new coord
   **/
  public void setX(double input)
  {
    jni_setX(getCPtr(),input);
  }
  
  /**
   * Sets the longitude/y coord
   * @param  input  the new coord
   **/
  public void setY(double input)
  {
    jni_setY(getCPtr(),input);
  }

  /**
   * Sets the altitude/z coord
   * @param  input  the new coord
   **/
  public void setZ(double input)
  {
    jni_setZ(getCPtr(),input);
  }
      
  /**
   * Creates a java object instance from a C/C++ pointer
   *
   * @param cptr C pointer to the object
   * @return a new java instance of the underlying pointer
   */
  public static Position fromPointer(long cptr)
  {
    Position ret = new Position();
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
  public static Position fromPointer(long cptr, boolean shouldManage)
  {
    Position ret = new Position();
    ret.manageMemory=shouldManage;
    ret.setCPtr(cptr);
    return ret;
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

