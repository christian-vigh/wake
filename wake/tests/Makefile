#
# Test makefile for wake
#

# Main rule for all tests
test		:  				\
			builtins 		\
			isbuiltin		\
			list.first		\
			list.last 		\
			list.length 		\
			list.rest		\
			list.map		\
			list.pairmap		\
			list.reverse		\
			list.eq 		\
			list.ne			\
			list.caseeq		\
			list.casene		\
			list.range		\
			list.quote		\
			string.length		\
			string.compare		\
			string.casecompare	\
			string.eq		\
			string.caseeq		\
			string.ne		\
			string.casene		\
			string.lt		\
			string.caselt		\
			string.gt		\
			string.casegt		\
			string.le		\
			string.casele		\
			string.ge		\
			string.casege		\
			string.search 		\
			string.casesearch	\
			string.reverse		\
			string.tolower		\
			string.toupper 		\
			string.implode		\
			string.explode		\
			string.substring	\
			string.sprintf		\
			string.chr		\
			string.repeat		\
			string.quote		\
			logical			\
			random			\
			math.plus		\
			math.minus		\
			math.mul		\
			math.div		\
			math.idiv		\
			math.mod		\
			math.pow		\
			math.incr		\
			math.decr		\
			math.and		\
			math.or			\
			math.xor		\
			math.not		\
			math.shl		\
			math.shr		\
			date			\
			guid			\
			counter			\
			file			\
			print 			\
			warning 		\
			log			\
			error

# Tests the <builtins> function
builtins 	:
	@echo "*************  Builtin functions :"
	@echo "$(<builtins>)" | sed 's/^/\t/' | sed 's/ /\n\t/g'
	@echo "*************  Builtin functions (extensions only) :"
	@echo "$(<builtins> wake)" | sed 's/^/\t/' | sed 's/ /\n\t/g'

# Tests the <isbuiltin> function
isbuiltin 	:
	@echo "*************  Isbuiltin(words) = $(<isbuiltin> words)"
	@echo "*************  Isbuiltin(<builtins>) = $(<isbuiltin> <builtins>)"
	@echo "*************  IsBuiltin(junk) = $(<isbuiltin> junk)"
	
# <error>
error 		:
	@echo "$(<error> This is a generated error message)"
	
# <warning>
warning 	:
	@echo "$(<warning> This is a generated warning message)"
	
# <print>
print 		:
	@echo "$(<print> This is a generated printed message)"
	
# <log>
<LOGFILE>	=	log.txt
log 		:
	@echo "$(<log> This is a generated log message in file $(<LOGFILE>))"
	
# <list.first>
list.first 	: 
	@echo "First item of empty list               : [$(<list.first>)]"
	@echo "First item of empty list (with spaces) : [$(<list.first> )]"
	@echo "First item of list [aa b c]            : [$(<list.first> aa b c)]"
	
# <list.last>
list.last 	: 
	@echo "Last item of empty list               : [$(<list.last>)]"
	@echo "Last item of empty list (with spaces) : [$(<list.last> )]"
	@echo "Last item of list [aa b c]            : [$(<list.last> aa b c)]"

# <list.length>
list.length 	:	
	@echo "Length of empty list                  : [$(<list.length>)]"
	@echo "Length of empty list (with spaces)    : [$(<list.length> )]"
	@echo "Length of list [aa b c]               : [$(<list.length> aa b c)]"

# <list.rest>
list.rest 	:	
	@echo "Rest of empty list                    : [$(<list.rest>)]"
	@echo "Rest of empty list (with spaces)      : [$(<list.rest>)]"
	@echo "Rest of list [aa b c]                 : [$(<list.rest> aa b c)]"

# <list.chop>
list.chop 	:	
	@echo "Chop of empty list                    : [$(<list.chop>)]"
	@echo "Chop of empty list (with spaces)      : [$(<list.chop>)]"
	@echo "Chop of list [aa b c]                 : [$(<list.chop> aa b c)]"

# <list.map>
LISTMAPPER 	=  XX$(1)ZZ
list.map 	:
	@echo "List map of [a b c]                   : [$(<list.map> LISTMAPPER, a b c)]"
	@echo "List map of empty list                : [$(<list.map> LISTMAPPER)]"
	@echo "List map of empty list (with spaces)  : [$(<list.map> LISTMAPPER, )]"

