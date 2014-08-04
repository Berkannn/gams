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
#include "Java_Platform.h"
#include "gams/utility/java/Acquire_VM.h"


gams::platforms::Java_Platform::Java_Platform (
  jobject obj,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
  : Base (knowledge, sensors, self)
{
  if (platforms && knowledge)
  {
    (*platforms)[get_id ()].init_vars (*knowledge, get_id ());
    status_ = (*platforms)[get_id ()];
  }
  
  //We have to create a globla ref to the object or we cant call it
  JNIEnv * env = gams_jni_get_env ();
  obj_ = (jobject) env->NewGlobalRef (obj);
  class_ = (jclass) env->NewGlobalRef (env->GetObjectClass (obj_));
}

gams::platforms::Java_Platform::~Java_Platform ()
{
}

void
gams::platforms::Java_Platform::operator= (const Java_Platform & rhs)
{
  if (this != &rhs)
  {
    platforms::Base * dest = dynamic_cast <platforms::Base *> (this);
    const platforms::Base * source =
      dynamic_cast <const platforms::Base *> (&rhs);

    *dest = *source;
  }
}
 
int
gams::platforms::Java_Platform::analyze (void)
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

double
gams::platforms::Java_Platform::get_gps_accuracy () const
{
  gams::utility::java::Acquire_VM jvm;
  jdouble result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "getGpsAccuracy", "()D" );

  if (call)
  {
    result = jvm.env->CallDoubleMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire getGpsAccuracy() from custom Java platform class.\n");
  }

  return result;
}

std::string gams::platforms::Java_Platform::get_id () const
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

std::string gams::platforms::Java_Platform::get_name () const
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

double
gams::platforms::Java_Platform::get_move_speed () const
{
  gams::utility::java::Acquire_VM jvm;
  jdouble result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "getGpsAccuracy", "()D" );

  if (call)
  {
    result = jvm.env->CallDoubleMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire getGpsAccuracy() from custom Java platform class.\n");
  }

  return result;
}

int
gams::platforms::Java_Platform::home (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "home", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire home() from custom Java platform class.\n");
  }

  return result;
}

int
gams::platforms::Java_Platform::land (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "land", "()I" );

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

int
gams::platforms::Java_Platform::move (const utility::Position & position,
  const double & epsilon)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID move_call = jvm.env->GetMethodID(
    class_, "move", "(Lcom.gams.utility.Position;D)I" );
  jclass pos_class = jvm.env->FindClass ("com.gams.utility.Position");
  jmethodID pos_const = jvm.env->GetMethodID(pos_class, "<init>", "(JJJ)V");

  if (move_call)
  {
    jobject inpos = jvm.env->NewObject (
      pos_class, pos_const, position.x, position.y, position.z);
    jdouble inepsilon (epsilon);
    result = jvm.env->CallIntMethod (obj_, move_call, inpos, inepsilon);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire move() from custom Java platform class.\n");
  }

  return result;
}

int
gams::platforms::Java_Platform::sense (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "sense", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire sense() from custom Java platform class.\n");
  }

  return result;
}

void
gams::platforms::Java_Platform::set_move_speed (const double & speed)
{
  gams::utility::java::Acquire_VM jvm;

  jmethodID call = jvm.env->GetMethodID(class_, "setMoveSpeed", "(D)V" );

  if (call)
  {
    jdouble jspeed (speed);
    jvm.env->CallVoidMethod (obj_, call, jspeed);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire setMoveSpeed() from custom Java platform class.\n");
  }
}

int
gams::platforms::Java_Platform::takeoff (void)
{
  gams::utility::java::Acquire_VM jvm;
  jint result (0);

  jmethodID call = jvm.env->GetMethodID(class_, "takeoff", "()I" );

  if (call)
  {
    result = jvm.env->CallIntMethod (obj_, call);
  }
  else
  {
    knowledge_->print (
      "ERROR: Unable to acquire takeoff() from custom Java platform class.\n");
  }

  return result;
}

jobject
gams::platforms::Java_Platform::get_java_instance (void)
{
  return obj_;
}
