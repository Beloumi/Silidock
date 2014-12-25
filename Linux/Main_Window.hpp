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
 * Main_Window.hpp
 *
 *  Created on: 19.11.2014
 *      Author: Axel von dem Bruch
 */

#ifndef MAIN_WINDOW_HPP_
#define MAIN_WINDOW_HPP_


class Main_Window : public Fl_Window
{
    public:
        Main_Window();
        ~Main_Window() {}

        Fl_Button *set_button;
        Fl_Button *conf_button;
        Fl_Button *quit_button;
        Fl_Button *reload_button;
        Fl_Button *help_button;

        Fl_Group *group; // contains program buttons
};

#endif /* MAIN_WINDOW_HPP_ */
