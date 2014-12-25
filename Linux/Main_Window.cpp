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
 * Main_Window.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Axel von dem Bruch
 */


// FLTK-Header
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Display.H>

#include <FL/Fl_Shared_Image.H>


// C++ Header
#include <iostream>
#include <fstream>

#include "Main_Window.hpp"
#include "Editor_Window.hpp"

// C-Header
extern "C"
{
	#include <string.h>
	#include <time.h>
}


using namespace std;

// functions:
void button_cb(Fl_Widget* buttonptr, void* data);
void quit_button_cb( Fl_Widget* o, void*);
void set_button_cb( Fl_Widget* o, void*);
void conf_button_cb( Fl_Widget* o, void*);
void reload_button_cb( Fl_Widget* o, void*);
void help_button_cb( Fl_Widget* o, void*);

// functions from main.cpp:
void execute_program(const char* program_call, const char* param );
int start_window();
void set_time(void*);

//Settings:
extern string display_date;

// needed to set window size and position
 extern long screen_width;
 extern long screen_height;
 extern int border_space;
 extern int button_size;

 extern long start_pos_X;
 extern long start_pos_Y;
 extern long window_width;
 extern long window_height;

// values needed to set location of buttons
extern long button_start_pos_Y;
extern int step_horizontal;
extern int step_vertical;


// from EditorWindow to check if window is alive
// in set_button_cb and config_button_cb
extern bool program_window_alive;
extern bool config_window_alive;
extern const char* program_file;
extern const char* config_file;


static Fl_Multiline_Output *multiout; // date, time: updated by set_time
static char date_and_time [15];

Fl_Color button_color = fl_rgb_color(255, 248, 216);
Fl_Color button_pressed__color = fl_rgb_color(200, 198, 180);
Fl_Color window_color = fl_rgb_color(0,0,0);
Fl_Color tooltip_color = fl_rgb_color(178, 212, 245);
Fl_Color multioutput_color = fl_rgb_color(255,255,225);


// default constructor:
Main_Window::Main_Window() : Fl_Window(start_pos_X, start_pos_Y, 0,0)
{
    color(window_color);
    fullscreen_off();
    clear_border(); // no title bar

    // Tooltip: display immediately
    Fl_Tooltip::enable();
    Fl_Tooltip::delay(0.1f);
    Fl_Tooltip::color(tooltip_color);
    Fl_Tooltip::font(FL_HELVETICA_BOLD_ITALIC);
    Fl_Tooltip::hoverdelay(0.1f);
    Fl_Tooltip::size(18);
    Fl_Tooltip::textcolor(FL_BLACK);

    // Begin adding children to this window
    begin();

    set_button = new Fl_Button( 1, 1, button_size / 3, button_size / 3);
    Fl_PNG_Image *wheel_image = new Fl_PNG_Image( "imgs/wheel.png" ); // load
    set_button->image(wheel_image->copy( button_size / 3, button_size / 3)); //insert in appropriate size
    delete wheel_image; // delete image (copy is in use)
    set_button->callback(set_button_cb);

    conf_button = new Fl_Button( button_size / 3 + 2, 1, button_size / 3, button_size / 3);
    Fl_PNG_Image *setting_image = new Fl_PNG_Image( "imgs/setting.png" );
    conf_button->image(setting_image->copy( button_size / 3, button_size / 3));
    delete setting_image;
    conf_button->callback(conf_button_cb);

    quit_button = new Fl_Button( button_size / 3 + 2 + button_size / 3 +2, 1, button_size / 3, button_size / 3);
    Fl_PNG_Image *close_image = new Fl_PNG_Image( "imgs/close.png" );
    quit_button->image(close_image->copy(button_size / 3, button_size / 3));
    delete close_image;
    quit_button->callback(quit_button_cb);

    help_button = new Fl_Button( 1, button_size / 3 + 2, button_size / 3, button_size / 3);
    Fl_PNG_Image *help_image = new Fl_PNG_Image( "imgs/help.png" );
    help_button->image(help_image->copy( button_size / 3, button_size / 3));
    delete help_image;
    help_button->callback(help_button_cb);

    reload_button = new Fl_Button( button_size / 3 + 2, button_size / 3 + 2, button_size / 3, button_size / 3);
    Fl_PNG_Image *reload_image = new Fl_PNG_Image( "imgs/reload.png" );
    reload_button->image(reload_image->copy( button_size / 3, button_size / 3));
    delete reload_image;
    reload_button->callback(reload_button_cb);


    if (display_date == "yes")
    {
        multiout = new Fl_Multiline_Output(0, button_size / 3 + button_size / 3 + 2, border_space + button_size + border_space,30);
        multiout->labeltype(FL_NORMAL_LABEL); //(FL_SHADOW_LABEL);
        if (button_size == 40)
        {
            multiout->textsize(8);
        }
        else if (button_size == 60)
        {
            multiout->textsize(11);
        }
        else if (button_size == 80)
        {
            multiout->textsize(11);
        }
        multiout->color(multioutput_color);
    }

    // Group for program buttons
    group = new Fl_Group (5 + step_horizontal, button_start_pos_Y + step_vertical, window_width, window_height );

    end();

    show();
}


