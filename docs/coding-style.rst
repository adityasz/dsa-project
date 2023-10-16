.. _codingstyle:

Coding Style
============

This is a short document describing the preferred coding style for this
project. It is adapted from the Linux Kernel Coding Style.

Coding style is very personal, and I won't **force** my views on
anybody, but this is what goes for anything that I have to be able to
maintain, and I'd prefer it for most other things too. Please at least
consider the points made here.

First off, I'd suggest printing out a copy of the GNU coding standards,
and NOT read it. Burn them, it's a great symbolic gesture.

Anyway, here goes:


1. Indentation
--------------

Tabs are 8 characters, and thus indentations are also 8 characters.

Rationale: The whole idea behind indentation is to clearly define where
a block of control starts and ends. Especially when you've been looking
at your screen for 20 straight hours, you'll find it a lot easier to see
how the indentation works if you have large indentations.

Now, some people will claim that having 8-character indentations makes
the code move too far to the right, and makes it hard to read on a
80-character terminal screen. The answer to that is that if you need
more than 3 levels of indentation, you're screwed anyway, and should fix
your program.

In short, 8-char indents make things easier to read, and have the added
benefit of warning you when you're nesting your functions too deep.
Heed that warning.

The preferred way to ease multiple indentation levels in a switch
statement is to align the ``switch`` and its subordinate ``case`` labels
in the same column instead of ``double-indenting`` the ``case`` labels.
For example:

.. code-block:: c

	switch (suffix) {
	case 'G':
	case 'g':
		mem <<= 30;
		break;
	case 'M':
	case 'm':
		mem <<= 20;
		break;
	case 'K':
	case 'k':
		mem <<= 10;
		fallthrough;
	default:
		break;
	}

Don't put multiple statements on a single line unless you have
something to hide:

.. code-block:: c

	if (condition) do_this;
		do_something_everytime;

Don't use commas to avoid using braces:

.. code-block:: c

	if (condition)
		do_this(), do_that();

Always uses braces for multiple statements:

.. code-block:: c

	if (condition) {
		do_this();
		do_that();
	}

Don't put multiple assignments on a single line either.
Avoid tricky expressions.

Get a decent text editor and don't leave whitespace at the end of lines.


2. Breaking long lines and strings
----------------------------------

Coding style is all about readability and maintainability using commonly
available tools.

The preferred limit on the length of a single line is 80 columns.

Statements longer than 80 columns should be broken into sensible chunks,
unless exceeding 80 columns significantly increases readability and does
not hide information.

Descendants are always substantially shorter than the parent and are
placed substantially to the right. A very commonly used style is to
align descendants to a function open parenthesis.

These same rules are applied to function headers with a long argument list.

However, never break user-visible strings such as printk messages
because that breaks the ability to grep for them.


3. Placing Braces and Spaces
----------------------------

The other issue that always comes up in C styling is the placement of
braces. Unlike the indent size, there are few technical reasons to
choose one placement strategy over the other, but the preferred way, as
shown to us by the prophets Kernighan and Ritchie, is to put the opening
brace last on the line, and put the closing brace first, thusly:

.. code-block:: c

	if (x is true) {
		we do y
	}

This applies to all non-function statement blocks (if, switch, for,
while, do). E.g.:

.. code-block:: c

	switch (action) {
	case KOBJ_ADD:
		return "add";
	case KOBJ_REMOVE:
		return "remove";
	case KOBJ_CHANGE:
		return "change";
	default:
		return NULL;
	}

However, there is one special case, namely functions: they have the
opening brace at the beginning of the next line, thus:

.. code-block:: c

	int function(int x)
	{
		body of function
	}

