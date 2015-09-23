/**************************************************************************************************************

    NAME
        internals.c

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
 *   NAME
 *	wake_builtin_builtins - callback for the $(<builtins>) function.
 *
 *   PROTOTYPE
 *	$(<builtins> [category],[sort])
 *
 *   DESCRIPTION
 *	Expands to the list of defined builtin functions.
 *
 *   PARAMETERS
 *	category -
 *		Optional category.
 *
 *	sort -
 *		If the sort parameter expands to a non-empty string, the resulting list will be sorted.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_builtins )
   {
	int					i ;
	function_table_entry **			p ;
	function_table_entry *			q ;
	char *					category	=  "" ;
	int					sort		=  0 ;
	

	/* Macro arguments */
	if  ( argv [0]  !=  NULL  &&  *argv [0] )
	   {
		category	=  argv [0] ;

		if  ( argv [1]  !=  NULL  &&  * argv [1] )
			sort = 1 ;
	    } 

	/* Sort  */
	wake_sort_builtins ( 0 ) ;
	wake_sort_builtins ( 1 ) ;

	/* If a non-empty argument has been specified, we must return the results in sorted order */
	if  ( sort ) 
	   {
		if  ( * category )
			p	=  sorted_builtin_functions_by_category ;
		else
			p	=  sorted_builtin_functions_by_name ;
	    }
	else
		p	=  builtin_functions ;

	// Loop through the loaded builtin functions
	for  ( i = 0 ; i  <  builtin_function_count ; i ++, p ++ )
	   {
		q	=  * p ;

		if  ( category  !=  NULL  &&  * category  &&  strcmp ( q -> func_category, category ) )
			continue ;

		output	=  variable_buffer_output ( output, q -> name, q -> len ) ;
		output  =  variable_buffer_output ( output, " ", 1 ) ;
	    }

	// All done, return
	return ( output - 1 ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_isbuiltin - callback for the $(<isbuiltin>) function.
 *
 *   PROTOTYPE
 *	$(<isbuiltin> funcname)
 *
 *   DESCRIPTION
 *	Expands to "1" if "funcname" is a builtin function, or to the empty string otherwise.
 *
 *   PARAMETERS
 *	funcname -
 *		Name of the builtin function to be tested.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_isbuiltin )
   {
	function_table_entry *			p ;
	int					i ;


	// Ignore empty function names
	if  ( argv  ==  NULL  ||  argv [0]  ==  NULL )
		return ( output ) ;

	// Loop through builtin function names
	for  ( i = 0 ; i < builtin_function_count ; i ++ )
	   {
		p	=  builtin_functions [i] ;

		if  ( ! strncmp ( p -> name, argv [0], p -> len ) )
		   {
			output	=  variable_buffer_output ( output, "1", 1 ) ;
			break ;
		    }
	    }

	// All done, return
	return ( output ) ;
    }

# endif		/* WAKE_EXTENSIONS */