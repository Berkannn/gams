/*********************************************************************
 * Usage of this software requires acceptance of the GAMS-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/gams-cmu/wiki/License
 *********************************************************************/
package com.gams.platforms;

import com.gams.utility.Position;

/**
 * Interface for defining a platform to be used by GAMS. Care must be taken
 * to make all methods non-blocking, to prevent locking up the underlying
 * MADARA context.
 */
public class DebuggerPlatform extends BasePlatform
{
  /**
   * Default constructor
   **/
  public DebuggerPlatform ()
  {
  }
   
  /**
   * Analyzes the platform.
   * @return   status information (@see Status)
   **/
  public int analyze ()
  {
    System.out.println("Platform.analyze called");
    return Status.OK.value ();
  }
  
  /**
   * Returns the position accuracy in meters
   * @return position accuracy
   **/
  public double getPositionAccuracy ()
  {
    System.out.println("  Platform.getPositionAccuracy called");
    return 0.0;
  }

  /**
   * Returns the current GPS position 
   **/
  public Position getPosition ()
  {
    Position position = new Position (0.0, 0.0, 0.0);
    System.out.println("  Platform.getPosition called");
    return position;
  }
  
  /**
   * Returns to the home location. This should be
   * a non-blocking call.
   * @return   status information (@see Status)
   **/
  public int home ()
  {
    System.out.println("  Platform.home called");
    return Status.OK.value ();
  }
  
  /**
   * Requests the platform to land. This should be
   * a non-blocking call.
   * @return   status information (@see Status)
   **/
  public int land ()
  {
    System.out.println("  Platform.land called");
    return Status.OK.value ();
  }
  
  /**
   * Initializes a move to the target position. This should be
   * a non-blocking call.
   * @param   target     the new position to move to
   * @param   proximity  the minimum distance between current position
   *                   and target position that terminates the move.
   * @return  status information (@see Status)
   **/
  public int move (Position target, double proximity)
  {
    System.out.println("  Platform.move called");
    return Status.OK.value ();
  }
   
  /**
   * Get sensor radius
   * @return minimum radius of all available sensors for this platform
   */
  public double getMinSensorRange ()
  {
    System.out.println("  Platform.getMinSensorRange called");
    return 0.0;
  }

  /**
   * Gets the movement speed
   * @return movement speed
   **/
  public double getMoveSpeed ()
  {
    System.out.println("  Platform.getMoveSpeed called");
    return 0.0;
  }

  /**
   * Gets the unique id of the platform. This should be an alphanumeric
   * id that can be part of a MADARA variable name. Specifically, this
   * is used in the variable expansion of .platform.{yourid}.*
   * @return the id of the platform (alphanumeric only: no spaces!)
   **/
  public java.lang.String getId ()
  {
    return "java_debugger";
  }

  /**
   * Gets the name of the platform
   * @return the name of the platform
   **/
  public java.lang.String getName ()
  {
    return "Java Debugger";
  }

  /**
   * Gets results from the platform's sensors. This should be
   * a non-blocking call.
   * @return   1 if moving, 2 if arrived, 0 if an error occurred
   **/
  public int sense ()
  {
    System.out.println("Platform.sense called");
    return Status.OK.value ();
  }
  
  /**
   * Sets move speed
   * @param speed new speed in meters/second
   **/
  public void setMoveSpeed (double speed)
  {
    System.out.println("  Platform.setMoveSpeed called with " + speed);
  }
      
  /**
   * Takes off. This should be
   * a non-blocking call.
   * @return   status information (@see Status)
   **/
  public int takeoff ()
  {
    System.out.println("  Platform.takeoff called");
    return Status.OK.value ();
  }
  
  /**
   * Stops moving
   **/
  public void stopMove ()
  {
    System.out.println("  Platform.stopMove called");
  }
}

