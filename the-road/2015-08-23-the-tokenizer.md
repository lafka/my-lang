# The Tokenizer

To convert source files into the underlying AST we need to convert from
semi-structured text to s-expressions.

## Parser rules

* The most basic form is a symbol. There are 6 distinct symbols:
   1. a string `".."`
   2. integer number `123`
   3. a decimal number `1.23`
   4. a kewyword `:keyword`
   5. a linked list `[ expr... ]`
   5. a fixed size array/tuple `( expr... )`

* Symbols are separated by whitespace, either a ASCII- space, vertical tab or newline
* Everything that is NOT a symbol is a macro or a function call

For instance the input:

```
a = 1
debug when
	a > 1 -> a - 1
	_ -> 0
```

Should be tokenized to (represented as a comma separated list):
```
'a', '=', 1, '\n',
'debug', 'when', '\n',
'a', '>', '1', '->', 'a', '-', 1, '\n',
'_', '->', 0, '\n',
'end'
```

Which should result in code approximate to:
```
(let (a 1)
	(debug (if (> a 1) (- a 1) 0)))
```

Since the current idea is to run on top of a full language (lisp), there are
(as of yet) no special forms - only macros and functions calls. 


Once the source have been tokenized macro can the take effect. For instance
there will be a infix `assign` macro defined as `lhs :: any = rhs :: any`
which takes the assignment of `a = 1` and converts it to the ast `(let (a 1))`
Then the macro block `when body...` processes `body...`.

This means we have a few different form of pre-processing macros:
 * infix macros (ie. `lhs = rhs`, `lhs -> rhs`, `lhs := rhs`)
 * block macros (`when body...`) where body must pattern match (ie `_ -> expr`)
 * regular macro (`debug expr..`)

Already we have a special case `debug` which can seem like a block macro
printing out whatever the result of `expr..`. The difference being that a block
macro may contain separate executions paths (ie multiple `->` blocks) while
from `debug`'s point of view there is only on possible expression to evaluate;
being the result of the `when body..` block.

Right now it seems like we have three type of code representatoins:
 1. The string representing the source
 2. The intermediate tokenized code
 3. The converted ast code

In essence the final representation is actually no different than any other
types. We could just as easy have defined the example above as its pure sat
version:

```
(:let,
	(:a, 1),
	(:debug, (:if, (:>, :a, 1), (:-, :a, 1) 0)))
```


## Quick note on list/array

  I at first thought there should be minimal work in the tokenzier and rather
	rely on macros to define list/arrays. This could work out as it's better to rely
  on the parser for fundamental things rather than magically calling macros
	that may or may not be redefined.  Still need to see which will be picked as
	it will dicate how flexible the core language will be in regards to macros

In our pre-conception language there is still no way to construct a array or
list in code. This would be intrinsic in the AST, but we need some mechanism
for it.

```
# expr... here might be a proper,space separated thing: `'1', ',', '2'..`,
# a single blob `'1,2,3'` or even a combination `'1,2,', '3'`
# in any event it will SHOULD always be a comma separated sequence

# we could maybe define this as a macro??
macro [expr...] ->
  flatmap expr (e) ->
		String.split e, Array.new(" ", ","), Map.new.set(:empty, :false)
end

macro (expr...) ->
	parts = String.split e, Array.new(" ", ","), Map.new.set(:empty, :false)
	apply Array.new, parts
end

# now we can do this:
List.new(1, 2, 3) := [1,2,3]
Array.new(1, 2, 3) := ( 1 , 2 , 3 )
```

## note on annotations

A expression can be anything. BUT to allow the compiler and macros to do
magical things one could resort to annotating an expression.

So for instance let's define a unsigned int of size 8:

```
a :: int(size: 8, signed: false) = 1
```

This is fine, we could create an entry in a `type` table that would along with
a unique reference so we could check it out.... OR
annotate the extra metadata (which we will need in the future anyway to convey
linenumbers/filepaths etc for errors) and pass it along.
This means there might be a requirement for an intermediate AST before the
actual lisp code is generated. Something like Elixir's AST maybe (`{:+, [env..], [args]}`)???
or some state that always follows the parser. Meaning it will be more difficult
to keep track of everything but the AST will be simpler... Gonna sleep on it!
 
