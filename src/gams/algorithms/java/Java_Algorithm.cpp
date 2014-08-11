/**
 * Copyright (c) 2014 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The names “Carnegie Mellon University,” "SEI” and/or “Software
 *    Engineering Institute" shall not be used to endorse or promote products
 *    derived from this software without prior written permission. For written
 *    permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *    appear in their names without prior written permission of
 *    permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 * 
 *      This material is based upon work funded and supported by the Department
 *      of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *      University for the operation of the Software Engineering Institute, a
 *      federally funded research and development center. Any opinions,
 *      findings and conclusions or recommendations expressed in this material
 *      are those of the author(s) and do not necessarily reflect the views of
 *      the United States Department of Defense.
 * 
 *      NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *      INSTITUTE MATERIAL IS FURNISHED ON AN “AS-IS” BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 **/
#include "Java_Algorithm.h"
#include "gams/utility/java/Acquire_VM.h"


gams::algorithms::Java_Algorithm::Java_Algorithm (
  jobject obj,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self,
  variables::Devices * devices)
  : Base (knowledge, platform, sensors, self, devices)
{
  //We have to create a globla ref to the object or we cant call it
  JNIEnv * env = gams_jni_get_env ();
  if (env)
  {
    obj_ = (jobject) env->NewGlobalRef (obj);
    class_ = (jclass) env->NewGlobalRef (env->GetObjectClass (obj_));
  }
}

gams::algorithms::Java_Algorithm::~Java_Algorithm ()
{
  gams::utility::java::Acquire_VM jvm;
  if (jvm.env)
  {
    jvm.env->DeleteGlobalRef (obj_);
    jvm.env->DeleteGlobalRef (class_);
  }
}

void
gams::algorithms::Java_Algorithm::operator= (const Java_Algorithm & rhs)
{
  if (this != &rhs)
  {
    gams::utility::java::Acquire_VM jvm;
    algorithms::Base * dest = dynamic_cast <algorithms::Base *> (this);
    const algorithms::Base * source =
      dynamic_cast <const algorithms::Base *> (&rhs);

    *dest = *source;

    if (jvm.env)
    {
      jvm.env->DeleteGlobalRef (obj_);
      jvm.env->DeleteGlobalRef (class_);

      obj_ = jvm.env->NewGlobalRef (rhs.obj_);
      class_ = (jclass) jvm.env->NewGlobalRef (rhs.class_);
    }
  }
}
 
int
gams::algorithms::Java_Algorithm::analyze (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "analyze", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire land() from custom Java platform class.\n");
  }

  return result;
}

std::string gams::algorithms::Java_Algorithm::get_id () const
{
  gams::utility::java::Acquire_VM jvm;
  std::string id;
  jstring result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "getId", "()Ljava.lang.String;" );

  if (call)
  {
    result = (jstring) jvm.env->CallObjectMethod (obj_, call);
    const char * id_chars = jvm.env->GetStringUTFChars(result, 0);
    id = id_chars;
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire getId() from custom Java platform class.\n");
  }

  return id;
}

std::string gams::algorithms::Java_Algorithm::get_name () const
{
  gams::utility::java::Acquire_VM jvm;
  std::string name;
  jstring result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "getName", "()Ljava.lang.String;" );

  if (call)
  {
    result = (jstring) jvm.env->CallObjectMethod (obj_, call);
    const char * name_chars = jvm.env->GetStringUTFChars(result, 0);
    name = name_chars;
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire getName() from custom Java platform class.\n");
  }

  return name;
}

int
gams::algorithms::Java_Algorithm::execute (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "execute", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire execute() from custom Java platform class.\n");
  }

  return result;
}

int
gams::algorithms::Java_Algorithm::plan (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "plan", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire plan() from custom Java platform class.\n");
  }

  return result;
}

jobject
gams::algorithms::Java_Algorithm::get_java_instance (void)
{
  return obj_;
}
