/* menu-config.cpp
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

#include "menu-config.h"
#include <iostream>
#include <cstdio>

#include "display-config-API.h"
//#include "display-settings-wrappers.h"

#include <cmath>

bool isWholeNumber (double num, 
                    double epsilon = 1e-10) 
{
	double fractionalPart = std::fabs (num - std::round (num));
	return fractionalPart < epsilon;
}

SystemTrayMenu::SystemTrayMenu (std::string   newStatusIconPath,
	                        std::string   newUiPath) :
	statusIconPath  (newStatusIconPath), 
	statusIcon      (nullptr), 
	uiPath          (newUiPath), 
	window          (nullptr),
	mainGrid        (nullptr),   
	scaleBox        (nullptr),    
	scaleDownBtn    (nullptr), 
	scaleDisplayed  (nullptr),
	scaleUpBtn      (nullptr),
	resetBtn        (nullptr),   
	taskSW          (nullptr),     
	closeBtn        (nullptr),
	scaleIndex      (0),
	scaleValueS     (),
	scaleKeys       (),
	supportedScales ()
{	

	construct_status_icon ();
	construct_window ();
	
}

SystemTrayMenu::~SystemTrayMenu () 
{
	// Do nothing for now
}

SystemTrayMenu::SystemTrayMenu (const SystemTrayMenu& other) :
	statusIconPath  (other.get_status_icon_path ()), 
	statusIcon      (nullptr), 
	uiPath          (other.get_ui_path ()), 
	window          (nullptr),
	mainGrid        (nullptr),   
	scaleBox        (nullptr),    
	scaleDownBtn    (nullptr), 
	scaleDisplayed  (nullptr),
	scaleUpBtn      (nullptr),
	resetBtn        (nullptr),   
	taskSW          (nullptr),     
	closeBtn        (nullptr),
	scaleIndex      (0),
	scaleValueS     (),
	scaleKeys       (),
	supportedScales ()
{
	// Do nothing for now
	/*
	// Store the paths for future reference
	statusIconPath = other.get_status_icon_path ();
	uiPath = other.get_ui_path ();

	// Build the popup window AKA menu
	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, uiPath.c_str (), NULL);
	window = GTK_WIDGET (gtk_builder_get_object (builder, "popupWindow"));

	// Create a status icon and ignore the fact that it is deprecated
	// Most "modern" ways of creating a system tray icon ended up using GtkStatusIcon anyways
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	statusIcon = gtk_status_icon_new_from_file (statusIconPath.c_str ());
	G_GNUC_END_IGNORE_DEPRECATIONS
	*/
}

SystemTrayMenu& SystemTrayMenu::operator= (const SystemTrayMenu& other) 
{
	// Do nothing for now
	statusIconPath = other.get_status_icon_path ();
	uiPath = other.get_ui_path ();
	return *this;
}

std::string SystemTrayMenu::get_status_icon_path () const
{
	return statusIconPath;
}

std::string SystemTrayMenu::get_ui_path () const
{
	return uiPath;
}

void SystemTrayMenu::status_icon_show ()
{
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_visible (statusIcon, TRUE);
	G_GNUC_END_IGNORE_DEPRECATIONS
}

void SystemTrayMenu::status_icon_hide ()
{
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_visible (statusIcon, FALSE);
	G_GNUC_END_IGNORE_DEPRECATIONS
}

void SystemTrayMenu::refresh_scale_displayed ()
{

	scaleValueS = std::to_string (scaleKeys[scaleIndex]) + "%";
	//std::cout << "scaleValueS: " << scaleValueS << "\n";
	const char * scaleC = scaleValueS.c_str();
	gtk_label_set_label (GTK_LABEL (scaleDisplayed), scaleC);

}

void SystemTrayMenu::construct_status_icon ()
{
	if (statusIcon != NULL) {
		G_GNUC_BEGIN_IGNORE_DEPRECATIONS
		gtk_status_icon_set_visible (statusIcon, FALSE);
		G_GNUC_END_IGNORE_DEPRECATIONS	
		g_object_unref (statusIcon);
	}
	// Create a status icon and ignore the fact that it is deprecated
	// Most "modern" ways of creating a system tray icon ended up using GtkStatusIcon anyways
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	statusIcon = gtk_status_icon_new_from_file (statusIconPath.c_str ());
	G_GNUC_END_IGNORE_DEPRECATIONS
	g_signal_connect (statusIcon, "activate", G_CALLBACK (stm_status_icon_activated), this);
	//g_signal_connect (statusIcon, "button-press-event", G_CALLBACK (stm_status_icon_button_pressed), this);

	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_set_visible (statusIcon, TRUE);
	G_GNUC_END_IGNORE_DEPRECATIONS	
}

