/**************************************************************************************************************

    NAME
        misc.c

    DESCRIPTION
        Various builtin functions.

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
# include	"counter.h"
# include	<uuid/uuid.h>



/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_random - Generates a random number.
 *
 *   PROTOTYPE
 *	$(<random> [low,high])
 *
 *   DESCRIPTION
 *	Generates a random number.
 *
 *   PARAMETERS
 *	low, high -
 *		Low and high value for the random number. If not specified, the returned value will be a double
 *		comprised between 0 and 1.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_random )
   {
	static int 	randomized 	=  0 ;
	int 		low 		=  0 ;
	int 		high 		=  RAND_MAX ;
	int 		value ;
	int 		result ;
	char 		buffer [64] ;
	
	
	if  ( argv [0]  !=  NULL )
	   {
		if  ( * argv [0]  &&  ! wake_integer_value ( argv [0], & low ) )
			ERROR_INTVAL ( 1, argv [0], funcname ) ;

		if  ( argv [1]  !=  NULL )
		   {
			if  ( * argv [1]  &&  ! wake_integer_value ( argv [1], & high ) )
				ERROR_INTVAL ( 2, argv [1], funcname ) ;
		    }
	    }

	if  ( ! randomized )
	   {
		srand ( time ( NULL ) ) ;
		randomized 	=  1 ;
	    }

	value 	=  rand ( ) ;
	
	if  ( ! high )
		high 	=  RAND_MAX ;
	
	if  ( low  >  high )
	   {
		low 	=  0 ;
		high 	=  RAND_MAX ;
	    }
	
	result 	=  ( value % ( high - low ) ) + low ;
	sprintf ( buffer, "%d", result ) ;
	
	output 	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;
	
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_randomize - Randomizes the pseudo-number generator.
 *
 *   PROTOTYPE
 *	$(<randomize>)
 *
 *   DESCRIPTION
 *	Randomizes the pseudo-number generator.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_randomize )
   {
	srand ( time ( NULL ) ) ;
	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_guid - Generates a GUID.
 *
 *   PROTOTYPE
 *	$(<guid>)
 *
 *   DESCRIPTION
 *	Generates a GUID.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_guid )
   {
	uuid_t		uuid ;
	char		buffer [40] ;

	uuid_generate ( uuid ) ;

	sprintf ( buffer, "{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			uuid [0], uuid [1], uuid [2], uuid [3], uuid [4], uuid [5], uuid [6], uuid [7], 
			uuid [8], uuid [9], uuid [10], uuid [11], uuid [12], uuid [13], uuid [14], uuid [15] ) ;

	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_rawguid - Generates a GUID.
 *
 *   PROTOTYPE
 *	$(<rawguid>)
 *
 *   DESCRIPTION
 *	Generates a raw GUID (with only hex digits).
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_rawguid )
   {
	uuid_t		uuid ;
	char		buffer [40] ;

	uuid_generate ( uuid ) ;

	sprintf ( buffer, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
			uuid [0], uuid [1], uuid [2], uuid [3], uuid [4], uuid [5], uuid [6], uuid [7], 
			uuid [8], uuid [9], uuid [10], uuid [11], uuid [12], uuid [13], uuid [14], uuid [15] ) ;

	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *	Counter support functions.
 *
 *==============================================================================================================*/

static counter *		counter_store		=  NULL ;
static counter *		counter_store_tail	=  NULL ;


// counter_define -
//	Defines a counter of the specified id, start value and increment.
counter *	counter_define ( char *  id, int  start, int  increment )
   {
	counter *		pc		=  counter_find ( id ) ;


	// Counter already exists : just reset it and return
	if  ( pc  !=  NULL )
	    {
		pc -> start	=  start ;
		pc -> increment	=  increment ;
		pc -> current	=  pc -> start ;

		return ( pc ) ;
	    }

	// Counter does not exist : create a new one
	pc			=  ( counter * ) xmalloc ( sizeof ( counter ) ) ;
	pc -> name		=  xstrdup ( id ) ;
	pc -> start		=  start ;
	pc -> increment		=  increment ;
	pc -> current		=  start ;

	// Insert the new counter into the linked list
	if  ( counter_store  ==  NULL )
	   {
		counter_store			=  pc ;
		counter_store_tail		=  pc ;
	    }
	else
	   {
		counter_store_tail -> next	=  pc ;
		counter_store_tail		=  pc ;
	    }

	pc -> next		=  NULL ;

	// All done, return
	return ( pc ) ;
    }