# <list.map>
PAIRFUNCTION 	=  $(1) -> $(2)
list.pairmap 	:
	@echo "List pair of [a b c] and [d e f]      : [$(<list.pairmap> PAIRFUNCTION, a b c, d e f)]"
	@echo "List pair of [a b c] and [d]          : [$(<list.pairmap> PAIRFUNCTION, a b c, d )]"
	@echo "List pair of [a b c] and []           : [$(<list.pairmap> PAIRFUNCTION, a b c)]"
	
# <list.reverse>
list.reverse 	:	
	@echo "Reverse of empty list                 : [$(<list.reverse>)]"
	@echo "Reverse of empty list (with spaces)   : [$(<list.reverse>)]"
	@echo "Reverse of list [a123 b c]            : [$(<list.reverse> a123 b c)]"
	@echo "Reverse of list [a b c d]             : [$(<list.reverse> a b c d)]"

# <list.eq>
list.eq 	:
	@echo  "list.eq of empty lists               : $(<list.eq> ,)"
	@echo  "list.eq of empty lists with spaces   : $(<list.eq> , )"
	@echo  "list.eq(a b c,)                      : $(<list.eq> a b c, )"
	@echo  "list.eq(a b c,a b c)                 : $(<list.eq> a b c, a b c)"	
	@echo  "list.eq(a b c,a b c d)               : $(<list.eq> a b c, a b c d)"	
	@echo  "list.eq(a b c,A B C)                 : $(<list.eq> a b c, A B C)"	

# <list.ne>
list.ne 	:
	@echo  "list.ne of empty lists               : $(<list.ne> ,)"
	@echo  "list.ne of empty lists with spaces   : $(<list.ne> , )"
	@echo  "list.ne(a b c,)                      : $(<list.ne> a b c, )"
	@echo  "list.ne(a b c,a b c)                 : $(<list.ne> a b c, a b c)"	
	@echo  "list.ne(a b c,a b c d)               : $(<list.ne> a b c, a b c d)"	
	@echo  "list.ne(a b c,A B C)                 : $(<list.ne> a b c, A B C)"	

# <list.caseeq>
list.caseeq 	:
	@echo  "list.caseeq of empty lists               : $(<list.caseeq> ,)"
	@echo  "list.caseeq of empty lists with spaces   : $(<list.caseeq> , )"
	@echo  "list.caseeq(a b c,)                      : $(<list.caseeq> a b c, )"
	@echo  "list.caseeq(a b c,a b c)                 : $(<list.caseeq> a b c, a b c)"	
	@echo  "list.caseeq(a b c,a b c d)               : $(<list.caseeq> a b c, a b c d)"	
	@echo  "list.caseeq(a b c,A B C)                 : $(<list.caseeq> a b c, A B C)"	

# <list.casene>
list.casene 	:
	@echo  "list.casene of empty lists               : $(<list.casene> ,)"
	@echo  "list.casene of empty lists with spaces   : $(<list.casene> , )"
	@echo  "list.casene(a b c,)                      : $(<list.casene> a b c, )"
	@echo  "list.casene(a b c,a b c)                 : $(<list.casene> a b c, a b c)"	
	@echo  "list.casene(a b c,a b c d)               : $(<list.casene> a b c, a b c d)"	
	@echo  "list.casene(a b c,A B C)                 : $(<list.casene> a b c, A B C)"	

# <range>
list.range	:
	@echo  "range(1,10)                              : $(<range> 1, 10)"
	@echo  "range(1,5, 0.3)                          : $(<range> 1, 5, 0.3)"

# <list.quote>
list.quote	:
	@echo "list.quote()                              : $(<list.quote> ,')"
	@echo "list.quote(value1 value2)                 : $(<list.quote> value1 value2,')"
	@echo "list.quote(value1[ value2,[,])            : $(<list.quote> value1[ value2,[,])"

# <string.length>
string.length	:
	@echo "string.length()                           : $(<string.length>)"
	@echo "string.length( )                          : $(<string.length>   )"
	@echo "string.length(12345)                      : $(<string.length> 12345)"

# <string.compare>
string.compare	:
	@echo "string.compare(,)                         : $(<string.compare> ,)"
	@echo "string.compare(, )                        : $(<string.compare> , )"
	@echo "string.compare(abc,abc)                   : $(<string.compare> abc,abc)"
	@echo "string.compare(abcd,abc)                  : $(<string.compare> abcd,abc)"
	@echo "string.compare(abc,ABC)                   : $(<string.compare> abc,ABC)"
	@echo "string.compare(ABC,abc)                   : $(<string.compare> ABC,abc)"
	
