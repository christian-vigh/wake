/**************************************************************************************************************

    NAME
        date.c

    DESCRIPTION
        Date/time functions.

    AUTHOR
        Christian Vigh, 09/2015.

    HISTORY
    [Version : 1.0]    [Date : 2015/09/03]     [Author : CV]
        Initial version.

 **************************************************************************************************************/
# ifndef	WAKE_EXTENSIONS
#	warning	This file must be compiled with -DWAKE_EXTENSIONS.
# else

# include	"wake.h"
# include	<sys/types.h>
# include	<time.h>


/**************************************************************************************************************

	Date/time macros 

 **************************************************************************************************************/
# define	IS_LEAP(year)			( (year) % 4 == 0  &&  ( (year) % 100  !=  0  ||  (year) % 400  ==  0 ) )
# define	MONTH_LENGTH(year,month)	( ( IS_LEAP ( year ) ) ? leap_year_month_lengths [ month ] : nonleap_year_month_lengths [ month ] )


/**************************************************************************************************************

	Date/time strings

 **************************************************************************************************************/
static char *	short_day_names []	= 
   {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    } ;

static char *	long_day_names []	= 
   {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    } ;

static char *	short_month_names []	=
   {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    } ;

static char *	long_month_names []	= 
   {
	"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
    } ;

static int	nonleap_year_month_lengths [12]	=  {  31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30, 31 } ;
static int	leap_year_month_lengths    [12]	=  {  31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30, 31 } ;

static int	nonleap_year_table [13]		=  { -1, 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 } ;		/* 1 = january */
static int	leap_year_table    [13]		=  { -1, 6, 2, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 } ;		/* 1 = january */

static int	nonleap_day_count  [13]		=  {  0,   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334 } ;
static int 	leap_day_count     [13]		=  {  0,   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335 } ;


/**************************************************************************************************************

	Helper functions

 **************************************************************************************************************/
// english_suffix :
//	Returns the english suffix corresponding to a number ("st" for "1st", "nd" for "2nd", etc.)
static char *	english_suffix ( int  mday )
   {
	if  ( mday  >=  10  &&  mday  <=  19)
		return ( "th" ) ;
	else 
	   {
		switch  ( mday % 10 ) 
		   {
			case	1 :  return ( "st" ) ;
			case	2 :  return ( "nd" ) ;
			case	3 :  return ( "rd" ) ;
		    }
	    }

	return ( "th" ) ;
    }


// gmtoffset -
//	Returns a GMT offset (hours and minutes)
static char *  gmtoffset ( int  is_localtime, int  offset, int  rfc_format )
   {
	static char	buffer [32] ;
	char		sign		=  ( ! is_localtime  ||  offset  >=  0 ) ?  '+' : '-' ;
	char *		colon		=  ( rfc_format ) ?  ":" : "" ;
	

	if  ( ! is_localtime )
		offset	=  0 ;

	sprintf ( buffer, "%c%02d%s%02d", sign,  abs ( offset / 3600 ), colon, abs ( ( offset % 3600 ) / 60 ) ) ;

	return ( buffer ) ;
    }

// Helper functions for time computations.
// Taken from PHP source code.
static int	positive_mod ( int  x, int  y )
   {
	int	tmp	=  x % y ;

	if  ( tmp  <  0 )
		tmp	+=  y ;

	return ( tmp ) ;
    }

static int  century_value ( century )
   {
	return ( 6 - ( positive_mod ( century, 4 ) * 2 ) ) ;
    }


static int  day_of_year ( int  y, int  m, int  d )
   {
	return ( ( ( IS_LEAP ( y ) ) ?  leap_day_count [m] : nonleap_day_count [m] ) + d - 1 ) ;
    }


// day_of_week -
//	Returns the day of week.
//	Taken from PHP source code.
static int  day_of_week ( int  year, int  month, int  day, int  iso )
   {
	int	c1, y1, m1, dow ;

	c1	=  century_value ( year / 100 ) ;
	y1	=  positive_mod ( year, 100 ) ;
	m1	=  IS_LEAP ( year ) ?  leap_year_table [ month ] : nonleap_year_table [ month ] ;

	dow	=  positive_mod ( ( c1 + y1 + m1 + ( y1 / 4 ) + day ), 7) ;

	if  ( iso )
	   {
		if  ( ! dow )
			dow	=  7 ;
	    }

	return ( dow ) ;
    }


