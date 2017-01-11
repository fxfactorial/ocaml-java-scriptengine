ScriptEngine
------------

These are OCaml bindings to
`Java`'s
[ScriptEngine](https://docs.oracle.com/javase/7/docs/api/javax/script/ScriptEngine.html).

Evaluate `JavaScript` code in OCaml by letting Java do all the hard
work :) 

Note: You'll need to have a `JDK` installed and a `java`, `javac`
accessible on your path in order for this to properly install.

Here's some example code: 

```ocaml
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
```

# Installation

Once this is on `opam`, you'll be able to get it with 

```
$ opam install scriptengine
```

# How it works
This works by directly calling the relevant `Java` code for evaluating
`JavaScript` through the `JNI` and then passing those results back to `OCaml`