# <string.casecompare>
string.casecompare	:
	@echo "string.casecompare(,)                     : $(<string.casecompare> ,)"
	@echo "string.casecompare(, )                    : $(<string.casecompare> , )"
	@echo "string.casecompare(abc,abc)               : $(<string.casecompare> abc,abc)"
	@echo "string.casecompare(abcd,abc)              : $(<string.casecompare> abcd,abc)"
	@echo "string.casecompare(abc,ABC)               : $(<string.casecompare> abc,ABC)"
	@echo "string.casecompare(ABC,abc)               : $(<string.casecompare> ABC,abc)"

# <string.eq>
string.eq 		:
	@echo "string.eq(,)                              : $(<string.eq> ,)"
	@echo "string.eq(, )                             : $(<string.eq> , )"
	@echo "string.eq(abc,abc)                        : $(<string.eq> abc,abc)"
	@echo "string.eq(abc,ABC)                        : $(<string.eq> abc,ABC)"
	@echo "string.eq(ABC,abc)                        : $(<string.eq> ABC,abc)"
	@echo "string.eq(abc,xyz)                        : $(<string.eq> abc,xyz)"
	@echo "string.eq(xyz,abc)                        : $(<string.eq> xyz,abc)"

# <string.caseeq>
string.caseeq 		:
	@echo "string.caseeq(,)                          : $(<string.caseeq> ,)"
	@echo "string.caseeq(, )                         : $(<string.caseeq> , )"
	@echo "string.caseeq(abc,abc)                    : $(<string.caseeq> abc,abc)"
	@echo "string.caseeq(abc,ABC)                    : $(<string.caseeq> abc,ABC)"
	@echo "string.caseeq(ABC,abc)                    : $(<string.caseeq> ABC,abc)"
	@echo "string.caseeq(abc,xyz)                    : $(<string.caseeq> abc,xyz)"
	@echo "string.caseeq(xyz,abc)                    : $(<string.caseeq> xyz,abc)"
	
# <string.ne>
string.ne 		:
	@echo "string.ne(,)                              : $(<string.ne> ,)"
	@echo "string.ne(, )                             : $(<string.ne> , )"
	@echo "string.ne(abc,abc)                        : $(<string.ne> abc,abc)"
	@echo "string.ne(abc,ABC)                        : $(<string.ne> abc,ABC)"
	@echo "string.ne(ABC,abc)                        : $(<string.ne> ABC,abc)"
	@echo "string.ne(abc,xyz)                        : $(<string.ne> abc,xyz)"
	@echo "string.ne(xyz,abc)                        : $(<string.ne> xyz,abc)"

# <string.casene>
string.casene 		:
	@echo "string.casene(,)                          : $(<string.casene> ,)"
	@echo "string.casene(, )                         : $(<string.casene> , )"
	@echo "string.casene(abc,abc)                    : $(<string.casene> abc,abc)"
	@echo "string.casene(abc,ABC)                    : $(<string.casene> abc,ABC)"
	@echo "string.casene(ABC,abc)                    : $(<string.casene> ABC,abc)"
	@echo "string.casene(abc,xyz)                    : $(<string.casene> abc,xyz)"
	@echo "string.casene(xyz,abc)                    : $(<string.casene> xyz,abc)"
	
# <string.lt>
string.lt 		:
	@echo "string.lt(,)                              : $(<string.lt> ,)"
	@echo "string.lt(, )                             : $(<string.lt> , )"
	@echo "string.lt(abc,abc)                        : $(<string.lt> abc,abc)"
	@echo "string.lt(abc,ABC)                        : $(<string.lt> abc,ABC)"
	@echo "string.lt(ABC,abc)                        : $(<string.lt> ABC,abc)"
	@echo "string.lt(abc,xyz)                        : $(<string.lt> abc,xyz)"
	@echo "string.lt(xyz,abc)                        : $(<string.lt> xyz,abc)"

# <string.caselt>
string.caselt 		:
	@echo "string.caselt(,)                          : $(<string.caselt> ,)"
	@echo "string.caselt(, )                         : $(<string.caselt> , )"
	@echo "string.caselt(abc,abc)                    : $(<string.caselt> abc,abc)"
	@echo "string.caselt(abc,ABC)                    : $(<string.caselt> abc,ABC)"
	@echo "string.caselt(ABC,abc)                    : $(<string.caselt> ABC,abc)"
	@echo "string.caselt(abc,xyz)                    : $(<string.caselt> abc,xyz)"
	@echo "string.caselt(xyz,abc)                    : $(<string.caselt> xyz,abc)"

