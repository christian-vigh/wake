/**************************************************************************************************************

    NAME
        wake.h

    DESCRIPTION
        Wake extensions for the GNU Make program.

    AUTHOR
        Christian Vigh, 09/2013.

    HISTORY
    [Version : 1.0]    [Date : 2013/09/24]     [Author : CV]
        Initial version.

    [Version : 2.0]    [Date : 2015/09/02]     [Author : CV]
	. See wake.c

 **************************************************************************************************************/

# ifdef		WAKE_EXTENSIONS
# ifndef	__WAKE_H__
# define	__WAKE_H__

# include	"makeint.h"
# include	"filedef.h"
# include	"function.h"
# include	"variable.h"

# include	"wake.h"


/***
	Various defines.
 ***/

// Authoring
# define	WAKE_EXTENSIONS_VERSION		"1.0.0"
# define	WAKE_EXTENSIONS_AUTHOR		"Christian Vigh"
# define	WAKE_EXTENSIONS_UPDATE		"09/2015"

// Limits
# define	WAKE_STRING_BUFFER_SIZE		65535
# define	WAKE_DEFAULT_LINE_WIDTH	 	110

// Variable names
# define	WAKE_EXTENSIONS_VARNAME		"WAKE_EXTENSIONS"		/* Set to 1 - this proves the makefile that it can use Thrak extensions		*/
# define	WAKE_LOGFILE_VARNAME		"<LOGFILE>"			/* If defined, the <log> function will also log the message to the file		*/
# define	WAKE_DEFAULT_LOGFILE		"Makefile.log"			/* Default log file name							*/
# define	WAKE_MAKEFILES			"Wakefile", "wakefile"		/* Additional default makefiles for wake					*/

// Error macros
# define	ERROR_INTVAL(argidx, value, func)	\
			fatal ( reading_file, 0, "Incorrect integer value '%s' for parameter #%d of function '%s'", value, argidx, func )
# define	ERROR_FLOATVAL(argidx, value, func)	\
			fatal ( reading_file, 0, "Incorrect numeric value '%s' for parameter #%d of function '%s'", value, argidx, func )

/***
	The structure used to hold the list of strings given in command switches of a type that takes string arguments. 
 ***/
typedef struct stringlist
  {
	const char **		list ;						/* Nil-terminated list of strings.		*/
	unsigned int		idx ;						/* Index into above.				*/
	unsigned int		max ;						/* Number of pointers allocated.		*/
   } stringlist ;


/***
	Command-line options.
 ***/
typedef struct wake_cmdline
   {
	stringlist *		print_builtins ;				/* Prints the builtin function list		*/
	stringlist *		print_signatures ;				/* Prints the builtin function signatures	*/
	stringlist *		print_usage ;					/* Prints the builtin functions usage text	*/
	stringlist *		print_help ;					/* Prints the builtin functions help text	*/
	int			print_categories ;				/* Prints the builtin function categories	*/
	int			standard_make ;					/* Disable Wake extensions - run as normal make */
    }  wake_cmdline ;



/***
	Function prototypes & externals.
 ***/
extern void		wake_initialize			( void ) ;
extern void		wake_process_cmdline		( wake_cmdline *		options ) ;
extern void		wake_sort_builtins		( int				name_or_category ) ;


extern wake_cmdline			wake_cmdline_options ;
extern int				wake_line_width ;
extern int				wake_enabled ;				/* 1 if Wake extensions are enabled (the default) */

extern function_table_entry **		sorted_builtin_functions_by_name ;
extern function_table_entry **		sorted_builtin_functions_by_category ;


/***
	Utility functions.
 ***/
extern int		wake_function_argc		( char **			argv ) ;
extern char *		wake_trim			( char *			value ) ;
extern int		wake_integer_value		( char *			input,
							  int *				output ) ;
extern int		wake_double_value		( char *			input,
							  double *			output ) ;
extern char *		wake_format_string		( char *			str, 
							  int				width, 
							  int				indent_spaces ) ;
extern int  		wake_is_empty_list		( char *			ptr ) ;

/***
	Builtin functions (ie, macros like $(something))
 ***/
