# INTRODUCTION #
The **wake** command provides several enhancements (let's call it that way...) to the traditional **make** command.

Below is an overview of such enhancements :

- Core : 
	- The concept of "here documents" (such as it can be found in shell or php scripts) has been introduced ; You can, for example, define variables using here documents :

			TARGETS 	=  <<END
						main.c
						utilities.c
						glob.c
			END
	
		Note that here documents are also implemented for rules. The idea is to group rule commands into a single shell script before execution. However, in the current version, this feature is partially functional ; its works for variable definitions, but does not have the intended functionnality for rules, which was to group commands within a shell script before executing them.

	- Multiline comments : they are introduced by the sequence "\#/\*" and terminated by "*/#". Multiline comments can also reside on a single line...
	
	- Variable names are made of alphanumeric characters, plus the following ones :

		<>.[]~-_&+=/%*|^!

	- Regarding the C language, the C preprocessor and the make utility (and others), I always hated the fact that the backslash character, used to introduce continuation lines, would have to be followed by a newline character, and nothing else. I spent too many time rerunning a compilation, because there was a continuation line in which spaces where present between the backslash and the newline ; was it so hard to consider spaces after a backslash as inert characters until a newline is met ? this is why **wake** allows for spaces and tabs between the backslash of the continuation line and the newline character. 

- A bunch of extension functions that provide additional features for :
	- String manipulation
	- List manipulation
	- Arithmetic functions
	- Logical functions
	- ... and various other functions, such as defining an auto-increment counter, generating a GUID, etc.

The following paragraphs describe more precisely the **wake** features.

# SYNTACTIC SUGAR #

This section describe additional syntactic "enhancements" that have been added to the traditional **make** command.

**1. Variable names**

Variable names can contain alphanumeric characters, as well as the following ones :

	<>.[]~-_&+=/%*|^!

**2. Multiline comments**

Multiline comments use the special construct "#/*  */#", such as in the following example :

	#/*
			The dependencies here should be listed in alphabetical order. 
	  */#
	target : a b c

Note that a multiline comment can reside in a single line :

	target : #/* list of targets */# a b c

which will internally expand to :

	target : a b c 

**3. Continuation lines**

Spaces and tabs are authorized after the backslash character that ends a continuation line.

**4. Here documents**

Here documents have the same purpose as the ones that can be found in shell or php scripts.
 
A here document starts with the special construct "<<" (two left angle brackets) at the end of a line, optionnally followed by a keyword ("TARGETEND" in the example below) :

	TARGETS 	=  <<TARGETEND
						value 1
						...
						value n
	TARGETEND

In the above example, the TARGET variable will be set to the contents enclosed between the "<<TARGETEND" and "TARGETEND" keywords.
	
Although spaces are allowed after the "<<" construct, the ending delimiter must start at the very beginning of the line.

Only variable definitions and rule commands can host a here document. The "<<" construct will be ignored in any other place of a makefile.

The ending delimiter "END" is assumed if no keyword is specified after the "<<" construct, as in the following example :

	TARGETS 	=  <<
						value 1
						...
						value n
	END

**4.1 Here documents for variable definitions**

Here documents for variables are intended for those non-US developers (like me) that must type finger-breaking keyboard combinations just to produce a single backslash character.

The following definition :

	MYVAR  			=  <<
		value 1
		value 2 
		...
		value n
	END

is equivalent to :

	MYVAR 			=  value 1 \
					   value 2 \
					   ...
					   value n

**4.2 Here documents for makefile rules**

Rules can contain a here document construct, after the dependencies part, just to tell that all the commands related to the rule should be grouped into an external shell script before execution.

Tab characters before commands are not needed. The first tab character of each line, if present, will be removed.

Example :

	target 	:  main.c sub.c <<
		echo "Building main.c and sub.c"		
	END

Two ideas lay behind this implementation :

- The first one is to improve global command execution performance, in situations where it is not needed to rely on **make** to check each command exit code.
- The second one is to allow alternative script interpreters, such as in the following example :

		target 	:  main.c sub.c <<
		#!/usr/bin/php
		echo "Building main.c and sub.c; dependencies to be rebuilt :\n"		

		# $< will give depencencies to be rebuilt as plain strings ; we must enclose them
		# with quotes so that php will not complain, hence the call to the $(<list.quote>) function
		foreach  ( $i  in  $(<list.quote> $<)
			echo "- $i\n"
	END

As of version 1.0.0, separate shell scripts are not yet implemented, since it will require to add a function equivalent to **construct\_command\_argv()** and **construct\_command\_argv\_internal()** for putting commands into a separate shell script before executing them. As common software sense says : "Next release !"
 
# NEW AUTOMATIC VARIABLES

The **wake** utility implements tenths of automatic variables in various domains :

- String manipulations
- List manipulations
- Math operations
- Logical operations
- Misc operations

In order not to pollute the existing makefiles name space, automatic variable names are enclosed within angle brackets, such as in the following example, which expands to the first element of the specified list ("*item*") :

	FIRST 	=  $(<list.first> item1 item2 item3)

Keep in mind that quite all the functions *expand* to something. There is no notion of return value here, except the *expanded* result.

All numeric arguments are considered as doubles and converted to integers whenever needed. Numeric values can be specified in hex or octal notation such as in : 0xFF00AA89 or 0o377.

The following paragraphs give a reference about the automatic variables provided by **wake**.


## Date functions ##

### $(< date> [ *format* [, *time*] ] ), $(< gmdate> [ *format* [, *time*] ] ) ###

Formats a date using the specified *format* string. If *time* is unspecified, the current Unix time will be used.

The format specifiers are compatible with the PHP date() function. They are listed below :

- a : lowercase "am" or "pm"
- A : uppercase "AM" or "PM"
- B : Internet Swatch hour
- c : Full date, in ISO8601 format (2015-01-01T23:10:00+01:00)
- d : Day of month, as a two-digits number
- D : Short day name (Mon, Tue, etc.) 
- e : Timezone name.
- F : Long month name (January, February, etc.)
- g : AM/PM hour, without the leading zero (1-12)
- G : Hour, without the leading zero (0-23)
- h : AM/PM hour, with the leading zero (01-12)
- H : Hour, with the leading zero (01-23)
- i : Minute, with the leading zero (00-59)
- I : 1 if daylight savings time is active, 0 otherwise
- j : Day of month, without the leading zero (1-31)
- l : Long day name (Monday, Tuesday, etc.)
- L : 1 is the year is leap, 0 otherwise
- M : Short month name (Jan, Feb, etc.)
- m : Month of year, with the leading zero (01-12)
- n : Month of year, without the leading zero (1-12)
- N : Day of week, from 1 (monday) to 7 (sunday)
- o : ISO8601 year
- O : GMT offset, in ISO8601 format (eg : +0200)
- P : GMT offset, in RFC822 format (eg : +02:00)
- r : RFC822 date (Thu, 21 Dec 2000 16:01:07 +0200)
- s : Seconds, with the leading zero (00-59)
- S : 2-letters english suffix for day of month (st, nd, rd, th...)
- t : Month length
- T : Timezone abbreviation
- U : Unix time
- w : Day of week, from 0 (sunday) to 6 (saturday)
- W : ISO8601 week number (where weeks start on monday)
- y : 2-digits year
- Y : 4-digits year
- z : Day of year (0-365)
- Z : GMT offset in seconds  

The default format specifier is : "Y-m-d H:i:s".

Examples :

	# Assuming that current time is : 2015-01-01 17:30:00 ...
 
	VAR1 	=  $(<date>)
	# VAR1 will expand to : 2015-01-01 17:30:00

	VAR2 	=  $(<date> Y-m-d)
	# VAR2 will expand to : 2015-01-01

	VAR3	=  $(<date> Y-m-d H:i:s,1234567890)
	# VAR3 will expand to : 2009-02-14 00:31:30

### $(< now> ) ###

Returns the number of seconds elapsed since January 1st, 1970.

## File functions ##

### $(< tempfile> [ *directory* [, *prefix* [, *suffix*] ) ###

Expands to a new, unique temporary file name.

If no *directory* is specified, the generated path will use the system temp directory (/tmp on Linux systems).

An optional *prefix* and *suffix* can be specified.

A new filename is generated upon each invocation, so if you wish to reuse the same variable several times, use the ":=" assignment operator instead of "=" :

	VAR	:=  $(<tempfile>)
	# $(VAR) can be called several times, its contents will never be reevaluated.

Examples :

	VAR1	:= 	$(<tempfile>)
	# VAR1 expands to something like : /tmp/22e0f0.1

	VAR2	:=  $(<tempfile> .,prefix.,.txt)
	# VAR2 expands to something like : ./prefix.22e0f0.1.txt

	VAR3 	:=  $(<tempfile>,prefix.,.txt)
	# VAR3 expands to something like : /tmp/prefix.22e0f0.1.txt
	 
### $(< filecontents> *file*) ###

Expands to the contents of the specified file.

### $(<stat.mode> *file*) ###

Expands to the access mode of the specified file.

### $(<stat.ctime> *file*) ###

Expands to the creation time of the specified file.

### $(<stat.mtime> *file*) ###

Expands to the last modification time of the specified file.

### $(<stat.atime> *file*) ###

Expands to the last access time of the specified file.

### $(<stat.size> *file*) ###

Expands to the size of the specified file.

### $(<stat.uid> *file*) ###

Expands to the user id of the owner of the specified file.

### $(<stat.gid> *file*) ###

Expands to the group id of the owner of the specified file.


## Internal functions ##

Internal functions allow to query for builtin functions. No so useful but helps for debugging...

### $(< builtins> [*category* [, *sort*]) ###

Expands to the list of builtin functions.

If *category* is specified, then only the functions belonging to this category will be listed. Currently, only two categories are defined :

- builtins : the original GNU Make builtin functions
- wake : **wake** extensions

Results are sorted if the *sort* parameter is non-empty.

### $(< isbuiltin> *funcname*) ###

Expands to "1" if "funcname" is a builtin function, or to the empty string otherwise.

## List functions ##

The list functions operate on contents that are to be seen as space-separated strings.

### $(<list.caseeq> *list1*,*list2*) ###

Compares two lists. Expands to 1 if they are equal, or to the empty string if not.
Comparison is case-insensitive.

### $(<list.casene> *list1*,*list2*) ###

Compares two lists. Expands to 1 if they are not equal, or to the empty string otherwise.
Comparison is case-insensitive.

### $(<list.chop> *list*) ###

Expands to *list*, minus its last item.

	VAR1 		=  $(<list.chop> a b c)
	# Expands to "a b" 

### $(<list.eq> *list1*,*list2*) ###

Compares two lists. Expands to 1 if they are equal, or to the empty string if not.
Comparison is case-sensitive.

### $(<list.first> *list*) ###

Expands to the first item of *list*.
 
Examples :

	VAR1		=  $(<list.first> a b c)
	# VAR1 is now equal to "a"
	
	LIST 		=  x y z 
	VAR2 		=  $(<list.first> $(LIST))
	# VAR2 is now equal to "x"

### $(<list.last> *list*) ###

Expands to the last item of *list*.
 
Examples :

	VAR1		=  $(<list.last> a b c)
	# VAR1 is now equal to "c"
	
	LIST 		=  x y z 
	VAR2 		=  $(<list.last> $(LIST))
	# VAR2 is now equal to "z"

### $(<list.length> *list*) ###

Expands to the number of items in *list*.

Examples :

	VAR1		=  $(<list.length>)
	# VAR1 expands to 0

	VAR2 		)  $(<list.length> a b c)
	# VAR2 expands to 3

### $(<list.map> *function*, *list*)

Applies a function to each item of the specified list. The result is a list that contains the result of the mapping function applied to each source item.

Within the function, $(1) refers to the current list item.

Examples :

	LISTMAPPER 	=  X$(1)Z
	MAP			=  $(<list.map> LISTMAPPER, a b c)
	# MAP will expand to : XaZ XbZ XcZ

### $(<list.ne> *list1*,*list2*) ###

Compares two lists. Expands to 1 if they are not equal, or to the empty string otherwise.
Comparison is case-sensitive.

### $(<list.pairmap> *function*, *list*)

Applies a function to each word of the 'list1' and 'list2'. The result is the catenation of applying *func(x,y)*, where *x* comes from *list1* and *y* from *list2.*

Within the function, $(1) refers to the current list item of *list1*, and $(2) to the current list item of *list2*.

The two lists do not need to have the same number of elements. An empty string will be supplied instead for each list item not present in the other list.

Examples :

	PAIRFUNCTION 	=  $(1)/$(2)	
	VAR 			=  $(<list.pairmap> PAIRFUNCTION, a b c, d e f)
	# VAR will expand to : a/d b/e c/f

### $(<list.quote> *value* [, *delim* [, *right_delim* [, *escape* ] ] ]) ###

Quotes list items.

*value* is the list whose items are to be quoted. Note that no quoting will occur on an empty list.

*delim* is the delimiter to be used for quoting individual list items. The default is a double quote character. 

*right_delim*, if specified, is the right delimiter to be used, if it differs from *delim*.

*escape*, if specified, is the escape character to be used whenever a character equal to *delim* or *right_delim* is encountered in *value*.

Examples :

	VAR1		=  $(<list.quote> a b c)
	# Expands to : "a" "b" "c"

	VAR2 		=  $(<list.quote> a b c,[,])
	# Expands to : [a] [b] [c]

	VAR3 		=  $(<list.quote> a[ b c],[,],\\)
	# Expands to : [a\[] [b] [c\]]

### $(<list.range> *start*,*stop*,*increment*)

Generates a list starting from *start* up to *stop*, by *step* increments.

Examples :

	VAR1 	=  $(<list.range> 1, 5, 1)
	# VAR1 will expand to : 1 2 3 4 5

	VAR2 	=  $(<list.range> 1, 2, 0.3)
	# VAR2 will expand to : 1 1.3 1.6 1.9

### $(<list.rest> *list*) ###

Expands to the tail of the list.

Examples :

	VAR1 		=  $(<list.rest> a b c)
	# VAR1 expands to "b c"

### $(<list.reverse> *list*) ###

Reverses a list.

Examples :

	VAR 		=  $(<list.reverse> a b c d)
	# VAR will expand to : d c b a

## Logical functions ##

Logical functions allows to evaluate an expression based on a condition :

- **$(< if> cond, value1 [, value2 ])**
- **$(< ifnot> cond, value1 [, value2 ])**
- **$(< if/d> cond, value1 [, value2 ])**
- **$(< ifnot/d> cond, value1 [, value2 ])**

- **$(< iflt> cond1, cond2, value1 [, value2 ])**
- **$(< ifle> cond1, cond2, value1 [, value2 ])**
- **$(< ifgt> cond1, cond2, value1 [, value2 ])**
- **$(< ifge> cond1, cond2, value1 [, value2 ])**
- **$(< ifeq> cond1, cond2, value1 [, value2 ])**
- **$(< ifne> cond1, cond2, value1 [, value2 ])**

- **$(< iflt/i> cond1, cond2, value1 [, value2 ])**
- **$(< ifle/i> cond1, cond2, value1 [, value2 ])**
- **$(< ifgt/i> cond1, cond2, value1 [, value2 ])**
- **$(< ifge/i> cond1, cond2, value1 [, value2 ])**
- **$(< ifeq/i> cond1, cond2, value1 [, value2 ])**
- **$(< ifne/i> cond1, cond2, value1 [, value2 ])**

- **$(< iflt/d> cond1, cond2, value1 [, value2 ])**
- **$(< ifle/d> cond1, cond2, value1 [, value2 ])**
- **$(< ifgt/d> cond1, cond2, value1 [, value2 ])**
- **$(< ifge/d> cond1, cond2, value1 [, value2 ])**
- **$(< ifeq/d> cond1, cond2, value1 [, value2 ])**
- **$(< ifne/d> cond1, cond2, value1 [, value2 ])**

$(< if>) expands to *value1* if *cond* is not empty, or to *value2* otherwise. 

$(< ifnot>) expands to *value1* if *cond* is empty, or to *value2* otherwise.

The $(< if*xx*>) functions expand to *value1* if the condition is satisfied, or to *value2* otherwise.

Functions whose names are ending with **/i** perform a case-insensitive comparison, while the functions whose names are ending with **/d** consider their test expression as numeric values.
 
## Math functions ##

The math functions allow to perform basic mathematical operations.

### $(= expression) ###

Evaluates the specified infix expression. This feature uses my expression evaluator package ([https://github.com/christian-vigh/eval](https://github.com/christian-vigh/eval "Expression evaluator package")).

You will have a comprehensive documentation on expression syntax [here](https://github.com/christian-vigh/eval/blob/master/README.md "View the expression syntax help file").

Of course, this feature make all the other math functions implemented in *wake* kind of useless 


### $(<+> *value1*, *value2* [,.., *valuen*]) ###

Computes the addition of two or more values.

Examples :

	VAR 	=  $(<+> 2, 3)
	# VAR expands to : 5

### $(<-> *value1*, *value2* [,.., *valuen*]) ###

Computes the subtraction of two or more values.

Examples :

	VAR 	=  $(<-> 10, 3)
	# VAR expands to : 7

### $(<*> *value1*, *value2* [,.., *valuen*]) ###

Computes the multiplication of two or more values.

Examples :

	VAR 	=  $(<*> 2, 3)
	# VAR expands to : 6

### $(</> *value1*, *value2* [,.., *valuen*]) ###

Computes the division of two or more values.

Examples :

	VAR 	=  $(</> 5,2)
	# VAR expands to : 2.5

### $(<//> *value1*, *value2* [,.., *valuen*]) ###

Computes the integer division of two or more values.

Examples :

	VAR 	=  $(<//> 5,2)
	# VAR expands to : 2

### $(<%> *value1*, *value2*) ###

Computes the modulo of two values.

Examples :

	VAR 	=  $(<%> 5,2)
	# VAR expands to : 1

### $(<**> *value1*, *value2*) ###

Computes *value1* raised to the power of *value2*.

Examples :

	VAR 	=  $(<%> 2,3)
	# VAR expands to : 8

### $(<&> *value1*, *value2* [,.., *valuen*]) ###

Computes the bitwise AND of two or more values.

Examples :

	VAR 	=  $(<&> 7,2)
	# VAR expands to : 2

### $(<|> *value1*, *value2* [,.., *valuen*]) ###

Computes the bitwise OR of two or more values.

Examples :

	VAR 	=  $(<|> 4,2)
	# VAR expands to : 6

### $(<^> *value1*, *value2* [,.., *valuen*]) ###

Computes the bitwise XOR of two or more values.

Examples :

	VAR 	=  $(<^> 6,2)
	# VAR expands to : 4

### $(<~> *value1*) ###

Computes the bitwise NOT of the specified value.

Examples :

	VAR 	=  $(<~> 0)
	# VAR expands to : 0xFFFFFFFF

### $(<< *value1*, *value2*) ###

Shifts *value1* left by *value2* bits.

Examples :

	VAR 	=  $(<< 3,1)
	# VAR expands to : 6

### $(>> *value1*, *value2*) ###

Shifts *value1* right by *value2* bits.

Examples :

	VAR 	=  $(>> 6,1)
	# VAR expands to : 3

### $(<++> *value1*) ###

Increments the specified value by 1.

Examples :

	VAR 	=  $(<++> 2)
	# VAR expands to : 3

### $(<--> *value1*) ###

Decrements the specified value by 1.

Examples :

	VAR 	=  $(<--> 2)
	# VAR expands to : 1

## Message functions ##

Message functions are used to display messages each time they are expanded.

### $(< error> text) ###

Displays the specified message then exits. Not really useful, since the **error** directive already exists.

### $(< log> text) ###

Outputs the specified text to the console and logs it into a log file.

The default log file name is *Makefile.log*, but it can be overridden by defining the < LOGFILE> variable :

	<LOGFILE> 	=  mylog.txt 

### $(< print> text) ###

Outputs the specified message upon each expansion.

### $(< warning> text) ###

Displays the specified message. Not really useful, since the **warning** directive already exists.





## String functions ##
 
The string functions operate on string and perform various manipulations.

### String comparisons ###

- **$(<string.eq> value1,value2)**
- **$(<string.ne> value1,value2)**
- **$(<string.lt> value1,value2)**
- **$(<string.le> value1,value2)**
- **$(<string.gt> value1,value2)**
- **$(<string.ge> value1,value2)**
- **$(<string.caseeq> value1,value2)**
- **$(<string.casene> value1,value2)**
- **$(<string.caselt> value1,value2)**
- **$(<string.casele> value1,value2)**
- **$(<string.casegt> value1,value2)**
- **$(<string.casege> value1,value2)**

Compares two string for equality (.eq), unequality (.ne), less than (.lt), less than or equal (.le), greater than (.gt) or greater than or equal (.ge). Expands to 1 if the comparison is true or to the empty string otherwise.
Comparisons are case-sensitive.

The *<string.casexx>* functions are case-insensitive versions.

Examples :

 	VAR1	=  $(<string.eq> abc,abc)
	# VAR1 will expand to 1

	VAR2 	=  $(<string.eq> ABC,abc)
	# VAR2 will expand to nothing (use $(<string.caseeq>) for case-insensitive comparisons)

### $(<string.casecompare> *value1*,*value2*) ###

Compares *value1* with *value2* ; returns 1 if *value1* is greater than *value2*, -1 if it is less, and 0 if both strings are equal.
Comparisons is case-insensitive.

Examples :

	VAR1		=  $(<string.compare> abc,abc)
	# VAR1 will expand to 0.

	VAR2 		=  $(<string.compare> ABCD,abc)
	# VAR2 will expand to 1.

	VAR3 		=  $(<string.compare> abc,xyz)
	# VAR3 will expand to -1.

	VAR4 		=  $(<string.compare> ABC,abc)
	# VAR4 will expand to 0. 

### $(<string.casesearch> *string1*,*string2*) ###

Searches the substring 'string2' within 'string1'. Expands to its position if the searched string has been found, or to the empty string if no match.

String indexes start from zero.

Search is case-insensitive.

Examples :
	
	VAR1	=  $(<string.search> abcdef,bcd)
	# VAR1 expands to 1

	VAR2	=  $(<string.search> abcdef,BCD)
	# VAR2 expands to 1 (search is case-insensitive)

	VAR3 	=  $(<string.search> abc def, )
	# VAR3 expands to 3 (the searched string is a space)

### $(<string.chr> *integer*)

Expands to the ascii value of the specified integer value.

Examples :

	VAR1 	=  $(<string.chr> 65)
	# VAR1 expands to : A

	VAR2 	=  $(<string.chr> 10)
	# VAR2 expands to the newline character

### $(<string.compare> *value1*,*value2*) ###

Compares *value1* with *value2* ; returns 1 if *value1* is greater than *value2*, -1 if it is less, and 0 if both strings are equal.
Comparisons is case-sensitive.

Examples :

	VAR1		=  $(<string.compare> abc,abc)
	# VAR1 will expand to 0.

	VAR2 		=  $(<string.compare> abcd,abc)
	# VAR2 will expand to 1.

	VAR3 		=  $(<string.compare> abc,xyz)
	# VAR3 will expand to -1.

	VAR4 		=  $(<string.compare> ABC,abc)
	# VAR4 will expand to -1. 

### $(<string.explode> *separator*,*string*) ###

Explodes a string using the specified separator.

Examples :
	
	VAR 		=  $(<string.explode> /, a/b/c/d)
	# VAR expands to : a b c d

### $(<string.implode> *separator*,*list*) ###

Catenates all the elements in *list* using the specified *separator*.

List items are separated by spaces.

Example :

	VAR 	=  $(<string.implode> /,a b c d)
	# VAR expands to a/b/c/d

### $(<string.length> *value*) ###

Expands to the length of the specified string value, or to the empty string if the supplied value is empty.

Examples :

	VAR1		=  $(<string.length> abcde)
	# VAR1 expands to 5

	VAR2 		=  $(<string.length> )
	# VAR2 expands to the empty string

### $(<string.quote> *value* [, *delim* [, *right_delim* [, *escape* ] ] ]) ###

Quotes the specified *value*. The default delimiter is the double quote character ("). 

If the string contains the delimiter, it will be escaped using the specified *escape* character.

If a right delimiter is specified, it will be used as the terminating quoting character, *delim* being used as the starting quoting character.

Examples :

	VAR1 		=  $(<string.quote> abc)
	# VAR1 expands to : "abc"

	VAR2 		=  $(<string.quote> abc,[,])
	# VAR2 expands to : [abc]

	VAR3 		=  $(<string.quote> [abc],[,])
	# VAR3 expands to : [\[abc\]]

	VAR4 		=  $(<string.quote> [abc],[,],!)
	# VAR4 expands to : [![abc!]]
	

### $(<string.repeat> *string*,*count*) ###

Repeats a string.

Examples :

	VAR 	=  $(<string.repeat> abc,3)
	# VAR expands to : abcabcabc

### $(<string.reverse> *string*) ###

Expands to the reversed value of the specified string.

Examples :
	
	VAR 	=  $(<string.reverse> abcd)
	# VAR expands to dcba

### $(<string.search> *string1*,*string2*) ###

Searches the substring 'string2' within 'string1'. Expands to its position if the searched string has been found, or to the empty string if no match.

String indexes start from zero.

Search is case-sensitive.

Examples :
	
	VAR1	=  $(<string.search> abcdef,bcd)
	# VAR1 expands to 1

	VAR2	=  $(<string.search> abcdef,BCD)
	# VAR2 expands to the empty string (search is case-sensitive)

	VAR3 	=  $(<string.search> abc def, )
	# VAR3 expands to 3 (the searched string is a space)

### $(<string.sprintf> *format* [, *arg...*]) ###

An equivalent to the C sprintf() function. Recognized format specifiers are : diouxXfeEgGcs.

Examples :

	VAR1		=  $(<string.sprintf> %08X, 123)
	# VAR1 expands to : 0000007B

	VAR2 		=  $(<string.sprintf [%3s],A)
	# VAR2 expands to : [A  ]

### $(<string.substring> string, start [, length]) ###

Returns a substring of *string*.

*start* is the zero-based offset of the first character of the substring to be extracted.
If specified, *length* is the number of characters to extract from *string* ; otherwise, all the characters starting from *start* will be extracted.

Examples :

	VAR1 	=  $(<string.substring> abcdef,2)
	# VAR1 expands to cdef

	VAR2 	=  $(<string.substring> abcdef,999)
	# VAR2 expands to the empty string

	VAR3 	=  $(<string.substring> abcdef,2,2)
	# VAR3 expands to : cd

### $(<string.tolower> *string*) ###

Converts the specified string to lowercase.

Examples :

	VAR 	=  $(<string.tolower> ABC)
	# VAR expands to abc

### $(<string.toupper> *string*) ###

Converts the specified string to uppercase.

Examples :

	VAR 	=  $(<string.toupper> abc)
	# VAR expands to ABC

## Miscellaneous functions ##

### $(< counter> *id*) ###

Retrieves the next value of the specified counter, *id*.

If *id* does not yet exist, it will be created with an initial value of 1 and an increment of one.

The counter value is expanded each time it is invoked.

### $(< defcounter> *id* [, *start* [, *increment* ] ]) ###

Creates a counter variable with the name *id*.

A counter variable has its value incremented each time it is expanded.

the *start* parameter gives the starting value for the counter. The default is 1.

The *increment* parameter gives the increment value to be added to the counter value each time it is invoked. The default value is 1.

The *$(<counter> id)* function is used to retrieve the next value of a counter.

Examples :

	$(<defcounter> MYCOUNTER2, 100, 15)
	MYCOUNTER = $(<counter> MYCOUNTER2)

	counter.define	:
		@echo "counter(MYCOUNTER) [1]                  : $(MYCOUNTER)"
		# Will display : 100
		@echo "counter(MYCOUNTER) [2]                  : $(MYCOUNTER)"
		# Will display : 115
		@echo "counter(MYCOUNTER) [3]                  : $(MYCOUNTER)"
		# Will display : 130

### $(< guid>) ###

Generates a GUID.

Examples :

	VAR 	=  $(<guid>)
	# VAR expands to something like : {97C57573-48AA-4377-935D-7A83598CCF22}

### $(< random> [*low* [, *high*]) ###

Generates a random number between *low* and *high*. If none specified, the returned value will be between 0 and RAND_MAX.

### $(< randomize>) ###

Randomizes the seed of the pseudo-random number generator. This function should be called in every Makefile using the $(< random>) function, otherwise consecutive runs will produce the same values.

Examples :

	$(<randomize>)
 
### $(< rawguid>) ###

Generates a "raw" GUID, ie a GUID without curly braces and dashes.

Examples :

	VAR 	=  $(<rawguid>)
	# VAR expands to something like : 97C5757348AA4377935D7A83598CCF22

### $(< undefcounter> *id*) ###

Deletes the specified counter. No that useful but...

# COMMAND-LINE OPTIONS #

- **--stdmake**

Run as the normal make command (disable wake extensions). Note that this behavior is strictly experimental and used only for debugging purposes.

- **--builtins[=category]**

Displays a list of available builtin functions.

- **--builtin-categories**

Displays a list of available builtin categories.

- **--builtin-help[=name]**

Displays help text for the specified builtin function name, or for all functions. Note that **wake** also includes help text for GNU make builtins.

- **--builtin-signatures[=category]**

Displays a list of available builtin functions, with their signature.

- **--builtin-usage[=name]**

Displays a usage text for the specified builtin function name, or for all functions. Note that **wake** also includes usage text for GNU make builtins.

# IMPLEMENTATION #

**1.1 Why implementing here documents ?**

Apart from providing syntactical sugar (for variable definitions) or trying to improve performance by grouping commands into a shell script then executing it (for makefile rules), the idea was to provide some kind of lookahead system for **make**, at a point where the current context is not known (is it a variable definition ? a rule ? or else ?).

Two functions from the original **make** command are contributing to that :

- **readline()**, which reads the next line from a makefile. In fact, it collects lines and group them together as long as there are lines ending with the continuation character, backslash. readline() does not have any notion of the context in which **make** is currently evolving.
- **eval()**, which interprets the contents of a makefile put in a memory buffer.

	Implementing lookahead features such as the one required for here documents required a slightly acrobatic exercise :

- The **readline()** function, not knowing the context of the current line that is being read, can however recognize a here document construct ; in that case, it will collect all the lines between the starting "<<" and the ending keyword, such as in :


		target		:  <<END
			command1
			...
			commandn
		END

	The returned value will thus be the above contents. However, the **eval()** function expects only the rule part of the contents ("target :"), not the shell commands following the rule, since they will be read after **eval()** recognized the start of the rule.

- This is where the **eval()** function has been modified : if **readline()** returns the information that a here document has been found, then only the rule part will be retained, while individual content lines will be buffered. On the next loop iteration, instead of calling **readline()**, the next line that has been buffered will be returned and that, until the last buffered line has been exhausted.

# IMPLEMENTING NEW FUNCTIONS #

Implementing new functions require to follow the steps detailed below :

## Adding a new source file ##

New source files should be added into the **wake/lib/builtins** directory. You can name the files as you want but keep in mind that compiled object files are put in the **wake** root directory ; so consider that there may be a collision with a legacy object file (for example, don't name your source file "main.c", since it already exists in the root directory).

In a future version, object files will be put into their source directory.

For the moment, files implementing new functions are named **builtins-xxx.c**.

## Modifying the Makefile.am file ##

Makefile.am is the key file for adding new sources to the project ; it contains the following variables :

### WAKEDEFINITIONS ###

Contains the macro definitions to be used during compilation :
 
	WAKEDEFINITIONS		=  -DWAKE_EXTENSIONS -DEXPERIMENTAL -DHAVE_DOS_PATHS

Note that the **WAKE_EXTENSIONS** macro must be defined for **wake** extensions to be compiled.

### WAKEDIR ###

Subdirectory name of the **wake** extensions. You should not have to modify this variable.

### WAKEINCLUDEDIR ###

Include directory for **wake** extensions. You should not have to modify this variable.

### WAKELIBDIR ###

Directory containing the source files for the **wake** extensions. You should not have to modify this variable. 

### WAKESOURCES ###

List of source files to be included for the compilation of the **wake** extensions.

If you add a new source file, then you will need to add its path into this variable.

### WAKEINCLUDES ###

List of include files needed by the **wake** extensions.

### WAKELIBS ###

External libraries to be added during the linking process.
 
## DECLARE YOUR NEW FUNCTION ##

To declare your new function, just use the WAKE_BUILTIN macro and add it into the **wake/include/wake.h** include file :

	WAKE_BUILTIN ( wake_builtin_myextensions_foobar ) ;

Note that there is no constraint on the name of the function you wish to implement.

##  DESCRIBE YOUR NEW FUNCTION ##

Your new function must be described by adding a new entry into the **wake\_function\_table** array defined in the **wake/include/builtins.h** include file ; you can use the **FT_ENTRY** macro for that :
 
	FT_ENTRY
	   (
		"foobar",
		*minargs*, *maxargs*,
		*expands*,
		wake_builtin_myextensions_foobar,
		FUNC_HELP
		   (
			N_ ( "myextensions" ),
			N_ ( "" ),
			N_ ( "My foobar function." ),
			N_ ( "Complete description of my foobar function" )
		    )
	    )

The first argument (*"foobar"*) is the function name. 

*minargs* and *maxargs* are the minimum and maximum number of arguments authorized when invoking this function.

*expands* will be 1 most of the time if your function expands to something. Specify 0 if no expansion is required, such as for the $(< randomize>) function.

The next argument is the function entry point within the source (*wake\_builtin\_myextensions\_foobar*).

The **FUNC_HELP** macro is an addition to the traditional GNU Make command and allows for specifying :

- The extension group name to which this function belong (initially, only the *builtins* and *wake* groups are defined)
- The call syntax ; it is a free form text displayed when help is invoked on the command line for the specified function
- A short usage text
- A help text

## IMPLEMENT YOUR NEW FUNCTION ##

- Either create a new file in the **wake/lib/builtins** directory, or use one of the existing *builtins-\*.c* ones.
- Declare your new function :

		WAKE\_BUILTIN ( wake_builtin\_myextensions\_foobar )
       	   {

            }
- Three parameters are provided to you :
	- **output** : A pointer to an output buffer where the expanded contents of your function are to be put. 
	- **argv**, a array of pointer to strings that holds the arguments passed to your function
	- **funcname**, the name of the called function. This trick (which is already present in GNU make), allows you to specify the same entry point for several functions. This is the case for example of the **wake\_builtin\_math\_operation**, which handles several math functions : $(<+>), $(<->), etc.

The last element of the **argv** array is always NULL. If you want to get the number of arguments defined, you can call the utility function **wake\_function\_argc()**, passing the **argv** array.

If your function expands to something else, you must call the GNU Make **variable\_buffer\_output()** function to catenate your new content to the existing one, as in the following example :

	output 	= variable_buffer_output ( output, "Hello world !", sizeof ( "Hello world !" ) - 1 ) ;

The first parameter is a pointer to the original **output** buffer. The second one is the string to be copied, and the third one the number of characters to be copied.

Note that the function will reallocate the **output** buffer if not enough space is available. Keep in mind that it is mandatory to preserve its return value.

Finally, the function MUST return the current value of **output** ; if you forget that, then unexpected results will occur, one of them being a core dump...

## AN EXAMPLE FUNCTION ##

The following paragraphs show how to implement a function called $(foobar), that prints "Hello world !" followed by its optional argument.

### DECLARE YOUR FUNCTION IN WAKE.H ###

Add the following declaration to the **wake.h** file (note that the only purpose of the *myextension* prefix is to ensure that you do not pollute existing namespace) :

	WAKE_EXTENSION ( myextension\_foobar ) ;

### DESCRIBE YOU FUNCTION IN BUILTINS.H ###

Add the following entry in the **wake\_function\_table** array :

	FT_ENTRY
	   (
		"foobar",
		0, 1,
		1,
		myextension_foobar,
		FUNC_HELP
		   (
			N_ ( "myextensions" ),
			N_ ( "[text]" ),
			N_ ( "Expands to the string \"Hello world !\" followed by an optional message." ),
			N_ ( "Expands to the string \"Hello world !\" followed by an optional message." 
		 	     "A newline is inserted before the message if it has been specified." )
		    )
	    )

### IMPLEMENT YOUR FUNCTION IN WAKE/LIB/BUILTINS/BUILTIN-xxx.c ###

(or whatever name you like).

	WAKE_EXTENSION ( myextension_foobar )
	   {
			int  		argc 		=  wake_function_argc ( argv ) ;	// Retrieve argument count, which is not passed to the function

			// Add the string "Hello world !" to the expansion result 
			output 	=  variable_buffer_output ( output, "Hello world !", sizeof ( "Hello world !" ) - 1 ) ;

			// If an argument has been specified, add a newline and the contents of the
			// argument to the expansion result
			if  ( argc )
			   {
					output 	=  variable_buffer_output ( output, & '\n', 1 ) ;
					output  =  variable_buffer_output ( output, argv [0], strlen ( argv [0] ) ) ;
		     	}

			// Never forget to return the pointer after the expanded result
			return ( output ) ;
  	    }

### DECLARE YOUR SOURCE FILE ###

As described in section **Modifying the Makefile.am file**, you should add in the Makefile.am file any C source or include file needed for your extension to work.