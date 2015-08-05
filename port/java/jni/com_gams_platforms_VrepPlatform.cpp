#include "com_gams_platforms_VrepPlatform.h"

#include "gams/utility/Position.h"

#include "gams/loggers/Global_Logger.h"

extern "C" {
#include "extApi.h"
}

/*
 * Class:     com_gams_platforms_VrepPlatform
 * Method:    jni_simStart
 * Signature: (Ljava/lang/String;I)I
 */
jint JNICALL Java_com_gams_platforms_VrepPlatform_jni_1simStart
  (JNIEnv * env, jobject, jstring host, jint port)
{
  const char * host_str = env->GetStringUTFChars (host, 0);
  jint ret_val(-1);

  if (host_str)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_DETAILED,
      "jni::com.gams.platforms.VrepPlatform::simStart: " \
      "conecting to VREP at %s:%u\n", host_str, port);
    ret_val = simxStart(host_str, port, true, true, 2000, 5);
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "jni::com.gams.platforms.VrepPlatform::simStart: " \
      "failure getting host string\n");
  }

  if (ret_val == -1)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "jni::com.gams.platforms.VrepPlatform::simStart: " \
      "failure connecting to VREP\n");
  }

  // clean up the allocated elements
  env->ReleaseStringUTFChars (host, host_str);

  return ret_val;
}

/*
 * Class:     com_gams_platforms_VrepPlatform
 * Method:    jni_addModelToEnvironment
 * Signature: (ILjava/lang/String;Z)I
 */
jint JNICALL Java_com_gams_platforms_VrepPlatform_jni_1addModelToEnvironment
  (JNIEnv * env, jobject, jint cid, jstring file, jboolean is_client_side)
{
  const char * file_str = env->GetStringUTFChars (file, 0);
  simxUChar client_side = is_client_side ? 1 : 0;

  jint node_id;

  if (file_str)
  {
    if (simxLoadModel (cid, file_str, client_side, &node_id,
      simx_opmode_oneshot_wait) != simx_error_noerror)
    {
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_ERROR,
        "jni::com.gams.platforms.VrepPlatform::addModelToEnvironment: " \
        "failure loading model in VREP\n");
      exit (-1);
    }
  
    if (node_id < 0)
    {
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_ERROR,
        "jni::com.gams.platforms.VrepPlatform::addModelToEnvironment: " \
        "received invalid handle from VREP\n");
      exit (-1);
    }
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "jni::com.gams.platforms.VrepPlatform::addModelToEnvironment: " \
      "failure getting file string\n");
  }

  // clean up the allocated elements
  env->ReleaseStringUTFChars (file, file_str);

  return node_id;
}

/*
 * Class:     com_gams_platforms_VrepPlatform
 * Method:    jni_getVrepPosition
 * Signature: (II)J
 */
jlong JNICALL Java_com_gams_platforms_VrepPlatform_jni_1getVrepPosition
  (JNIEnv *, jobject, jint cid, jint hid)
{
  gams::utility::Position* ret_val = new gams::utility::Position();

  simxFloat curr_arr[3];
  simxGetObjectPosition (cid, hid, sim_handle_parent, curr_arr,
                        simx_opmode_oneshot_wait);

  ret_val->x = curr_arr[0];
  ret_val->y = curr_arr[1];
  ret_val->z = curr_arr[2];

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_DETAILED,
    "jni::com.gams.platforms.VrepPlatform::getVrepPosition: " \
    "return position \"%s\"\n", ret_val->to_string ().c_str ());

  return (jlong)ret_val;
}

/*
 * Class:     com_gams_platforms_VrepPlatform
 * Method:    jni_setVrepPosition
 * Signature: (IIJ)V
 */
void JNICALL Java_com_gams_platforms_VrepPlatform_jni_1setVrepPosition
  (JNIEnv *, jobject, jint cid, jint hid, jlong pos_ptr)
{
  gams::utility::Position* pos = (gams::utility::Position*) pos_ptr;

  if (pos)
  {
    simxFloat vrep_arr[3];
    vrep_arr[0] = pos->x;
    vrep_arr[1] = pos->y;
    vrep_arr[2] = pos->z;
    simxGetObjectPosition (cid, hid, sim_handle_parent, vrep_arr,
                          simx_opmode_oneshot_wait);
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
      "jni::com.gams.platforms.VrepPlatform::setVrepPosition: " \
      "invalid position\n");
  }
}
