/**************************************************************************************************************

    NAME
        builtins.h

    DESCRIPTION
        Builtin functions definitions.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/02]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifdef		WAKE_EXTENSIONS
# ifndef	__BUILTINS_H__
#	define	__BUILTINS_H__


static function_table_entry		wake_function_table [] =
   {
	// General functions
	 FT_ENTRY
	    (
		"<builtins>",
		0,  2,
		1,  
		wake_builtin_builtins,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "[category][,[sort])" ),
			N_ ( "Expands to the list of defined builtin functions." ),
			N_ ( "Expands to the list of defined builtin functions (not to be confused with user-defined functions). "
			     "'category' can be used to restrict the list to the specified category. The category name for GNU make native "
			     "builtin functions is \"builtins\". Thrak extensions have category names starting with \"wake\". If the 'sort' "
			     "argument is non-empty, the expanded results will be sorted." )
		    )
	     ),
	 FT_ENTRY
	    (
		"<isbuiltin>",
		1,  1,
		1,  
		wake_builtin_isbuiltin,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "name" ),
			N_ ( "Checks if the specified name is a builtin function." ),
			N_ ( "Checks if the specified name is a builtin function." )
		    )
	     ),
 	 FT_ENTRY
	    (
		"<error>",
		1, 255,
		1,  
		wake_builtin_message,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "text..." ),
			N_ ( "Outputs an error message then exits." ),
			N_ ( "Outputs an error message then exits." )
		    )
	      ),
 	FT_ENTRY
	   (
		"<warning>",
		1, 255,
		1,  
		wake_builtin_message,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "text..." ),
			N_ ( "Outputs a warning message." ),
			N_ ( "Outputs a warning message." )
		    )
	     ),
 	FT_ENTRY
	   (
		"<print>",
		1, 255,
		1,  
		wake_builtin_message,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "text..." ),
			N_ ( "Outputs an informational message." ),
			N_ ( "Outputs an informational message." )
		    )
	     ),
 	FT_ENTRY
	   (
		"<message>",
		1, 255,
		1,  
		wake_builtin_message,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "text..." ),
			N_ ( "Outputs an informational message." ),
			N_ ( "Outputs an informational message." )
		    )
	     ),
 	FT_ENTRY
	   (
		"<log>",
		1, 99,
		1,
		wake_builtin_message,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "text..." ),
			N_ ( "Outputs an informational message." ),
			N_ ( "Outputs an informational message." )
		    )
	     ),

	// List functions
 	FT_ENTRY
	   (
		"<list.first>",
		1, 99,
		1,
		wake_builtin_list_first,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list[,list...]" ),
			N_ ( "Expands to the first word of a list." ),
			N_ ( "Expands to the first word of a list." )
		    )
	     ),
 	FT_ENTRY
	   (
		"<list.last>",
		1, 99,
		1,  
		wake_builtin_list_last,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list[, list...]" ),
			N_ ( "Expands to the last word of a list." ),
			N_ ( "Expands to the last word of a list. Note that unlike the $(lastword) function, this function will return"
			     "the very last word of its argument list, not the last word of the first argument." )
		    )
	     ),
	FT_ENTRY
	   (
		"<list.length>",
		1, 99,
		1,  
		wake_builtin_list_length,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list[,list...]" ),
			N_ ( "Expands to the number of words in a list." ),
			N_ ( "Expands to the number of words in a list. Note that this function accepts an arbitrary" )
		    )
	     ),
	FT_ENTRY
	   (
		"<list.rest>",
		1, 99,
		1,  
		wake_builtin_list_rest,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list[,list...]" ),
			N_ ( "Expands to the specified list without its first word." ),
			N_ ( "Expands to the specified list without its first word. This is the equivalent of the Lisp's cdr function." )
		    )
	     ),
	FT_ENTRY
	   (
		"<list.chop>",
		1, 99,
		1,
		wake_builtin_list_chop,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list[,list...]" ),
			N_ ( "Expands to the specified list with its last element removed." ),
			N_ ( "Expands to the specified list with its last element removed. This is the equivalent of the Lisp's chop function." )
		    )
	     ),
	FT_ENTRY
	   (
		"<list.map>",
		1, 99,
		0,
		wake_builtin_list_map,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "func,list[,list...]" ),
			N_ ( "Calls 'func' for each word in 'list'." ),
			N_ ( "Calls 'func' for each word in 'list'. The resulting value is the catenation of applying func(x) to each word of 'list'." ) 
		    )
	    ),
	FT_ENTRY
	   (
		"<list.pairmap>",
		1,  3,
		0,
		wake_builtin_list_pairmap,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "func,list1,list2" ),
			N_ ( "Calls 'func' for each word of 'list1' and 'list2'." ),
			N_ ( "Calls 'func' for each word of 'list1' and 'list2'. The resulting value is the catenation of applying func(x,y) to each word "
			     "of 'list1' and 'list2'." ) 
		    )
	     ),
	FT_ENTRY 
	   (
		"<list.reverse>",
		0,  1,
		1,
		wake_builtin_list_reverse,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list..." ),
			N_ ( "Reverses the set of supplied list(s)." ),
			N_ ( "Reverses the set of supplied list(s)." ) 
		    )
	     ),
	FT_ENTRY
	   (
		"<list.eq>",
		0,  2,
		1,  
		wake_builtin_list_equality,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list1,list2" ),
			N_ ( "Check if 'list1' is equal to 'list2'." ),
			N_ ( "Check if 'list1' is equal to 'list2'. Expand to 1 if both lists are equal (ie, they have the same elements and "
			      " the same number of elements), or to the empty string otherwise. Comparisons are case-sensitive." ) 
		    )
	     ),
	FT_ENTRY
	   (
		"<list.ne>",
		0,  2,
		1,
		wake_builtin_list_equality,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list1,list2" ),
			N_ ( "Check if 'list1' is equal to 'list2'." ),
			N_ ( "Check if 'list1' is equal to 'list2'. Expand to 1 if both lists are not equal (ie, they either have different elements "
			      " or different numbers of elements), or to the empty string otherwise. Comparisons are case-sensitive." ) 
		    )
	      ),
	FT_ENTRY
	   (
		"<list.caseeq>",
		0,  2,
		1,
		wake_builtin_list_equality,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list1,list2" ),
			N_ ( "Check if 'list1' is equal to 'list2'." ),
			N_ ( "Check if 'list1' is equal to 'list2'. Expand to 1 if both lists are equal (ie, they have the same elements and "
			      " the same number of elements), or to the empty string otherwise. Comparisons are case-insensitive." ) 
		    )
	     ),
	FT_ENTRY
	   (
		"<list.casene>",
		0,  2,
		1,  
		wake_builtin_list_equality,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "list1,list2" ),
			N_ ( "Check if 'list1' is equal to 'list2'." ),
			N_ ( "Check if 'list1' is equal to 'list2'. Expand to 1 if both lists are not equal (ie, they either have different elements "
			      " or different numbers of elements), or to the empty string otherwise. Comparisons are case-insensitive." ) 
		    )
	    ),
	FT_ENTRY
	   (
		"<list.quote>",
		1,  4,
		1,  
		wake_builtin_list_quote,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value [, left_quote [, right_quote [, escape ]]]" ),
			N_ ( "Quotes each item of the specified list." ),
			N_ ( "Quotes each item of the specified list. The default character is double quote and the default escape character is backslash." )
		    )
	    ),
	FT_ENTRY
	   (
		"<range>",
		2,  3,
		1,  
		wake_builtin_list_range,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "start, stop [, step]" ),
			N_ ( "Generates a lis of range from 'start' to 'sopt' using the specified step." ),
			N_ ( "Generates a lis of range from 'start' to 'sopt' using the specified step (default is 1)." )
		    )
	     ),

	// String functions
	FT_ENTRY 
	   (
		"<string.length>",
		0,  1,
		1,
		wake_builtin_string_strlen,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string" ),
			N_ ( "Expands to the length of the specified string." ),
			N_ ( "Expands to the length of the specified string. Leading spaces are counted in the result." ) 
		    )
	     ),
	FT_ENTRY
	   (
		"<string.compare>",
		2,  2,
		1,
		wake_builtin_string_compare_simple,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Case-sensitively compares 'string1' with 'string2'. Returns the same result as the C function strcmp()." ),
			N_ ( "Compares 'string1' with 'string2'. Expands to the empty string if both strings are equal, to -1 if 'string1' is "
			     "lexicographically less than 'string2' and to 1 if 'string1' is greater than 'string2' (ie, it returns the same result "
			     "as the C function strcmp()). Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY 
	   (
		"<string.casecompare>",
		2,  2,
		1,
		wake_builtin_string_compare_simple,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Case-insensitively compares 'string1' with 'string2'. Returns the same result as the C function strcasecmp()." ),
			N_ ( "Compares 'string1' with 'string2'. Expands to the empty string if both strings are equal, to -1 if 'string1' is "
			     "lexicographically less than 'string2' and to 1 if 'string1' is greater than 'string2' (ie, it returns the same result "
			     "as the C function strcasecmp()). Comparison is case-insensitive." )
		    )
	      ),
	FT_ENTRY
	   (
		"<string.eq>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are equal, or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are equal, or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.caseeq>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are equal, or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are equal, or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	    (
		"<string.ne>",
		2,  2,
		1,  
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are different, or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are different, or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.casene>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are different, or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if both 'string1' and 'string2' are different, or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.lt>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is strictly less than 'string2', or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if 'string1' is strictly less than 'string2', or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.caselt>",
		2,  2,
		1,  
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is strictly less than 'string2', or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if 'string1' is strictly less than 'string2', or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.gt>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is strictly greater than 'string2', or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if 'string1' is strictly greater than 'string2', or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY 
	   (
		"<string.casegt>",
		2,  2,
		1,  
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is strictly greater than 'string2', or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if 'string1' is strictly greater than 'string2', or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.le>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.casele>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.ge>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-sensitive." ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.casege>",
		2,  2,
		1,
		wake_builtin_string_compare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-insensitive." ),
			N_ ( "Expands to 1 if 'string1' is greater than or equal to 'string2', or to the empty string otherwise. Comparison is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.search>",
		2,  2,
		1,
		wake_builtin_string_search,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Returns the position of 'string2' in 'string1'. Expands to nothing if 'string2' does not contain 'string1'. The search is case-sensitive." ),
			N_ ( "Returns the position of 'string2' in 'string1'. Expands to nothing if 'string2' does not contain 'string1'. The search is case-sensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.casesearch>",
		2,  2,
		1,
		wake_builtin_string_search,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string1,string2" ),
			N_ ( "Returns the position of 'string2' in 'string1'. Expands to nothing if 'string2' does not contain 'string1'. The search is case-insensitive." ),
			N_ ( "Returns the position of 'string2' in 'string1'. Expands to nothing if 'string2' does not contain 'string1'. The search is case-insensitive." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.reverse>",
		0,  1,
		1,
		wake_builtin_string_reverse,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string" ),
			N_ ( "Reverses a string." ),
			N_ ( "Reverses a string ; for example,\n"
			     "\t$(<string.reverse> ABC\n"
			     "will expand to : CBA." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.tolower>",
		0,  1,
		1,
		wake_builtin_string_change_case,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string" ),
			N_ ( "Converts a string to lowercase characters." ),
			N_ ( "Converts a string to lowercase characters ; for example,\n"
			     "\t$(<string.tolower> ABC\n"
			     "will expand to : abc." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.toupper>",
		0,  1,
		1,
		wake_builtin_string_change_case,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string" ),
			N_ ( "Converts a string to uppercase characters." ),
			N_ ( "Converts a string to uppercase characters ; for example,\n"
			     "\t$(<string.toupper> abc\n"
			     "will expand to : ABC." )
		    )
	     ),
	FT_ENTRY 
	   (
		"<string.implode>",
		2,  2,
		1,
		wake_builtin_string_implode,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "sep,list" ),
			N_ ( "Implements the PHP function implode()." ),
			N_ ( "Implements the PHP function implode(). Concatenates every element in 'list' using 'sep' as a separator." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.explode>",
		2,  2,
		1,
		wake_builtin_string_explode,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "sep,string" ),
			N_ ( "Implements the PHP function explode()." ),
			N_ ( "Implements the PHP function explode(). Converts 'string' to a list using the specified separator 'sep' to extract "
			     "individual items." )
		    )
	     ),
	FT_ENTRY
	   (
		"<string.substring>",
		2, 3,
		1,
		wake_builtin_string_substring,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string,start[,length]" ),
			N_ ( "Extracts a substring from a string." ),
			N_ ( "Extracts a substring from a string. If 'length' is omitted, the substring will include all characters starting from 'start'. "
			     "The start index is zero-based, and an empty string will be returned if it's beyond the last character of the string." ) 
		    )
	    ),
	FT_ENTRY
	   (
		"<string.sprintf>",
		1, 255,
		1,
		wake_builtin_string_sprintf,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string,start[,length]" ),
			N_ ( "Formatted print (see PHP sprintf() function)." ),
			N_ ( "Formatted print (see PHP sprintf() function)." )
		    )
	    ),
	FT_ENTRY
	   (
		"<string.repeat>",
		2, 2,
		1,
		wake_builtin_string_repeat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "string,count" ),
			N_ ( "Repeats a string the specified number of times." ),
			N_ ( "Repeats a string the specified number of times." )
		    )
	    ),
	FT_ENTRY
	   (
		"<string.chr>",
		1, 1,
		1,
		wake_builtin_string_chr,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "integer" ),
			N_ ( "Converts an integer to its equivalent ascii character." ),
			N_ ( "Converts an integer to its equivalent ascii character." )
		    )
	    ),
	FT_ENTRY
	   (
		"<string.quote>",
		1, 4,
		1,
		wake_builtin_string_quote,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value [, left_quote [, right_quote [, escape ]]]" ),
			N_ ( "Quotes a string." ),
			N_ ( "Quotes a string. The default character is double quote and the default escape character is backslash." )
		    )
	    ),
	FT_ENTRY
	   (
		"<random>",
		0, 2,
		1,
		wake_builtin_random,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "low,high" ),
			N_ ( "Generates a random number." ),
			N_ ( "Generates a random number between the two specified values, or between 0 and RAND_MAX if not specified." )
		    )
	    ),
	FT_ENTRY
	   (
		"<randomize>",
		0, 0,
		0,
		wake_builtin_randomize,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Initializes the pseudo-random number generator." ),
			N_ ( "Initializes the pseudo-random number generator." )
		    )
	    ),
	FT_ENTRY
	   (
		"<guid>",
		0, 0,
		1,
		wake_builtin_guid,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Generates a unique GUID." ),
			N_ ( "Generates a unique GUID." )
		    )
	    ),
	FT_ENTRY
	   (
		"<rawguid>",
		0, 0,
		1,
		wake_builtin_rawguid,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Generates a unique GUID containing only hex digits." ),
			N_ ( "Generates a unique GUID containing only hexadecimal digits." )
		    )
	    ),
	FT_ENTRY
	   (
		"<counter>",
		1, 3,
		1,
		wake_builtin_counter,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "id [, start [, increment ] ]" ),
			N_ ( "Generates a counter value, incremented at each invocation." ),
			N_ ( "Generates a counter value, incremented at each invocation. If the counter has not been created using the <defcounter> function, "
			     "it will be created with an initial value of 1 and an increment of 1, unless a start and increment values are specified." )
		    )
	    ),
	FT_ENTRY
	   (
		"<defcounter>",
		1, 3,
		0,
		wake_builtin_defcounter,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "id [, start [, increment ] ]" ),
			N_ ( "Creates a counter." ),
			N_ ( "Creates a counter. 'start' is the counter starting value (default : 1), and "
			     "'increment' the increment value added at each invocation (default : 1)." )
		    )
	    ),
	FT_ENTRY
	   (
		"<undefcounter>",
		1, 1,
		0,
		wake_builtin_undefcounter,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "id" ),
			N_ ( "Undefines a counter." ),
			N_ ( "Undefines a counter." )
		    )
	    ),
	FT_ENTRY
	   (
		"<+>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Computes the sum of two or more values" ),
			N_ ( "Computes the sum of two or more values" )
		    )
	    ),
	FT_ENTRY
	   (
		"<->",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Computes the subtraction of two or more values" ),
			N_ ( "Computes the subtraction of two or more values" )
		    )
	    ),
	FT_ENTRY
	   (
		"<*>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Computes the mulitplication of two or more values" ),
			N_ ( "Computes the mulitplication of two or more values" )
		    )
	    ),
	FT_ENTRY
	   (
		"</>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Computes the division of two or more values" ),
			N_ ( "Computes the division of two or more values" )
		    )
	    ),
	FT_ENTRY
	   (
		"<//>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Computes the integer division of two or more values" ),
			N_ ( "Computes the integer division of two or more values" )
		    )
	    ),
	FT_ENTRY
	   (
		"<%>",
		2, 2,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1, value2" ),
			N_ ( "Computes the modulo of one value with another" ),
			N_ ( "Computes the modulo of one value with another" )
		    )
	    ),
	FT_ENTRY
	   (
		"<**>",
		2, 2,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value, power" ),
			N_ ( "Computes the power of the specified value" ),
			N_ ( "Computes the power of the specified value" )
		    )
	    ),
	FT_ENTRY
	   (
		"<&>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Performs a binary AND operation between its operands" ),
			N_ ( "Performs a binary AND operation between its operands" )
		    )
	    ),
	FT_ENTRY
	   (
		"<|>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Performs a binary OR operation between its operands" ),
			N_ ( "Performs a binary OR operation between its operands" )
		    )
	    ),
	FT_ENTRY
	   (
		"<^>",
		2, 255,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Performs a binary XOR operation between its operands" ),
			N_ ( "Performs a binary XOR operation between its operands" )
		    )
	    ),
	FT_ENTRY
	   (
		"<~>",
		1, 1,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value1 [, ..., valuen]" ),
			N_ ( "Performs a binary NOT operation" ),
			N_ ( "Performs a binary NOT operation" )
		    )
	    ),
	FT_ENTRY
	   (
		"<<",
		2, 2,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value, count" ),
			N_ ( "Shifts the specified value of 'count' bits to the left" ),
			N_ ( "Shifts the specified value of 'count' bits to the left" )
		    )
	    ),
	FT_ENTRY
	   (
		">>",
		2, 2,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value, count" ),
			N_ ( "Shifts the specified value of 'count' bits to the right" ),
			N_ ( "Shifts the specified value of 'count' bits to the right" )
		    )
	    ),
	FT_ENTRY
	   (
		"<++>",
		1, 1,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value" ),
			N_ ( "Increments a value" ),
			N_ ( "Increments a value" )
		    )
	    ),
	FT_ENTRY
	   (
		"<-->",
		1, 1,
		1,
		wake_builtin_math_operation,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "value" ),
			N_ ( "Decrements a value" ),
			N_ ( "Decrements a value" )
		    )
	    ),
	FT_ENTRY
	   (
		"<=>",
		1, 1,
		1,
		wake_builtin_math_eval,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "expression" ),
			N_ ( "Evaluates an expression" ),
			N_ ( "Evaluates an expression" )
		    )
	    ),
	FT_ENTRY
	   (
		"<now>",
		0, 0,
		1,
		wake_builtin_unixtime,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Returns the current Unix local time, in seconds" ),
			N_ ( "Returns the current Unix local time, in seconds" )
		    )
	    ),
	FT_ENTRY
	   (
		"<date>",
		0, 0,
		1,
		wake_builtin_date,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Formats a Unix time using local time" ),
			N_ ( "Formats a Unix time using local time. The available formats are the ones accepted by the PHP date() function." )
		    )
	    ),
	FT_ENTRY
	   (
		"<gmdate>",
		0, 0,
		1,
		wake_builtin_date,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "" ),
			N_ ( "Formats a Unix time" ),
			N_ ( "Formats a Unix time. The available formats are the ones accepted by the PHP date() function." )
		    )
	    ),

	// Logical functions
	FT_ENTRY
	   (
		"<if>",
		2, 3,
		1,
		wake_builtin_logical_if,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "condition, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise (condition is evaluated as false for an empty string, true otherwise)" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifnot>",
		2, 3,
		1,
		wake_builtin_logical_if,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "condition, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'condition' is false, or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise (condition is evaluated as false for an empty string, true otherwise)" )
		    )
	    ),

	FT_ENTRY
	   (
		"<if/d>",
		2, 3,
		1,
		wake_builtin_logical_if,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "condition, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise (condition is evaluated as a number)" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifnot/d>",
		2, 3,
		1,
		wake_builtin_logical_if,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "condition, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'condition' is false, or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'condition' is true, or 'value2' otherwise (condition is evaluated as a number)" )
		    )
	    ),

	/* Comparisons on strings (case-sensitive) */
	FT_ENTRY
	   (
		"<iflt>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2', or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifle>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2', or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifgt>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2', or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifge>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2', or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifeq>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2', or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifne>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2', or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2', or 'value2' otherwise" )
		    )
	    ),

	/* Comparisons on strings (case-insensitive) */
	FT_ENTRY
	   (
		"<iflt/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifle/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifgt/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifge/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifeq/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifne/i>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2' (case-insensitive comparison), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2' (case-insensitive comparison), or 'value2' otherwise" )
		    )
	    ),

	/* Comparisons on decimals */
	FT_ENTRY
	   (
		"<iflt/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifle/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is less than or equal to 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifgt/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifge/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is greater than or equal to 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifeq/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is equal to 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),
	FT_ENTRY
	   (
		"<ifne/d>",
		3, 4,
		1,
		wake_builtin_logical_ifcompare,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "test1, test2, value1 [, value2 ]" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2' (comparison on decimal values), or 'value2' otherwise" ),
			N_ ( "Expands to 'value1' if 'test1' is different from 'test2' (comparison on decimal values), or 'value2' otherwise" )
		    )
	    ),


	// File functions
	FT_ENTRY
	   (
		"<tempfile>",
		0, 3,
		1,
		wake_builtin_file_tempfile,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "[dir, prefix [suffix]]" ),
			N_ ( "Returns a unique temporary file name" ),
			N_ ( "Returns a unique temporary file name" )
		    )
	    ),
	FT_ENTRY
	   (
		"<filecontents>",
		1, 1,
		1,
		wake_builtin_file_filecontents,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the contents of the specified file" ),
			N_ ( "Returns the contents of the specified file" )
		    )
	    ),
	FT_ENTRY
	   (
		"<stat.mode>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the access mode of the specified file" ),
			N_ ( "Returns the access mode of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.ctime>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the creation time of the specified file" ),
			N_ ( "Returns the creation time of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.mtime>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the last modification time of the specified file" ),
			N_ ( "Returns the last modification time of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.atime>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the last access time of the specified file" ),
			N_ ( "Returns the last access time of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.size>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the size of the specified file" ),
			N_ ( "Returns the size of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.uid>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the user id of the owner of the specified file" ),
			N_ ( "Returns the user id of the owner of the specified file" )
		    )
	    ),  
	FT_ENTRY
	   (
		"<stat.gid>",
		1, 1,
		1,
		wake_builtin_file_stat,
		FUNC_HELP
		   (
			N_ ( "wake" ),
			N_ ( "filename" ),
			N_ ( "Returns the group id of the owner of the specified file" ),
			N_ ( "Returns the group id of the owner of the specified file" )
		    )
	    )
    } ;


# endif		/*  __BUILTINS_H__  */
# endif		/*  WAKE_EXTENSIONS */