# <string.gt>
string.gt 		:
	@echo "string.gt(,)                              : $(<string.gt> ,)"
	@echo "string.gt(, )                             : $(<string.gt> , )"
	@echo "string.gt(abc,abc)                        : $(<string.gt> abc,abc)"
	@echo "string.gt(abc,ABC)                        : $(<string.gt> abc,ABC)"
	@echo "string.gt(ABC,abc)                        : $(<string.gt> ABC,abc)"
	@echo "string.gt(abc,xyz)                        : $(<string.gt> abc,xyz)"
	@echo "string.gt(xyz,abc)                        : $(<string.gt> xyz,abc)"

# <string.casegt>
string.casegt 		:
	@echo "string.casegt(,)                          : $(<string.casegt> ,)"
	@echo "string.casegt(, )                         : $(<string.casegt> , )"
	@echo "string.casegt(abc,abc)                    : $(<string.casegt> abc,abc)"
	@echo "string.casegt(abc,ABC)                    : $(<string.casegt> abc,ABC)"
	@echo "string.casegt(ABC,abc)                    : $(<string.casegt> ABC,abc)"
	@echo "string.casegt(abc,xyz)                    : $(<string.casegt> abc,xyz)"
	@echo "string.casegt(xyz,abc)                    : $(<string.casegt> xyz,abc)"
	
# <string.le>
string.le 		:
	@echo "string.le(,)                              : $(<string.le> ,)"
	@echo "string.le(, )                             : $(<string.le> , )"
	@echo "string.le(abc,abc)                        : $(<string.le> abc,abc)"
	@echo "string.le(abc,ABC)                        : $(<string.le> abc,ABC)"
	@echo "string.le(ABC,abc)                        : $(<string.le> ABC,abc)"
	@echo "string.le(abc,xyz)                        : $(<string.le> abc,xyz)"
	@echo "string.le(xyz,abc)                        : $(<string.le> xyz,abc)"

# <string.casele>
string.casele 		:
	@echo "string.casele(,)                          : $(<string.casele> ,)"
	@echo "string.casele(, )                         : $(<string.casele> , )"
	@echo "string.casele(abc,abc)                    : $(<string.casele> abc,abc)"
	@echo "string.casele(abc,ABC)                    : $(<string.casele> abc,ABC)"
	@echo "string.casele(ABC,abc)                    : $(<string.casele> ABC,abc)"
	@echo "string.casele(abc,xyz)                    : $(<string.casele> abc,xyz)"
	@echo "string.casele(xyz,abc)                    : $(<string.casele> xyz,abc)"
	
# <string.ge>
string.ge 		:
	@echo "string.ge(,)                              : $(<string.ge> ,)"
	@echo "string.ge(, )                             : $(<string.ge> , )"
	@echo "string.ge(abc,abc)                        : $(<string.ge> abc,abc)"
	@echo "string.ge(abc,ABC)                        : $(<string.ge> abc,ABC)"
	@echo "string.ge(ABC,abc)                        : $(<string.ge> ABC,abc)"
	@echo "string.ge(abc,xyz)                        : $(<string.ge> abc,xyz)"
	@echo "string.ge(xyz,abc)                        : $(<string.ge> xyz,abc)"

# <string.casege>
string.casege 		:
	@echo "string.casege(,)                          : $(<string.casege> ,)"
	@echo "string.casege(, )                         : $(<string.casege> , )"
	@echo "string.casege(abc,abc)                    : $(<string.casege> abc,abc)"
	@echo "string.casege(abc,ABC)                    : $(<string.casege> abc,ABC)"
	@echo "string.casege(ABC,abc)                    : $(<string.casege> ABC,abc)"
	@echo "string.casege(abc,xyz)                    : $(<string.casege> abc,xyz)"
	@echo "string.casege(xyz,abc)                    : $(<string.casege> xyz,abc)"
	
# <string.search> 
string.search		:
	@echo "string.search(abc def, )                  : $(<string.search> abc def, )"
	@echo "string.search(abcdef,bcd)                 : $(<string.search> abcdef,bcd)"
	@echo "string.search(abcdef,BCD)                 : $(<string.search> abcdef,BCD)"

# <string.casesearch> 
string.casesearch	:
	@echo "string.casesearch(abc def, )              : $(<string.casesearch> abc def, )"
	@echo "string.casesearch(abcdef,bcd)             : $(<string.casesearch> abcdef,bcd)"
	@echo "string.casesearch(abcdef,BCD)             : $(<string.casesearch> abcdef,BCD)"
	
