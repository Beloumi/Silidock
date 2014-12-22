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
 * main.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Axel von dem Bruch
 */




// FLTK-Header
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/fl_ask.H>

// FLTK Image-Header:
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_GIF_Image.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_Shared_Image.H>

// C++ Header
#include <string>
#include <vector>
#include <iostream>
#include <cerrno>





// internal header
#include "Main_Window.hpp"


using namespace std;

// function in main.cpp:
//void execute_program(const char* program_call, const char* param );
int start_window();
void set_time(void*);
Fl_Image* create_image(const char *filename);

struct date_element { string label; string program; string img; }; // Element of program_dockbar.txt

// functions and variables from Config_File_Einlesen.cpp:
vector<date_element> read_programs ();// get the programs
void read_settings ();// set the settings

// functions and variables from Main_Window.cpp:
void button_cb(Fl_Widget* buttonptr, void* data); // program call
extern Fl_Color button_color;

// from Platform_Dependent:
void pre_call();

long screen_width = Fl::w();
long screen_height = Fl::h();

// default settings:
int columns = 1;
string dockbar_position = "left";
string display_date = "no";
int button_size = 60; // width and height
int border_space = 5; // width and height
int font_size = 12;
unsigned int label_text_length = 6;
// this will be set by reading config_dockbar.txt
// in Config_file_read_settings.cpp
long window_width = border_space + button_size + border_space;
long window_height = screen_height;
// values needed to set location of buttons
long button_start_pos_X;
long button_start_pos_Y;
int step_horizontal;
int step_vertical;

Main_Window *win;


int main()
{
	pre_call(); // platform dependent codes

   return start_window();
}


