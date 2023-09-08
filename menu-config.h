/* menu-config.h
 *
 * Copyright 2023 Luke Li
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "timingFunctions.h"

#include <gtk/gtk.h>
#include "display-settings-wrappers.h"
//#include <libappindicator/app-indicator.h>
//#include <libayatana-appindicator/app-indicator.h>


#ifndef MENU_CONFIG_API_H
#define MENU_CONFIG_API_H

// A Menu that can be invoked from the system tray, implemented as a GtkWindow **not** GtkMenu
// For better flexibility
class SystemTrayMenu {
private:
	std::string       statusIconPath;
	GtkStatusIcon    *statusIcon;
	std::string       uiPath;
 
	GtkWidget        *window;         //GtkWindow
	GtkWidget        *mainGrid;       //GtkGrid
	GtkWidget        *scaleBox;       //GtkBox
	GtkWidget        *scaleDownBtn;   //GtkButton
	GtkWidget        *scaleDisplayed; //GtkLabel
	GtkWidget        *scaleUpBtn;     //GtkButton
	GtkWidget        *resetBtn;       //GtkButton
	GtkWidget        *taskSW;         //GtkSwitch
	GtkWidget        *closeBtn;       //GtkButton

	// Value displayed on scaleDisplayed are percentage rouneded to 25%
	// scaleValue is updated when the program is launched, but is then only
	// ...updated internally, meaning external change will not be reflected
	long unsigned int scaleIndex;
	std::string       scaleValueS;
	std::vector<int>  scaleKeys; //Approximate values
	ScalesMap         supportedScales; //Approximate value : actual value

public:        
	SystemTrayMenu                              (std::string           newStatusIconPath, //path of a image file
	                                             std::string           newUiPath);       //path of a .ui file
	~SystemTrayMenu                             ();

	// Copy & Assignment methods are unlikely to be used, but can suppress warnings
	SystemTrayMenu                              (const SystemTrayMenu& other); 
	SystemTrayMenu& operator=                   (const SystemTrayMenu& other);

	// Access methods just in case
	std::string     get_status_icon_path        () const;
	std::string     get_ui_path                 () const;

	// Show & hide tray icon
	void            status_icon_show            ();
	void            status_icon_hide            ();

	// Refresh the scaleDisplayed label, usually used after a change of scaleValueS
	void            refresh_scale_displayed     ();
	// Reset the status icon, used when the program starts or after a change of display scale
	void            construct_status_icon       ();
	// Reset everything in the popup window, used when the program starts or after a change of display scale
	void            construct_window            ();
	// Set a widget's background to red
	void            set_widget_bkgd_to_red      (GtkWidget*             widget); 

	// Call back functions (wrapped by public functions for proper g_signal_connect)
	// ...and their helpers
	void            status_icon_activated       ();
	void            calculate_window_coordinate (int                   &x, 
	                                             int                   &y,
                                                     GdkScreen            *&screen);

	void            scale_down_btn_clicked      ();
           
	void            scale_up_btn_clicked        ();
           
	void            reset_btn_clicked           ();
           
	void            close_btn_clicked           ();

	void            task_sw_set                 (gboolean state);
};

// Call back functions with namespace stm_
void     stm_status_icon_activated      (GtkStatusIcon */*statusIcon*/,
                                         gpointer       userData);

gboolean stm_status_icon_button_pressed (GtkStatusIcon */*statusIcon*/,
                                         GdkEventButton event,
					 gpointer       userData);

void     stm_scale_down_btn_clicked     (GtkButton     */*scaleDownBtn*/,
                                         gpointer       userData);
            
void     stm_scale_up_btn_clicked       (GtkButton     */*scaleUpBtn*/,
                                         gpointer       userData);
            
void     stm_reset_btn_clicked          (GtkButton     */*resetBtn*/,
                                         gpointer       userData);
            
void     stm_close_btn_clicked          (GtkButton     */*closeBtn*/,
                                         gpointer       userData);
        
gboolean stm_task_sw_set                (GtkSwitch     */*taskSW*/,
                                         gboolean       state,
                                         gpointer       userData);
				    

#endif