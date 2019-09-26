#include "Platform.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <intrin.h>
#include <Psapi.h>
#include <TlHelp32.h>


namespace hop
{
void cpuid( int reg[4], int fctId ) { __cpuid( reg, fctId ); }

ProcessID startChildProcess( const char* path, char** )
{
   ProcessID newProcess = 0;
   STARTUPINFO si         = {0};
   PROCESS_INFORMATION pi = {0};

   // TODO Fix arguments passing
   si.cb = sizeof( si );
   if( !CreateProcess( NULL, (LPSTR)path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ) )
   {
      return (ProcessID)-1;
   }
   newProcess = (ProcessID)pi.hProcess;
   return newProcess;
}

ProcessInfo getProcessInfoFromPID( ProcessID pid )
{
   ProcessInfo info = {};

   HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid );
   if( NULL != hProcess )
   {
      HMODULE hMod;
      DWORD cbNeeded;
      if( EnumProcessModules( hProcess, &hMod, sizeof( hMod ), &cbNeeded ) )
      {
         if( GetModuleBaseName( hProcess, hMod, (LPSTR)info.name, sizeof( info.name ) ) > 0 )
         {
            info.pid = pid;
         }
      }
   }

   return info;
}

ProcessInfo getProcessInfoFromProcessName( const char* name )
{
   ProcessInfo info = {};
   info.pid = -1;

   HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
   PROCESSENTRY32 entry;
   entry.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, name) == 0)
            {  
               info.pid = entry.th32ProcessID;
               strncpy( info.name, name, sizeof( info.name ) - 1 );
            }
        }
    }

   CloseHandle(snapshot);

   return info;
}

}  // namespace hop