# <string.reverse>
string.reverse 		:
	@echo "string.reverse()                          : $(<string.reverse>)"
	@echo "string.reverse( )                         : $(<string.reverse>  )"
	@echo "string.reverse(a)                         : $(<string.reverse> a)"
	@echo "string.reverse(abc)                       : $(<string.reverse> abc)"
	@echo "string.reverse(abcd)                      : $(<string.reverse> abcd)"
	
# <string.tolower>
string.tolower 		:
	@echo "string.tolower(abc)                       : $(<string.tolower> abc)"
	@echo "string.tolower(ABCdef01)                  : $(<string.tolower> ABCdef01)"
	
# <string.toupper>
string.toupper 		:
	@echo "string.toupper(abc)                       : $(<string.toupper> abc)"
	@echo "string.toupper(ABCdef01)                  : $(<string.toupper> ABCdef01)"
	
# <string.implode>
string.implode 		:
	@echo "string.implode( / ,a b c d)		: $(<string.implode> /, a b c d)"
	@echo "string.implode( /, ) 			: $(<string.implode> /, )"
	@echo "string.implode( /, a) 			: $(<string.implode> /, a)"
	
# <string.explode>
string.explode 		:
	@echo "string.explode( /, a/b/c/d)		: $(<string.explode> /, a/b/c/d)"
	@echo "string.explode( /, /a)			: $(<string.explode> /, /a)"
	@echo "string.explode( /, a/)			: $(<string.explode> /, a/)"
	@echo "string.explode( /, /a/)			: $(<string.explode> /, /a/)"
	@echo "string.explode( //, a//b//c)		: $(<string.explode> //, a//b//c)"
	
# <string.substring>
string.substring 	:
	@echo "string.substring(,100,200)		: $(<string.substring> ,100,200)"
	@echo "string.substring(abcd,0,4)		: $(<string.substring> abcd,0,4)"
	@echo "string.substring(abcd,0,3)		: $(<string.substring> abcd,0,3)"
	@echo "string.substring(abcd,0,2)		: $(<string.substring> abcd,0,2)"
	@echo "string.substring(abcd,0,1)		: $(<string.substring> abcd,0,1)"
	@echo "string.substring(abcd,0,0)		: $(<string.substring> abcd,0,0)"
	@echo "string.substring(abcd,2,4)		: $(<string.substring> abcd,2,4)"
	
# <random>
$(<randomize>)
random			:
	@echo "random()					: $(<random>)"
	@echo "random()					: $(<random>)"
	@echo "random(1,10)				: $(<random> 1,10)"
	@echo "random(1,10)				: $(<random> 1,10)"

# <+>
math.plus		:
	@echo "(<+> 1,)					: $(<+> 1,)"
	@echo "(<+> 1,2)				: $(<+> 1, 2)"
	@echo "(<+> 1,2,3)				: $(<+> 1, 2,3)"
	@echo "(<+> 1,2,3.14156)			: $(<+> 1, 2, 3.14156)"

# <->
math.minus		:
	@echo "(<-> 1,)					: $(<-> 1,)"
	@echo "(<-> 100, 1, 2, 3, 4)			: $(<-> 100, 1, 2, 3, 4 )"

# <*>
math.mul		:
	@echo "(<*> 2,)					: $(<*> 2,)"
	@echo "(<*> 2, 3, 4, 5)				: $(<*> 2, 3, 4, 5 )"


# </>
math.div		:
	@echo "(</> 2,)					: $(</> 2,)"
	@echo "(</> 100, 2, 2)				: $(</> 100, 2, 2 )"
	@echo "(</> 101, 2, 2)				: $(</> 101, 2, 2 )"

# <//>
math.idiv		:
	@echo "(<//> 2,)				: $(<//> 2,)"
	@echo "(<//> 100, 2, 2)				: $(<//> 100, 2, 2 )"
	@echo "(<//> 101, 2, 2)				: $(<//> 101, 2, 2 )"

# <%>
math.mod		:
	@echo "(<%> 2,)					: $(<%> 2,)"
	@echo "(<%> 100, 2)				: $(<%> 100, 2 )"
	@echo "(<%> 100, 3)				: $(<%> 100, 3 )"
	@echo "(<%> 100, 17)				: $(<%> 100, 17 )"

# <**>
math.pow		:
	@echo "(<*> 2,)					: $(<**> 2,)"
	@echo "(<*> 2,1)				: $(<**> 2,1)"
	@echo "(<*> 2,8)				: $(<**> 2,8)"

