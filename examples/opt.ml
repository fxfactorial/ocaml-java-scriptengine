let () =
  Printf.printf "Started at %f\n" (Unix.time ());

  let js_code = ScriptEngine.JavaScript {|
var do_speak = function(param) {
  return param.toUpperCase();
}
do_speak('hello world');

|}
  in

  let f () =
    let engine = new ScriptEngine.javascript_engine in
    let result = engine#eval js_code in
    print_endline result
  in
  f ();
  Gc.major ();
  Printf.printf "Finished at %f\n" (Unix.time ())
