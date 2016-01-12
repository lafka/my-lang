# I heard a rumour

So apparently compilers these days do everything in like a million
passes, and it's actually recommended to do as little as possible in
each pass.

In my perfect world I would like to do:
 * pass #1: tokenize everything
 * pass #2: infix -> prefix + convert to AST
 * pass #3: type, typing, typed (magically happens to find missing functions!)
 * pass #4: macro expansion!
 * by now we have a "valid" AST, write an interpreter - or compiler - to run it later

Once we have something looking like an AST there is a question of
running it which requires, *drumrolls*, a runtime!

So back to tokenizing!!!!

I have some code:

```
a = 1 + n;
debug when
  a > 1 -> a - 1;
  _     -> 0
```

I did the splits!!!! all the syntax even looks ok^^

```
['a' '=' '1' '+', 'n' '\n'
 'debug' 'when' '\n'
 'a' '>' '1' '->' 'a' '-' '1' '\n'
 '_' '->' '0'
]
```


The expected AST looks a little bit like:

```
(main, [], [
	(=, [], (a,
	         (+, [], (1, n)))),

	(debug, [], [
		(when, [], [
			(->, [], ((>, a, 1), (- a 1))),
			(->, [], (:true, 0)) ]) ]) ])
```

Imagine we have some system behind which has the following pattern:

```
to_ast [left, infixed, right | rest],
			 (t, meta, block) -> (t, meta, block ++ (infixed, [], (left, right))
       when refs[infixed].infix
```

Which is all fine if we are given perfectly chunked expressions....
so given the tokens ["a", "=", "b", "c"] or ["a", "c", "=", "b"] would
be confusing for the AST converter.


So not only do we need to handle varying style of prefix and infix
notation for some cases, we also need to build a tree from a list of
tokens which we have very little idea of how is most reasonable to
assemble.

I'm thinking the conversion should not be using tokens that directly.
So for instance `1 + n` is a simple expression we can easily handle.
At the very end of a codepath there will always be atomic value. Once
we find that we can trace back!!


```
['a', '=', '1', '+', 'n', '\n', 'debug', 'when', '\n', ....]
```

So we have a few ways we can terminate the current expression:
 * newline
 * semi-colon
 * comma (atleast for lists, tuple)
 * grouping `()`, `[]`, `{}`, `<<>>`

Some are of course a bit context aware, for instance none of these
will terminate a string, lists must be closed by a `]` etc.

Given these 'breaks' will stop at the end of an expression, resulting
in the following block: `['a', '=', 1, '+', 'n']`

We have the two following things to care about:
 * `+ :: A, A -> A`
 * `= :: Expr, Expr -> Void`

So they both have an arity of two! Meaning there are two cases pr.  fun:
 * `to_ast [parent | left "+" right"] -> to_ast parent, (+, [], [left, right])`


* Partials:
  - everything is prefix, except SOME THINGS
  - we are always binding left to right
  - given `fun/3` we could `(((fun a) b) c)`
  - or `(((fun _ _ c) _ b) a)
  - this also means that we can not bind `(a +) 1` but have todo `(+ a) 1`
