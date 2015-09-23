/**************************************************************************************************************

    NAME
        string.c

    DESCRIPTION
        Builtin functions related to wake internal data.

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
 *  Helper functions.
 *
 *==============================================================================================================*/

# define	ALIGN_LEFT		0
# define	ALIGN_RIGHT		1


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_strlen - Expands to the length of a string.
 *
 *   PROTOTYPE
 *	$(<string.length> string)
 *
 *   DESCRIPTION
 *	Expands to the length of the specified string, or to the empty string if the supplied value is empty.
 *
 *   PARAMETERS
 *	string -
 *		String whose length is to be measured.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_strlen )
   {
	char			buffer [32] ;
	unsigned int		size ;

	if  ( argv  ==  NULL  ||  argv [0]  ==  NULL )
		return ( output ) ;

	size	=  strlen ( argv [0] ) ;

	if  ( ! size )
		return ( output ) ;

	sprintf ( buffer, "%d", size ) ;
	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_compare - Entry point for all the <string.xxx> comparison functions.
 *
 *   PROTOTYPE
 *	$(<string.compare>	string1,string2)
 *	$(<string.casecompare>	string1,string2)
 *	$(<string.eq>		string1,string2)
 *	$(<string.caseeq>	string1,string2)
 *	$(<string.ne>		string1,string2)
 *	$(<string.casene>	string1,string2)
 *	$(<string.lt>		string1,string2)
 *	$(<string.caselt>	string1,string2)
 *	$(<string.gt>		string1,string2)
 *	$(<string.casegt>	string1,string2)
 *	$(<string.le>		string1,string2)
 *	$(<string.casele>	string1,string2)
 *	$(<string.ge>		string1,string2)
 *	$(<string.casege>	string1,string2)
 *
 *   DESCRIPTION
 *	<string.compare> and <string.casecompare> expand to -1 if 'string1' is less than 'string2', 0 if they
 *	are equal, and 1 if 'string1' is greater than 'string2'.
 *	The .eq, .ne, .lt, .gt, .le, .ge perform a logical comparison and expand to 1 if the result of the 
 *	comparison is true, and to the empty string otherwise. For example, <string.lt> expands to 1 if 'string1'
 *	is strictly less than 'string2', and to the empty string otherwise ('string1' >= 'string2').
 *
 *   PARAMETERS
 *	string1, string2 -
 *		Strings to be compared.
 *
 *==============================================================================================================*/
// Flags than condition the comparison and its result
# define	KEEP_LT					0x01		/* Retain the result if 'string1' is less than 'string2'	*/
# define	KEEP_EQ					0x02		/* ... or equal to						*/
# define	KEEP_GT					0x04		/* ... or greater than						*/
# define	KEEP_NE					( KEEP_LT | KEEP_GT )
# define	KEEP_LE					( KEEP_LT | KEEP_EQ )
# define	KEEP_GE					( KEEP_GT | KEEP_EQ )

// Defines an entry in the comparison_definition table
# define	COMPARE(name,cmpfunc,flags)		{ STRING_SIZE_TUPLE ( name ), ( ( unsigned char ) flags ), 0, cmpfunc }

// Checks if a flag equals the specified bits
# define	FLAGEQ(var,flags)			( ( var & ( flags ) )  ==  ( flags ) )

// Comparison table
static struct comparison_definition
   {
	char *		name ;						/* Comparison function name, after the '<string.' token		*/
	char		length ;					/* Comparison function name length, before the '>' token	*/
	unsigned char	flags ;						/* Flags that guide the comparison process			*/
	short int	reserved ;					/* To keep cmpfunc QWORD-aligned				*/
	int		( * cmpfunc ) ( const char *  a, const char *  b ) ;
    }  cmpdefs []	=
	  {
		  COMPARE ( "eq"		, strcmp	, KEEP_EQ ),
		  COMPARE ( "caseeq"		, strcasecmp	, KEEP_EQ ),
		  COMPARE ( "ne"		, strcmp	, KEEP_NE ),
		  COMPARE ( "casene"		, strcasecmp	, KEEP_NE ),
		  COMPARE ( "lt"		, strcmp	, KEEP_LT ),
		  COMPARE ( "caselt"		, strcasecmp	, KEEP_LT ),
		  COMPARE ( "gt"		, strcmp	, KEEP_GT ),
		  COMPARE ( "casegt"		, strcasecmp	, KEEP_GT ),
		  COMPARE ( "le"		, strcmp	, KEEP_LT | KEEP_EQ ),
		  COMPARE ( "casele"		, strcasecmp	, KEEP_LT | KEEP_EQ ),
		  COMPARE ( "ge"		, strcmp	, KEEP_GT | KEEP_EQ ),
		  COMPARE ( "casege"		, strcasecmp	, KEEP_GT | KEEP_EQ ),
		  { NULL }
           } ;


WAKE_BUILTIN ( wake_builtin_string_compare_simple )
   {
	const char *		real_funcname						=  funcname + sizeof ( "<string." ) - 1 ;
	int			status ;


	// Call strcmp() or strcasecmp() depending on the function name
	if  ( * ( real_funcname )  ==  'c'  &&  * ( real_funcname + 1 )  ==  'a' )	// strcasecmp
		status	=  strcasecmp ( argv [0], argv [1] ) ;
	else
		status  =  strcmp ( argv [0], argv [1] ) ;

	// Return result expansion
	if  ( status  <  0 )
		output	=  variable_buffer_output ( output, "-1", 2 ) ;
	else if  ( status  >  0 )
		output  =  variable_buffer_output ( output, "1", 1 ) ;
	else
		output	=  variable_buffer_output ( output, "0", 1 ) ;

	// All done, return
	return ( output ) ;
    }


WAKE_BUILTIN ( wake_builtin_string_compare )
   {
	const char *			real_funcname						=  funcname + sizeof ( "<string." ) - 1 ;
	struct comparison_definition *	e							=  cmpdefs ;
	unsigned char			flags							=  0 ;
	int				status ;
	int				keep ;
	int				( * cmpfunc ) ( const char *  a, const char *  b )	=  NULL ;


	// Search for the function name
	while  ( e -> name  !=  NULL )
	   {
		if  ( ! strncmp ( e -> name, real_funcname, e -> length ) )
		   {
			flags		=  e -> flags ;
			cmpfunc		=  e -> cmpfunc ;
			break ;
		    }

		e ++ ;
	    }

	// If not found, complain
	if  ( cmpfunc  ==  NULL )
		OS ( error, reading_file, "Internal error : wake_builtin_string_compare() has been called with an incorrect function name, '%s'.", 
				funcname ) ;

	// Perform the comparison
	status	=  cmpfunc ( argv [0], argv [1] ) ;

	// Build the result 
	keep	=  0 ;

	if  ( FLAGEQ ( flags, KEEP_NE ) )
		keep = ( status  !=  0 ) ;
	else if  ( FLAGEQ ( flags, KEEP_LE ) )
		keep = ( status  <=  0 ) ;
	else if  ( FLAGEQ ( flags, KEEP_LT ) )
		keep = ( status  <   0 ) ;
	else if  ( FLAGEQ ( flags, KEEP_GE ) )
		keep = ( status  >=  0 ) ;
	else if  ( FLAGEQ ( flags, KEEP_GT ) )
		keep = ( status  >   0 ) ;
	else
		keep = ( status  ==  0 ) ;

	// If the comparison result is non-zero, then expand to "1"
	if  ( keep )
		output	=  variable_buffer_output ( output, "1", 1 ) ;

	// All done, return
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_search - Searches a substring within a string.
 *
 *   PROTOTYPE
 *	$(<string.search> string1,string2)
 *	$(<string.casesearch> string1,string2)
 *
 *   DESCRIPTION
 *	Searches the substring 'string2' within 'string1'. Expands to its position if the searched string has 
 *	been found, or to the empty string if no match.
 *
 *   PARAMETERS
 *	string1 -
 *		String to search in.
 *
 *	string2 -
 *		String to be searched.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_search )
   {
	char			buffer [32] ;
	char *			status ;


	if  ( * ( funcname + 8 )  ==  'c' )		// $(string.casesearch ...)
		status	=  strcasestr ( argv [0], argv [1] ) ;
	else
		status	=  strstr ( argv [0], argv [1] ) ;

	if  ( status  ==  NULL )
		return ( output ) ;

	sprintf ( buffer, "%d", ( int ) ( status - argv [0] ) ) ;
	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_reverse - Reverses a string.
 *
 *   PROTOTYPE
 *	$(<string.reverse> string)
 *
 *   DESCRIPTION
 *	Reverses the specified string.
 *
 *   PARAMETERS
 *	string -
 *		String to be reversed.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_reverse )
   {
	char *			p,
	     *			q,
	     *			start ;
	int			length, i ;

	if  ( argv  ==  NULL  ||  argv [0]  ==  NULL  ||  ! * argv [0] )
		return ( output ) ;

	i	=  length	=  strlen ( argv [0] ) ;
	p	=  start	=  xmalloc ( length + 1 ) ;
	q	=  argv [0] + length - 1 ;


	while  ( i -- )
		* p ++	=  * q  -- ;

	* p	=  '\0' ;

	output	=  variable_buffer_output ( output, start, length ) ;
	free ( start ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_change_case - Converts a string to upper- or lower- case.
 *
 *   PROTOTYPE
 *	$(<string.tolower> string)
 *	$(<string.toupper> string)
 *
 *   DESCRIPTION
 *	Converts the specified string to lower- or upper-case.
 *
 *   PARAMETERS
 *	string -
 *		String to be converted.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_change_case )
   {
	char *			p ;


	if  ( argv  ==  NULL  ||  argv [0]  ==  NULL  ||  ! * argv [0] )
		return ( output ) ;

	p	=  argv [0] ;

	if  ( * ( funcname + 10 )  ==   'l' )					// tolower
	   {
		do
		   {
			* p	=  tolower ( ( int ) * p ) ;
		    }  while  ( * ( ++ p ) ) ;
 	    }
	else									// toupper
	   {
		do
		   {
			* p	=  toupper ( ( int ) * p ) ;
		    }  while  ( * ( ++ p ) ) ;
 	    }

	output	=  variable_buffer_output ( output, argv [0], ( int ) ( p - argv [0] ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_implode - Implements the PHP function implode().
 *
 *   PROTOTYPE
 *	$(<string.implode> sep,list)
 *
 *   DESCRIPTION
 *	Implements the PHP function implode() ie, concatenates all elements in 'list' using the specified separator
 *	'sep'.
 *
 *   PARAMETERS
 *	sep -
 *		Separator string to use when concatenating two elements of 'list'.
 *
 *	list -
 *		List to be implode'd.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_implode )
   {
	char *			sep		=  argv [0] ;
	int			sep_length	=  strlen ( sep ) ;
	char *			list		=  argv [1] ;
	char *			item ;
	int			count		=  0 ;


	do
	   {
		while  ( * list  &&  isspace ( ( int ) * list ) )
			list ++ ;

		item		=  list ;

		while  ( * list  &&  ! isspace ( ( int ) * list ) )
			list ++ ;

		if  ( item  !=  list )
		   {
			if  ( count )
				output	=  variable_buffer_output ( output, sep, sep_length ) ;

			output	=  variable_buffer_output ( output, item, ( int ) ( list - item ) ) ;
		    }

		count ++ ;
	    }  while  ( * list ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_explode - Implements the PHP function explode().
 *
 *   PROTOTYPE
 *	$(<string.explode> sep,list)
 *
 *   DESCRIPTION
 *	Implements the PHP function explode() ie, makes a list from 'list' using the specified separator as an
 *	item separator.
 *
 *   PARAMETERS
 *	sep -
 *		Separator string to use when exploding 'list'.
 *
 *	list -
 *		List to be explode'd.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_explode )
   {
	char 	* sep 			=  argv [0] ;				// Separator
	int 	sep_length 		=  strlen ( sep ) ;			// Separator length
	char 	* list 			=  argv [1] ;				// List
	char  	* p 			=  list ;				// Pointer to the beginning of the current item
	char 	* q 			=  p ;					// Pointer to the current character
	int 	had_item 		=  0 ;					// Number of list items encountered so far
	
	
	// Loop through list characters
	while  ( * q )
	   {
		// If a separator is found, then copy the previous characters as a list item
		if  ( * q  ==  * sep  &&  ! strncmp ( q, sep, sep_length ) )
		   {
			// Add a space separator if we already extracted one or more items
			if  ( had_item )
				output 		=  variable_buffer_output ( output, " ", 1 ) ;
			
			// Collect current item
			output 		=  variable_buffer_output ( output, p, ( int ) ( q - p ) ) ;
			had_item ++ ;
			
			// Go past the current separator
			q	       +=  sep_length ;
			p 		=  q ;
		    }
		else
			q ++ ;
	    }

	// If characters remain after the last found separator, collect them to form a new item
	// If no characters have been found after the last separator, then do nothing, since it will be of no use to add an extra
	// space at the end of the expanded variable
	if  ( p  <  q )
	   {
		if  ( had_item )
			output 		=  variable_buffer_output ( output, " ", 1 ) ;
		
		output 	=  variable_buffer_output ( output, p, ( int ) ( q - p ) ) ;
	    }
   
	// All done, return
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_substring - Returns a substring of a string
 *
 *   PROTOTYPE
 *	$(<string.substring> string, start [, length])
 *
 *   DESCRIPTION
 *	Returns a substring of a string.
 *
 *   PARAMETERS
 *	string -
 *		Subject string.
 *
 *	start -
 *		Start index (zero-based).
 *
 *	length -
 *		Number of characters to extract. If not specified, all characters from 'start' up to the end of
 *		the string will be extracted.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_substring )
   {
	char 		* s 		=  argv [0] ;
	int 		string_length 	=  strlen ( s ) ;
	int 		start ;
	int 		length 		= 0 ;
	

	if  ( ! wake_integer_value ( argv [1], & start ) )
		ERROR_INTVAL ( 2, argv [1], funcname ) ;
	
	if  ( argv [2]  !=  NULL  &&  ! wake_integer_value ( argv [2], & length ) )
		ERROR_INTVAL ( 3, argv [2], funcname ) ;

	if  ( start  <  string_length )
	   {
		length 		=  MIN ( length, string_length - start ) ;
		output 		=  variable_buffer_output ( output, s + start, length ) ;
	    }
	    
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_chr - chr() function
 *
 *   PROTOTYPE
 *	$(<string.chr> integer)
 *
 *   DESCRIPTION
 *	Converts an integer to the equivalent ascii character.
 *
 *   PARAMETERS
 *	integer -
 *		Value to be converted as an ascii character.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_chr )
   {
	int 		value ;
	char		buffer [1] ;

	wake_integer_value ( argv [0], & value ) ;

	if  ( ! wake_integer_value ( argv [0], & value ) )
		ERROR_INTVAL ( 1, argv [0], funcname ) ;

	* buffer	=  ( char ) value ;
	output		=  variable_buffer_output ( output, buffer, 1 ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_repeat - Repeats a string
 *
 *   PROTOTYPE
 *	$(<string.repeat> string,count)
 *
 *   DESCRIPTION
 *	Repeats a string the specified number of times.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_string_repeat )
   {
	char *		str		=  argv [0] ;
	int		str_length	=  strlen ( str ) ;
	int		count ;

	if  ( ! wake_integer_value ( argv [1], & count )  ||  count  <  0 )
		ERROR_INTVAL ( 2, argv [1], funcname ) ;

	while  ( count -- )
		output	=  variable_buffer_output ( output, str, str_length ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_sprintf - String formatting.
 *
 *   PROTOTYPE
 *	$(<string.sprintf> format [, arg...])
 *
 *   DESCRIPTION
 *	Formats a string like the C printf() function.
 *	This is a dirty implementation that scans the format string for format specifiers, and :
 *	- extracts every format specifier
 *	- Individually call the sprintf() function with the extracted format specifier, using the appropriate
 *	  argument
 *
 *==============================================================================================================*/
# define	SPRINTF_BUFLEN			4096 

static char	sprintf_buffer [ SPRINTF_BUFLEN ] ;
static int	sprintf_buffer_length		=  0 ;


static void	sprintf_bflush ( char **  output )
   {
	if  ( sprintf_buffer_length )
	   {
		* output			=  variable_buffer_output ( * output, sprintf_buffer, sprintf_buffer_length ) ;
		sprintf_buffer_length	=  0 ;
	    }
    }


static void	sprintf_bch ( char **  output, char  ch )
   {
	if  ( sprintf_buffer_length + 1  >  SPRINTF_BUFLEN )
		sprintf_bflush ( output ) ;

	sprintf_buffer [ sprintf_buffer_length ++ ]	=  ch ;
    }


static void	sprintf_bstr ( char **  output, char *  new_string )
   {
	int		length		=  strlen ( new_string ) ;


	if  ( length  >  SPRINTF_BUFLEN  ||  sprintf_buffer_length + length  >  SPRINTF_BUFLEN )
	   {
		sprintf_bflush ( output ) ;
		* output		=  variable_buffer_output ( * output, new_string, length ) ;
	    }
	else
	    {
		strcpy ( sprintf_buffer + sprintf_buffer_length, new_string ) ;
		sprintf_buffer_length	+=  length ;
	     }
    }



WAKE_BUILTIN ( wake_builtin_string_sprintf )
   {
	static char	printf_specifiers []	=  "diouxXfeEgGcs" ;			// Recognized printf() specifiers

	int		argc			=  wake_function_argc ( argv ) ;	// We need to know how many arguments were specified in order to
											// replace the missing ones with NULL values, in order to avoid
											// a segmentation fault
	int		next_arg_index		=  1 ;					// Next argument for next format specifier
	char		format_buffer [ 1024 ] ;					// Holds the current format specifier
	char *		buffer			=  NULL ;
	int		buffer_size		=  0 ;	
	char *		p			=  argv [0] ;				// pointer to the format string
	char *		q ;


	// Loop through format string characters
	while  ( * p )
	   {
		// All regular characters are copied as is from the format string to the output buffer
		if  ( * p  !=  '%' )
			sprintf_bch ( & output, * p ) ;
		// '%%' construct : also copy as is
		else if  ( * ( p + 1 )  ==  '%' )
		   {
			sprintf_bch ( & output, '%' ) ;
			sprintf_bch ( & output, '%' ) ;
			p ++ ;
		    }
		// Format specifier found
		else
		   {
			// Jump from the percent sign to the character specifying the display format
			p ++ ;
			q	=  p ;

			while  ( * q  &&  strchr ( printf_specifiers, * q )  ==  NULL )
				q ++ ;

			// If found, process the specification
			if  ( * q )
			   {
				// Extract the format specification, from the leading '%' sign to the display format
				int		length				=  q - p + 2 ;
				int		width				=  0 ;
				char *		dp				=  format_buffer ;

				strncpy ( format_buffer, p - 1, length ) ;
				format_buffer [ length ]	=  0 ;

				// Try to see if there is a width specifier
				while  ( * dp  &&  ! isdigit ( ( int ) * dp ) )
					dp ++ ;

				if  ( * dp )
					width	=  atoi ( dp ) ;
				else	
					width	=  32 ;

				// Adapt our internal sprintf() buffer to the size of the string
				if  ( width  >  buffer_size  ||  
					( * q  ==  's'  &&  strlen ( argv [ next_arg_index ] )  >  buffer_size ) )
				   {
					buffer_size	=  ( ( ( buffer_size + width + 1 + 1024 ) / 1024 ) * 1024 ) ;

					if  ( buffer  !=  NULL )
						buffer	=  xrealloc ( buffer, buffer_size ) ;
					else
						buffer  =  malloc ( buffer_size ) ;
				    }

				// Process the display format, which is where the 'q' variable points
				switch ( * q )
				   {
					// A character
					case	'c' :
						if  ( next_arg_index + 1  <=  argc )
							sprintf_bch ( & output, * argv [ next_arg_index ++ ] ) ;
						break ;

					// An integer
					case  'd' :  case  'i' :  case  'o' :  case  'u' :  case  'x' :  case  'X' :
						if  ( next_arg_index + 1  <=  argc )
						   {
							int		integer_value ;

							if  ( ! wake_integer_value ( argv [ next_arg_index ], & integer_value ) )
								ERROR_INTVAL ( next_arg_index - 1, argv [ next_arg_index ], funcname ) ;

							sprintf ( buffer, format_buffer, integer_value ) ;
							sprintf_bstr ( & output, buffer ) ;
							next_arg_index ++ ;
						    }

						break ;

					// A float
					case  'f' :  case  'e' :  case  'E' :  case  'g' :  case  'G' :
						if  ( next_arg_index + 1  <=  argc )
						   {
							double		float_value ;

							if  ( ! wake_double_value ( argv [ next_arg_index ], & float_value ) )
								ERROR_FLOATVAL ( next_arg_index - 1, argv [ next_arg_index ], funcname ) ;

							sprintf ( buffer, format_buffer, float_value ) ;
							sprintf_bstr ( & output, buffer ) ;
							next_arg_index ++ ;
						    }

						break ;

					// A string
					case  's' :
						if  ( next_arg_index + 1  <=  argc )
						   {
							sprintf ( buffer, format_buffer, argv [ next_arg_index ] ) ;
							sprintf_bstr ( & output, buffer ) ;
						    }

						break ;
				    }

				p	=  q ;
			    }
			// Otherwise output it as is (ie, a '%' format specifier without an invalid display format character will be displayed as is)
			else
				sprintf_bstr ( & output, p ) ;
		    }

		p ++ ;
	    }

	sprintf_bflush ( & output ) ;

	if  ( buffer  !=  NULL ) 
		free ( buffer ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_string_quote - Quotes a string.
 *
 *   PROTOTYPE
 *	$(<string.quote> value [, delim [, right_delim [, escape ] ] ])
 *
 *   DESCRIPTION
 *	Quotes a string.
 *
 *   PARAMETERS
 *	value -
 *		String to be quoted.
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
WAKE_BUILTIN ( wake_builtin_string_quote )
   {
	int		argc		=  wake_function_argc ( argv ) ;
	char *		value		=  argv [0] ;
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

	// Perform the quoting
	output	=  variable_buffer_output ( output, & left_quote, 1 ) ;

	while  ( * value )
	   {
		// Escape the current character if it is either a left or right quote
		if  ( * value  ==  left_quote  ||  * value  ==  right_quote )
			output	=  variable_buffer_output ( output, & escape, 1 ) ;

		output	=  variable_buffer_output ( output, value, 1 ) ;

		value ++ ;
	    }

	output	=  variable_buffer_output ( output, & right_quote, 1 ) ;

	return ( output ) ;
    }

# endif		/* WAKE_EXTENSIONS */