void SystemTrayMenu::construct_window ()
{
	if (window != NULL) {
		gtk_widget_hide (window);
		gtk_widget_destroy (window);
		window = NULL;
	} else {
		/* scaleIndex now updates with get_supported_scales
		// Get the current scaleIndex by seraching through possible scales
		// Not the most efficient, but I cannot be bothered
		double scale = get_display_scaling ();
		for (const auto& pair : supportedScales) {
			if (scale == pair.second) {
				for (int i = 0; i < scaleKeys.size(); i++) {
					if (scaleKeys[i] == pair.first) {
						scaleIndex = i;
						break;
					}
				}
				break;
			}
		}
		*/
	}

	// Build the popup window AKA menu
	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, uiPath.c_str (), NULL);
	window = GTK_WIDGET  (gtk_builder_get_object (builder, "popupWindow"));
	//Grab all the other widget to be programmed
	mainGrid = GTK_WIDGET (gtk_builder_get_object (builder, "mainGrid"));
	scaleBox = GTK_WIDGET (gtk_builder_get_object (builder, "scaleBox"));
	scaleDownBtn = GTK_WIDGET (gtk_builder_get_object (builder, "scaleDownBtn"));
	scaleDisplayed = GTK_WIDGET (gtk_builder_get_object (builder, "scaleDisplayed"));
	scaleUpBtn = GTK_WIDGET (gtk_builder_get_object (builder, "scaleUpBtn"));
	resetBtn = GTK_WIDGET (gtk_builder_get_object(builder, "resetBtn"));
	taskSW = GTK_WIDGET (gtk_builder_get_object (builder, "taskSW"));
	closeBtn = GTK_WIDGET (gtk_builder_get_object (builder, "closeBtn"));

	
	g_signal_connect (scaleDownBtn, "clicked", G_CALLBACK (stm_scale_down_btn_clicked), this);
	g_signal_connect (scaleUpBtn, "clicked", G_CALLBACK (stm_scale_up_btn_clicked), this);
	g_signal_connect (resetBtn, "clicked", G_CALLBACK (stm_reset_btn_clicked), this);
	g_signal_connect (closeBtn, "clicked", G_CALLBACK (stm_close_btn_clicked), this);
	g_signal_connect (taskSW, "state-set", G_CALLBACK (stm_task_sw_set), this);

	// Get ready for scale changes
	supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	// If we are at extreme values, disable one button
	if (scaleIndex == 0)
		gtk_widget_set_sensitive (scaleDownBtn, FALSE);
	else if (scaleIndex == (scaleKeys.size () - 1))
		gtk_widget_set_sensitive (scaleUpBtn, FALSE);

	// Set the taskbar switch's intial state 
	bool taskbarVisibility = get_taskbar_visibility ();
	gtk_switch_set_state (GTK_SWITCH (taskSW), taskbarVisibility);

	set_widget_bkgd_to_red (gtk_bin_get_child (GTK_BIN (closeBtn)));

	refresh_scale_displayed ();
}

