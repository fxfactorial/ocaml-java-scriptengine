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

extern "C" {

  CAMLprim value
  scriptengine_ml_init_jvm(value jvm_parameters)
  {
    CAMLparam0();
    CAMLlocal3(environment_tuple, jvm_wrapper, env_wrapper);

    JavaVMOption options[1];
    JNIEnv *env = nullptr;
    JavaVM *jvm = nullptr;
    JavaVMInitArgs vm_args;

    environment_tuple = caml_alloc_tuple(2);

    jvm_wrapper = caml_alloc(sizeof(JavaVM*), Abstract_tag);
    env_wrapper = caml_alloc(sizeof(JNIEnv*), Abstract_tag);

    const char *option_string =
      (std::string("-Djava.class.path=") +
       std::string(String_val(jvm_parameters))).c_str();

    options[0].optionString = (char*)option_string;

    memset(&vm_args, 0, sizeof(vm_args));

    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;

    Store_field(environment_tuple, 0, jvm_wrapper);
    Store_field(environment_tuple, 1, env_wrapper);

    CAMLreturn(environment_tuple);
  }

  CAMLprim value
  scriptengine_ml_destroy_jvm(value jvm)
  {
    CAMLparam1(jvm);
    CAMLreturn(Val_unit);
  }

  CAMLprim value
  scriptengine_ml_init_js_engine(value __attribute__((unused)))
  {
    CAMLparam0();
    CAMLreturn(Val_unit);
  }

  CAMLprim value
  scriptengine_ml_destroy_js_engine(value scriptengine)
  {
    CAMLparam1(scriptengine);
    CAMLreturn(Val_unit);
  }

  CAMLprim value
  scriptengine_ml_eval_js(value jvm, value script_engine, value js_string)
  {
    CAMLparam3(jvm, script_engine, js_string);
    CAMLreturn(Val_unit);
  }

}