// counter_find -
//	Retrieves a counter structure.
//	If the retrieve_value parameter is non-zero, the current counter value will be post-incremented.
counter *	counter_find ( char *  id )
   {
	counter *	pc ;

	for  ( pc = counter_store ; pc  !=  NULL ; pc = pc -> next )
	   {
		if  ( ! strcmp ( pc -> name, id ) )
			return ( pc ) ;
	    }

	return ( NULL ) ;
    }


// counter_undefine -
//	Undefines a counter. No that much useful but has been provided for consistency reasons.
void	counter_undefine ( char *  id )
   {
	counter *	pc ;
	counter *	last_pc		=  NULL ;

	for  ( pc = counter_store ; pc  !=  NULL ; pc = pc -> next )
	   {
		if  ( ! strcmp ( pc -> name, id ) )
		   {
			if  ( pc  ==  counter_store )
			   {
				counter_store		=  pc -> next ;

				if  ( counter_store  ==  NULL )
					counter_store_tail	=  NULL ;
			    }
			else 
			   {
				last_pc -> next		=  pc -> next ;

				if  ( pc  ==  counter_store_tail )
					counter_store_tail	=  last_pc ;
			    }

			break ;
		    }

		last_pc		=  pc ;
	    }
    }


// counter_reset -
//	Resets a counter to its initial value.
void	counter_reset ( counter *  pc )
   {
	pc -> current	=  pc -> start ;
    }


// counter_value -
//	Retrieves the next value of a counter.
int	counter_value ( counter *  pc )
   {
	int		value	=  pc -> current ;

	pc -> current	+=  pc -> increment ;

	return ( value ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	Counter-related functions.
 *
 *   PROTOTYPE
 *	$(<defcounter> id [, start [, increment ] ])
 *	$(<counter> id [, start [, increment ] ])
 *	$(<undefcounter> id)
 *
 *   DESCRIPTION
 *	Counters are like variables whose value is post-incremented after retrieving it.
 *	Each counter has a name, a starting integer value and an increment, which is added to the current value
 *	after it has been retrieved.
 *	The $(<defcounter>) function creates a counter with an initial value of of "start" and the specified
 *	increment.
 *	$(<counter>) retrieves the next value of the specified counter. If it does not exist, it will be created
 *	$(<undefcounter>) undefines an existing counter. 
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_counter )
   {
	int		argc		=  wake_function_argc ( argv ) ;
	char *		name		=  argv [0] ;
	int		start		=  1, 
			increment	=  1 ;
	counter *	pc ;
	char		buffer [32] ;


	// Retrieve start and increment values
	if  ( argc  >  1 )
	   {
		if  ( ! wake_integer_value ( argv [1], & start ) )
			ERROR_INTVAL ( 2, argv [1], funcname ) ;

		if  ( argc  >  2 )
		  {
			if  ( ! wake_integer_value ( argv [2], & increment ) )
				ERROR_INTVAL ( 3, argv [2], funcname ) ;
		   }
	    }

	// Find the counter ; create it if undefined
	pc	=  counter_find ( name ) ;

	if  ( pc  ==  NULL )
		pc	=  counter_define ( name, start, increment ) ;

	// Retrieve the counter value to the output buffer
	sprintf ( buffer, "%d", counter_value ( pc ) ) ;
	output		 =  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	// All done, return
	return ( output ) ;
    }


WAKE_BUILTIN ( wake_builtin_defcounter )
   {
	int		argc		=  wake_function_argc ( argv ) ;
	char *		name		=  argv [0] ;
	int		start		=  1, 
			increment	=  1 ;


	// Retrieve start and increment values
	if  ( argc  >  1 )
	   {
		if  ( ! wake_integer_value ( argv [1], & start ) )
			ERROR_INTVAL ( 2, argv [1], funcname ) ;

		if  ( argc  >  2 )
		  {
			if  ( ! wake_integer_value ( argv [2], & increment ) )
				ERROR_INTVAL ( 3, argv [2], funcname ) ;
		   }
	    }

	counter_define ( name, start, increment ) ;

	return ( output ) ;
   }


WAKE_BUILTIN ( wake_builtin_undefcounter )
   {
	char *		name		=  argv [0] ;

	counter_undefine ( name ) ;

	return ( output ) ;
    }

# endif		/* WAKE_EXTENSIONS */