// isoweek_from_date -
//	Computes the ISO week number from the specified date.
static void  isoweek_from_date ( int  y, int  m, int  d, int *  iso_week, int *  iso_year )
   {
	int		y_leap			=  IS_LEAP ( y ), 
			y_previous_leap		=  IS_LEAP ( y - 1 ) ;
	int		doy			=  day_of_year ( y, m, d ) + 1 ;
	int		weekday, jan1_weekday ;

	//if  ( y_leap  &&  m  >  2 )
	//	doy ++ ;

	jan1_weekday	=  day_of_week ( y, 1, 1, 1 ) ;
	weekday		=  day_of_week ( y, m, d, 1 ) ;

	if  ( ! weekday )
		weekday		=  7 ;

	if  ( ! jan1_weekday )
		jan1_weekday	=  7 ;

	/* Find if Y M D falls in year Y-1, week# 52 or 53 */
	if  ( doy  <=  ( 8 - jan1_weekday )  &&  jan1_weekday  >  4 )
	   {
		* iso_year	=  y - 1 ;

		if  ( jan1_weekday  ==  5  ||  ( y_previous_leap  &&  jan1_weekday  ==  6 ) )
			* iso_week	=  53 ;
		else
			* iso_week	=  52 ;
	    }
	else
		* iso_year	=  y ;

	/* Find if Y M D falls in year Y+1, week# 1 */
	if  ( * iso_year  ==  y )
	   {
		int		i	=  ( y_leap ) ?  366 : 365 ;

		if  ( ( i - ( doy - y_leap ) )  <  4 - weekday )
		   {
			( * iso_year ) ++ ;
			* iso_week	=  1 ;

			return ;
		    }
	    }

	/* Find if Y M D falls in year Y, week# 1 through 53 */
	if  ( * iso_year  ==  y )
	   {
		int	j =  doy + ( 7 - weekday ) + ( jan1_weekday - 1 ) ;

		* iso_week	=  j / 7 ;

		if  ( jan1_weekday )
			( * iso_week ) -- ;
	    }
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_unixtime   - Returns the current Unix time
 *
 *   PROTOTYPE
 *	$(<now>)
 *
 *   DESCRIPTION
 *	Returns the current Unix time in seconds.
 *
 *==============================================================================================================*/
WAKE_BUILTIN ( wake_builtin_unixtime )
   {
	time_t		now ;
	char		buffer [80] ;

	time ( & now ) ;
	sprintf ( buffer, "%d", ( int ) now ) ;

	output	=  variable_buffer_output ( output, buffer, strlen ( buffer ) ) ;

	return ( output ) ;
    }


/*==============================================================================================================
 *
 *   NAME
 *	wake_builtin_date - Formats a date.
 *
 *   PROTOTYPE
 *	$(<date> [ format [, time] ] )
 *	$(<gmdate> [ format [, time] ] )
 *
 *   DESCRIPTION
 *	Formats a time value.
 *
 *   PARAMETERS
 *	format -
 *		A format string, that accepts the same specifiers than the PHP date() function.
 *		If not specified, the default format is 'Y-m-d H:i:s'.
 * 
 *	time -
 *		A time value. If not specified, the current Unix time is used.
 *
 *==============================================================================================================*/
static char *	__wake_builtin_date ( char *  output, char * format, time_t  time_value, int  is_localtime )
   {
	struct tm *	tm		=  ( is_localtime ) ?  localtime ( & time_value ) : gmtime ( & time_value ) ;
	char *		p		=  format ;
	int		ap_hour ;
	char		buffer [128] ;
	char *		buffer_p ;
	int		iso_year, iso_week ;
	int		iso_values_set	=  0,
			tz_set		=  0 ;


	tm -> tm_year		+=  1900 ;
	ap_hour			 =  ( tm -> tm_hour  %  12 ) ?  tm -> tm_hour % 12 : 12 ; 

	while  ( * p )
	   {
		buffer_p	=  NULL ;
		* buffer	=  0 ;

		switch ( * p )
		   {
			// 'a' : lowercase "am" or "pm"
			case	'a' :
				buffer_p	=  ( tm -> tm_hour  >=  12 ) ?  "pm" : "am" ;
				break ;

			// 'A' : uppercase "AM" or "PM"
			case	'A' :
				buffer_p	=  ( tm -> tm_hour  >=  12 ) ?  "PM" : "AM" ;
				break ;

			// 'B' : internet Swatch hour
			case	'B' :
			   {
				int	result		=
				   (
				      (
					 (
					    time_value -
						( time_value - 
						   (
						     ( time_value % 86400 ) + 3600
						    )
						 )
					  ) * 10
				       ) / 864
				    ) ;

				while  ( result  <  0 )
					result	+=  1000 ;

				result	%=  1000 ;
				sprintf ( buffer, "%03d", result ) ;
				break ;
			    }

			// 'c' : Full date, in ISO8601 format
			case	'c' :
				sprintf ( buffer, "%04d-%02d-%02dT%02d:%02d:%02d%s",
						tm -> tm_year, tm -> tm_mon + 1, tm -> tm_mday,
						tm -> tm_hour, tm -> tm_min, tm -> tm_sec,
						gmtoffset ( is_localtime, tm -> tm_gmtoff, 1 ) 
					 ) ;
				break ;

			// 'd' : day of month, as a two-digits number
			case	'd' : 
				sprintf ( buffer, "%02d", tm -> tm_mday ) ;
				break ;
			
			// 'D' : short day name
			case	'D' :
				buffer_p	=  short_day_names [ tm -> tm_wday ] ;
				break ;

			// 'e' : Timezone name
			case	'e' :
				if  ( ! tz_set )
					tzset ( ) ;

				buffer_p	=  tzname [0] ;
				break ;

			// 'F' : Long month name
			case	'F' :
				buffer_p	=  long_month_names [ tm -> tm_mon ] ;
				break ;

			// 'g' : Hour without leading zero (1..12)
			case	'g' :
				sprintf ( buffer, "%d", ap_hour ) ;
				break ;

			// 'G' : Hour without leading zero (0..23)
			case	'G' :
				sprintf ( buffer, "%d", tm -> tm_hour ) ;
				break ;
			
			// 'h' : Hour with leading zero (01..12)
			case	'h' :
				sprintf ( buffer, "%02d", ap_hour ) ;
				break ;

			// 'H' : Hour with leading zero (00..23)
			case	'H' :
				sprintf ( buffer, "%02d", tm -> tm_hour ) ;
				break ;
			
			// 'i' : Minute, with leading zero (00..59)
			case	'i' :
				sprintf ( buffer, "%02d", tm -> tm_min ) ;
				break ;

			// 'I' : 1 if daylight savings time is active, 0 otherwise
			case	'I' :
				sprintf ( buffer, "%d", tm -> tm_isdst ) ;
				break ;

			// 'j' : day of month, without the leading zero
			case	'j' : 
				sprintf ( buffer, "%d", tm -> tm_mday ) ;
				break ;

			// 'l' : Long day name
			case	'l' :
				buffer_p	=  long_day_names [ tm -> tm_wday ] ;
				break ;

			// 'L' : 1 is the year is leap, 0 otherwise
			case	'L' :
				sprintf ( buffer, "%d", IS_LEAP ( tm -> tm_year ) ) ;
				break ;

			// 'M' : short month name 
			case	'M' :
				buffer_p	=  short_month_names [ tm -> tm_mon ] ;
				break ;

			// 'm' : month of year, as a two-digits number
			case	'm' :
				sprintf ( buffer, "%02d", tm -> tm_mon + 1 ) ;
				break ;
			
			// 'n' : month of year, without the leading zero
			case	'n' :
				sprintf ( buffer, "%d", tm -> tm_mon + 1 ) ;
				break ;
			
			// 'N' : day of week, from 1 (monday) to 7 (sunday)
			case	'N' :
				sprintf ( buffer, "%d", ( tm -> tm_wday ) ?  tm -> tm_wday : 7 ) ;
				break ;

			// 'o' : ISO8601 year
			case	'o' :
				if  ( ! iso_values_set )
				   {
					isoweek_from_date ( tm -> tm_year, tm -> tm_mon, tm -> tm_mday, & iso_week, & iso_year ) ;
					iso_values_set	=  1 ;
				    }

				sprintf ( buffer, "%d", iso_year ) ;
				break ;

			// 'O' : GMT offset in ISO8601 format (+0200)
			case	'O' :
				buffer_p	=  gmtoffset ( is_localtime, tm -> tm_gmtoff, 0 ) ;
				break ;

			// 'P' : GMT offset in RFC822 format (+02:00)
			case	'P' :
				buffer_p	=  gmtoffset ( is_localtime, tm -> tm_gmtoff, 1 ) ;
				break ;

			// 'r' : RFC822 date
			case	'r' :
				sprintf ( buffer, "%3s %02d %3s %04d %02d:%02d:%02d %s", 
						short_day_names [ tm -> tm_wday ],
						tm -> tm_mday,
						short_month_names [ tm -> tm_mon ],
						tm -> tm_year,
						tm -> tm_hour, tm -> tm_min, tm -> tm_sec,
						gmtoffset ( is_localtime, tm -> tm_gmtoff, 0 ) 
					 ) ;
				break ;

			// 's' : Seconds, with leading zero (00..59)
			case	's' :
				sprintf ( buffer, "%02d", tm -> tm_sec ) ;
				break ;

			// 'S' : 2-letters english suffix for a day of month
			case	'S' :
				buffer_p	=  english_suffix ( tm -> tm_mday ) ;
				break ;

			// 't' : month length
			case	't' :
				sprintf ( buffer, "%d", MONTH_LENGTH ( tm -> tm_year, tm -> tm_mon ) ) ;
				break ;

			// 'T' : Timezone abbreviation
			case	'T' :
				if  ( ! tz_set )
					tzset ( ) ;

				buffer_p	=  tzname [1] ;
				break ;

			// 'u' : microsecond time. Always return '000000' since we accept an integer value as time
			case	'u' :
				buffer_p	=  "000000" ;
				break ;

			// 'U' : Unix time
			case	'U' :
				sprintf ( buffer, "%d", ( int ) time_value ) ;
				break ;

			// 'w' : day of week, from 0 (sunday) to 6 (saturday)
			case	'w' :
				sprintf ( buffer, "%d", tm -> tm_wday ) ;
				break ;

			// 'W' : ISO8601 week number (were weeks start on monday)
			case	'W' :
				if  ( ! iso_values_set )
				   {
					isoweek_from_date ( tm -> tm_year, tm -> tm_mon, tm -> tm_mday, & iso_week, & iso_year ) ;
					iso_values_set	=  1 ;
				    }

				sprintf ( buffer, "%d", iso_week ) ;
				break ;

			// 'y' : 2-digits year
			case	'y' :
				sprintf ( buffer, "%02d", ( tm -> tm_year % 100 ) ) ;
				break ;

			// 'Y' : 4-digits year
			case	'Y' :
				sprintf ( buffer, "%04d", tm -> tm_year ) ;
				break ;

			// 'z' : day of year, from 0 to 365
			case	'z' :
				sprintf ( buffer, "%d", tm -> tm_yday ) ;
				break ;

			// 'Z' : GMT offset in seconds
			case	'Z' :
				sprintf ( buffer, "%d", ( int ) tm -> tm_gmtoff ) ;
				break ;

			// Escape character : copy the next one without interpretation unless this is the last one
			case	'\\' :
				if  ( * ( p + 1 ) )
					p ++ ;
				/* Intentionally fall through the default case */

			// Other characters : copy as is
			default :
				buffer [0]	=  * p ;
				buffer [1]	=  0 ;
				break ;
		    }

		if  ( buffer_p  ==  NULL )
			buffer_p	=  buffer ;

		output	=  variable_buffer_output ( output, buffer_p, strlen ( buffer_p ) ) ;
		p ++ ;	
	    }	

	return ( output ) ;
    }


WAKE_BUILTIN ( wake_builtin_date )
   {
	int		is_localtime		=  ( funcname [1]  ==  'd' ) ;
	char *		format ;
	time_t		time_value ;


	if  ( argv [0]  &&  * argv [0] )
	   {
		format		=  argv [0] ;

		if  ( argv [1] )
		   {
			int	value ;

			if  ( ! wake_integer_value ( argv [1], & value ) )
				ERROR_INTVAL ( 2, argv [1], funcname ) ;

			time_value	=  value ;
		    }
		else
			time ( & time_value ) ;
	    }
	else
	   {
		format	=  "Y-m-d H:i:s" ;
		time ( & time_value ) ;
	    }

	output	=  __wake_builtin_date ( output, format, time_value, is_localtime ) ;

	return ( output ) ;
    }

# endif		/* WAKE_EXTENSIONS */