// program button
void button_cb(Fl_Widget* buttonptr, void* data)
{
    const char* program_call = (const char*) data;
    Fl_Button *butt;
    butt = (Fl_Button *) buttonptr;
    butt->color(button_pressed__color);

    execute_program(program_call,NULL);
}

void quit_button_cb( Fl_Widget* o, void*)
{
    exit(0);
}

void set_button_cb( Fl_Widget* o, void*) // open file to set programs
{
    if (program_window_alive == 1)//allow only one window
    {
        return;
    }
    const char* title = program_file;
    Editor_Window *w = new Editor_Window(
        border_space + button_size + border_space,
        100, //border_space + button_size + border_space,
        screen_width / 2 + 10,
        screen_height / 2 + 50 ,
        title);
    program_window_alive = 1;
}

void conf_button_cb( Fl_Widget* o, void*)
{
    if (config_window_alive == 1)//allow only one window
    {
        return;
    }
//  execute_program("/bin/nano", "config_dockbar.txt");
    const char* title = config_file;
    Editor_Window *w = new Editor_Window(
        border_space + button_size + border_space,
        100, //border_space + button_size + border_space,
        screen_width / 2 + 10,
        screen_height / 2 + 50 ,
        title);
    config_window_alive = 1;
}

void help_button_cb( Fl_Widget* o, void*)
{
    const char* title = "help_dockbar.txt";

    // get file content
    string content = "";
    string line;
    ifstream ifs (title);
    if (ifs.is_open())
    {
        while ( getline (ifs,line) )
        {
            content += line + "\n";
        }
        ifs.close();
    }
    else cerr<<"Unable to open file"<<endl;

    // create window
    Fl_Window *help_win = new Fl_Window(border_space + button_size + border_space,
        100, //border_space + button_size + border_space,
        screen_width / 2 + 10,
        screen_height / 4 + 10 ,
        title);

    Fl_Text_Display *help_display = new Fl_Text_Display(
        5,5, screen_width / 2 , screen_height / 4  );

    Fl_Text_Buffer *buff = new Fl_Text_Buffer();
    buff->append(content.c_str() ); // add content to buffer
    help_display->buffer(buff); // register buffer
    help_win->add(help_display);

    help_win->show();
}

void reload_button_cb( Fl_Widget* o, void*) // reload
{
/*
    while(Fl::first_window())
    {
       Fl::first_window()->hide();
    }
  */
    config_window_alive = 0;
    program_window_alive = 0;
    start_window();
}

// set date and time, update in 1s
void set_time(void*)
{
    time_t t = time(NULL);
    struct tm now = *localtime(&t);

    if ( snprintf(date_and_time, 15, "%02d.%02d.%02d\n%02d:%02d", now.tm_mday, now.tm_mon, now.tm_year-100, now.tm_hour, now.tm_min) > 0 )
    {
        multiout->value(date_and_time);

        Fl::repeat_timeout(1.0, set_time); // update in 1s
    }
    else
    {
        cerr<<"error: formating date and time"<<endl;
    }
}
