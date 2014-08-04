#include "com_gams_algorithms_BaseAlgorithm.h"
#include "gams/algorithms/java/Java_Algorithm.h"

namespace algorithms = gams::algorithms;

/*
 * Class:     com_gams_algorithms_BaseAlgorithm
 * Method:    jni_Base
 * Signature: (Ljava/lang/Object;)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_algorithms_BaseAlgorithm_jni_1Base
  (JNIEnv *, jobject, jobject subclass)
{
  return (jlong) new algorithms::Java_Algorithm (subclass);
}


/*
 * Class:     com_gams_algorithms_BaseAlgorithm
 * Method:    jni_getKnowledgeBase
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_algorithms_BaseAlgorithm_jni_1getKnowledgeBase
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  algorithms::Base * current = (algorithms::Base *)cptr;
  if (current)
  {
    result = (jlong) current->get_knowledge_base ();
  }

  return result;
}

/*
 * Class:     com_gams_algorithms_BaseAlgorithm
 * Method:    jni_getSelf
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_algorithms_BaseAlgorithm_jni_1getSelf
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  algorithms::Base * current = (algorithms::Base *)cptr;
  if (current)
  {
    result = (jlong) current->get_self ();
  }

  return result;
}

/*
 * Class:     com_gams_algorithms_BaseAlgorithm
 * Method:    jni_getPlatform
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_algorithms_BaseAlgorithm_jni_1getPlatform
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  algorithms::Base * current = (algorithms::Base *)cptr;
  if (current)
  {
    result = (jlong) current->get_platform ();
  }

  return result;
}

/*
 * Class:     com_gams_algorithms_BaseAlgorithm
 * Method:    jni_getAlgorithmStatus
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_com_gams_algorithms_BaseAlgorithm_jni_1getAlgorithmStatus
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  algorithms::Base * current = (algorithms::Base *)cptr;
  if (current)
  {
    result = (jlong) current->get_algorithm_status ();
  }

  return result;
}
