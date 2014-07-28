
#include "com_gams_variables_Sensor.h"
#include "gams/variables/Sensor.h"

namespace containers = Madara::Knowledge_Engine::Containers;
namespace engine = Madara::Knowledge_Engine;
namespace variables = gams::variables;

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_Sensor
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_gams_variables_Sensor_jni_1Sensor__
  (JNIEnv * env, jobject)
{
  return (jlong) new variables::Sensor ();
}

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_Sensor
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_variables_Sensor_jni_1Sensor__J
  (JNIEnv * env, jobject, jlong cptr)
{
  return (jlong) new variables::Sensor (*(variables::Sensor *)cptr);
}

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_freeSensor
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_gams_variables_Sensor_jni_1freeSensor
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (variables::Sensor *) cptr;
}

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_gams_variables_Sensor_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  variables::Sensor * current = (variables::Sensor *) cptr;
  if (current)
    result = env->NewStringUTF(current->get_name ().c_str ());

  return result;
}

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_init
 * Signature: (JJJLjava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_com_gams_variables_Sensor_jni_1init
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name, jdouble region)
{
  variables::Sensor * current = (variables::Sensor *) cptr;

  if (current)
  {
    double region_copy (region);
    const char * str_name = env->GetStringUTFChars(name, 0);

    if (type == 0)
    {
      engine::Knowledge_Base * kb = (engine::Knowledge_Base *) context;
      current->init_vars (str_name, kb, region_copy);
    }

    env->ReleaseStringUTFChars(name, str_name);
  }
}

/*
 * Class:     com_gams_variables_Sensor
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_gams_variables_Sensor_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  variables::Sensor * current = (variables::Sensor *) cptr;
  if (current)
    result = env->NewStringUTF(current->get_name ().c_str ());

  return result;
}
