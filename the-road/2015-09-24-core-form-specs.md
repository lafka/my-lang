# Core form specification

**Note** written on the bus, and in a old pig house listening to a stoner-punk
band doing rehearsal :)

Code for this can be found in the-road-src/2015-09-24-core

Previously we jumbled our way through the idea of having a core form for the
code that's used to store all the type information.

The idea is novel, but to get started I'll lay out the stripped specifications
without anything but the most basic type information.

I'll take the following code and turn it into the core form (note that we don't
have any standard library, or any functions that we don't define ourselves).
At this point type inference is nothing to worry about and I will just write
the type information. Also note that we are actually defining the type for
all the symbols return by the tokenizer. The only thing we we are lacking here
to fulfil the minimum functionality of our language is the `Module`, `Map`,
`Type` and `Process` types.


```
type Type      :: {}
type Nothing   :: Type
type Something :: Type { value: Nothing }

type String  :: Something { }
type Integer :: Something { }
type Float   :: Something { }
type Keyword :: Something { }
type List    :: Something { }
type Array   :: Something { }

type :: Block { arguments: Array, body: Array }
type :: Function { arguments: Array, body: Array }

nil      :: Nothing
anything :: Something

int   :: Integer = 1
str   :: String = "string"
float :: Float = 1.0
list  :: List = []
array :: Array = ()
fun   :: Function = () -> Nothing

Nothing
```

With this minimal definition we can reduce some properties of HOW the language
will work:

 * A type is really just a set of (k,v) pair
 * Type is not just compiler definitions, but first class citizens of the language
   meaning we can programmatically define them (we'll get back on this later)
 * Types seem to be monadic (not that I know what that is...)
 * Function calls take a fixed size array, and executes a code block
	 which is a fixed size array


The code above is one single block that takes no input and returns `Nothing`.
All of the code in a project is divided into blocks, a block is conceptually a
function, it expects an array of arguments from the parent block and it exports
some code (a tree of instructions), a set of types. Blocks have two purposes:
scoping and serving as jump table for use by scheduler.

A prime example of this is a Fibonacci sequence (assuming we have the type
definitions from above):

```
init :: Integer = 1
stream (n, acc) -> (acc, n + acc), (init, init)
```

Which results in the following pseudo core form (remember this is mean to be
readable):

```
$#block :: Integer () {
	let init, Integer{value: 1}

	export _#stream/2 ($1#n, $2#acc), {
		($2#acc, (+, $1#n, $2#acc))
	}

	stream
		Function{
			arguments: (Integer, Integer),
			body: ( (_#stream/2, $1#n, $2#acc) )
		},
		init,
		init
}
```

**Note**: There is no real difference between the above semi-iterative code
and a more AST like `(let (init, Integer{value: 1}) (export .... stream ...))`
the core form should be very compatible with the AST form.


**Note: Parser issues**

	Assuming everything is on same line we are fine... But when tokenizing we
	want to match multi line strings, blocks, arrays, lists etc AND be able to
	provide line numbers for errors etc. I'v realized that this is mostly todo
	with the tokenizer (ie when we want to check for matching braces for blocks).
