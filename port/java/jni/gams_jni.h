
#ifndef _Included_com_gams_JNI
#define _Included_com_gams_JNI

#include <jni.h>
#include "gams/GAMS_Export.h"

#ifdef _GAMS_ANDROID_
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);
#endif

JNIEXPORT JNIEnv* gams_jni_get_env();

JNIEXPORT JavaVM* gams_jni_jvm();

JNIEXPORT jclass gams_jni_class();
JNIEXPORT jclass jni_string_cls();

JNIEXPORT JNIEnv* jni_attach();
JNIEXPORT void jni_detach();

#endif