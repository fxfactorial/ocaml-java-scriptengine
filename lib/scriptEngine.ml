type js_code = JavaScript of bytes

module JavaCode = struct

  type jvm
  type jni_env

  type t = (jvm * jni_env)

  type js_scriptengine

  type class_path = Class_path of bytes

  (* Starting a JVM is surprisingly expensive, at least to me. *)
  external init : string -> t = "scriptengine_ml_init_jvm"
  external destroy_jvm : jvm -> unit = "scriptengine_ml_destroy_jvm"

  (* The actual scriptengine itself *)
  external init_js_scriptengine : unit ->
    js_scriptengine = "scriptengine_ml_init_js_engine"
  external destroy_js_scriptengine :
    js_scriptengine -> unit = "scriptengine_ml_destroy_js_engine"

  external eval_js :
    jvm -> js_scriptengine -> bytes -> bytes = "scriptengine_ml_eval_js"

end

let class_path = ref ".:./"

class java_environment =
  let (jvm, jni_env) = JavaCode.init !class_path in
  object
    val jvm = jvm
    val jni_env = jni_env
    method unsafe_jvm_ptr = jvm
    method unsafe_jni_env_ptr = jni_env
  initializer Gc.finalise JavaCode.destroy_jvm jvm
end

(** Creates a JavaScript evaluator, note creates a JVM, which can be
    slow *)
class javascript_engine = object

  val jvm = new java_environment
  val script_engine = JavaCode.init_js_scriptengine ()

  (* initializer Gc.finalise JavaCode.destroy_js_scriptengine script_engine *)

  method eval (JavaScript src) =
    JavaCode.eval_js jvm#unsafe_jvm_ptr script_engine src

end