# define		WAKE_BUILTIN(func)		char *  func ( char *  output, char **  argv, const char *  funcname UNUSED )

/* lib/builtins/internals.c */
extern WAKE_BUILTIN ( wake_builtin_builtins			) ;
extern WAKE_BUILTIN ( wake_builtin_isbuiltin			) ;

/* lib/builtins/message.c */
extern WAKE_BUILTIN ( wake_builtin_message			) ;

/*lib/builtins/list.c */
extern WAKE_BUILTIN ( wake_builtin_list_first			) ;
extern WAKE_BUILTIN ( wake_builtin_list_last			) ;
extern WAKE_BUILTIN ( wake_builtin_list_length			) ;
extern WAKE_BUILTIN ( wake_builtin_list_rest			) ;
extern WAKE_BUILTIN ( wake_builtin_list_chop			) ;
extern WAKE_BUILTIN ( wake_builtin_list_map			) ;
extern WAKE_BUILTIN ( wake_builtin_list_pairmap			) ;
extern WAKE_BUILTIN ( wake_builtin_list_reverse			) ;
extern WAKE_BUILTIN ( wake_builtin_list_equality		) ;
extern WAKE_BUILTIN ( wake_builtin_list_range			) ;
extern WAKE_BUILTIN ( wake_builtin_list_quote			) ;

/* lib/builtins/string.c */
extern WAKE_BUILTIN ( wake_builtin_string_strlen		) ;
extern WAKE_BUILTIN ( wake_builtin_string_compare_simple	) ;
extern WAKE_BUILTIN ( wake_builtin_string_compare		) ;
extern WAKE_BUILTIN ( wake_builtin_string_search		) ;
extern WAKE_BUILTIN ( wake_builtin_string_reverse		) ;
extern WAKE_BUILTIN ( wake_builtin_string_change_case		) ;
extern WAKE_BUILTIN ( wake_builtin_string_implode		) ;
extern WAKE_BUILTIN ( wake_builtin_string_explode		) ;
extern WAKE_BUILTIN ( wake_builtin_string_substring		) ;
extern WAKE_BUILTIN ( wake_builtin_string_sprintf		) ;
extern WAKE_BUILTIN ( wake_builtin_string_chr			) ;
extern WAKE_BUILTIN ( wake_builtin_string_repeat		) ;
extern WAKE_BUILTIN ( wake_builtin_string_quote			) ;

/* lib/builtins/misc.c */
extern WAKE_BUILTIN ( wake_builtin_random			) ;
extern WAKE_BUILTIN ( wake_builtin_randomize			) ;
extern WAKE_BUILTIN ( wake_builtin_guid				) ;
extern WAKE_BUILTIN ( wake_builtin_rawguid			) ;
extern WAKE_BUILTIN ( wake_builtin_counter			) ;
extern WAKE_BUILTIN ( wake_builtin_defcounter			) ;
extern WAKE_BUILTIN ( wake_builtin_undefcounter			) ;

/* lib/builtins/math.c */
extern WAKE_BUILTIN ( wake_builtin_math_operation		) ;
extern WAKE_BUILTIN ( wake_builtin_math_not			) ;
extern WAKE_BUILTIN ( wake_builtin_math_eval			) ;

/* lib/builtins/date.c */
extern WAKE_BUILTIN ( wake_builtin_unixtime			) ;
extern WAKE_BUILTIN ( wake_builtin_unixgmtime			) ;
extern WAKE_BUILTIN ( wake_builtin_date				) ;

/* lib/builtins/logical.c */
extern WAKE_BUILTIN ( wake_builtin_logical_if			) ;
extern WAKE_BUILTIN ( wake_builtin_logical_ifcompare		) ;

/* lib/builtins/file.c */
extern WAKE_BUILTIN ( wake_builtin_file_tempfile		) ;
extern WAKE_BUILTIN ( wake_builtin_file_filecontents		) ;
extern WAKE_BUILTIN ( wake_builtin_file_stat			) ;

# endif		/* __WAKE_H__ */

# endif		/* WAKE_EXTENSIONS */

