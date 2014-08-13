/*********************************************************************
 * Usage of this software requires acceptance of the GAMS-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/gams-cmu/wiki/License
 *********************************************************************/
package com.gams.variables;

import java.util.AbstractMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class SensorMap extends AbstractMap<java.lang.String, Sensor>
{
  private native void jni_freeSensorMap(long[] ptrs, int length);

  private Set<Map.Entry<java.lang.String, Sensor>> mySet;
  private final boolean freeable;

  public SensorMap(java.lang.String[] keys, long[] vals)
  {
    this(keys, vals, true);
  }

  public SensorMap(java.lang.String[] keys, long[] vals, boolean freeable)
  {
    this.freeable = freeable;

    if (keys == null || vals == null || keys.length != vals.length)
      return;

    mySet = new HashSet<Map.Entry<java.lang.String, Sensor>>();

    for (int x = 0; x < keys.length; x++)
    {
      mySet.add(new SensorMapEntry(keys[x], vals[x], freeable));
    }
  }

  /**
   * @see java.util.AbstractMap#entrySet ()
   */
  @Override
  public Set<Map.Entry<java.lang.String, Sensor>> entrySet()
  {
    return mySet;
  }

  /**
   * Deletes the C instantiation. To prevent memory leaks, this <b>must</b> be called
   * before an instance of {@link com.madara.SensorMap SensorMap} gets garbage collected
   */
  public void free()
  {
    if (!freeable)
      return;

    long[] ptrs = new long[mySet == null ? 0 : mySet.size()];
    int pos = 0;
    
    for (Map.Entry<java.lang.String, Sensor> entry : mySet)
    {
      ptrs[pos++] = entry.getValue().getCPtr();
    }

    jni_freeSensorMap(ptrs, ptrs.length);

    mySet = null;

  }

  private static class SensorMapEntry implements Map.Entry<java.lang.String, Sensor>
  {
    private java.lang.String key;
    private Sensor record;

    private SensorMapEntry(java.lang.String key, long val, boolean isNew)
    {
      this.key = key;
      record = Sensor.fromPointer(val, isNew);
    }

    /**
     * @see java.util.Map.Entry#getKey ()
     */
    public java.lang.String getKey()
    {
      return key;
    }

    /**
     * @see java.util.Map.Entry#getValue ()
     */
    public Sensor getValue()
    {
      return record;
    }

    /**
     * @see java.util.Map.Entry#setValue (java.lang.Object)
     */
    public Sensor setValue(Sensor value)
    {
      throw new UnsupportedOperationException("This map does not allow modification");
    }
  }
}