void SystemTrayMenu::set_widget_bkgd_to_red (GtkWidget* widget) 
{
	gchar *css = g_strdup_printf ("%s { background-color: IndianRed; }",
		                       gtk_widget_class_get_css_name (GTK_WIDGET_GET_CLASS (widget)));

	GtkCssProvider *css_provider = gtk_css_provider_new ();
	gtk_css_provider_load_from_data (css_provider, css, -1, nullptr);
	gtk_style_context_add_provider (gtk_widget_get_style_context (widget),
	                                GTK_STYLE_PROVIDER (css_provider),
	                                GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	gtk_widget_show(widget);
	
	g_free(css);
}

void SystemTrayMenu::status_icon_activated ()
{
	std::cout << "Status icon activated\n";

	int x=10, y=0;
	GdkScreen *screen;

	calculate_window_coordinate (x, y, screen);

	gtk_window_move (GTK_WINDOW (window), x, y);
	gtk_widget_show_all (window);
	gtk_window_move (GTK_WINDOW (window), x, y);
}

void SystemTrayMenu::calculate_window_coordinate (int        &x,
                                                  int        &y,
						  GdkScreen *&screen)
{
	GdkRectangle area;
	GtkOrientation orientation;
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	gtk_status_icon_get_geometry (statusIcon, &screen, &area, &orientation);
	G_GNUC_END_IGNORE_DEPRECATIONS

	int winWidth, winHeight;
	gtk_window_get_size (GTK_WINDOW (window), &winWidth, &winHeight);

	//The X,Y coordinate of the centre of the icon
	int areaX = area.x + area.width/2;
	int areaY = area.y + area.height/2;

	// Used for internal settings only, decides where the popup window will be
	// ...in relation to the icon
	// 0:directly beneath
	// 1:left adjusted
	short displayLocation = 1;

	switch (displayLocation) {
		case 0:
			x = areaX - winWidth/2;
			y = areaY + area.height/2;
			break;
		case 1:
			x = areaX;
			y = areaY + area.height/2;
			break;
		default:
			x = 0;
			y = 0;
			std::cout << "Unknown displayLocation in calculate_window_coordinate\n";
	}
	std::cout << "x: " << x << " y: " << y <<"\n";
	// TODO: figure out the screen dimension at high scale
	
}

void SystemTrayMenu::scale_down_btn_clicked ()
{
	// Update the supported scales, in case the users did something funny outside the program
	supportedScales = get_supported_scales (scaleKeys, scaleIndex);

	// If fractional scaling is off, we should probably turn it on
	if (!get_fractional_scaling ()) { 
		set_fractional_scaling (true);
		supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	}

	scaleIndex -= 1;

	// Exception handling
	if (scaleIndex > (scaleIndex + 1)) { //Unsigned integers can only have zero overflow
		g_warning ("scale index underflow in scale_down_btn_clicked");
		scaleIndex = 0;
	} else {
		set_display_scaling (supportedScales[scaleKeys[scaleIndex]]);
	}

	// If the scale is now a whole number, we should turn off fractional scaling
	if (isWholeNumber (supportedScales[scaleKeys[scaleIndex]])) {
		set_fractional_scaling (false);
		supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	}

	// If the scales are not on the boundary, enable buttons
	if (scaleIndex < (scaleKeys.size() - 1)) {
		gtk_widget_set_sensitive (scaleUpBtn, TRUE);
	} else {
		gtk_widget_set_sensitive (scaleUpBtn, FALSE);
	}
	if (scaleIndex > 0) {
		gtk_widget_set_sensitive (scaleDownBtn, TRUE);
	} else {
		gtk_widget_set_sensitive (scaleDownBtn, FALSE);
	}

	construct_status_icon ();
	construct_window ();
}

void SystemTrayMenu::scale_up_btn_clicked ()
{
	// Update the supported scales, in case the users did something funny outside the program
	supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	
	// If fractional scaling is off, we should probably turn it on
	if (!get_fractional_scaling ()) { 
		set_fractional_scaling (true);
		supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	}

	for (const auto &scale : scaleKeys) {
		std::cout << supportedScales[scale] << ":" << supportedScales[scale] << " ";
	}
	std::cout << scaleIndex << "\n";

	scaleIndex += 1;

	std::cout << scaleIndex << " " << scaleKeys[scaleIndex] << "\n";

	// Exception handling
	if (scaleIndex > (scaleKeys.size () - 1)) {
		g_warning ("scale index underflow in scale_down_btn_clicked");
		scaleIndex = 0;
	} else {
		set_display_scaling (supportedScales[scaleKeys[scaleIndex]]);
	}

	// If the scale is now a whole number, we should turn off fractional scaling
	if (isWholeNumber (supportedScales[scaleKeys[scaleIndex]])) {
		set_fractional_scaling (false);
		supportedScales = get_supported_scales (scaleKeys, scaleIndex);
	}

	// If the scales are not on the boundary, enable buttons
	if (scaleIndex < (scaleKeys.size () - 1)) {
		gtk_widget_set_sensitive (scaleUpBtn, TRUE);
	} else {
		gtk_widget_set_sensitive (scaleUpBtn, FALSE);
	}
	if (scaleIndex > 0) {
		gtk_widget_set_sensitive (scaleDownBtn, TRUE);
	} else {
		gtk_widget_set_sensitive (scaleDownBtn, FALSE);
	}

	construct_status_icon ();
	construct_window ();
}

void SystemTrayMenu::reset_btn_clicked ()
{
	send_modprobe_reset_commands ();
}

void SystemTrayMenu::close_btn_clicked ()
{
	gtk_widget_hide (window);
}

void SystemTrayMenu::task_sw_set (gboolean state)
{
	set_taskbar_visibility (state);
}


void stm_status_icon_activated (GtkStatusIcon */*statusIcon*/, 
                                gpointer userData) 
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->status_icon_activated ();
}

gboolean stm_status_icon_button_pressed (GtkStatusIcon */*statusIcon*/,
                                         GdkEventButton event,
                                         gpointer /*userData*/)
{
	// Not Used
	std::cout << "status icon button pressed: " << event.type << "\n";
	return true;
}

void stm_scale_down_btn_clicked (GtkButton */*scaleDownBtn*/,
                                 gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->scale_down_btn_clicked ();
}

void stm_scale_up_btn_clicked (GtkButton */*scaleUpBtn*/,
                               gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->scale_up_btn_clicked ();
}

void stm_reset_btn_clicked (GtkButton */*resetBtn*/,
                            gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->reset_btn_clicked ();
}

void stm_close_btn_clicked (GtkButton */*closeBtn*/,
                            gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->close_btn_clicked ();
}

gboolean stm_task_sw_set (GtkSwitch */*taskSW*/,
                          gboolean   state,
                          gpointer   userData) 
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->task_sw_set (state);
	return false; //Allow the default handler to pass through as well
}


