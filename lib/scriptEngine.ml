type js_code = JavaScript of bytes

module JavaCode = struct

  type jvm
  type jni_env

  type t = (jvm * jni_env)

  type js_scriptengine

  type jni_version = One_1 | One_2 | One_4 | One_6 | One_8

  external init : jni_version -> string -> t = "scriptengine_ml_init_jvm"
  external destroy_jvm : jvm -> unit = "scriptengine_ml_destroy_jvm"

  (* The actual scriptengine itself *)
  external init_js_scriptengine : jni_env ->
    js_scriptengine = "scriptengine_ml_init_js_engine"
  external destroy_js_scriptengine :
    jni_env -> js_scriptengine -> unit = "scriptengine_ml_destroy_js_engine"

  external eval_js :
    jni_env -> js_scriptengine -> bytes -> bytes = "scriptengine_ml_eval_js"

end

class java_environment ?(class_path=".:./") jni_version =
  object
    val pair = JavaCode.init jni_version class_path
    method unsafe_jvm_ptr = fst pair
    method unsafe_jni_env_ptr = snd pair

    initializer Gc.finalise (fun (jvm, env) ->
        JavaCode.destroy_jvm jvm;
      ) pair

  end

(** Creates a JavaScript evaluator, note creates a JVM, which can be
    slow *)
class javascript_engine ?(java_env=new java_environment JavaCode.One_8) () =
  object
    val script_engine =
      JavaCode.init_js_scriptengine java_env#unsafe_jni_env_ptr

    initializer

      Gc.finalise
        (JavaCode.destroy_js_scriptengine java_env#unsafe_jni_env_ptr)
        script_engine

    method eval (JavaScript src) =
      JavaCode.eval_js java_env#unsafe_jni_env_ptr script_engine src

  end
