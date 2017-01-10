(* let () = *)
(*   Printf.printf "Started at %f\n" (Unix.time ()); *)
(*   let f () = *)
(*     let engine = new ScriptEngine.javascript_engine in *)
(*     () *)
    (* let result = engine#eval (ScriptEngine.JavaScript "1 + 1") in *)
    (* print_endline result *)
  (* in *)
  (* f (); *)
  (* Printf.printf "Finished at %f\n" (Unix.time ()); *)
  (* Gc.major () *)


let () =
  let g = new ScriptEngine.javascript_engine in
  ()