# <++>
math.incr		:
	@echo "(<++>)					: $(<++>)"
	@echo "(<++> 100)				: $(<++> 100)"

# <-->
math.decr		:
	@echo "(<-->)					: $(<-->)"
	@echo "(<--> 100)				: $(<--> 100)"

# <&>
math.and		:
	@echo "(<&> 2,)					: $(<&> 2,)"
	@echo "(<&> 0xF0, 0x10)				: $(<&> 0xF0, 0x10)"

# <|>
math.or		:
	@echo "(<|> 2,)					: $(<|> 2,)"
	@echo "(<|> 0xF0, 0x10)				: $(<|> 0xF0, 0x10)"
	@echo "(<|> 0xF0, 0x04)				: $(<|> 0xF0, 0x04)"

# <^>
math.xor		:
	@echo "(<^> 2,)					: $(<^>  2,)"
	@echo "(<^> 0xF0, 0x10)				: $(<^> 0xF0, 0x10)"
	@echo "(<^> 0x02, 0x01)				: $(<^> 0x02, 0x01)"

# <~>
math.not		:
	@echo "(<~> )                                   : $(<~> )"
	@echo "(<~> 0)                                  : $(<~> 0)"
	@echo "(<~> 1)                                  : $(~> 1)"
	@echo "(<~> 0x1234)                             : $(<~> 0x1234)"

# <<
math.shl		:
	@echo "(<< 0x0F,)				: $(<< 0x0F,)"
	@echo "(<< 0x0F, 4)				: $(<< 0x0F, 4)"


# >>
math.shr		:
	@echo "(>> 0x0F,)				: $(>> 0x0F,)"
	@echo "(>> 0xF0, 4)				: $(>> 0xF0, 4)"

# Date functions
date	:
	@echo "(now)                                    : $(<now>)"
	@echo "Default date format (Y-m-d H:i:s)        : $(<date>)"
	@echo "Date formats :"
	@echo "- 'a' (am/pm in lowercase)               : $(<date> a)"
	@echo "- 'a' (am/pm in uppercase)               : $(<date> A)"
	@echo "- 'B' (internet swatch hour)             : $(<date> B)"
	@echo "- 'c' (full ISO8601 format)              : $(<date> c)"
	@echo "- 'd' (day of month)                     : $(<date> d)"
	@echo "- 'D' (short day name)                   : $(<date> D)"
	@echo "- 'F' (long month name)                  : $(<date> F)"
	@echo "- 'g' (hour without leading zero 1..12)  : $(<date> g)"
	@echo "- 'G' (hour 0..23 without leading zero)  : $(<date> G)"
	@echo "- 'h' (hour 01..12 with leading zero)    : $(<date> h)"
	@echo "- 'H' (hour 00..23 with leading zero)    : $(<date> H)"
	@echo "- 'i' (minute 00..59 with leading zero)  : $(<date> i)"
	@echo "- 'I' (isdst)                            : $(<date> I)"
	@echo "- 'j' (day of month without leading zero): $(<date> j)"
	@echo "- 'l' (long day name)                    : $(<date> l)"
	@echo "- 'L' (is leap year?)                    : $(<date> L)"
	@echo "- 'M' (short month name)                 : $(<date> M)"
	@echo "- 'm' (month of year, with leading zero) : $(<date> m)"
	@echo "- 'n' (month, without leading zero)      : $(<date> n)"
	@echo "- 'N' (day of week, 1..7)                : $(<date> N)"
	@echo "- 'O' (ISO8601 GMT offset, +0200)        : $(<date> O)"
	@echo "- 'P' (RFC822 GMT offset, +02:00)        : $(<date> P)"
	@echo "- 'r' (RFC822 date)                      : $(<date> r)"
	@echo "- 's' (seconds with leading zero, 00..59): $(<date> s)"
	@echo "- 'S' (english suffix for day of month)  : $(<date> S)"
	@echo "- 't' (month length)                     : $(<date> t)"
	@echo "- 'u' (microsecond time)                 : $(<date> u)"
	@echo "- 'U' (Unix time)                        : $(<date> U)"
	@echo "- 'w' (day of week, 0..6)                : $(<date> w)"
	@echo "- 'y' (year, 2 digits)                   : $(<date> y)"
	@echo "- 'Y' (year, 4 digits)                   : $(<date> Y)"
	@echo "- 'z' (day of year, 0..365)              : $(<date> z)"
	@echo "- 'Z' (GMT offset in seconds)            : $(<date> Z)"
	@echo "- 'W' (ISO week number)                  : $(<date> W)"
	@echo "- 'o' (ISO year)                         : $(<date> o)"
	@echo "- 'e' (timezone name)                    : $(<date> e)"
	@echo "- 'T' (timezone abbreviation)            : $(<date> T)"

