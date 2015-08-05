#include "com_gams_platforms_VrepUavPlatform.h"

#include "gams/loggers/Global_Logger.h"

extern "C" {
#include "extApi.h"
}

/*
 * Class:     com_gams_platforms_VrepUavPlatform
 * Method:    jni_getTargetHandle
 * Signature: (II)I
 */
jint JNICALL Java_com_gams_platforms_VrepUavPlatform_jni_1getTargetHandle
  (JNIEnv *, jobject, jint cid, jint nid)
{
  //find the dummy base sub-object
  simxInt handles_count = 0,*handles = NULL;
  simxInt parents_count = 0,*parents = NULL;
  simxGetObjectGroupData (cid, sim_object_dummy_type, 2, &handles_count,
    &handles, &parents_count, &parents, NULL, NULL, NULL, NULL,
    simx_opmode_oneshot_wait);

  // find node base
  simxInt node_base = -1;
  for(simxInt i = 0; i < handles_count; ++i)
  {
    if(parents[i] == nid)
    {
      node_base = handles[i];
      break;
    }
  }

  // find the target sub-object of the base sub-object
  jint ret_val = -1; // node_target
  simxGetObjectChild (cid, node_base, 0, &ret_val,
    simx_opmode_oneshot_wait);

  if (ret_val< 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::platforms::VREP_UAV::get_target_handle:" \
      " invalid target handle id\n");
  }

  return ret_val;
}
