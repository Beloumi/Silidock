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
#include <string>
#include <sstream>
#include <iostream>
#include <cerrno>

// for error messages:
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>

#include <stdio.h>
#include <stdlib.h>

// C-Header
extern "C" {
#include <string>
// for fork, exec, getpid, getppid
#include <unistd.h>
// for kill, signal, waitpid
#include <sys/wait.h>

// to set flag FD_CLOEXEC for file descriptors and set_lowest_fd
#include <fcntl.h>

// for directory content of /proc/PID/fd/
#include <dirent.h>
}

using namespace std;

// called in main before Main_Window
void pre_call() {
    // prevent zombies:
 //   signal(SIGCHLD, SIG_IGN);// ignore the SIGCHLD signal
	int set_lowest_fd( int fd ); // fd: The minimum file descriptor that you want to get
}

void execute_program(const char* program_call, const char* param )
{

    pid_t child = vfork();

    if(child == 0) // child process: child == 0
    {
        int child_pid = getpid();
        //cout<<program_call<<": child PID: "<<child_pid<<", parent PID: " << getppid() <<endl;
        char *args[2]; // arguments for exec
        args[0] = (char*)program_call; // first argument is program_call
        args[1] = (char*)param;
        args[2] = NULL; // no arguments

/*
        // close file descriptors before exec:
        // This should work for all UNIX, but with worse performance
        int fd;
        int MAX_FD_NUMBER = sysconf(_SC_OPEN_MAX);
        // get open file descriptors
        for (fd = 3;  fd < MAX_FD_NUMBER;  ++fd)
        {
        	close(fd);// ignore if it fails with EBADF
        }
*/

        // close all opened file descriptors, which are copied from parent process:
        // - get file descriptor name=number from dir "/proc/PID/fd/"
        // THIS MIGHT ONLY WORK FOR LINUX!!!
        // to get content of directory thanx to:
        //  http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
        const char* prefix = "/proc/";
        const char* suffix = "/fd/";
        char child_proc_dir[16]; // directory contains file descriptors
        sprintf(child_proc_dir,"%s%d%s",prefix, child_pid, suffix);

        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir (child_proc_dir)) != NULL) {
        	  // get files and directories within directory
        	  while ((ent = readdir (dir)) != NULL) {
        		  // convert file name to int
        		  char* end;
        		  cout<<"end: "<<*end<<endl;
        		   int fd = strtol(ent->d_name, &end, 32);
        		   if (!*end) // Converted successfully: valid file descriptor
        		   {
        			   if (fd > 2)
        			   {
        				   close(fd); // close the file descriptor
        				   // or set the flag FD_CLOEXEC, so they will be closed automatically
        				   // fcntl( fd, F_SETFD, FD_CLOEXEC );// F_SETFD = set flag for fd: close-on-exec flag FD_CLOEXEC
        			   }
        		   }
        	  }
        	  closedir (dir);
        } else {
       	 cerr<< "can not open directory: " << child_proc_dir <<endl;
        }

        // replace the child process with exec*-function
        execv(program_call,args);

        // the following code is only executed if exec fails:
        // for fork():
        // exit(3);

        // for vfork(): exit(0) would exit the main program, so call
        _exit(2);
        // or terminate the process by PID:
//        kill(child_pid, SIGKILL); // kill child process
    }
    else if (child == -1) // fork error: child < 0
    {
    	string fork_note = "fork failed for program: \n" + string(program_call);
    	string fork_error = "";

        if (errno == EAGAIN)
        {
        	fork_error = "\n To much processes";
        }
        else if (errno == ENOMEM)
        {
        	fork_error = "\n Not enough space available.";
        }
        else
        {
        	fork_error = "\n Unexpected error: " + errno;
        }
        string message = fork_note + fork_error;
        fl_alert( message.c_str() );
        Fl::run();

    }
    else // this is executed by parent process
    {
    	usleep(50); // give some time to get status of child: 50 microseconds
    		// otherwise waitpid may return 0 even if failed

    	// get and store useful errors of exec:
    	string child_error = "";
		if ( errno == EACCES)
        {
			child_error = "\n Permission denied or process file not executable.";
        }
        else if ( errno == ENOENT)
        {
        	child_error = "\n Invalid path or file.";
        }
        else if ( errno == EPERM)
        {
        	child_error = "\n Superuser privileges required.";
        }
        else if ( errno == ENOEXEC)
        {
        	child_error = "\n Unsupported format of file.";
        }
		else
		{
			child_error = "\n unexpected error:" + errno;
		}
		int child_status;
		if ( waitpid(child, &child_status, WNOHANG | WUNTRACED) < 0) // < 0 waitpid failed
		{
        	string waitpid_message = "Error - Execution failed: \n   " + string(program_call);
        	string message = waitpid_message  + child_error;
            fl_alert( message.c_str() );
            Fl::run();
		}

		else if ( WIFEXITED( child_status ) &&  WEXITSTATUS( child_status ) != 0) // child process failed although waitpid does not
		{
        	string waitpid_message = "Child process error - Execution failed: \n   " + string(program_call);
        	string message = waitpid_message  + child_error;
            fl_alert( message.c_str() );
            Fl::run();
		}

    	// prevent zombies: if not SIGCHLD signal is ignored
        pid_t p;
        // Reap all pending child processes
        do {
            p = waitpid(-1, NULL, WNOHANG);
            	// -1: wait for any child,
            	// NULL: don't store status infos,
            	// WNOHANG: don't suspend parent process
        } while (p != (pid_t)0 && p != (pid_t)-1);
        	// p == 0: status of child is not available, p == -1: error
    }
}

