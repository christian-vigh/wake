/**************************************************************************************************************

    NAME
        wake.c

    DESCRIPTION
        Thrak extensions for the GNU make utility.

    AUTHOR
        Christian Vigh, 09/2013.

    HISTORY
    [Version : 1.0]    [Date : 2013/09/24]     [Author : CV]
        Initial version.

    [Version : 2.0]    [Date : 2015/09/02]     [Author : CV]
	. Created the wake/include and wake/lib tree
	. Renamed and moved utility functions to lib/utilities.c
	. Created a source file for each make function group

 **************************************************************************************************************/

# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"makeint.h"
# include	"filedef.h"
# include	"variable.h"
# include	"dep.h"
# include	"job.h"
# include	"commands.h"
# include	"debug.h"
# include	"function.h"

# ifdef		_AMIGA
# include	"amiga.h"
# endif

# include 	<stdlib.h>
# include	<sys/ioctl.h>
# include	<termio.h>
# include 	<sys/param.h>
# include	<math.h>

# include	"wake.h"
# include	"builtins.h"


/***
	Variables.
 ***/
// Wake-specific command-line options are grouped in this structure
wake_cmdline	wake_cmdline_options ;

// Line width for printing messages
int		wake_line_width			=  WAKE_DEFAULT_LINE_WIDTH ;

// Whether Wake extensions should be enabled or not
int		wake_enabled			=  1 ;

// Builtin functions
extern function_table_entry * 			builtin_functions [] ;
extern unsigned int				builtin_function_count ;



    
/**************************************************************************************************************
 **************************************************************************************************************
 **************************************************************************************************************
 ******                                                                                                  ******
 ******                                                                                                  ******
 ******                      INTERNAL COMMAND-LINE OPTION PROCESSING FUNCTIONS                           ******
 ******                                                                                                  ******
 ******                                                                                                  ******
 **************************************************************************************************************
 **************************************************************************************************************
 **************************************************************************************************************/

// __wake_print_builtins -
//	Prints the builtin function list to standard output.
static void	__wake_print_builtins ( const char **  list, int  count )
   {
	wake_sort_builtins ( 0 ) ;
	wake_sort_builtins ( 1 ) ;

	function_table_entry **		p ;
	int				i, j ;


	if  ( ** list )
		p	=  sorted_builtin_functions_by_category ;
	else
		p	=  sorted_builtin_functions_by_name ;

	for  ( i = 0 ;  i  <  builtin_function_count ; i ++, p ++ )
	   {
		if   ( ! ** list )
			printf ( "%s\n", ( * p ) -> name ) ;
		else
		   {
			for  ( j = 0 ; j  <  count ; j ++ )
			   {
				if  ( ! strcmp ( ( * p ) -> func_category, list [j] ) )
					printf ( "%s\n", ( * p ) -> name ) ;
			    }
		    }
	    }
    }


// __wake_print_categories -
//	Prints the builtin function categories to standard output.
static void	__wake_print_categories ( )
   {
	wake_sort_builtins ( 0 ) ;
	wake_sort_builtins ( 1 ) ;

	function_table_entry **		p		=  sorted_builtin_functions_by_category ;
	char *				previous	=  "" ;
	int				i ;


	for  ( i = 0 ;  i  <  builtin_function_count ; i ++, p ++ )
	   {
		if  ( strcmp ( ( * p ) -> func_category, previous ) )
		   {
			previous	=  ( * p ) -> func_category ;
			printf ( "%s\n", previous ) ;
		    }
	    }
    }


// __wake_print_signatures -
//	Prints the builtin function signatures to standard output.
static char *	__wake_function_signature ( function_table_entry *  e )
   {
	static char		buffer [ 256 ] ;

	sprintf ( buffer, "$(%s %s)", e -> name, e -> func_signature ) ;
	return ( buffer ) ;
    }


static void	__wake_print_signatures ( const char **  list, int  count )
   {
	wake_sort_builtins ( 0 ) ;
	wake_sort_builtins ( 1 ) ;

	function_table_entry **		p ;
	int				i, j ;

	if  ( ** list )
		p	=  sorted_builtin_functions_by_category ;
	else
		p	=  sorted_builtin_functions_by_name ;

	for  ( i = 0 ;  i  <  builtin_function_count ; i ++, p ++ )
	   {
		if   ( ! ** list )
			printf ( "%s\n", __wake_function_signature ( * p ) ) ;
		else
		   {
			for  ( j = 0 ; j  <  count ; j ++ )
			   {
				if  ( ! strcmp ( ( * p ) -> func_category, list [j] ) )
					printf ( "%s\n", __wake_function_signature ( * p ) ) ;
			    }
		    }
	    }
    }


