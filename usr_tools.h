#ifndef USR_TOOLS_H
#define USR_TOOLS_H

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
//------------------------------------------------------------------------------

//***	var		****************************************************************
//------------------------------------------------------------------------------



#endif // USR_TOOLS_H
