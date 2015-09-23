/**************************************************************************************************************

    NAME
        logical.c

    DESCRIPTION
        Builtin logical functions.

    NOTES
	A string expands to boolean true if it is non-empty, or false otherwise.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/05]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"wake.h"


/*==============================================================================================================

	Definitions for the builtin_ifcompare() function.

 *==============================================================================================================*/
// Comparisons on two values are performed with the strcmp/strcasecmp function for string values, or by substracting
// a value from another for numeric values. Both methods return a negative value if value 1 is less than value 2, 0 
// if they are equal, and a positive value if value 1 is greater than value 2.
// The logical functions can thus be expressed as a combination of the LESS, EQUAL and GREATER bits to be compared with
// the result of the comparison, after it has been converted into a LESS/EQUAL/GREATER constant.
# define	LESS			0x01
# define	EQUAL			0x02
# define	GREATER			0x04

// Function types
# define	IFLT			( LESS )
# define	IFLE			( LESS | EQUAL )
# define	IFEQ			( EQUAL )
# define	IFNE			( LESS | GREATER )
# define	IFGE			( EQUAL | GREATER )
# define	IFGT			( GREATER )

// Function flags
# define	FLAG_NONE		0
# define	FLAG_NUMERIC		0x01
# define	FLAG_CASE_INSENSITIVE	0x02

// Holds the definition of logical comparison functions
typedef struct logical_if
   {
	int		type ;
	char *		name ;
	int		flags ;
    }  logical_if ;

static logical_if	logical_if_functions []		=
   {
	{ IFLT, "<iflt>", FLAG_NONE },
	{ IFLE, "<ifle>", FLAG_NONE },
	{ IFEQ, "<ifeq>", FLAG_NONE },
	{ IFNE, "<ifne>", FLAG_NONE },
	{ IFGE, "<ifge>", FLAG_NONE },
	{ IFGT, "<ifgt>", FLAG_NONE },

	{ IFLT, "<iflt/d>", FLAG_NUMERIC },
	{ IFLE, "<ifle/d>", FLAG_NUMERIC },
	{ IFEQ, "<ifeq/d>", FLAG_NUMERIC },
	{ IFNE, "<ifne/d>", FLAG_NUMERIC },
	{ IFGE, "<ifge/d>", FLAG_NUMERIC },
	{ IFGT, "<ifgt/d>", FLAG_NUMERIC },

	{ IFLT, "<iflt/i>", FLAG_CASE_INSENSITIVE },
	{ IFLE, "<ifle/i>", FLAG_CASE_INSENSITIVE },
	{ IFEQ, "<ifeq/i>", FLAG_CASE_INSENSITIVE },
	{ IFNE, "<ifne/i>", FLAG_CASE_INSENSITIVE },
	{ IFGE, "<ifge/i>", FLAG_CASE_INSENSITIVE },
	{ IFGT, "<ifgt/i>", FLAG_CASE_INSENSITIVE },

	{ 0, 0, 0 }
    } ;