// __wake_print_help -
//	Prints the builtin function usage (help == 0) or help (help  ==  1) to standard output.
static void	__wake_print_help ( const char **  list, int  count, int  help )
   {
	wake_sort_builtins ( 0 ) ;
	wake_sort_builtins ( 1 ) ;

	function_table_entry **		p ;
	int				i, j ;
	int				accepted ;
	int				printed		=  0 ;

	if  ( ** list )
		p	=  sorted_builtin_functions_by_category ;
	else
		p	=  sorted_builtin_functions_by_name ;

	for  ( i = 0 ;  i  <  builtin_function_count ; i ++, p ++ )
	   {
		accepted	=  0 ;

		// The current builtin will be accepted if no builtin name has been specified on the command line,
		// meaning that we will list the usage text for all builtin functions...
		if   ( ! ** list )
			accepted	=  1 ; 
		// ... or if we encounter one of the builtin names specified on the command line
		else
		   {
			for  ( j = 0 ; j  <  count ; j ++ )
			   {
				if  ( ! strcmp ( ( * p ) -> name, list [j] ) )
				   {
					accepted	=  1 ;
					break ;
				    }
			    }
		    }

		// Builtin function accepted for usage printing...
		if  ( accepted )
		   {
			char *		hp	=  ( help ) ?  ( * p ) -> func_help : ( * p ) -> func_usage ;

			// Add an extra newline between two help texts
			if  ( help  &&  printed )
				printf ( "\n" ) ;

			printf ( "%s :\n", __wake_function_signature ( * p ) ) ;
			printf ( "%s\n", wake_format_string ( hp, wake_line_width, 8 ) ) ;
			printed ++ ;
		    }
	    }
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_initialize - Initialize the various parts of Thhak extensions.
 *
 *   PROTOTYPE
 *	void  wake_initialize ( void ) ;
 *
 *   DESCRIPTION
 *	Initializes the various components of the Thrak extensions for GNU make.
 *
 *==============================================================================================================*/
void  wake_initialize ( )
   {
	// Load our builtin-extension functions
	hash_load_functions ( wake_function_table, FUNCTABLE_SIZE ( wake_function_table ) ) ;

	// Get terminal width
	struct winsize		ws ;

	ws. ws_col	=  0 ;

	if  ( ioctl ( STDOUT_FILENO, TIOCGWINSZ, & ws )  !=  -1 )
	   {
		if  ( ws. ws_col )
			wake_line_width	=  ws. ws_col ;
	    }

	// Define some variables
	define_variable_cname ( WAKE_EXTENSIONS_VARNAME, "1", o_default, 0 ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_process_cmdline - Process wake-specific command-line options.
 *
 *   PROTOTYPE
 *	void  wake_process_cmdline ( wake_cmdline_options *  options ) ;
 *
 *   DESCRIPTION
 *	Process wake-specific command-line options.
 *
 *==============================================================================================================*/
void	wake_process_cmdline ( wake_cmdline *		options )
   {
	// --builtins option :
	//	Prints the list of builtin functions, optionnally for the specified class.
	if  ( options -> print_builtins )
	   {
		__wake_print_builtins ( options -> print_builtins -> list, options -> print_builtins -> idx ) ;
		die ( 0 ) ;
	    }

	// --builtin-categories -
	//	Prints the list of builtin function categories.
	if  ( options -> print_categories )
	   {
		__wake_print_categories ( ) ;
		die ( 0 ) ;
	    }

	// --builtin-signatures :
	//	Prints the list of builtin function signatures.
	if  ( options -> print_signatures )
	   {
		__wake_print_signatures ( options -> print_signatures -> list, options -> print_signatures -> idx ) ;
		die ( 0 ) ;
	    }

	// --builtin-usage :
	//	Prints brief usage text for the specified builtin functions.
	if  ( options -> print_usage )
	   {
		__wake_print_help ( options -> print_usage -> list, options -> print_usage -> idx, 0 ) ;
		die ( 0 ) ;
	    }

	// --builtin-help :
	//	Prints help text for the specified builtin functions.
	if  ( options -> print_help )
	   {
		__wake_print_help ( options -> print_help -> list, options -> print_help -> idx, 1 ) ;
		die ( 0 ) ;
	    }

	// --stdmake :
	//	Disables Wake extensions.
	if  ( options -> standard_make )
	   {
		printf ( "running as standard make...\n" ) ;
		wake_enabled	=  0 ;
	    }
    }

# endif		/* WAKE_EXTENSIONS */
