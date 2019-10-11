#include "Platform.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

namespace hop
{

void cpuid( int reg[4], int fctId )
{
   asm volatile( "cpuid"
                 : "=a"( reg[0] ), "=b"( reg[1] ), "=c"( reg[2] ), "=d"( reg[3] )
                 : "a"( fctId ), "c"( 0 ) );
}

ProcessInfo getProcessInfoFromPID( ProcessID pid )
{
   ProcessInfo info;
   info.pid = -1;
   info.name[0] = '\0';

   char cmd[128] = {};
   const unsigned writeSize =
       snprintf( cmd, sizeof( cmd ), "ps -p %d -o comm= | xargs basename | tr -d '\n'", pid );

   // Has truncation occured ?
   if( writeSize >= sizeof( cmd ) )
      return info;

   // Get name from PID
   if( FILE* fp = popen( cmd, "r" ) )
   {
      if( fgets( info.name, sizeof( info.name ), fp ) != nullptr )
      {
         info.pid = pid;
      }
      pclose( fp );
   }

   return info;
}

ProcessInfo getProcessInfoFromProcessName( const char* name )
{
   ProcessInfo info;
   info.pid = -1;
   info.name[0] = '\0';

   if( strlen( name ) > 0 )
   {
      // Get actual PID from process name
      char cmd[128] = {};

      /**
       * Trying to find a whole word match for the process name. We also need to remove the
       * grep process from the result. On MacOs the process name also contains the path, we
       * thus have to check for a preceding '/'
       */
      const unsigned writeSize = snprintf(
          cmd,
          sizeof( cmd ),
          "ps -A -o pid,comm | grep '[ \\/]%s$' | grep -v grep | awk '{print $1}'",
          name );

      // Has truncation occured ?
      if( writeSize >= sizeof( cmd ) )
         return info;

      // Get name from PID
      if( FILE* fp = popen( cmd, "r" ) )
      {
         char pidStr[16] = {};
         if( fgets( pidStr, sizeof( pidStr ), fp ) != nullptr )
         {
            info.pid = strtol( pidStr, nullptr, 10 );
            strncpy( info.name, name, sizeof( info.name ) - 1 );
         }
         else
         {
            info.pid = -1;
         }
         pclose( fp );
      }
   }

   return info;
}

ProcessID startChildProcess( const char* path, char** args )
{
   ProcessID newProcess = 0;

   newProcess = (ProcessID) fork();
   if ( newProcess == 0 )
   {
      int res = execvp( path, args );
      if ( res < 0 )
      {
         exit( 0 );
      }
   }
   return newProcess;
}

} //  namespace hop