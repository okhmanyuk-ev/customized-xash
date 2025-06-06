#include <unistd.h> // fork
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "platform/platform.h"
#include "menu_int.h"
#include <string>

static qboolean Sys_FindExecutable( const char *baseName, char *buf, size_t size )
{
	char *envPath;
	char *part;
	size_t length;
	size_t baseNameLength;
	size_t needTrailingSlash;

	if( !baseName || !baseName[0] )
		return false;

	envPath = getenv( "PATH" );
	if( !COM_CheckString( envPath ) )
		return false;

	baseNameLength = Q_strlen( baseName );
	while( *envPath )
	{
		part = Q_strchr( envPath, ':' );
		if( part )
			length = part - envPath;
		else
			length = Q_strlen( envPath );

		if( length > 0 )
		{
			needTrailingSlash = ( envPath[length - 1] == '/' ) ? 0 : 1;
			if( length + baseNameLength + needTrailingSlash < size )
			{
				string temp;

				Q_strncpy( temp, envPath, length + 1 );
				Q_snprintf( buf, size, "%s%s%s",
					temp, needTrailingSlash ? "/" : "", baseName );

				if( access( buf, X_OK ) == 0 )
					return true;
			}
		}

		envPath += length;
		if( *envPath == ':' )
			envPath++;
	}
	return false;
}

void Platform_ShellExecute( const char *path, const char *parms )
{
	char xdgOpen[128];

	if( !Q_strcmp( path, GENERIC_UPDATE_PAGE ) || !Q_strcmp( path, PLATFORM_UPDATE_PAGE ))
		path = DEFAULT_UPDATE_PAGE;

	if( Sys_FindExecutable( OPEN_COMMAND, xdgOpen, sizeof( xdgOpen ) ) )
	{
		const char *argv[] = { xdgOpen, path, NULL };
		pid_t id = fork( );
		if( id == 0 )
		{
			execv( xdgOpen, (char **)argv );
			fprintf( stderr, "error opening %s %s", xdgOpen, path );
			_exit( 1 );
		}
	}
	else
	{
		Con_Reportf( (std::string(S_WARN) + "Could not find " + OPEN_COMMAND + " utility\n").c_str() );
	}
}

void Posix_Daemonize( void )
{
	// to be accessed later
	if( ( host.daemonized = Sys_CheckParm( "-daemonize" ) ) )
	{
#if XASH_POSIX && defined(_POSIX_VERSION)
		pid_t daemon;

		daemon = fork();

		if( daemon < 0 )
		{
			Host_Error( "fork() failed: %s\n", strerror( errno ) );
		}

		if( daemon > 0 )
		{
			// parent
			Con_Reportf( "Child pid: %i\n", daemon );
			exit( 0 );
		}
		else
		{
			// don't be closed by parent
			if( setsid() < 0 )
			{
				Host_Error( "setsid() failed: %s\n", strerror( errno ) );
			}

			// set permissions
			umask( 0 );

			// engine will still use stdin/stdout,
			// so just redirect them to /dev/null
			close( STDIN_FILENO );
			close( STDOUT_FILENO );
			close( STDERR_FILENO );
			open("/dev/null", O_RDONLY); // becomes stdin
			open("/dev/null", O_RDWR); // stdout
			open("/dev/null", O_RDWR); // stderr

			// fallthrough
		}
#else
		Sys_Error( "Daemonize not supported on this platform!" );
#endif
	}

}

#if !XASH_SDL

void Platform_Init( void )
{
	Posix_Daemonize();
}
void Platform_Shutdown( void ) {}
#endif

#if XASH_TIMER == TIMER_POSIX
double Platform_DoubleTime( void )
{
	struct timespec ts;
#if XASH_IRIX
	clock_gettime( CLOCK_SGI_CYCLE, &ts );
#else	
	clock_gettime( CLOCK_MONOTONIC, &ts );
#endif
	return (double) ts.tv_sec + (double) ts.tv_nsec/1000000000.0;
}

void Platform_Sleep( int msec )
{
	usleep( msec * 1000 );
}
#endif // XASH_TIMER == TIMER_POSIX
