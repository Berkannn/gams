
#include "gams_jni.h"

#ifndef _MADARA_JAVA_ACQUIRE_VM_H_
#define _MADARA_JAVA_ACQUIRE_VM_H_

/**
 * @file Acquire_GIL.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains helper classes for Acquiring the GIL.
 **/

namespace gams
{
  namespace utility
  {
    namespace java
    {
      /**
       * @class Acquire_GIL
       * @brief This class encapsulates accessing the global interpreter lock
       **/
      class Acquire_VM
      {
      public:
        Acquire_VM ()
        {
          env = gams_jni_get_env ();
        }

        ~Acquire_VM()
        {
          jni_detach();
        }

        JNIEnv * env;
      };
    }
  }
}

#endif // not defined _MADARA_JAVA_ACQUIRE_VM_H_
