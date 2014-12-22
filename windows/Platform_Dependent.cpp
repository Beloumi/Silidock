/*
 	 Copyright 2014 Axel von dem Bruch

     This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 */



/*
 * Platform_Dependent.cpp
 *
 *  Created on: 02.12.2014
 *      Author: Axel von dem Bruch
 */




// C++ Header
#include <iostream>
//#include <sstream>


// C-Header
extern "C" {
#include <windows.h>
#include <WinBase.h> // CreateProcess ...Windows 7
//#include <Processthreadsapi.h> // CreateProcess Windows 8...
//#include <Shellapi.h> ShellExecute
//#include <Winuser.h> // for error messages ShellExecute
//#include <tchar.h>// for _T
}


using namespace std;

// called in main before Main_Window
void pre_call() {

}



void execute_program(const char* program_call, const char* param )
{
    string file_and_path (program_call);
    string path = file_and_path.substr(0, file_and_path.find_last_of("/\\") );

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the process.
    if( CreateProcess(
            program_call, // executable file
            (LPSTR) param,  // Parameter for command line (NULL)
            NULL,           // Process handle
            NULL,           // Thread handle
            FALSE,          // Inheritance
            CREATE_DEFAULT_ERROR_MODE | DETACHED_PROCESS,// Own error mode and no console
            NULL,           // Environment of parent
            (LPCSTR) path.c_str(),// Directory of executable file
            &si,            // Pointer to STARTUPINFO structure
            &pi )           // Pointer to PROCESS_INFORMATION structure
    == FALSE)
    {
        // get error description
        DWORD errCode = GetLastError();
        char *err;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            errCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
            (LPTSTR) &err,
            0,
            NULL);

        // concatenate with program_call
        char error_message[strlen(err) + strlen(program_call) + 1];
        strcpy (error_message,err);
        strcat (error_message,program_call);
        // display message
        MessageBox(NULL, (LPCTSTR)error_message, TEXT("Error"), MB_OK);

        return;
    }

    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
