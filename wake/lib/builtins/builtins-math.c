/**************************************************************************************************************

    NAME
        math.c

    DESCRIPTION
        Builtin functions related to maths and arithmetics.

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
# include	<math.h>


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_math_operation - Performs mathematical operations on variables.
 *
 *   PROTOTYPE
 *	$(<+> value1, value2)
 *
 *   DESCRIPTION
 *	Performs the specified math operation.
 *
 *==============================================================================================================*/
# define	DIVISION_BY_ZERO		"DIV#0" 

WAKE_BUILTIN ( wake_builtin_math_operation )
   {
	double		arg_values [ 256 ] ;
	int		argc		=  0 ;
	double		result		=  0 ;
	char		buffer [64] ;
	int		i ;
	int		length ;


	// Convert arguments to double values
	i		=  0 ;
	arg_values [0]	=  0 ;

	while  ( * argv )
	   {
		if  ( ** argv )
		   {
			double		value ;

			if  ( ! wake_double_value ( * argv, & value ) )
				ERROR_FLOATVAL ( i + 1, * argv, funcname ) ;

			arg_values [ argc ++ ]	=  value ;
		    }
		else
			arg_values [ argc ++ ]	=  0 ;

		argv ++ ;
		i ++ ;
	    }

	// Addition
	if  ( ! strcasecmp ( funcname, "<+>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	+=  arg_values [i] ;
	    }
	// Subtraction
	else if  ( ! strcasecmp ( funcname, "<->" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	-=  arg_values [i] ;
	    }
	// Multiplication
	else if  ( ! strcasecmp ( funcname, "<*>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	*=  arg_values [i] ;
	    }
	// Division - handle divisions by zero
	else if  ( ! strcasecmp ( funcname, "</>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
		   {
			if  ( arg_values [i] )
				result	/=  arg_values [i] ;
			else
			   {
				output	=  variable_buffer_output ( output, DIVISION_BY_ZERO, sizeof ( DIVISION_BY_ZERO ) - 1 ) ;

				return ( output ) ;
			    }
		    }
	    }
	// Integer division - handle divisions by zero
	else if  ( ! strcasecmp ( funcname, "<//>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
		   {
			if  ( arg_values [i] )
				result	=  floor ( result / arg_values [i] ) ;
			else
			   {
				output	=  variable_buffer_output ( output, DIVISION_BY_ZERO, sizeof ( DIVISION_BY_ZERO ) - 1 ) ;

				return ( output ) ;
			    }
		    }
	    }
	// Modulo
	else if  ( ! strcasecmp ( funcname, "<%>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
		   {
			if  ( arg_values [i] )
				result	=  fmod ( result, arg_values [i] ) ;
			else
			   {
				output	=  variable_buffer_output ( output, DIVISION_BY_ZERO, sizeof ( DIVISION_BY_ZERO ) - 1 ) ;

				return ( output ) ;
			    }
		    }
	    }
	// Power
	else if  ( ! strcasecmp ( funcname, "<**>" ) )
	   {
		result		=  pow ( arg_values [0], arg_values [1] ) ;
	    }
	// Incrementation
	else if  ( ! strcasecmp ( funcname, "<++>" ) )
	   {
		result		=  arg_values [0] + 1 ;
	    }
	// Decrementation
	else if  ( ! strcasecmp ( funcname, "<-->" ) )
	   {
		result		=  arg_values [0] - 1 ;
	    }
	// Binary and
	else if  ( ! strcasecmp ( funcname, "<&>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	=  ( int ) result  &  ( int ) arg_values [i] ;
	    }
	// Binary or
	else if  ( ! strcasecmp ( funcname, "<|>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	=  ( int ) result  |  ( int ) arg_values [i] ;
	    }
	// Binary xor
	else if  ( ! strcasecmp ( funcname, "<^>" ) )
	   {
		result		=  arg_values [0] ;

		for  ( i = 1 ; i  <  argc ; i ++ )
			result	=  ( int ) result  ^  ( int ) arg_values [i] ;
	    }
	// Left shift
	else if  ( ! strcasecmp ( funcname, "<<" ) )
	   {
		result		=  ( int ) arg_values [0]  << ( int ) arg_values [1] ;
	    }
	// Right shift
	else if  ( ! strcasecmp ( funcname, ">>" ) )
	   {
		result		=  ( int ) arg_values [0]  >> ( int ) arg_values [1] ;
	    }
	// Binary not
	else if  ( ! strcasecmp ( funcname, "<~>" ) )
	   {
		result		=  ~ ( int ) arg_values [0] ;
	    }

	// Try to return an integer value whenever possible
	if  ( ( int ) result  ==  result )
		length	=  sprintf ( buffer, "%d", ( int ) result ) ;
	// Otherwise, return a double value
	else
	   {
		length	=  sprintf ( buffer, "%g", result ) ;

		// Remove trailing zeros
		i	=  strlen ( buffer ) - 1 ;

		while  ( i  >=  0  &&  buffer [i]  ==  '0' )
		   {
			buffer [i]	=  0 ;
			i -- ;
		    }
	    }

	output	=  variable_buffer_output ( output, buffer, length ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_math_not - Performs a bitwise not of a value
 *
 *   PROTOTYPE
 *	$(<~> value)
 *
 *   DESCRIPTION
 *	Bitwise negation of the specified value.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_math_not )
   {
	char		buffer [64] ;
	double		value ;
	int		result ;
	int		length ;


	if  ( ! wake_double_value ( * argv, & value ) )
		ERROR_FLOATVAL ( 1, * argv, funcname ) ;

	result		=  ~( ( int ) value ) ;
	length		=  sprintf ( buffer, "%d", result ) ;

	output		=  variable_buffer_output ( output, buffer, length ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_math_eval
 *
 *   PROTOTYPE
 *	$(<=> expression)
 *
 *   DESCRIPTION
 *	Evaluates a math expression.
 *
 *==============================================================================================================*/

# define	OP_ADD			1
# define	OP_SUB			2
# define	OP_DIV			3
# define	OP_MUL			4


static double  math_eval ( char * expression ) 
   {
    }


WAKE_BUILTIN ( wake_builtin_math_eval )
   {
	char		buffer [64] ;
	double		value ;
	int		result ;
	int		length ;


	value		=  math_eval ( argv [0] ) ;
	length		=  sprintf ( buffer, "%g", result ) ;

	output		=  variable_buffer_output ( output, buffer, length ) ;

	return ( output ) ;
    }

# endif		/* WAKE_EXTENSIONS */