Heretic people all over the world have claimed that this inconsistency
is ... well ... inconsistent, but all right-thinking people know that
(a) K&R are **right** and (b) K&R are right. Besides, functions are
special anyway (you can't nest them in C).

Note that the closing brace is empty on a line of its own, **except** in
the cases where it is followed by a continuation of the same statement,
ie a ``while`` in a do-statement or an ``else`` in an if-statement, like
this:

.. code-block:: c

	do {
		body of do-loop
	} while (condition);

and

.. code-block:: c

	if (x == y) {
		..
	} else if (x > y) {
		...
	} else {
		....
	}

Rationale: K&R.

Also, note that this brace-placement also minimizes the number of empty
(or almost empty) lines, without any loss of readability. Thus, as the
supply of new-lines on your screen is not a renewable resource (think
25-line terminal screens here), you have more empty lines to put
comments on.

Do not unnecessarily use braces where a single statement will do.

.. code-block:: c

	if (condition)
		action();

and

.. code-block:: none

	if (condition)
		do_this();
	else
		do_that();

This does not apply if only one branch of a conditional statement is a
single statement; in the latter case use braces in both branches:

.. code-block:: c

	if (condition) {
		do_this();
		do_that();
	} else {
		otherwise();
	}

Also, use braces when a loop contains more than a single simple
statement:

.. code-block:: c

	while (condition) {
		if (test)
			do_something();
	}

3.1. Spaces
***********

The style for use of spaces depends (mostly) on function-versus-keyword
usage. Use a space after (most) keywords. The notable exceptions are
sizeof, typeof, alignof, and __attribute__, which look somewhat like
functions (and are usually used with parentheses, although they are not
required in the language, as in: ``sizeof info`` after ``struct fileinfo
info;`` is declared).

So use a space after these keywords::

	if, switch, case, for, do, while

but not with sizeof, typeof, alignof, or __attribute__. E.g.,

.. code-block:: c

	s = sizeof(struct file);

Do not add spaces around (inside) parenthesized expressions. This
example is **bad**:

.. code-block:: c

	s = sizeof( struct file );

When declaring pointer data or a function that returns a pointer type, the
preferred use of ``*`` is adjacent to the data name or function name and not
adjacent to the type name. Examples:

.. code-block:: c

	char *linux_banner;
	unsigned long long memparse(char *ptr, char **retptr);
	char *match_strdup(substring_t *s);

Use one space around (on each side of) most binary and ternary operators,
such as any of these::

	=  +  -  <  >  *  /  %  |  &  ^  <=  >=  ==  !=  ?  :

but no space after unary operators::

	&  *  +  -  ~  !  sizeof  typeof  alignof  __attribute__  defined

no space before the postfix increment & decrement unary operators::

	++  --

no space after the prefix increment & decrement unary operators::

	++  --

and no space around the ``.`` and ``->`` structure member operators.

Do not leave trailing whitespace at the ends of lines. Some editors with
``smart`` indentation will insert whitespace at the beginning of new
lines as appropriate, so you can start typing the next line of code
right away. However, some such editors do not remove the whitespace if
you end up not putting a line of code there, such as if you leave a
blank line. As a result, you end up with lines containing trailing
whitespace.

Git will warn you about patches that introduce trailing whitespace, and
can optionally strip the trailing whitespace for you; however, if
applying a series of patches, this may make later patches in the series
fail by changing their context lines.

4. Commenting
-------------

Comments are good, but there is also a danger of over-commenting. NEVER
try to explain HOW your code works in a comment: it's much better to
write the code so that the **working** is obvious, and it's a waste of
time to explain badly written code.

Generally, you want your comments to tell WHAT your code does, not HOW.
Also, try to avoid putting comments inside a function body: if the
function is so complex that you need to separately comment parts of it,
you should probably go back to chapter 6 for a while. You can make small
comments to note or warn about something particularly clever (or ugly),
but try to avoid excess. Instead, put the comments at the head of the
function, telling people what it does, and possibly WHY it does it.

The preferred style for long (multi-line) comments is:

.. code-block:: c

	/*
	 * This is the preferred style for multi-line
	 * comments. Please use it consistently.
	 *
	 * Description: A column of asterisks on the left side,
	 * with beginning and ending almost-blank lines.
	 */

The preferred style for Doxygen comments is:

.. code-block:: c

	/**
	 * This is the preferred style for Doxygen comments.
	 *
	 * Note that the first line has two asterisks.
	 */

It's also important to comment data, whether they are basic types or derived
types. To this end, use just one data declaration per line (no commas for
multiple data declarations). This leaves you room for a small comment on each
item, explaining its use.

5. Macros, Enums and RTL
------------------------

Names of macros defining constants and labels in enums are capitalized.

.. code-block:: c

	#define CONSTANT 0x12345

Enums are preferred when defining several related constants.

CAPITALIZED macro names are appreciated but macros resembling functions
may be named in lower case.

Generally, inline functions are preferable to macros resembling
functions.

Macros with multiple statements should be enclosed in a do - while
block:

.. code-block:: c

	#define macrofun(a, b, c)			\
		do {					\
			if (a == 5)			\
				do_this(b, c);		\
		} while (0)

Things to avoid when using macros:

1) macros that affect control flow:

.. code-block:: c

	#define FOO(x)					\
		do {					\
			if (blah(x) < 0)		\
				return -EBUGGERED;	\
		} while (0)

is a **very** bad idea. It looks like a function call but exits the
``calling`` function; don't break the internal parsers of those who will
read the code.

2) macros that depend on having a local variable with a magic name:

.. code-block:: c

	#define FOO(val) bar(index, val)

might look like a good thing, but it's confusing as hell when one reads
the code and it's prone to breakage from seemingly innocent changes.

3) macros with arguments that are used as l-values: FOO(x) = y; will
bite you if somebody e.g. turns FOO into an inline function.

4) forgetting about precedence: macros defining constants using
expressions must enclose the expression in parentheses. Beware of
similar issues with macros using parameters.

.. code-block:: c

	#define CONSTANT 0x4000
	#define CONSTEXP (CONSTANT | 3)

5) namespace collisions when defining local variables in macros
resembling functions:

.. code-block:: c

	#define FOO(x)				\
	({					\
		typeof(x) ret;			\
		ret = calc_ret(x);		\
		(ret);				\
	})

ret is a common name for a local variable - __foo_ret is less likely to
collide with an existing variable.

The cpp manual deals with macros exhaustively.

References
----------

The C Programming Language, Second Edition
by Brian W. Kernighan and Dennis M. Ritchie.
Prentice Hall, Inc., 1988.
ISBN 0-13-110362-8 (paperback), 0-13-110370-9 (hardback).

The Practice of Programming
by Brian W. Kernighan and Rob Pike.
Addison-Wesley, Inc., 1999.
ISBN 0-201-61586-X.

GNU manuals - where in compliance with K&R and this text - for cpp, gcc,
gcc internals and indent, all available from https://www.gnu.org/manual/

WG14 is the international standardization working group for the programming
language C, URL: http://www.open-std.org/JTC1/SC22/WG14/

Kernel CodingStyle_, by greg@kroah.com at OLS 2002:
http://www.kroah.com/linux/talks/ols_2002_kernel_codingstyle_talk/html/

.. _CodingStyle:
	https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/process/coding-style.rst
