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
 * Read_Files.cpp
 *
 *  Created on: 19.11.2014
 *      Author: Axel von dem Bruch
 */


/*

return "" for missing value
if x_img is empty: button displays label
if x_program is empty: displays folder of silidock
*/



#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>



using namespace std;

 struct date_element { string label; string program; string img; };

// from EditorWindow
extern const char* program_file;
extern const char* config_file;

//Settings:
 extern string display_date;
 extern int columns;
 extern string dockbar_position;

// needed to set window size and position
extern long screen_width;
extern long screen_height;

 extern int border_space;
 extern int button_size;
 extern int font_size;
 extern int label_text_length;


long start_pos_X;
long start_pos_Y;
 extern long window_width;
 extern long window_height;


 vector<date_element> read_programs ()
 {
    string line; // ausgelesene Zeile
    int line_number = 0;

    vector <date_element> dates; // return value
    int date_index = 0; // increment

    // open file
    ifstream ifs;
    ifs.open(program_file);
    if (ifs.is_open() != true )
    {
        cerr <<"Can not open file "<<program_file<<"."<<endl;
    }

    // Datei auslesen:
    while ( ! ifs.eof() )
    {
        getline (ifs, line);
        line_number++;

        if (  line.empty() )
        {
            continue;
        }
        else if ( string(line).at(0) == '#' )
        {
            continue;
        }
        else
        {
            date_element date_x; // struct: label, program, img

            // label:
            string x_label;
            unsigned int bracket_pos = line.find_first_of( '[' );
            if (bracket_pos == line.npos)
            {
                cerr <<"Missing program label in line "<<line_number<<endl;
                x_label = "";
            }
            else
            {
                unsigned int bracket_close_pos =  line.find_last_of(']');
                if ( bracket_close_pos == line.npos )
                {
                    cerr<<"Missing closing bracket in line "<<line_number<<endl;
                    x_label = "";
                }
                else
                {
                    x_label = line.substr( bracket_pos + 1, bracket_close_pos - bracket_pos - 1 ); // all inside '[' and ']' (pos, pos+count)
                }
            }
            date_x.label = x_label;

            // Program call:
            string x_program;
            unsigned int brace_pos = line.find_first_of( '{' );
            if (brace_pos == line.npos)
            {
                cerr <<"Missing program call in line "<<line_number<<endl;
                x_program = "";
            }
            else
            {
                unsigned int brace_close_pos =  line.find_last_of('}');
                if ( brace_close_pos == line.npos )
                {
                    cerr<<"Missing closing brace in line "<<line_number<<endl;
                    x_program = "";
                }
                else
                {
                     x_program = line.substr( brace_pos + 1,  brace_close_pos - brace_pos - 1 ); // alles zwischen '{' und '}' (pos, pos+count)
                }
            }
            date_x.program = x_program;
            // Image:
            string x_img ;
            unsigned int angle_bracket_pos = line.find_first_of( '<' );
            if (angle_bracket_pos == line.npos)
            {
                x_img = "";
            }
            else
            {
                unsigned int angle_bracket_close_pos =  line.find_last_of('>');
                if ( angle_bracket_close_pos == line.npos )
                {
                    cerr<<"Missing closing angle bracket in line "<<line_number<<endl;
                    x_img = "";
                }
                else
                {
                     x_img = line.substr( angle_bracket_pos + 1,  angle_bracket_close_pos - angle_bracket_pos - 1 ); // all inside '{' and '}' (pos, pos+count)
                }
            }
            date_x.img = x_img;
            dates.push_back(date_x);
            date_index++;
        }

    } // End while
    ifs.close();
    return dates;
 }

// get settings from file config_dockbar.txt
 void read_settings ()
 {

    string line; // ausgelesene Zeile
    int line_number = 0;

    // open file
    ifstream ifs;
    ifs.open(config_file);
    if (ifs.is_open() != true )
    {
        cerr <<"can not open file "<<config_file<<"."<<endl;
    }

    // read file:
    while ( ! ifs.eof() )
    {
        getline (ifs, line);
        line_number++;

        if (  line.empty() )
        {
            continue;
        }
        else if ( string(line).at(0) == '#' ) // comment
        {
            continue;
        }
        else
        {
        	// if config.txt was opened with another program, there might be a '\r' left
        	if ( line.at(line.length() - 1) == '\r')
        	{
        		line = line.substr(0, line.size()-1);
        	}
            string key;
            string value;

            int equal_pos = line.find_first_of( '=' );
            key = line.substr( 0, equal_pos );
            value = line.substr( equal_pos + 1, line.length() );

            if (key.empty() || value.empty())
            {
                continue;
            }

            if (key == "date")
            {
                if (value == "yes")
                {
                    display_date = "yes";
                }
                else
                {
                    display_date = "no";
                }
            }
            else if (key == "size")
            {
                if (value == "small")
                {
                    button_size = 40;
                    font_size = 10;
                    label_text_length = 6;
                }
                else if (value == "medium")
                {
                    button_size = 60;
                    font_size = 12;
                    label_text_length = 8;
                }
                else if (value == "large")
                {
                    button_size = 80;
                    font_size = 14;
                    label_text_length = 9;
                }
            }
            else if (key == "columns")
            {
                sscanf(value.c_str(), "%d", &columns);

                if (columns * (border_space + button_size + border_space) > screen_width ) // larger than screen size
                {
                    columns = screen_width / (border_space + button_size + border_space);
                }
            }
            else if (key == "position")
            {
                if (value == "left")
                {
                    dockbar_position = "left";
                    start_pos_X = 0;
                    start_pos_Y = 0;
                    window_width = columns * (border_space + button_size + border_space);
                    window_height = screen_height;
                }
                else if (value == "right")
                {
                    dockbar_position = "right";
                    start_pos_X = screen_width - columns * (border_space + button_size + border_space);
                    start_pos_Y = 0;
                    window_width = columns * (border_space + button_size + border_space);
                    window_height = screen_height;
                }
                else if (value == "top")
                {
                    dockbar_position = "top";
                    start_pos_X = 0;
                    start_pos_Y = 0;
                    window_width = screen_width;
                    window_height = columns * (border_space + button_size + border_space);
                }
                else if (value == "bottom")
                {
                    dockbar_position = "bottom";
                    start_pos_X = 0;
                    start_pos_Y = screen_height - columns * (border_space + button_size + border_space);
                    window_width = screen_width;
                    window_height = (border_space + button_size + border_space) * columns;
                }
            }
        }
    } // End while

    ifs.close();
 }
