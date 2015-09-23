/**************************************************************************************************************

    NAME
        counter.h

    DESCRIPTION
        Definitions for counter macros.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/05]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifdef		WAKE_EXTENSIONS
# ifndef	__COUNTER_H__
#	define	__COUNTER_H__


/***
	Counter structure.
 ***/
typedef struct  counter
   {
	char *			name ;			// Counter name
	int			start ;			// Start value
	int			increment ;		// Increment
	int			current ;		// Current value
	struct counter *	next ;			// Pointer to next counter
    }  counter ;


/***
	Counter functions.
 ***/
extern counter *	counter_define		( char *	id, 
						  int		start, 
						  int		increment ) ;
extern counter *	counter_find		( char *	id ) ;
extern void		counter_undefine	( char *	id ) ;
extern void		counter_reset		( counter *	counter ) ;
extern int		counter_value		( counter *	counter ) ;

# endif		/*  __COUNTER_H__  */
# endif		/*  WAKE_EXTENSIONS */

