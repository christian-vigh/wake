/**************************************************************************************************************

    NAME
        list.c

    DESCRIPTION
        Builtin functions related to list manipulation.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/02]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"wake.h"


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_first - Expands to the first element of a list.
 *
 *   PROTOTYPE
 *	$(<list.first> list)
 *
 *   DESCRIPTION
 *	Expands to the first element of a list.
 *
 *   PARAMETERS
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_first )
   {
	unsigned int		i ;
	char *			words		=  argv [0] ;
	char *			p		=  find_next_token ( ( const char ** ) & words, & i ) ;


	if  ( p  !=  NULL )
		output	=  variable_buffer_output ( output, p, i ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_last - Expands to the last element of a list.
 *
 *   PROTOTYPE
 *	$(<list.last> list)
 *
 *   DESCRIPTION
 *	Expands to the last element of a list.
 *
 *   PARAMETERS
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_last )
   {
	unsigned int		i, j ;
	char *			words ;
	char *			p		=  NULL ;
	char *			next ;


	// Cycle through the list of words of each individual arguments
	for  ( i = 0 ; ; i ++ )
	   {
		if  ( argv [i]  ==  NULL )
			break ;

		words	=  argv [i] ;

		// For each argument, cycle through the words that compose it
		while  ( ( next = find_next_token ( ( const char ** ) & words, & j ) )  !=  NULL )
			p = next ;
	    }

	// If a match has been found, copy it to the output buffer
	if  ( p  !=  NULL  &&  * p )
		output	=  variable_buffer_output ( output, p, j ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_length - Expands to the number of words in a list.
 *
 *   PROTOTYPE
 *	$(<list.length> list)
 *
 *   DESCRIPTION
 *	Expands to the number of words in 'list'.
 *
 *   PARAMETERS
 *	list -
 *		List whose elements are to be counted.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_length )
   {
	unsigned int		i, j ;
	char *			words ;
	int			count		=  0 ;
	char			buffer [ 64 ] ;


	// Cycle through the list of words of each individual arguments
	for  ( i = 0 ; ; i ++ )
	   {
		if  ( argv [i]  ==  NULL )
			break ;

		words	=  argv [i] ;

		// For each argument, cycle through the words that compose it
		while  ( find_next_token ( ( const char ** ) & words, & j )  !=  NULL )
			count ++ ;
	    }

	// Copy the number of elements found
	sprintf ( buffer, "%d", count ) ;
	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_rest - Expands to a list without its first element.
 *
 *   PROTOTYPE
 *	$(<list.rest> list)
 *
 *   DESCRIPTION
 *	Expands to the specified list without its first element. This is the equivalent of Lisp cdr function.
 *
 *   PARAMETERS
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_rest )
   {
	unsigned int		i, j ;
	char *			words		=  argv [0] ;
	char *			next ;


	// Check if the list is not empty
	if  ( words  !=  NULL )
	   {
		next	=  find_next_token ( ( const char ** ) & words, & j ) ;

		if  ( next  ==  NULL )
			return ( output ) ;
	    }

	// Cycle through the list of words of each individual arguments
	for  ( i = 0 ; ; i ++ )
	   {
		if  ( words  ==  NULL )
			break ;

		// For each argument, cycle through the words that compose it
		while  ( ( next = find_next_token ( ( const char ** ) & words, & j ) )  !=  NULL )
		   {
			output	=  variable_buffer_output ( output, next, j ) ;
			output	=  variable_buffer_output ( output, " ", 1 ) ;
		    }

		// Jump to the next argument list
		words	=  argv [i+1] ;
	    }

	// Forget last trailing space
	return ( output - 1 ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_chop - Expands to a list with its last element removed.
 *
 *   PROTOTYPE
 *	$(<list.chop> list)
 *
 *   DESCRIPTION
 *	Expands to the specified list without its last element. This is the equivalent of Lisp chop function.
 *
 *   PARAMETERS
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_chop )
   {
	unsigned int		i, j ;
	char *			words		=  argv [0] ;
	char *			next ;
	char *			last_output	=  output ;


	// Cycle through the list of words of each individual arguments
	for  ( i = 0 ; ; i ++ )
	   {
		if  ( words  ==  NULL )
			break ;

		// For each argument, cycle through the words that compose it
		while  ( ( next = find_next_token ( ( const char ** ) & words, & j ) )  !=  NULL )
		   {
			last_output	=  output - 1 ;
			output		=  variable_buffer_output ( output, next, j ) ;
			output		=  variable_buffer_output ( output, " ", 1 ) ;
		    }

		// Jump to the next argument list
		words	=  argv [i+1] ;
	    }

	return ( last_output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_map - Applies a function to each word of a list.
 *
 *   PROTOTYPE
 *	$(<list.map> func,list)
 *
 *   DESCRIPTION
 *	Applies a function to each word of the specified list. The result is the catenation of applying 'func'
 *	to each word from 'list'.
 *
 *   PARAMETERS
 *	func -
 *		User-defined function to be applied to each word in 'list'. The word is passed as the first 
 *		parameter of 'func'.
 *
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_map )
   {
	unsigned int		i, j ;
	char *			words		=  argv [0] ;
	char *		call [3] 	=  { NULL, NULL, NULL } ;
	char *			p ;
	int			found		=  0 ;


	// Extract the function name ; do nothing if argument list is empty
	if  ( argv [0]  ==  NULL  || ( call [0] = find_next_token ( ( const char ** ) & words, & j ) )  ==  NULL )
		return ( output ) ;

	// Cycle through the list of words of each individual arguments
	for  ( i = 0 ; ; i ++ )
	   {
		if  ( words  ==  NULL )
			break ;

		// For each argument, cycle through the words that compose it
		while  ( ( p = find_next_token ( ( const char ** ) & words, & j ) )  !=  NULL )
		   {
			char *		q	=  xmalloc ( j + 1 ) ;

			strncpy ( q, p, j ) ;
			q [j] = 0 ;

			call [1]	=  q ;
			output		=  func_call ( output, call, "call" ) ;
			output		=  variable_buffer_output ( output, " ", 1 ) ;
			found		=  1 ;
			free ( q ) ;
		    }

		// Jump to the next argument list
		words	=  argv [i+1] ;
	    }

	if  ( found )
		output -- ;		// Omit trailing space

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_pairmap - Applies a function to each word of a list1 and list2.
 *
 *   PROTOTYPE
 *	$(<list.pairmap> func,list1,list2)
 *
 *   DESCRIPTION
 *	Applies a function to each word of the 'list1' and 'list2'. The result is the catenation of applying 
 *	'func(x,y)', where 'x' comes from 'list1' and 'y' from 'list2'.
 *
 *   PARAMETERS
 *	func -
 *		User-defined function to be applied to each paired-word in 'list1' and 'list2'. Words coming from
 *		'list1' and 'list2' are passed as the first and second argument of 'func', respectively.
 *
 *	list1 -
 *		A list of words.
 *
 *	list2 -
 *		A second list of words.
 *
 *  NOTES
 *	If 'list1' and 'list2' have different lengths, the 'func' function will be called as many times as
 *	there are words remaining in the longest list ; the missing argument will be an empty string. 
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_pairmap )
   {
	unsigned int		i1, i2, j ;
	char *			call [4] 	=  { NULL, NULL, NULL, NULL } ;
	char			empty []	=  "" ;
	char *			list1,
	     *			list2 ;
	char *			plist1,
	     *			plist2 ;
	int			empty_count	=  0 ;


	// Extract the function name ; do nothing if argument list is empty
	if  ( argv [0]  ==  NULL  || ( call [0] = find_next_token ( ( const char ** ) & argv [0], & j ) )  ==  NULL )
		return ( output ) ;

	// Handle the case that both lists may be empty
	if  ( argv [1]  ==  NULL )
		return ( output ) ;
	else
	   {
		list1		=  argv [1] ;

		if  ( argv [2]  ==  NULL )
			list2	=  empty ;
		else
			list2	=  argv [2] ;
	    }

	plist1		=  list1 ;
	plist2		=  list2 ;

	// Loop through each word in list1 and list2
	while  ( 1 )
	   {
		empty_count	=  0 ;

		// If the first list is not empty, get the next token, or the empty string
		plist1	=  find_next_token ( ( const char ** ) & list1, & i1 ) ;

		if  ( plist1  ==  NULL  ||  ! * plist1 )
		   {
			plist1	=  empty ;
			i1	=  0 ;
			empty_count ++ ;
		    }

		// Same for the second list
		plist2	=  find_next_token ( ( const char ** ) & list2, & i2 ) ;

		if  ( plist2  ==  NULL  ||  ! * plist2 )
		   {
			plist2	=  empty ;
			i2	=  0 ;
			empty_count ++ ;
		    }

		// If both lists are exhausted, exit processing
		if  ( empty_count  ==  2 )
			break ;

		// Allocate space for the 1st string
		char *		p1	=  xmalloc ( i1 + 1 ) ;

		strncpy ( p1, plist1, i1 ) ;
		p1 [i1]	=  '\0' ;

		// Then for the second string
		char *		p2	=  xmalloc ( i2 + 1 ) ;

		strncpy ( p2, plist2, i2 ) ;
		p2 [i2]	=  '\0' ;

		// Call the function
		call [1]	=  p1 ;
		call [2]	=  p2 ;
		output	=  func_call ( output, call, "call" ) ;
		output	=  variable_buffer_output ( output, " ", 1 ) ;
		free ( p1 ) ;
		free ( p2 ) ;
	    }

	return ( output - 1 ) ;
    }



/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_reverse - Reverses a list.
 *
 *   PROTOTYPE
 *	$(<list.reverse> list...)
 *
 *   DESCRIPTION
 *	Reverses the specified list(s). Multiple list are considered as a single list, thus the statement :
 *		$(<list.reverse> A B C, D E F)
 *	will expand to :
 *		F E D C B A
 *
 *   PARAMETERS
 *	list -
 *		A list of words.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_reverse )
   {
	char *			list ;					// Pointer to the list to be reversed
	int 			index ;					// Current character index in list
	int 			last_index ;				// Index of last non-space character encountered so far
	char 			ch ;					// Current list character
	int 			had_word 		=  0 ;		// 1 if at least one word has been met (meaning that a space needs to be inserted before next word)
	

	// Empty list
	if  ( argv  ==  NULL  ||  argv [0]  ==  NULL )
		return ( output ) ;
	
	// Get list parameter - we will start at the first non-space character at the end of the string
	list 	=  argv [0] ;
	index 	=  strlen ( list ) - 1 ;
	
	while  ( index  >=  0  &&  isspace ( ( int ) list [ index ] ) )
		index -- ;

	last_index 	=  index ;

 	// Reverse each individual argument, starting from the last one
	while  ( index  >=  0 )
	   {
		ch 	=  list [ index ] ;
		
		// Space : we have finished scanning the current word, so copy it to the output buffer
		if  ( isspace ( ( int ) ch ) )
		   {
			// If the list contain multiple words, we need to separate them with a space
			if  ( had_word )
				output	=  variable_buffer_output ( output, " ", 1 ) ;
			
			// Copy current word
			output	=  variable_buffer_output ( output, list + index + 1, last_index - index ) ;
			
			// Skip preceding spaces
			while ( index  >=  0  &&  isspace ( ( int ) list [ index ] ) )
				index -- ;
			
			last_index 	=  index ;	// "index" points to the first non-space character encountered
			had_word ++ ;			// Say that we found one more word
		    }
		// Not a space : simply try the previous character
		else
			index -- ;
	    }
		
	// last_index >= 0 means that we have a final word to process (the first one in the original list)
	if  ( last_index  >=  0 )
	   {
		if  ( had_word )
			output	=  variable_buffer_output ( output, " ", 1 ) ;
		
		output	=  variable_buffer_output ( output, list + index + 1, last_index - index ) ;
	    }

	// All done, return
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_equality - Tests if two lists are equal.
 *
 *   PROTOTYPE
 *	$(<list.eq> list1,list2)
 *	$(<list.ne> list1,list2)
 *	$(<list.caseeq> list1,list2)
 *	$(<list.casene> list1,list2)
 *
 *   DESCRIPTION
 *	Compares 'list1' and 'list2'. 
 *	- <list.eq> expands to 1 if both lists are equal or to the empty string if not. The comparison is case
 *	  sensitive.
 *	- <list.ne> expands to 1 if 'list1' is not equal to 'list2', or the empty string otherwise. The
 *	  comparison is case-insensitive.
 *	- <list.caseeq> and <list.casene> are the case-insentive versions of <list.eq> and <list.ne>, respectively.
 *
 *   PARAMETERS
 *	list1, list2 -
 *		Lists to be compared.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_equality )
   {
	int ( * cmpfunc ) ( const char *  a, const char *  b ) ;
	int		case_sensitive		=  1 ;
	int		negate_result		=  0 ;
	int		equals			=  0 ;


	// Determine which function has been called
	if  ( ! strcmp ( funcname, "<list.eq>" ) )
	   {
		negate_result		=  0 ;
		case_sensitive		=  1 ;
	    }
	else if  ( ! strcmp ( funcname, "<list.ne>" ) )
	   {
		negate_result		=  1 ;
		case_sensitive		=  1 ;
	    }
	else if  ( ! strcmp ( funcname, "<list.caseeq>" ) )
	   {
		negate_result		=  0 ;
		case_sensitive		=  0 ;
	    }
	else if  ( ! strcmp ( funcname, "<list.casene>" ) )
	   {
		negate_result		=  1 ;
		case_sensitive		=  0 ;
	    }
	else
		OS ( error, NILF, "Internal error : Unknown function \"%s\" defined in Thrak function table for wake_builtin_list_equality().", funcname ) ;

	// Compare function
	if  ( case_sensitive )
		cmpfunc		=  strcmp ;
	else
		cmpfunc		=  strcasecmp ;

	
	// Empty lists are considered equal, even if they contain only spaces. Also eliminate the cases 
	// where one list is empty and not the other
	if  ( wake_is_empty_list ( argv [0] ) )
	   {
		if  ( wake_is_empty_list ( argv [1] ) )
			equals 	=  1 ;
		else
			equals 	=  0 ;
	    }
	else if  ( wake_is_empty_list ( argv [1] ) )
	   {
		equals 	=  0 ;
	    }
	// Both lists are non-empty
	else	// 
	   {
		char *		argv0	=  argv [0] ;
		char *		argv1	=  argv [1] ;
		unsigned int	i0, i1 ;


		while  ( 1 )
		   {
			char *		p0	=  find_next_token ( ( const char ** ) & argv0, & i0 ) ;
			char *		p1	=  find_next_token ( ( const char ** ) & argv1, & i1 ) ;

			if  ( ( p0  ==  NULL  &&  p1  !=  NULL  )  ||  ( p0  !=  NULL  &&  p1  ==  NULL ) )
			   {
				equals	=  0 ;
				break ;
			    }
			else if  ( p0  ==  NULL  &&  p1  ==  NULL )
			   {
				equals	=  1 ;
				break ;
			    }
			else if  ( cmpfunc ( p0, p1 ) )
			   {
				equals	=  0 ;
				break ;
			    }
		    }
	    }

	if  ( negate_result )
		equals	=  ! equals ;

	if  ( equals )
		output	=  variable_buffer_output ( output, "1", 1 ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_list_range - Generates a range of values
 *
 *   PROTOTYPE
 *	$(<range> start, stop [, step] )
 *
 *   DESCRIPTION
 *	Generates a list starting from 'start' up to 'stop', by 'step' increments.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_range )
   {
	double		start, stop ;
	double		step		=  1 ;
	int		is_integer ;
	char		buffer [ 64 ] ;
	int		i ;
	double		f ;


	// Retrieve values
	if  ( ! wake_double_value ( argv [0], & start ) )
		ERROR_FLOATVAL ( 1, argv [0], funcname ) ;

	if  ( ! wake_double_value ( argv [1], & stop ) )
		ERROR_FLOATVAL ( 2, argv [1], funcname ) ;

	if  ( argv [2]  !=  NULL  &&  ! wake_double_value ( argv [2], & step ) )
		ERROR_FLOATVAL ( 3, argv [2], funcname ) ;

	// Check values consistencies
	if  ( start  <  stop )
	   {
		if  ( step  <  0 )
			fatal ( reading_file, 0, "Negative step (%g) implies that start value (%g) must be greater than or equal to stop value (%g)",
					step, start, stop ) ;
	    }
	else if  ( start  >  stop )
	   {
		if  ( step  >  0 )
			fatal ( reading_file, 0, "Positive step (%g) implies that start value (%g) must be less than or equal to stop value (%g)",
					step, start, stop ) ;
	    }

	// Use integer conversions if all values are integers
	is_integer	=  ( start  ==  ( int ) start  &&  stop  ==  ( int ) stop  &&  step  ==  ( int ) step ) ;

	// Generate the values 
	if  ( is_integer )
	   {
		for  ( i = start ; i < stop ; i += step )
		   {
			sprintf ( buffer, "%d", i ) ;

			if  ( i  >  start )
				output	=  variable_buffer_output ( output, " ", 1 ) ;

			output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;
		    }
	    }
	else
	   {
		for  ( f = start ; f < stop ; f += step )
		   {
			sprintf ( buffer, "%g", f ) ;

			if  ( f  >  start )
				output	=  variable_buffer_output ( output, " ", 1 ) ;

			output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;
		    }
	    }

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_quote - Quotes list items.
 *
 *   PROTOTYPE
 *	$(<string.quote> value [, delim [, right_delim [, escape ] ] ])
 *
 *   DESCRIPTION
 *	Quotes list items.
 *
 *   PARAMETERS
 *	value -
 *		List items. No quoting will occur on an empty list.
 *
 *	delim -
 *		(optional) Character used for quoting. If not specified, the double quote character will be used (").
 *
 *	right_delim -
 *		(optional) The <string.quote> function can be used to specify a left and right character to be
 *		used for quoting. For example :
 *
 *			$(<string.quote> hello world, [, ])
 *
 *		will expand to :
 *
 *			[hello world]
 *
 *	escape -
 *		Escape character to be used during quoting if the supplied value contains the left/right
 *		delimiters. The default value is backslash.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_list_quote )
   {
	int		argc		=  wake_function_argc ( argv ) ;
	char *		list		=  argv [0] ;
	char		left_quote	=  '"' ;
	char		right_quote	=  '"' ;
	char		escape		=  '\\' ;


	// Get optional arguments, if specified
	switch  ( argc )
	   {
		case	2 :
			left_quote	= 
			right_quote	=  * argv [1] ;
			break ;

		case	3 :
			left_quote	=  * argv [1] ;
			right_quote	=  * argv [2] ;
			break ;

		case	4 :
			left_quote	=  * argv [1] ;
			right_quote	=  * argv [2] ;
			escape		=  * argv [3] ;
			break ;
	    }

	// No quoting is performed on empty lists
	if  ( ! wake_is_empty_list ( list ) )
	   {
		while  ( 1 ) 
		   {
			unsigned int	i, length ;
			char *		p	=  find_next_token ( ( const char ** ) & list, & length ) ;


			if  ( p  ==  NULL )
				break ;

			output	=  variable_buffer_output ( output, & left_quote, 1 ) ;

			for  ( i = 0 ; i  <  length ; i ++ )
			   {
				if  ( p [i]  ==  left_quote  ||  p [i]  ==  right_quote )
					output	=  variable_buffer_output ( output, & escape, 1 ) ;

				output	=  variable_buffer_output ( output, p + i, 1 ) ;
			    }

			output	=  variable_buffer_output ( output, & right_quote, 1 ) ;
			output  =  variable_buffer_output ( output, " ", 1 ) ;
		    }
	    }

	return ( output ) ;
    }
# endif		/* WAKE_EXTENSIONS */