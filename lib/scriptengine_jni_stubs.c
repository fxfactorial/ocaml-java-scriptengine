/* -*- c++ -*- */

#include <jni.h>

#include <cstring>
#include <iostream>
#include <string>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>

using namespace std::literals::string_literals;

#define Jvm_tag(t) (JavaVM*)Field(t, 0)
#define JNI_env_tag(t) (JNIEnv*)Field(t, 0)
#define Scriptengine_tag(t) (jobject)Field(t, 0)

extern "C" {

  CAMLprim value
  scriptengine_ml_init_jvm(value jni_version, value jvm_parameters)
  {
    CAMLparam2(jni_version, jvm_parameters);
    CAMLlocal3(environment_tuple, jvm_wrapper, env_wrapper);

    JavaVMOption options[1];
    JNIEnv *env = nullptr;
    JavaVM *jvm = nullptr;
    JavaVMInitArgs vm_args;

    environment_tuple = caml_alloc_tuple(2);

    jvm_wrapper = caml_alloc(sizeof(JavaVM*), Abstract_tag);
    env_wrapper = caml_alloc(sizeof(JNIEnv*), Abstract_tag);

    const char *option_string =
      ("-Djava.class.path="s +
       std::string(String_val(jvm_parameters))).c_str();

    options[0].optionString = (char*)option_string;

    memset(&vm_args, 0, sizeof(vm_args));

    vm_args.version = ([&](void){
	std::cout << Int_val(jni_version) << "\n";
	switch (Int_val(jni_version)) {
	case 0: return JNI_VERSION_1_1;
	case 1: return JNI_VERSION_1_2;
	case 2: return JNI_VERSION_1_4;
	case 3: return JNI_VERSION_1_6;
	case 4:
	default:
	  return JNI_VERSION_1_8;
	}
      })();

    vm_args.nOptions = 1;
    vm_args.options = options;

    long status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);

    Store_field(jvm_wrapper, 0, (value)jvm);
    Store_field(env_wrapper, 0, (value)env);

    Store_field(environment_tuple, 0, jvm_wrapper);
    Store_field(environment_tuple, 1, env_wrapper);

    CAMLreturn(environment_tuple);
  }

  CAMLprim value
  scriptengine_ml_destroy_jvm(value jvm)
  {
    CAMLparam1(jvm);
    auto *jvm_ptr = Jvm_tag(jvm);
    jvm_ptr->DestroyJavaVM();
    CAMLreturn(Val_unit);
  }

  CAMLprim value
  scriptengine_ml_init_js_engine(value jni_env)
  {
    CAMLparam1(jni_env);
    CAMLlocal1(engine_wrapper);

    JNIEnv *env = JNI_env_tag(jni_env);
    jclass manager_class = env->FindClass("javax/script/ScriptEngineManager");

    jmethodID manager_constructor = env->GetMethodID(manager_class, "<init>", "()V");

    jobject manager_instance = env->NewObject(manager_class, manager_constructor);

    // Don't forget the ; on these methods
    jmethodID get_engine_by_name =
      env->GetMethodID(manager_class,
      		       "getEngineByName",
      		       "(Ljava/lang/String;)Ljavax/script/ScriptEngine;");

    jobject scriptengine = env->CallObjectMethod(manager_instance,
    						 get_engine_by_name,
    						 env->NewStringUTF("javascript"));

    env->DeleteLocalRef(manager_instance);
    engine_wrapper = caml_alloc(sizeof(jobject), Abstract_tag);
    Store_field(engine_wrapper, 0, (value)(env->NewGlobalRef(scriptengine)));
    CAMLreturn(engine_wrapper);
  }

  CAMLprim value
  scriptengine_ml_destroy_js_engine(value env_ml, value scriptengine_ml)
  {
    CAMLparam2(env_ml, scriptengine_ml);
    auto *env = JNI_env_tag(env_ml);
    auto *scriptengine = Scriptengine_tag(scriptengine_ml);
    env->DeleteGlobalRef(scriptengine);
    CAMLreturn(Val_unit);
  }

  CAMLprim value
  scriptengine_ml_eval_js(value jni_env, value script_engine, value js_string)
  {
    CAMLparam3(jni_env, script_engine, js_string);
    jobject script_engine_instance = Scriptengine_tag(script_engine);
    JNIEnv *env = JNI_env_tag(jni_env);

    jclass engine_class = env->FindClass("javax/script/ScriptEngine");
    // Gives some kind of intermediate result type
    jmethodID eval =
      env->GetMethodID(engine_class,
      		       "eval",
      		       "(Ljava/lang/String;)Ljava/lang/Object;");

    jobject eval_result =
      env->CallObjectMethod(script_engine_instance,
			    eval,
			    env->NewStringUTF(String_val(js_string)));
    jclass object_class = env->FindClass("java/lang/String");
    jmethodID to_string =
      env->GetMethodID(object_class,
		       "toString",
		       "()Ljava/lang/String;");
    jstring evaled_as_string =
      (jstring)env->CallObjectMethod(eval_result,
				     to_string);

    CAMLreturn(caml_copy_string(env->GetStringUTFChars(evaled_as_string, nullptr)));
  }

}
