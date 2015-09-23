# Creating a programming language

In a way programming has always bothered me. Not programming itself, but all
the legacy left in languages created decades ago. There's everything (these are
not all legacy issues, but design choises) from defaulting to global scope in
lua, portability quirks in shell scripting, over-defensive styles of many imperative
languages to the lack of non-bif guards in Erlang.

As an exercise I have decided to create my own toy language to get a better
view of the complexity and trade-offs language designers might encounter.

The "selling points" of the language are:

 * slightly "interpreted"
 * pattern matching
 * support meta-programming like lisp/elixir
 * lightweight processing
 * smp
 * actor based
 * semi-strongly typed (types can be specified or inferred, but will not be
   used for optimization. Unlike some strictly-typed languages a list can
   contain any type, meaning shit could potentially break at runtime but in
   most cases it will break at compile time)

All-in-all the runtime will look and feel alot like erlang while the syntax
will most likely be a mix of Elixir/Ruby, Haskell and C.

For instance collecting and printing odd numbers of an infinite number sequence

```
0..
	| Stream.filter (e) -> 0 == e % 2
	| IO.write
```

## Where does it end?

In the end it would be fun to have something that could be run on bare-metal
to create applications unrestricted by OS conventions. If it never gets that
far I'll atleast learn a thing or three.

## Starting out

To start out the language I'm thinking an initial version might "transpile" to
lisp code to support meta programming - I will strive to make the conversion
to/from the lisp AST to the native form of the language as identical as possible
but some of the optimizations I have in mind will make this difficult.

With lisp in mind, I will probably use picolisp for the task since it's very
minimal and the mini version have no dependencies meaning it can easily be embedded.
To get this to where I envision AND if I use picoLisp alot of changes will have
to be made to picoLisp, so I might only write a picoLisp POC! Anyway that's
a far way from now.

This endeavour will be documented here as I progress/regress and I will try to
make a honest account of the work I do.

First up will be to write a tokenizer for source files
