/**************************************************************************************************************

    NAME
        utilities.c

    DESCRIPTION
        Utility functions for the Wake extensions.

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


// wake_trim -
//	Trims a string value and returns a pointer to the new value.
char *	wake_trim ( char *  value )
   {
	char *			buffer ;
	register char *		p		=  value ;
	int			length ;


	while  ( * p  &&  isspace ( ( int ) * p ) )
		p ++ ;

	length	=  strlen ( p ) ;
	buffer	=  xmalloc ( length + 1 ) ;
	strcpy ( buffer, p ) ;
	p	=  buffer + length - 1 ;

	while  ( p  >=  buffer  &&  isspace ( ( int ) * p ) )
		p -- ;

	* ( p + 1 )	=  0 ;

	return ( buffer ) ;
    }


// wake_integer_value -
//	Converts a string to an integer value.
//	Returns 1 if the input string contained a valid integer value, 0 otherwise. In this case, the 
//	contents of output is set to zero.
//	Spaces are ignored.
# define	SKIPSPACES()		while  ( * p  &&  isspace ( ( int ) * p ) ) p ++
# define	FAIL()			{ * output = 0 ; return ( 0 ) ; }

int 	wake_integer_value ( char *  input, int *  output )
   {
	int		base		=  10 ;
	char *		p		=  input ;
	int		sign		=  1 ;
	int		value		=  0 ;
	int		digit ;


	SKIPSPACES ( ) ;				// Spaces are allowed at the start of the string

	if  ( ! * p )					// Empty string - bad value
		FAIL ( ) ;

	// Process leading sign or base specified (0 or 0x)
	if  ( * p  ==  '-' )
	   {
		sign	=  -1 ;
		p ++ ;

		SKIPSPACES ( ) ;
	    }
	else if  ( * p  ==  '+' )
	   {
		p ++ ;
		SKIPSPACES ( ) ;
	    }
	else if  ( * p  ==  '0'  &&  * ( p + 1 ) )
	   {
		p ++ ;

		if  ( isdigit ( ( int ) * p ) )
			base		=  8 ;
		else if  ( toupper ( ( int ) * p )  ==  'X' )
		   {
			base		=  16 ;
			p ++ ;
		    }
		else if  ( toupper ( ( int ) * p )  ==  'O' )
		   {
			base		=  8 ;
			p ++ ;
		    }
	    }

	// End of string : no digits found so far so fail
	if  ( ! * p )
		FAIL ( ) ;

	// Process digits in input value
	while  ( * p  &&  isxdigit ( ( int ) * p ) )
	   {
		if  ( * p  >  '9' )
			digit	=  * p - 'A' + 10 ;
		else
			digit	=  * p - '0' ;

		if  ( digit  >=  base ) 
			FAIL ( ) ;

		value	=  ( value * base ) + digit ;
		p ++ ;
	    }

	SKIPSPACES ( ) ;				// Spaces are allowed at the end of the string

	// Non-digit found : complain
	if  ( * p )
		FAIL ( ) ;

	// All done, return
	* output	 =  ( value * sign ) ;

	return ( 1 ) ;
    }


// wake_double_value -
//	Converts a string representing a float value to a double value.
int	wake_double_value ( char *  input, double *  output )
   {
	char *		p		=  input + strlen ( input ) - 1 ;
	char *		tail ;
	double		value		=  0 ;
	int		integer_value ;


	// First, check if the value is not an integer value, with optional base specifier
	if  ( wake_integer_value ( input, & integer_value ) )
	   {
		* output	=  ( double ) integer_value ;

		return ( 1 ) ;
	    }

	// Input value may be a float ; find the last non-space character 
	while  ( p  >  input  &&  isspace ( ( int ) * p ) )
		p -- ;

	// Then check if it is a double
	if  ( p  !=  input )
	   {
		value		=  strtod ( input, & tail ) ;

		// strtod() stopped scan before the last non-space character : return failure
		if  ( tail  !=  p + 1 )
			FAIL ( ) ;
	    }

	* output	=  value ;

	return ( 1 ) ;
    }


// wake_is_empty_list :
//	Checks if a string is empty, null or containing only spaces.
int  	wake_is_empty_list ( char *  ptr )
   {
	if  ( ptr  ==  NULL  ||  ! * ptr )
		return ( 1 ) ;
	
	while  ( * ptr )
	   {
		if  ( ! isspace ( ( int ) * ptr ) )
			return ( 0 ) ;
		
		ptr ++ ;
	    }
	    
	return ( 1 ) ;
    }


// wake_function_argc -
//	Returns the number of arguments passed to a function.
int	wake_function_argc ( char **  argv )
   {
	int		i ;

	for  ( i = 0 ; argv [i]  !=  NULL ; i ++ )
		;

	return ( i ) ;
    }


// __wake_builtins_compare_by_name -
//	Called by __wake_sort_builtins to sort builtins by name.
static int	__wake_builtins_compare_by_name ( const void *  a, const void *  b )
   {
	const function_table_entry *		ap ;
	const function_table_entry *		bp ;
	int					res ;

	ap	=  ( * ( ( function_table_entry ** ) a ) ) ;
	bp	=  ( * ( ( function_table_entry ** ) b ) ) ;
	res	=  strcmp ( ap -> name, bp -> name ) ;

	return ( res ) ;
    }


// __wake_builtins_compare_by_category -
//	Called by __wake_sort_builtins to sort builtins by category.
static int	__wake_builtins_compare_by_category ( const void *  a, const void *  b )
   {
	const function_table_entry *		ap ;
	const function_table_entry *		bp ;
	int					catres ;


	ap	=  ( * ( ( function_table_entry ** ) a ) ) ;
	bp	=  ( * ( ( function_table_entry ** ) b ) ) ;
	catres	=  strcmp ( ap -> func_category, bp -> func_category ) ;

	if  ( catres )
		return ( catres ) ;
	else
		return ( strcmp ( ap -> name, bp -> name ) ) ;
    }


// wake_sort_builtins -
//	Sorts the builtins by name (name_or_category == 0) or by category (name_or_category != 0), if not already done.
function_table_entry **		sorted_builtin_functions_by_name		=  NULL ;
function_table_entry **		sorted_builtin_functions_by_category		=  NULL ;
static int			last_builtin_function_count			=  0 ;


void	wake_sort_builtins ( int  name_or_category )
   {
	// Sort the builtin functions by name then by category, if it is the first time call or if new builtins have been added since
	if  ( builtin_function_count  !=  last_builtin_function_count )
	   {
		int		functable_size		=  builtin_function_count * sizeof ( function_table_entry ) ;

		if  ( sorted_builtin_functions_by_name  !=  NULL )
			free ( sorted_builtin_functions_by_name ) ;

		sorted_builtin_functions_by_name	=  ( function_table_entry ** ) malloc ( functable_size ) ;
		memcpy ( sorted_builtin_functions_by_name, builtin_functions, functable_size ) ;
		qsort ( sorted_builtin_functions_by_name, builtin_function_count, sizeof ( function_table_entry * ), __wake_builtins_compare_by_name ) ;

		if  ( sorted_builtin_functions_by_category  !=  NULL )
			free ( sorted_builtin_functions_by_category ) ;

		sorted_builtin_functions_by_category	=  ( function_table_entry ** ) malloc ( functable_size ) ;
		memcpy ( sorted_builtin_functions_by_category, builtin_functions, functable_size ) ;
		qsort ( sorted_builtin_functions_by_category, builtin_function_count, sizeof ( function_table_entry * ), __wake_builtins_compare_by_category ) ;
	    }
    }


// wake_format_string -
//	Formats a string using the specified width.
//	indent_spaces gives the number of characters to be written after each newline (the first line will not be indented).
# define	TABSIZE			8

char *	wake_format_string ( char *  str, int  width, int  indent_spaces )
   {
	char *			newstr ;
	int			size			=  strlen ( str ) ;
	char *			p,
	     *			q ;
	char *			lookahead_p,
	     *			lookahead_p2 ;
	int			anchor			=  indent_spaces ;
	int			current_column		=  0 ;
	int			i, ch, count ;

	
	// Paranoia : if the supplied input string is empty, allocate an empty string.
	if  ( ! size )
	   {
		newstr		=  ( char * )  xmalloc ( 1 ) ;
		* newstr	=  '\0' ;
		return ( newstr ) ;
	    }

	// Allocate space for the new string (approximately...)
	newstr		=  xmalloc ( strlen ( str ) * 2 ) ;
	p		=  str ;
	q		=  newstr ;

	// Prints initial spaces for the first line
	for  ( i = 0 ; i < indent_spaces ; i ++ )
		* q ++ = ' ' ;

	current_column	=  indent_spaces ;

	// Loop through input characters
	while  ( * p )
	   {
		// Check if we need to put a line break
		lookahead_p	=  p ;

		while  ( * lookahead_p  &&  isspace ( ( int ) * lookahead_p ) )
			lookahead_p ++ ;

		lookahead_p2	=  lookahead_p ;

		while  ( * lookahead_p2  &&  ! isspace ( ( int ) * lookahead_p2 ) )
			lookahead_p2 ++ ;

		// The next input word may cause the current column to exceed the specified line width
		// If yes, put a line break then start the new line with the next word
		if  ( current_column + ( int ) ( lookahead_p2 - p )  >  width )
		   {
			* q ++		=  '\n' ;

			for  ( i = 0 ; i  <  indent_spaces ; i ++ )
				* q ++	= ' ' ;

			current_column	=  anchor ;
			p		=  lookahead_p ;
		    }


		// Next character
		ch	=  * p ;

		// If a new line has been started, skip leading spaces
		if  ( current_column  ==  anchor )
		   {
			while  ( * p  ==  ' ' )
				p ++ ;

			ch	= * p ;
		    }

		// Stop on end of string
		if  ( ! ch )
			break ;

		// Escape sequence
		if  ( ch  ==  '\\' )
		   {
			p ++ ;

			switch  ( * p )
			   {
				case	't'	:  ch	=  '\t' ; break ;
				case	'n'	:  ch	=  '\n' ; break ;
				case	'b'	:  ch	=  '\b' ; break ;
				case	'r'	:  ch	=  '\r' ; break ;
				case	'v'	:  ch	=  '\v' ; break ;
				default		:  ch	=  * p  ; break ;
			    }
		    }

		// Process next character
		switch ( ch )
		   {
			// Newline : start a new line with 'indent_spaces' spaces 
			case		'\n' :
				* q ++	=  '\n' ;

				for  ( i = 0 ; i  <  anchor ; i ++ )
					* q ++	=  ' ' ;

				current_column	=  anchor ;
				break ;

			// Tab : jump to the next tab position
			case		'\t' :
				if  ( ! ( current_column % TABSIZE ) )
					count	=  TABSIZE ;
				else
					count	=  current_column % TABSIZE ;

				while  ( count -- )
				   {
					if  ( current_column + 1  >  width )
					   {
						* q ++	=  '\n' ;

						for  ( i = 0 ; i  <  anchor ; i ++ )
							* q ++	=  ' ' ;

						current_column	=  anchor ;
						break ;
					    }

					* q ++	=  ' ' ;
					current_column ++ ;
				    }
				break ;

			// Anchor character ('\b' ) : future indentations will start at this position
			case		'\b' :
				anchor		=  current_column ;
				break ;

			// Other characters
			default :
				if  ( current_column + 1  >  width )
				   {
					* q ++	=  '\n' ;

					for  ( i = 0 ; i  <  anchor ; i ++ )
						* q ++	=  ' ' ;

					current_column	=  anchor ;

					while  ( isspace ( ( int ) * p ) )
						p ++ ;
				    }
				
				* q ++		=  ch ;
				current_column ++ ;
		    }

		p ++ ;			// Jump to next input character
	    }
	    
	// All done, return
	* q = 0 ;
	return ( newstr ) ;
    }
# endif		/* WAKE_EXTENSIONS */