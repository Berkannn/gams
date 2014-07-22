
#include "gams_jni.h"

static JavaVM* gams_JVM = NULL;
static jclass gams_JNI = NULL;
static jclass jni_string = NULL;

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    gams_JNI = (jclass)env->NewGlobalRef(env->FindClass("com/gams/MadaraJNI"));
    jni_string = (jclass)env->NewGlobalRef(env->FindClass("java/lang/String"));

    gams_JVM = vm;

    return env->GetVersion();
}

void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    env->DeleteGlobalRef(gams_JNI);
    gams_JVM = NULL;
}

JNIEnv* gams_jni_get_env()
{
    JNIEnv* env;
    gams_JVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (env == 0) //Thread is not attached
    {
        gams_JVM->AttachCurrentThread((void**)&env, NULL);
    }
    return env;
}

JavaVM* gams_jni_jvm()
{
    return gams_JVM;
}

jclass gams_jni_class()
{
    return gams_JNI;
}

jclass jni_string_cls()
{
    return jni_string;
}

JNIEnv* jni_attach()
{
    return gams_jni_get_env();
}

void jni_detach()
{
    gams_JVM->DetachCurrentThread();
}
