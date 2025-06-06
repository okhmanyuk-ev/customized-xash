#ifndef STDLIB_H
#define STDLIB_H

#include <string>
#include <cstdarg>
#include <cstring>
#include "build.h"
#include "xash3d_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

// timestamp modes
enum
{
	TIME_FULL = 0,
	TIME_DATE_ONLY,
	TIME_TIME_ONLY,
	TIME_NO_SECONDS,
	TIME_YEAR_ONLY,
	TIME_FILENAME,
};

// a1ba: not using BIT macro, so flags can be copypasted into
// exported APIs headers and will get nice warning in case of changing values
#define PFILE_IGNOREBRACKET (1<<0)
#define PFILE_HANDLECOLON   (1<<1)
#define PFILE_TOKEN_MAX_LENGTH 1024
#define PFILE_FS_TOKEN_MAX_LENGTH 512

//
// build.c
//
int Q_buildnum( void );
int Q_buildnum_date( const char *date );
int Q_buildnum_compat( void );
const char *Q_PlatformStringByID( const int platform );
const char *Q_buildos( void );
const char *Q_ArchitectureStringByID( const int arch, const uint abi, const int endianness, const qboolean is64 );
const char *Q_buildarch( void );
const char *Q_buildcommit( void );

//
// crtlib.c
//
void Q_strnlwr( const char *in, char *out, size_t size_out );
size_t Q_colorstr( const char *string );
char Q_toupper( const char in );
char Q_tolower( const char in );
size_t Q_strncat( char *dst, const char *src, size_t siz );
qboolean Q_isdigit( const char *str );
qboolean Q_isspace( const char *str );
int Q_atoi( const char *str );
float Q_atof( const char *str );
void Q_atov( float *vec, const char *str, size_t siz );
#define Q_strchr  strchr
#define Q_strrchr strrchr
qboolean Q_stricmpext( const char *pattern, const char *text );
qboolean Q_strnicmpext( const char *pattern, const char *text, size_t minimumlen );
const byte *Q_memmem( const byte *haystack, size_t haystacklen, const byte *needle, size_t needlelen );
const char *Q_timestamp( int format );
int Q_vsnprintf( char *buffer, size_t buffersize, const char *format, va_list args );
int Q_snprintf( char *buffer, size_t buffersize, const char *format, ... );
#define Q_strpbrk strpbrk
void COM_StripColors( const char *in, char *out );
#define Q_memprint( val ) Q_pretifymem( val, 2 )
char *Q_pretifymem( float value, int digitsafterdecimal );
void COM_FileBase( const char *in, char *out, size_t size );
const char *COM_FileExtension( const char *in );
void COM_DefaultExtension( char *path, const char *extension, size_t size );
void COM_ReplaceExtension( char *path, const char *extension, size_t size );
void COM_ExtractFilePath( const char *path, char *dest );
const char *COM_FileWithoutPath( const char *in );
void COM_StripExtension( char *path );
void COM_RemoveLineFeed( char *str );
void COM_PathSlashFix( char *path );
char COM_Hex2Char( uint8_t hex );
void COM_Hex2String( uint8_t hex, char *str );
// return 0 on empty or null string, 1 otherwise
#define COM_CheckString( string ) ( ( !string || !*string ) ? 0 : 1 )
#define COM_CheckStringEmpty( string ) ( ( !*string ) ? 0 : 1 )
char *COM_ParseFileSafe( char *data, char *token, const int size, unsigned int flags, int *len, qboolean *quoted );
#define COM_ParseFile( data, token, size ) COM_ParseFileSafe( data, token, size, 0, NULL, NULL )
int matchpattern( const char *in, const char *pattern, qboolean caseinsensitive );
int matchpattern_with_separator( const char *in, const char *pattern, qboolean caseinsensitive, const char *separators, qboolean wildcard_least_one );

// libc implementations
static inline int Q_strcmp( const char *s1, const char *s2 )
{
	return unlikely(!s1) ?
		( !s2 ? 0 : -1 ) :
		( unlikely(!s2) ? 1 : strcmp( s1, s2 ));
}

static inline int Q_strncmp( const char *s1, const char *s2, size_t n )
{
	return unlikely(!s1) ?
		( !s2 ? 0 : -1 ) :
		( unlikely(!s2) ? 1 : strncmp( s1, s2, n ));
}

static inline char *Q_strstr( const char *s1, const char *s2 )
{
	return unlikely( !s1 || !s2 ) ? NULL : (char*)strstr( s1, s2 );
}

// libc extensions, be careful

#if XASH_WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif // XASH_WIN32

static inline int Q_stricmp( const char *s1, const char *s2 )
{
	return unlikely(!s1) ?
		( !s2 ? 0 : -1 ) :
		( unlikely(!s2) ? 1 : strcasecmp( s1, s2 ));
}

static inline int Q_strnicmp( const char *s1, const char *s2, size_t n )
{
	return unlikely(!s1) ?
		( !s2 ? 0 : -1 ) :
		( unlikely(!s2) ? 1 : strncasecmp( s1, s2, n ));
}

#if defined( HAVE_STRCASESTR )
#if XASH_WIN32
#define strcasestr stristr
#endif
static inline char *Q_stristr( const char *s1, const char *s2 )
{
	return unlikely( !s1 || !s2 ) ? NULL : (char *)strcasestr( s1, s2 );
}
#else // defined( HAVE_STRCASESTR )
char *Q_stristr( const char *s1, const char *s2 );
#endif // defined( HAVE_STRCASESTR )

#if defined( HAVE_STRCHRNUL )
#define Q_strchrnul strchrnul
#else
const char* Q_strchrnul(const char* s, int c);
#endif

#ifdef __cplusplus
}
#endif

size_t Q_strncpy(char* dst, const char* src, size_t siz);
size_t Q_strncpy(std::string& dst, const char* src, size_t siz);

void COM_FixSlashes(char* pname);
void COM_FixSlashes(std::string& str);

size_t Q_strlen(const char* str);
size_t Q_strlen(const std::string& str);

#endif//STDLIB_H
