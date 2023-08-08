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
#include "display-settings-wrappers.h"

SystemTrayMenu::SystemTrayMenu (std::string   newStatusIconPath,
	                        std::string   newUiPath) :
	statusIconPath (newStatusIconPath), 
	statusIcon (nullptr), 
	uiPath (newUiPath), 
	window (nullptr),
	mainGrid (nullptr),   
	scaleBox (nullptr),    
	scaleDownBtn (nullptr), 
	scaleDisplayed (nullptr),
	scaleUpBtn (nullptr),
	resetBtn (nullptr),   
	taskSW (nullptr),     
	closeBtn (nullptr),
	scaleValue (0),
	scaleValueS ()
{	



	// Create a status icon and ignore the fact that it is deprecated
	// Most "modern" ways of creating a system tray icon ended up using GtkStatusIcon anyways
	G_GNUC_BEGIN_IGNORE_DEPRECATIONS
	statusIcon = gtk_status_icon_new_from_file (statusIconPath.c_str ());
	G_GNUC_END_IGNORE_DEPRECATIONS



	// Build the popup window AKA menu
	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, uiPath.c_str (), NULL);
	window = GTK_WIDGET (gtk_builder_get_object (builder, "popupWindow"));
	//Grab all the other widget to be programmed
	mainGrid = GTK_WIDGET(gtk_builder_get_object(builder, "mainGrid"));
	scaleBox = GTK_WIDGET(gtk_builder_get_object(builder, "scaleBox"));
	scaleDownBtn = GTK_WIDGET(gtk_builder_get_object(builder, "scaleDownBtn"));
	scaleDisplayed = GTK_WIDGET(gtk_builder_get_object(builder, "scaleDisplayed"));
	scaleUpBtn = GTK_WIDGET(gtk_builder_get_object(builder, "scaleUpBtn"));
	resetBtn = GTK_WIDGET(gtk_builder_get_object(builder, "resetBtn"));
	taskSW = GTK_WIDGET(gtk_builder_get_object(builder, "taskSW"));
	closeBtn = GTK_WIDGET(gtk_builder_get_object(builder, "closeBtn"));

	g_signal_connect (statusIcon, "activate", G_CALLBACK(stm_status_icon_activated), this);
	g_signal_connect (scaleDownBtn, "clicked", G_CALLBACK(stm_scale_down_btn_clicked), this);
	g_signal_connect (scaleUpBtn, "clicked", G_CALLBACK(stm_scale_up_btn_clicked), this);
	g_signal_connect (resetBtn, "clicked", G_CALLBACK(stm_reset_btn_clicked), this);
	g_signal_connect (closeBtn, "clicked", G_CALLBACK(stm_close_btn_clicked), this);

	scaleValue = get_display_scaling ();
	scaleValueS = std::to_string (scaleValue);
	const char * scaleC = scaleValueS.c_str();
	gtk_label_set_label (GTK_LABEL (scaleDisplayed), scaleC);
}

SystemTrayMenu::~SystemTrayMenu () 
{
	// Do nothing for now
}

SystemTrayMenu::SystemTrayMenu (const SystemTrayMenu& other) :
	statusIconPath(other.get_status_icon_path ()), 
	statusIcon(nullptr), 
	uiPath(other.get_ui_path ()), 
	window(nullptr),
	mainGrid(nullptr),   
	scaleBox(nullptr),    
	scaleDownBtn(nullptr), 
	scaleDisplayed(nullptr),
	scaleUpBtn(nullptr),
	resetBtn(nullptr),   
	taskSW(nullptr),     
	closeBtn(nullptr),
	scaleValue (0),
	scaleValueS ()
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

void SystemTrayMenu::status_icon_activated ()
{
	std::cout << "Status icon activated\n";

	int x=0, y=0;
	GdkScreen *screen;
	calculate_window_coordinate (x, y, screen);
	gtk_window_move (GTK_WINDOW (window), x, y);
	gtk_widget_show_all (window);
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
	// 2:right adjusted
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

	
}

void SystemTrayMenu::scale_down_btn_clicked()
{
	
}

void SystemTrayMenu::scale_up_btn_clicked()
{

}

void SystemTrayMenu::reset_btn_clicked()
{

}

void SystemTrayMenu::close_btn_clicked()
{
	gtk_widget_hide (window);
}



void stm_status_icon_activated (GtkStatusIcon */*statusIcon*/,
                                gpointer       userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->status_icon_activated ();
}

void stm_scale_down_btn_clicked (GtkButton * /*scaleDownBtn*/,
                                 gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->scale_down_btn_clicked ();
}

void stm_scale_up_btn_clicked (GtkButton * /*scaleUpBtn*/,
                               gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->scale_up_btn_clicked ();
}

void stm_reset_btn_clicked (GtkButton * /*resetBtn*/,
                            gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->reset_btn_clicked ();
}

void stm_close_btn_clicked (GtkButton * /*closeBtn*/,
                            gpointer userData)
{
	SystemTrayMenu* thisMenu = static_cast<SystemTrayMenu*> (userData);
	thisMenu->close_btn_clicked ();
}