// find_logical_function -
//	Finds the entry corresponding to the specified function name.
static logical_if *	find_logical_function ( const char *  name )
   {
	logical_if *	p ;

	for  ( p = logical_if_functions ; p -> name  !=  NULL ; p ++ )
	   {
		if  ( ! strcmp ( p -> name, name ) )
			return ( p ) ;
	    }

	fatal ( reading_file, 0, "The '%s' function should be defined but does not appear in the list of existing logical functions", name ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	Logical functions.
 *
 *   PROTOTYPE
 *	$(<if> cond, value1 [, value2 ])
 *	$(<ifnot> cond, value1 [, value2 ])
 *	$(<if/d> cond, value1 [, value2 ])
 *	$(<ifnot/d> cond, value1 [, value2 ])
 *
 *	$(<iflt> cond1, cond2, value1 [, value2 ])
 *	$(<ifle> cond1, cond2, value1 [, value2 ])
 *	$(<ifgt> cond1, cond2, value1 [, value2 ])
 *	$(<ifge> cond1, cond2, value1 [, value2 ])
 *	$(<ifeq> cond1, cond2, value1 [, value2 ])
 *	$(<ifne> cond1, cond2, value1 [, value2 ])
 *
 *	$(<iflt/i> cond1, cond2, value1 [, value2 ])
 *	$(<ifle/i> cond1, cond2, value1 [, value2 ])
 *	$(<ifgt/i> cond1, cond2, value1 [, value2 ])
 *	$(<ifge/i> cond1, cond2, value1 [, value2 ])
 *	$(<ifeq/i> cond1, cond2, value1 [, value2 ])
 *	$(<ifne/i> cond1, cond2, value1 [, value2 ])
 *
 *	$(<iflt/d> cond1, cond2, value1 [, value2 ])
 *	$(<ifle/d> cond1, cond2, value1 [, value2 ])
 *	$(<ifgt/d> cond1, cond2, value1 [, value2 ])
 *	$(<ifge/d> cond1, cond2, value1 [, value2 ])
 *	$(<ifeq/d> cond1, cond2, value1 [, value2 ])
 *	$(<ifne/d> cond1, cond2, value1 [, value2 ])
 *
 *   DESCRIPTION
 *	Conditional if statements.
 *	<if> evaluates to value1 if "cond" is non-empty, and to value2 otherwise (<ifnot/d> is the reverse function).
 *	<if/d> evaluates to value1 if "cond" is either empty or zero.
 *	The <ifxx> functions perform case-sensitive comparisons on cond1 and cond2 ; <ifxx/i> perform case-insensitive comparisons ;
 *	and <ifxx/d> perform numeric comparisons.
 *
 *   PARAMETERS
 *	cond -
 *		Condition to be tested (false = empty string, true = non empty string).
 *
 *	cond1, cond2 -
 *		Values to be compared. 
 *
 *	value1 -
 *		String to be expanded if the condition is true.
 *
 *	value2 -
 *		String to be expanded if the condition is false.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_logical_if )
   {
	int		condition ;
	int		negate		=  ( ! strncmp ( funcname, "<ifnot", 6 ) ) ?  1 : 0 ;
	int		numeric		=  ( strstr ( funcname, "/d>" )  !=  NULL ) ?  1 : 0 ;
	char *		value1		=  argv [1] ;
	char *		value2		=  ( argv [2]  ==  NULL ) ?  "" : argv [2] ;
	char *		value ;


	if  ( numeric )
	   {
		double	decimal_condition ;
		char *	char_condition		=  wake_trim ( argv [0] ) ;

		if  ( ! * char_condition )
			condition	=  0 ;
		else if  ( ! wake_double_value ( argv [0], & decimal_condition ) )
			ERROR_FLOATVAL ( 1, argv [0], funcname ) ;
		else
			condition	=  ( int ) decimal_condition ;

		free ( char_condition ) ;
	    }
	else
	   {
		char *	char_condition		=  wake_trim ( argv [0] ) ;

		condition	=  ( * char_condition )  !=  0 ;

		free ( char_condition ) ;
	    }

	if  ( condition )
		value		=  ( negate ) ?  value2 : value1 ;
	else
		value		=  ( negate ) ?  value1 : value2 ;

	output	=  variable_buffer_output ( output, value, strlen ( value ) ) ;

	return ( output ) ;
    }


WAKE_BUILTIN ( wake_builtin_logical_ifcompare )
   {
	logical_if *	funcdef			=  find_logical_function ( funcname ) ;
	char *		cond1			=  argv [0] ;
	char *		cond2			=  argv [1] ;
	char *		value1			=  argv [2] ;
	char *		value2			=  ( argv [3]  ==  NULL ) ?  "" : argv [3] ;
	char *		value ;
	int		comparison_result ;


	// NUMERIC flag : cond1 and cond2 must be numeric, but an empty string is also accepted for the value zero
	if  ( funcdef -> flags  &  FLAG_NUMERIC )	
	   {
		double	decimal_condition1,
			decimal_condition2 ;
		char *	char_condition1		=  wake_trim ( cond1 ),
		     *	char_condition2		=  wake_trim ( cond2 ) ;
		double	diff ;


		if  ( ! * char_condition1 )
			decimal_condition1	=  0 ;
		else if  ( ! wake_double_value ( cond1, & decimal_condition1 ) )
			ERROR_FLOATVAL ( 1, cond1, funcname ) ;

		if  ( ! * char_condition2 )
			decimal_condition2	=  0 ;
		else if  ( ! wake_double_value ( cond2, & decimal_condition2 ) )
			ERROR_FLOATVAL ( 1, cond2, funcname ) ;

		diff	=  decimal_condition1 - decimal_condition2 ;

		if  ( diff  <  0 )
			comparison_result	=  -1 ;
		else if  ( diff  >  0 )
			comparison_result	=  +1 ;
		else
			comparison_result	=   0 ;

		free ( char_condition1 ) ;
		free ( char_condition2 ) ;
	    }
	// Otherwise, perform a normal string comparison
	else
	   {
		if  ( funcdef -> flags  &  FLAG_CASE_INSENSITIVE )
			comparison_result	=  strcasecmp ( cond1, cond2 ) ;
		else
			comparison_result	=  strcmp ( cond1, cond2 ) ;
	    }

	// Change the comparison result to one of the constants LESS, GREATER or EQUAL
	if  ( comparison_result  <  0 )
		comparison_result	=  LESS ;
	else if  ( comparison_result  >  0 )
		comparison_result	=  GREATER ;
	else
		comparison_result	=  EQUAL ;

	value	=  ( comparison_result  &  funcdef -> type ) ?  value1 : value2 ;
	output	=  variable_buffer_output ( output, value, strlen ( value ) ) ;

	return ( output ) ;
    }


# endif		/*  __WAKE_EXTENSIONS  */