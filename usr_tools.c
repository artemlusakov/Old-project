#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	<UserCAPI.h>
#include	"usr_tools.h"

//***	const	****************************************************************
//------------------------------------------------------------------------------

//***	type	****************************************************************
//------------------------------------------------------------------------------

//***	func	****************************************************************
float		minf( float v1_, float v2_ );
float		maxf( float v1_, float v2_ );
int			absint( int v_ );
int**		new_arr_int( const int nx_, const int ny_ );
void		del_arr_int( int** arr_, const int ny_ );
int			rd_file_txt_int( const char* fn_, int**	dat_, const int nx_, const int ny_ );
float**		new_arr_float( const int nx_, const int ny_ );
void		del_arr_float( float** arr_, const int ny_ );
int			rd_file_txt_float( const char* fn_, float**	dat_, int* len_, const int nx_, const int ny_ );

static int			rd_str( tGsFile* fl_, char* buf_, int len_ );
static int			scan_int( char* buf_, int len_, int* val_ );
static int			scan_float( char* buf_, int len_, float* val_ );
//------------------------------------------------------------------------------

//***	var		****************************************************************
//------------------------------------------------------------------------------

float		minf( float v1_, float v2_ )		{return v1_ < v2_ ? v1_ : v2_;}
float		maxf( float v1_, float v2_ )		{return v1_ > v2_ ? v1_ : v2_;}
int			absint( int v_ )		{return v_ < 0 ? (-v_) : (v_);}

/**	@brief new_arr_int
**	@param ny_
**	@return					**/
inline int**		new_arr_int( const int nx_, const int ny_ ) {
	int**		arr_;
	arr_ = (int**)calloc( ny_, sizeof(int*) );
	for( int y_ = 0; y_ < ny_; ++y_ )		{arr_[y_] = (int*)calloc( nx_ +1, sizeof(int) );}
	return arr_;
}
/**	@brief del_arr_int
**	@param nx_
**	@param ny_
**/
inline void			del_arr_int( int** arr_, const int ny_ ) {
	for( int y_ = 0;  y_ < ny_;  ++y_ )		{free(arr_[y_]);}
	free( arr_ );
}
/**	@brief чтение файла с данными, преобразование в 2-х мерный массив целых
**	@p[in]	: fn_ - имя файла
**	@p[out]	: dat_ -
**	@return	: = 0 - без ошибок, = -1 - ошибка		**/
int			rd_file_txt_int( const char* fn_, int**	dat_, const int nx_, const int ny_ ) {
	tGsFile*	fl_ = FileOpen( fn_, "r" );
	char		str_[1024];
	int			rc_,  x_ = 0,  y_ = 0;
	if( NULL == fl_ )		{return -1;}
	do		{
		//? читаем строку
		if( (rc_ = rd_str( fl_, str_, sizeof(str_) )) <= 0 )	{break;}
		dat_[y_][0]  = x_ = scan_int( str_, nx_, dat_[y_]+1 );
		++y_;
	} while( 0 != x_ && y_ < ny_ );
	FileClose( fl_ );
	if( 0 == x_ )		{return -1;}
	return 0;
}
/**	@brief new_arr_float
**	@param ny_
**	@return					**/
inline float**		new_arr_float( const int nx_, const int ny_ ) {
	float**		arr_;
	arr_ = (float**)calloc( ny_, sizeof(float*) );
	for( int y_ = 0; y_ < ny_; ++y_ )		{arr_[y_] = (float*)calloc( nx_, sizeof(float) );}
	return arr_;
}
/**	@brief del_arr_float
**	@param nx_
**	@param ny_
**/
inline void			del_arr_float( float** arr_, const int ny_ ) {
	for( int y_ = 0;  y_ < ny_;  ++y_ )		{free(arr_[y_]);}
	free( arr_ );
}
/**	@brief чтение файла с данными, преобразование в 2-х мерный массив целых
**	@p[in]	: fn_ - имя файла
**	@p[out]	: dat_ -
**	@return	: = 0 - без ошибок, = -1 - ошибка		**/
int			rd_file_txt_float( const char* fn_, float**	dat_, int* len_, const int nx_, const int ny_ ) {
	tGsFile*	fl_ = FileOpen( fn_, "r" );
	char		str_[1024];
	int			rc_,  x_ = 0,  y_ = 0;
	if( NULL == fl_ )		{
		PrintToDebug( "----CR-:no open file<%s>\r\n", fn_ );
		return -1;
	}
	do		{
		//? читаем строку
		if( (rc_ = rd_str( fl_, str_, sizeof(str_) )) <= 0 )	{break;}
//		PrintToDebug( "----CR-:(%d)%s\r\n" , rc_ , str_ );
		len_[y_]  = x_ = scan_float( str_, nx_, dat_[y_] );
//		PrintToDebug( "----CR-:x=%d y=%d\r\n" , len_[y_] , y_ );
		++y_;
	} while( 0 != x_ && y_ < ny_ );
	FileClose( fl_ );
//	PrintToDebug( "----CR-:x=%d y=%d\r\n" , x_ , y_ );
	if( 0 == x_ )		{return -1;}
	return 0;
}


/**	@brief rd_str
**	@param fl_
**	@param buf_
**	@param len_
**	@return	: 0= весь буфер, -1= ошибка, 0< длинна строки		**/
static int			rd_str( tGsFile* fl_, char* buf_, const int len_ ) {
	char		ch_;
	int		cnt_ = 0;
//	if( NULL == FileGets( buf_, len_, fl_ ) )		{return -1;}
//	else	{return strlen( buf_);}
	while( (cnt_ < len_) && (0 < FileRead( &ch_, sizeof(ch_), 1, fl_ )) ) {
		if( 0 == cnt_ && (' ' == ch_ || '\t' == ch_) )		{continue;}//= пропускаем строки из пробельных символов
		if( ch_ == '\n' || ch_ == '\r' )	{//= конец строки
			if( 0 == cnt_ )		{continue;}//= пропускаем пустые строки
			buf_[cnt_] = '\0';
			return cnt_;
		}
		buf_[cnt_++]  = ch_;
	}
	return cnt_ == len_ ? 0 : (cnt_ == 0 ? -1 : cnt_);
}

/**	@brief scan_int
**	@p[in]	bu_	:
**	@p[in]	en_	:
**	@p[out]	val_ : число или 0, если не было
**	@return	: число чисел				**/
static int			scan_int( char* buf_, int len_, int* val_ ) {
	char*		str_;
	char*		delim_	= " \t\n\r";
	int			cnt_ = 0;
	while( cnt_ < len_ && NULL != (str_ = strtok( buf_, delim_ )) )		{
		val_[cnt_]  = strtol( str_, NULL, 0 );
		buf_  = NULL;    ++cnt_;
	}
	return cnt_;
}

/**	@brief scan_float
**	@p[in]	bu_	:
**	@p[in]	en_	:
**	@p[out]	val_ : число или 0, если не было
**	@return	: число чисел				**/
static int			scan_float( char* buf_, int len_, float* val_ ) {
	char*		str_;
	char*		delim_	= " \t\n\r";
	int			cnt_ = 0;
	while( cnt_ < len_ && NULL != (str_ = strtok( buf_, delim_ )) )		{
		val_[cnt_]  = strtod( str_, NULL );
		buf_  = NULL;    ++cnt_;
	}
	return cnt_;
}
