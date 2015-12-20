# The interpreter

Due to time constraints and laziness I've decided to write a simple
interpreter for a minimal subset of the language.

That language should be a simple implementation of our future
language.

It will:
 - Only built in types: Integer, Float, String, Keyword
 - Support for minimal arithmetics (+, -, \*, /, \*\*, %)
 - Be interpreted (duh...)

It will NOT:
 - Be able todo much useful
 - Be able to emit any kind of machine- or byte-code
 - Have any scheduler, so no parallel, SMP, co-op thread or whatever
 - Have any meaningful form of IO
 - Have any form of memory management.


The interpreter should run through a set of expressions, evaluating
and printing the status of the expression (badmatch, continue etc).


## Tokenizing take num. 2

So back to tokenizing. Assuming uniform white space usage, and a few
other things we can get a quick parser up and running. But I want to
try something new.

Take the expression `a = b c`. This would be parsed to something like:
```
(:=,
	(
		:a,
		(:b, c)
	))
```

It's as trivial as `String.split("a = b c", " a")` to make this one
work (and a bit more to handle conversion to integers, arrays etc.
But something more interesting can be done to be able to give pretty
detailed parse exceptions: context.

So in this example we have the following contexts: `expr`, `astref` and `match`.
The root contexts is an `expr`, and we expect any other sub
expressions below it.

The first thing we get is the `a` which does not match anything
we know off, so it's an `astref`, later we find the `=` which is a
`match`, so our context is now `(expr, [astref, match])`.
Once we hit the `b c` part we get `(expr, [astref, match, expr])`
And then we suddenly had a AST!? So what exactly are the parsing
rules?

 - A string is defined using a matched set of quotes `'` or `"`.
 - A integer is defined by surrounding a `0..9*` with whitespace
 - A float is defined by suffixing a integer with `.0..9*`
 - An keyword is defined by `:[^\s]*` surrounded by whitespace
 - Any expression is terminated either by `;` or a newline `\n`
 - Any set of expressions can be grouped by parentheses
  - this means both the following are legal, and equivalent:
   - (somefun arg1 arg2)
   - +(arg1 arg2)
 - partial application should be possible, meaning `x = a + _` -> `(b) -> a + b`
 - infix operators: `left @op right` -> `(left right) -> @op(left right)`

Steps to make this:

- Write basic parser
 - tokenize and convert to AST in single pass
 - print out AST
- Add test support
 - prints every line evaluated with OK/FAILED





## First lesson: Implement a simple interpreted lisp

true | false -> :true | :false
nil -> ()

support operators as above. Along with the funs:
 - `quote`     - define a variable
 - `define`    - define a variable
 - `atom?`     - check if argument has the type atom (integer,keyword,float,string)
 - `keyword?`  - check if argument has the type keyword
 - `list?`     - check if argument has the type list
 - `number?`   - check if argument has the type number (float,integer)
 - `integer?`  - check if argument has the type integer
 - `float?`    - check if argument has the type float
 - `string?`   - check if argument has the type string - `keyword?`  -


```
(define wohlo)

(let
	((wohlo :b))
	(= wohlo :a))

(= wohlo :a)
```


### Memory management

Coming from a dynamic language I have absolutely no clue about this,
although I heard about this thing called `malloc` and `free`.

So the idea is: Integers and Floats will be represented in the native
size. If the platform has no support for floats, well then your out of
luck!.

Strings will be represented as a sequence of integers. So far it looks like the
actual structure of memory will be something like:

```
alloc = {
	free? => 0|1            # is this item free to be reused?
  size  => int            # the size of the data occupied
  buf   => data           # the actual allocated memory
}
```

and then each type would need to do something magical on its own for
data:

```
string = {
  type => "STRING"       # The actual type
	next => pointer | NULL # The part of the string
	buf  => some data
}

integer = {
	type => "INT"
  buf  => data
}

keyword = {
	type => "KEYWORD"
	sum => int  # the checksum of the thing! CRC32, SHA256 whatever is available
	buf => data # The string representation of the keyword
}
```

So this means the allocation of the string string "I consume memory"
will be `1 + sizeof(int) + sizeof(string.type) + sizeof(pointer) +
128`, a total of 321 bits.  so roughly 24 bytes overhead on a 64 bit machine,
12 bytes on a 32bit, 6bytes on a 16bit or 4bytes on a 8bit.

Not all to bad! If this every was to be used on something < 32bit
then static allocations would make more sense (how often do you
allocate random variables??)



### back to parsing!

So we fixed our memory. Let's go back to parsing!
Assuming we have entered perfect syntax into our REPL:

```
> (define str "oh my??")
> (define int1 1)
> (define int2 2)
>
> (when
> 	( (number? (define int (+ int1 int2))) (define str "oh my number") ))
>
> [str int]
#> ["oh my number" 3]
> (free str)
#> true
> (free str)
#> (error "I am free! either never allocated or, you deleted me from memory...")
```

This goes a bit like this:
1: in: `(define str "oh my???")`, process: [DEFINE, "str", "str"], call: op(DEFINE, "str", "str")
2: in: `(define int1 1)`, process: [DEFINE, "int1", 1], call: op(DEFINE, "int1", "1")
3: in: `(define int2 2)`, process: [DEFINE, "int2", 2], call: op(DEFINE, "int2", "2")
4: in: `(when (p1 e1))`, process: [WHEN, [p1,e1]], call: op(WHEN, [p1, e1])
5: in: `[str int]`, process: [LIST, [e1, e2]], op(LIST, [e1, e2])
6: in: `(free str)`, process: [FREE, "str"], op(free, "str")



For the actual parsing

1. Given buffer `b`, create AST NODE `p`
 1. Find EXPR-OPEN `(`, create AST NODE as child of `n`
  1. Look for next EXPR-OPEN or EXPR-CLOSE:
   1. EXPR-CLOSE: means this node  is "complete" and ready for parsing
   2. EXPR-OPEN: means this node has some children, add `_<#>` to parent, then descend as if 1.1


So take an example:

```
(define meal :lunch)
(when
	((== :lunch meal)  (define meal :dinner))
	((== :dinner meal) (define meal :night))
```

we build the parse tree:

```
(root (
	(n1 (define meal :lunch) ())
	(n2 "(when _1" (
		(n2a "(_1 _2" (
			(n2a1 (== :lunch meal) ())
			(n2a2 (define meal :dinner) ())
		)
	)
)
())
```
Note the difference between the incomplete string expr in n2, and n2a
versus the complete s-exp defined in n1, n2a1 and n2a2.
This tells us one thing, once the parenthesis matches and there are no
sub expressions we can convert it to an actual s-expr. This seems like
a cumbersome way to process it as we need to go k levels, where k is
the number of levels we must go down to find branchless expressions,
before we can fulfil an branch - I do believe that this will work
efficiently for arbitrary complex programs. 
