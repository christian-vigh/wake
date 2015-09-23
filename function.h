/**************************************************************************************************************

    NAME
        function.h

    DESCRIPTION
        Exported here some definitions of the initial make function table.

    AUTHOR
        Christian Vigh, 09/2013.

    HISTORY
    [Version : 1.0]    [Date : 2013/09/24]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	__FUNCTION_INCLUDED__
# define	__FUNCTION_INCLUDED__


/***
	Definitions for function table entries.
 ***/
char 		* func_call 	( char *  o, char **  argv, const char * funcname ) ;

# define 	FT_ENTRY(name, min, max, exp, func, help ) 	\
		   {						\
			STRING_SIZE_TUPLE ( name ),		\
			(min), (max),				\
			(exp),					\
			0,					\
			{ ( void * ) (func) },			\
			help					\
		    }

/***
	A pointer to a function table entry callback.
 ***/
typedef		char * ( * function_table_entry_callback )	( char *  output, char **  argv, const char *  function_name ) ;


/***
	Definition of a builtin function table entry.
 ***/
typedef struct function_table_entry
  {
	const char *				name ;			/* Function name						*/
	unsigned char				len ;			/* Name length, without the trailing zero			*/
	unsigned char				minimum_args ;		/* Minimum number of arguments					*/
	unsigned char				maximum_args ;		/* Maximum number of arguments					*/
	unsigned char				expand_args:1 ;		/* If non-zero, macros will be expanded before processing	*/
	unsigned char 				alloc_fn:1 ;		/* 1 if the function has been defined dynamically 		*/
	union 								/* Function callback, either static or dynamic 			*/
	   {
		function_table_entry_callback 	func_ptr ;
		gmk_func_ptr 			alloc_func_ptr ;
	    } fptr ;
	
	/* Additional fields provided by Wake extensions */
	char *					func_category ;		/* Category ("builtin", "wake", and more...)			*/
	char *					func_signature ;	/* Function signature						*/
	char *					func_usage ;		/* Usage help							*/
	char *					func_help ;		/* Help								*/
   }  function_table_entry ;


/* Defined function entries so far */
extern function_table_entry * 			builtin_functions [] ;
extern unsigned int				builtin_function_count ;


/***
	Various defines.
 ***/

/* A macro to provide help strings to a builtin function. */
# ifdef  WAKE_EXTENSIONS
#	define FUNC_HELP(category,signature,usage,help)			(category), (signature), (usage), (help)
# endif

/* Number of functions defined in an array of function_table_entry */
# define	FUNCTABLE_SIZE(table)				( ( unsigned int ) ( sizeof ( table ) / sizeof ( function_table_entry ) ) )
/* hash.c holds only "struct token" items ; compute the multiplier needed to hold items of size "function_table_entry" */
# define	HASHTABLE_MULTIPLIER				( ( sizeof ( function_table_entry ) + sizeof ( struct token * ) - 1 ) / sizeof ( struct token * ) )
/* Maximum number of entries held in the builtin functions hash table. Should be enough to avoid hash table collisions. */
# define	FUNCTABLE_MAX_HASH_SIZE				256
/* Maximum total number of builtin functions */
# define	FUNCTABLE_MAX_BUILTINS				( FUNCTABLE_MAX_HASH_SIZE * 10 )	


/*** 
	Function prototypes.
 ***/
extern void	hash_load_functions				( function_table_entry *	item_table, 
								  unsigned long			cardinality ) ;
extern char *	func_call					( char *			output, 
								  char **			argv, 
								  const char *			funcname ) ;


# endif		/*  __FUNCTION_INCLUDED__  */