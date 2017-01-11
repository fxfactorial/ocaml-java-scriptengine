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
    let engine =
      new ScriptEngine.javascript_engine ()
    in
    let result = engine#eval js_code in
    print_endline result
  in
  f ();
  Gc.major ();
  Printf.printf "Finished at %f\n" (Unix.time ())

let exhaust ic =
  let all_input = ref [] in
  (try while true do all_input := input_line ic :: !all_input; done
   with End_of_file -> ());
  close_in ic |> ignore;
  List.rev !all_input

let () =
  let js_code =
    open_in "./examples/js_code1.js"
    |> exhaust
    |> String.concat "\n"
  in
  let engine =
    new ScriptEngine.javascript_engine ()
  in
  engine#eval (ScriptEngine.JavaScript js_code)
  |> print_endline