# <guid>
guid		:
	@echo "guid 1                                   : $(<guid>)"
	@echo "guid 2                                   : $(<guid>)"
	@echo "guid 3                                   : $(<guid>)"
	@echo "rawguid 1                                : $(<rawguid>)"
	@echo "rawguid 2                                : $(<rawguid>)"
	@echo "rawguid 3                                : $(<rawguid>)"


# <string.sprintf>
string.sprintf	:
	@echo "sprintf(%c)                              : $(<string.sprintf> [%c])"
	@echo "sprintf(%c,A)                            : $(<string.sprintf> [%c],A)"
	@echo "sprintf(%20c,A)                          : $(<string.sprintf> [%-20c],A)"
	@echo "sprintf(%d)                              : $(<string.sprintf> [%d])"
	@echo "sprintf(%08X,123)                        : $(<string.sprintf> [%.8X],123)"
	@echo "sprintf(%f,123.456)                      : $(<string.sprintf> [%f],123.456)"
	@echo "sprintf(%s,coucou)                       : $(<string.sprintf> [%s],coucou)"
	@echo "sprintf(%100s,.)                         : $(<string.sprintf> [%-100s],.)"

# <string.chr>
string.chr	:
	@echo "string.chr(65)                           : $(<string.chr> 65)"
	@echo "string.chr(0x40)                         : $(<string.chr> 0x40)"

# <string.repeat>
string.repeat	:
	@echo "string.repeat(X,0)                       : $(<string.repeat> X,0)"
	@echo "string.repeat(X,1)                       : $(<string.repeat> X,1)"
	@echo "string.repeat(X,10)                      : $(<string.repeat> X,10)"
	@echo "string.repeat(ABC,3)                     : $(<string.repeat> ABC,3)"

# <string.quote>
string.quote	:
	@echo "string.quote()                           : $(<string.quote> ,')"
	@echo "string.quote(value)                      : $(<string.quote> value,')"
	@echo "string.quote(value[,[,])                 : $(<string.quote> value[,[,])"


# Counter functions
counter		:  counter.counter counter.define

counter.counter : 
	@echo "counter(1)                               : $(<counter> 1)"
	@echo "counter(2)                               : $(<counter> 2)"
	@echo "counter(3)                               : $(<counter> 3)"
	@echo "counter(MYCOUNTER) [1]                   : $(<counter> MYCOUNTER)"
	@echo "counter(MYCOUNTER) [2]                   : $(<counter> MYCOUNTER)"
	@echo "counter(MYCOUNTER) [3]                   : $(<counter> MYCOUNTER)"

$(<defcounter> MYCOUNTER2, 100, 15)
MYCOUNTER = $(<counter> MYCOUNTER2)
counter.define	:
	@echo "counter(MYCOUNTER2) [1]                  : $(MYCOUNTER)"
	@echo "counter(MYCOUNTER2) [2]                  : $(MYCOUNTER)"
	@echo "counter(MYCOUNTER2) [3]                  : $(MYCOUNTER)"

# Logical functions
logical		: logical.if logical.ifle logical.iflt logical.ifeq logical.ifne logical.ifge logical.ifgt logical.if/i logical.if/d

logical.if	: 
	@echo "if(,true,false)                          : $(<if> ,true,false)"
	@echo "ifnot(,true,false)                       : $(<ifnot> ,true,false)"
	@echo "if(0,true,false)                         : $(<if> 0,true,false)"
	@echo "ifnot(0,true,false)                      : $(<ifnot> 0,true,false)"
	@echo "if/d(,true,false)                        : $(<if/d> ,true,false)"
	@echo "ifnot/d(,true,false)                     : $(<ifnot/d> ,true,false)"
	@echo "if/d(0,true,false)                       : $(<if/d> 0,true,false)"
	@echo "ifnot/d(0,true,false)                    : $(<ifnot/d> 0,true,false)"
	@echo "if/d(1,true,false)                       : $(<if/d> 1,true,false)"
	@echo "ifnot/d(1,true,false)                    : $(<ifnot/d> 1,true,false)"

logical.ifle	:
	@echo "ifle(,0)                                 : $(<ifle> ,0,yes,no)"
	@echo "ifle(A,a)                                : $(<ifle> A,a,yes,no)"
	@echo "ifle(A,A)                                : $(<ifle> A,A,yes,no)"
	@echo "ifle(a,A)                                : $(<ifle> a,A,yes,no)"
	@echo "ifle(z,a)                                : $(<ifle> z,a,yes,no)"

