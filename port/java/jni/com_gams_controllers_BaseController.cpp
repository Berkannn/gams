
#include "com_gams_controllers_BaseController.h"
#include "gams/controllers/Base_Controller.h"

namespace containers = Madara::Knowledge_Engine::Containers;
namespace engine = Madara::Knowledge_Engine;
namespace controllers = gams::controllers;
namespace algorithms = gams::algorithms;
namespace platforms = gams::platforms;

typedef Madara::Knowledge_Record::Integer  Integer;

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_BaseControllerFromKb
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1BaseControllerFromKb
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result = 0;

  engine::Knowledge_Base * kb = (engine::Knowledge_Base *) cptr;
  if (kb)
  {
    result = (jlong) new gams::controllers::Base (*kb);
  }

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_BaseController
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1BaseController
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result = 0;

  controllers::Base * input = (controllers::Base *) cptr;
  if (input)
  {
    result = (jlong) new controllers::Base (*input);
  }

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_freeBaseController
 * Signature: (J)V
 */
void JNICALL Java_com_gams_controllers_BaseController_jni_1freeBaseController
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (controllers::Base *) cptr;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_toString
 * Signature: (J)Ljava/lang/String;
 */
jstring JNICALL Java_com_gams_controllers_BaseController_jni_1toString
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = env->NewStringUTF("BaseController");

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_analyze
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1analyze
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->analyze ();

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_execute
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1execute
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->execute ();

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initAccent
 * Signature: (JLjava/lang/String;Lcom/madara/KnowledgeList;)V
 */
void JNICALL Java_com_gams_controllers_BaseController_jni_1initAccent
  (JNIEnv * env, jobject, jlong cptr, jstring name, jlongArray argslist)
{
  controllers::Base * current = (controllers::Base *) cptr;

  if (current)
  {
    // get the name in C string format
    const char * str_name = env->GetStringUTFChars(name, 0);

    // create a knowledge vector to hold the arguments
    Madara::Knowledge_Vector args (env->GetArrayLength (argslist));
    jlong * elements = env->GetLongArrayElements (argslist, 0);

    // iterate through arguments and copy the knowledge record for each arg
    for (size_t i = 0; i < args.size (); ++i)
    {
      Madara::Knowledge_Record * cur_record = (Madara::Knowledge_Record *)elements[i];

      if (cur_record)
        args[i] = Madara::Knowledge_Record (*cur_record);
    }
    
    // call the initialization method
    current->init_accent (str_name, args);
    
    // clean up the allocated elements
    env->ReleaseLongArrayElements(argslist, elements, 0);
    env->ReleaseStringUTFChars(name, str_name);
  }
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initAlgorithm
 * Signature: (JLjava/lang/String;Lcom/madara/KnowledgeList;)V
 */
void JNICALL Java_com_gams_controllers_BaseController_jni_1initAlgorithm
  (JNIEnv * env, jobject, jlong cptr, jstring name, jlongArray argslist)
{
  controllers::Base * current = (controllers::Base *) cptr;

  if (current)
  {
    // get the name in C string format
    const char * str_name = env->GetStringUTFChars(name, 0);

    // create a knowledge vector to hold the arguments
    Madara::Knowledge_Vector args (env->GetArrayLength (argslist));
    jlong * elements = env->GetLongArrayElements (argslist, 0);

    // iterate through arguments and copy the knowledge record for each arg
    for (size_t i = 0; i < args.size (); ++i)
    {
      Madara::Knowledge_Record * cur_record = (Madara::Knowledge_Record *)elements[i];

      if (cur_record)
        args[i] = Madara::Knowledge_Record (*cur_record);
    }
    
    // call the initialization method
    current->init_algorithm (str_name, args);
    
    // clean up the allocated elements
    env->ReleaseLongArrayElements(argslist, elements, 0);
    env->ReleaseStringUTFChars(name, str_name);
  }
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initPlatform
 * Signature: (JLjava/lang/String;Lcom/madara/KnowledgeList;)V
 */
void JNICALL Java_com_gams_controllers_BaseController_jni_1initPlatform
  (JNIEnv * env, jobject, jlong cptr, jstring name, jlongArray argslist)
{
  controllers::Base * current = (controllers::Base *) cptr;

  if (current)
  {
    // get the name in C string format
    const char * str_name = env->GetStringUTFChars(name, 0);

    // create a knowledge vector to hold the arguments
    Madara::Knowledge_Vector args (env->GetArrayLength (argslist));
    jlong * elements = env->GetLongArrayElements (argslist, 0);

    // iterate through arguments and copy the knowledge record for each arg
    for (size_t i = 0; i < args.size (); ++i)
    {
      Madara::Knowledge_Record * cur_record = (Madara::Knowledge_Record *)elements[i];

      if (cur_record)
        args[i] = Madara::Knowledge_Record (*cur_record);
    }
    
    // call the initialization method
    current->init_platform (str_name, args);
    
    // clean up the allocated elements
    env->ReleaseLongArrayElements(argslist, elements, 0);
    env->ReleaseStringUTFChars(name, str_name);
  }
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initVars
 * Signature: (JJJ)V
 */
void JNICALL Java_com_gams_controllers_BaseController_jni_1initVars
  (JNIEnv * env, jobject, jlong cptr, jlong id, jlong processes)
{
  Integer tempId (id), tempProcesses (processes);
  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    current->init_vars (tempId, tempProcesses);
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initVarsAlgorithm
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_gams_controllers_BaseController_jni_1initVarsAlgorithm
  (JNIEnv * env, jobject, jlong cptr, jlong aptr)
{
  controllers::Base * current = (controllers::Base *) cptr;
  algorithms::Base * algorithm = (algorithms::Base *) aptr;

  if (current && algorithm)
  {
    current->init_vars (*algorithm);
  }
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_initVarsPlatform
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_gams_controllers_BaseController_jni_1initVarsPlatform
  (JNIEnv * env, jobject, jlong cptr, jlong pptr)
{
  controllers::Base * current = (controllers::Base *) cptr;
  platforms::Base * platform = (platforms::Base *) pptr;

  if (current && platform)
  {
    current->init_vars (*platform);
  }
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_monitor
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1monitor
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->monitor ();

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_plan
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1plan
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->plan ();

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_run
 * Signature: (JDD)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1run
  (JNIEnv * env, jobject, jlong cptr, jdouble period, jdouble duration)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->run (period, duration);

  return result;
}

/*
 * Class:     com_gams_controllers_BaseController
 * Method:    jni_systemAnalyze
 * Signature: (J)J
 */
jlong JNICALL Java_com_gams_controllers_BaseController_jni_1systemAnalyze
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  controllers::Base * current = (controllers::Base *) cptr;
  if (current)
    result = current->system_analyze ();

  return result;
}
