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


/* Editor_Window.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Axel von dem Bruch
 */



#include <FL/Fl_Text_Editor.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>


#include <FL/fl_ask.H>

// C++ Header
#include <iostream>
#include <fstream>
#include <cerrno>

#include "Editor_Window.hpp"

using namespace std;

// from main.cpp
extern long screen_width;
extern long screen_height;

// to check if window is alive: allow only one window of same type
bool program_window_alive = 0; // alive == 1
bool config_window_alive = 0; // alive == 1
const char* program_file = "program_dockbar.txt";
const char* config_file = "config_dockbar.txt";

// functions:
void save_cb( Fl_Widget* o, void* data);
void load_file(const char *file_name, Fl_Text_Buffer *textbuf);
void save_file(const char *file_name, const char* content);
void window_close_cb (Fl_Widget *widget, void *);



// default constructor:
Editor_Window::Editor_Window(int x, int y, int w, int h, const char *title)
    : Fl_Window( x,y,w,h,title )
{

    this->callback( window_close_cb );// to check if window already exists

    Fl_Text_Buffer *textbuf;

    // Begin adding children to this window
    begin();

        editor = new Fl_Text_Editor(5, 5, screen_width / 2, screen_height / 2);
        textbuf = new Fl_Text_Buffer;

        editor->buffer(textbuf);
        textbuf->call_modify_callbacks();
        // get file content
        load_file(title, textbuf);

        save_button = new Fl_Button( 5, screen_height / 2 + 5, 70, 30, "save");
        save_button->copy_tooltip( "save modifications" ); // program call as tooltip
        save_button->labelfont(FL_BOLD);
        save_button->labelsize(12);
        save_button->callback(save_cb, (void*) textbuf); // file_name (title) as parameter

    end();
    show();
}

void load_file(const char *file_name, Fl_Text_Buffer *textbuf)
{
  int r = textbuf->loadfile(file_name);
  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", file_name, strerror(errno));
  textbuf->call_modify_callbacks();
}

void save_file(const char *file_name, const char* content)
{
    ofstream ofs;
    ofs.open(file_name, ios::trunc);// overwrite content
    ofs <<content;
    ofs.close();
}

void save_cb(Fl_Widget* o, void* data)
{
    Fl_Button *button = (Fl_Button*) o;
    Fl_Window *win = button->parent()->as_window();
    const char* file_name = win->label();
    Fl_Text_Buffer *textbuf = (Fl_Text_Buffer *) data;
    const char* content = textbuf->text();
    save_file(file_name, content);
}

void window_close_cb (Fl_Widget *widget, void *)
{
    Fl_Window *window = (Fl_Window *)widget;
    const char* title = window->label();

    if(strcmp(title, config_file) == 0)
    {
        config_window_alive = 0;
    }

    else if(strcmp(title, program_file) == 0)
    {
        program_window_alive = 0;
    }
    else
    {
        cerr<<"invalid window title"<<endl;
    }
    window->hide();
}