logical.iflt	:
	@echo "iflt(,0)                                 : $(<iflt> ,0,yes,no)"
	@echo "iflt(A,a)                                : $(<iflt> A,a,yes,no)"
	@echo "iflt(A,A)                                : $(<iflt> A,A,yes,no)"
	@echo "iflt(a,A)                                : $(<iflt> a,A,yes,no)"
	@echo "iflt(z,a)                                : $(<iflt> z,a,yes,no)"

logical.ifeq	:
	@echo "ifeq(,0)                                 : $(<ifeq> ,0,yes,no)"
	@echo "ifeq(A,a)                                : $(<ifeq> A,a,yes,no)"
	@echo "ifeq(A,A)                                : $(<ifeq> A,A,yes,no)"
	@echo "ifeq(a,A)                                : $(<ifeq> a,A,yes,no)"
	@echo "ifeq(z,a)                                : $(<ifeq> z,a,yes,no)"

logical.ifne	:
	@echo "ifne(,0)                                 : $(<ifne> ,0,yes,no)"
	@echo "ifne(A,a)                                : $(<ifne> A,a,yes,no)"
	@echo "ifne(A,A)                                : $(<ifne> A,A,yes,no)"
	@echo "ifne(a,A)                                : $(<ifne> a,A,yes,no)"
	@echo "ifne(z,a)                                : $(<ifne> z,a,yes,no)"

logical.ifge	:
	@echo "ifge(,0)                                 : $(<ifge> ,0,yes,no)"
	@echo "ifge(A,a)                                : $(<ifge> A,a,yes,no)"
	@echo "ifge(A,A)                                : $(<ifge> A,A,yes,no)"
	@echo "ifge(a,A)                                : $(<ifge> a,A,yes,no)"
	@echo "ifge(z,a)                                : $(<ifge> z,a,yes,no)"

logical.ifgt	:
	@echo "ifgt(,0)                                 : $(<ifgt> ,0,yes,no)"
	@echo "ifgt(A,a)                                : $(<ifgt> A,a,yes,no)"
	@echo "ifgt(A,A)                                : $(<ifgt> A,A,yes,no)"
	@echo "ifgt(a,A)                                : $(<ifgt> a,A,yes,no)"
	@echo "ifgt(z,a)                                : $(<ifgt> z,a,yes,no)"

logical.if/d	:
	@echo "ifeq/d(,0)                               : $(<ifeq/d> ,0,yes,no)"
	@echo "iflt/d(-1,1)                             : $(<iflt/d> -1,1,yes,no)"
	@echo "ifgt/d(-1,1)                             : $(<ifgt/d> -1,1,yes,no)"
	@echo "ifeq(1.12,1.12)                          : $(<ifeq/d> 1.12,1.12,yes,no)"

logical.if/i	:
	@echo "ifle/i(,0)                               : $(<ifle/i> ,0,yes,no)"
	@echo "ifle/i(A,a)                              : $(<ifle/i> A,a,yes,no)"
	@echo "ifle/i(A,A)                              : $(<ifle/i> A,A,yes,no)"
	@echo "ifle/i(a,A)                              : $(<ifle/i> a,A,yes,no)"
	@echo "ifle/i(z,a)                              : $(<ifle/i> z,a,yes,no)"


# File functions
TEMPFILE	=  $(<tempfile>)
file		:
	@echo "tempfile ->                              : $(TEMPFILE)"
	@echo "tempfile (bis) ->                        : $(TEMPFILE)"
	@echo "tempfile(.,prefix,txt)                   : $(<tempfile> .,prefix.,.txt)"
	@echo "tempfile(,prefix,txt)                    : $(<tempfile> ,prefix.,.txt)"
	@echo "stat.mode(Makefile)                      : $(<stat.mode> Makefile)"
	@echo "stat.ctime(Makefile)                     : $(<stat.ctime> Makefile)"
	@echo "stat.mtime(Makefile)                     : $(<stat.mtime> Makefile)"
	@echo "stat.atime(Makefile)                     : $(<stat.atime> Makefile)"
	@echo "stat.size(Makefile)                      : $(<stat.size> Makefile)"
	@echo "stat.uid(Makefile)                       : $(<stat.uid> Makefile)"
	@echo "stat.gid(Makefile)                       : $(<stat.gid> Makefile)"