// called also in reload in Main_Window.cpp
int start_window()
{
    if (win != NULL) {
    	win->hide();
    }

    button_start_pos_X = 0;
    button_start_pos_Y = 0;
    int step_horizontal = 0;
    int step_vertical = 0;

    read_settings();

    win = new Main_Window();

    vector<date_element> daten = read_programs();// vector: label, program call, image from config.txt

    ////////////////////////////////////////////////////////////////////
    // settings: start-position of first button and gap between buttons:
    if (dockbar_position == "top" || dockbar_position == "bottom")
    {
        step_horizontal = border_space + button_size;
        step_vertical = 0;
        button_start_pos_Y = border_space;
        button_start_pos_X = step_horizontal + border_space;
    }
    else // left, right
    {
        step_horizontal = 0;
        step_vertical = border_space + button_size;

        button_start_pos_Y = button_size / 3 + button_size / 3 + 2 + 2;
        if (display_date == "yes")
        {
            button_start_pos_Y += 45; // 40 for date and 5 space
        }
        button_start_pos_X = border_space;
    }

    ////////////////////////////////////
    // create program buttons for group:
    int column_index = 0;
    int line_index = 0;
    unsigned int i;
    fl_register_images(); //initialize image library
    for (i = 0; i < daten.size(); i++)
    {
        Fl_Button *button = new Fl_Button(
        button_start_pos_X + column_index * step_horizontal,
        button_start_pos_Y + line_index * step_vertical,
        button_size,
        button_size);

        win->group->add(button); // add button to main window

        ///////////////////
        // image on button:
        string x_img = daten.at(i).img; // image file

        // if empty display label
        if (x_img.empty() )
        {
            string button_text = daten.at(i).label;
            if (button_text.length() > label_text_length)
            {
                button_text = button_text.substr(0, label_text_length);
            }
            char * button_C_string = new char[button_text.size() + 1];
            std::copy(button_text.begin(), button_text.end(), button_C_string);
            button_C_string[button_text.size()] = '\0';
            button->label(FL_NORMAL_LABEL, button_C_string );

            button->labelfont(FL_BOLD);
            button->labelsize(font_size);
        }
        else
        {
            Fl_Image* img;
            img = create_image(x_img.c_str() );

            if (img == NULL || img->h() == 0 ) //if missing image
            {

                string button_text = daten.at(i).label;
                if (button_text.length() > label_text_length)
                {
                    button_text = button_text.substr(0, label_text_length);
                }
/*                char button_label [button_text.length() +2];
                strcpy(button_label, " ");
                strcat(button_label, button_text.c_str());
                button->label(FL_NORMAL_LABEL, button_label );
*/

                char * button_C_string = new char[button_text.size() + 1];
                std::copy(button_text.begin(), button_text.end(), button_C_string);
                button_C_string[button_text.size()] = '\0';
                button->label(FL_NORMAL_LABEL, button_C_string );

                button->labelfont(FL_BOLD);
                button->labelsize(font_size);
            }
            else
            {
                 button->image(img->copy(button_size,button_size) );
                 delete img; // this was copied
            }
        } // end else: img is ""

        ///////////
        // tooltip:
        button->tooltip( daten.at(i).label.c_str() ); // label as tooltip

        /////////////////
        // programm call:
        const char * x_program = daten.at(i).program.c_str(); // program call as C string
        button->callback(button_cb, (void*) x_program); // program call as parameter

        //////////////////////
        // position of button:
        if (button_size > 40) // only for big size
        {
            button->box(FL_RSHADOW_BOX);
        }
        button->color(button_color); //Backgroundcolor

        if (dockbar_position == "left" || dockbar_position == "right")
        {
        	line_index++;

        	if (button_start_pos_Y + line_index * step_vertical + button_size > screen_height) // next column
        	{
        		 column_index++;

        		 	 if (column_index >= columns)
        		 	 {
        		 		 cerr<<"Not enough space to display all images"<<endl;
        		 		 break;
        		 	 }
        		 	 line_index = 0;
        		 	 button_start_pos_Y = 0;
        		 	 button_start_pos_X += border_space + button_size + border_space;
        	}
        }
        else // top, bottom
        {
        column_index++;

            if (button_start_pos_X + column_index * step_horizontal + button_size > screen_width) // next column
            {
                line_index++;
                if (line_index >= columns)
                {
                    cerr<<"Not enough space to display all images"<<endl;
                    break;
                }
                column_index = 0;
                button_start_pos_Y += border_space + button_size + border_space;
                button_start_pos_X = 0;
            }
        }
    } // End for-loop

    ///////////////////////
    // size of Main_Window:
    if ( columns == 1 && (dockbar_position == "top" || dockbar_position == "bottom"))
    {
    	win->size(button_start_pos_X + column_index * (button_size + border_space) , button_start_pos_Y + button_size + border_space);

    }
    else if ( columns == 1 && (dockbar_position == "left" || dockbar_position == "right"))
    {
    	win->size(button_start_pos_X + button_size + border_space, button_start_pos_Y + line_index * (button_size + border_space)  );
    }
    else // (columns != 1)
    {
    	win->size(window_width, window_height);
    }

    ////////////////////////////
    // date and time, update 1s:
    if (display_date == "yes")
    {
        Fl::add_timeout(1.0, set_time);
    }
    return Fl::run();
}


// helper function: String to Fl_Image
Fl_Image* create_image(const char *filename)
{
    string x_img(filename);

    fl_register_images();

    // get image format
    if ( x_img.substr(x_img.length() - 3, 3).compare("jpg") == 0 || x_img.substr(x_img.length() - 4, 4).compare("peg") == 0)
    {
       return  new Fl_JPEG_Image( x_img.c_str() );
    }
    else if ( x_img.substr(x_img.length() - 3, 3).compare("gif") == 0 || x_img.substr(x_img.length() - 4, 4).compare("GIF") == 0)
    {
    	return new Fl_GIF_Image (x_img.c_str() );
    }
    else if ( x_img.substr(x_img.length() - 3, 3).compare("png") == 0 || x_img.substr(x_img.length() - 4, 4).compare("PNG") == 0)
    {
    	return new Fl_PNG_Image(x_img.c_str());
    }
    else if ( x_img.substr(x_img.length() - 3, 3).compare("bmp") == 0 || x_img.substr(x_img.length() - 4, 4).compare("BMP") == 0)
    {
        return  new Fl_BMP_Image(x_img.c_str());
    }
    else // inappropriate extension: return wrong string
    {
        return NULL;
    }
}
