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


void 
on_menu_item_activate (GtkMenuItem *item, 
                       gpointer user_data)
{

	g_print("Menu item clicked!\n");
}

void
menu_init ()
{
        GtkWidget *menu;
	GtkWidget *menu_item;
	//GtkWidget *menu_item2;
	AppIndicator *indicator;
        
        gtk_init(NULL, NULL);

	indicator = app_indicator_new("my-indiator12153", "folder", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);

/* 	
	menu_item = gtk_menu_item_new_with_label("Menu Item 1");
	g_signal_connect(menu_item, "activate", G_CALLBACK(on_menu_item_activate), NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	menu_item2 = gtk_menu_item_new_with_label("Menu Item 2");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item2);

	app_indicator_set_menu(indicator, GTK_MENU(menu));
	app_indicator_set_label(indicator, "Lalala", "l");
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE); */
	
	menu = gtk_menu_new();

	GtkBuilder *builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder,
	                           "./menuUI.ui",
				   NULL);
	GtkWidget *box = GTK_WIDGET (gtk_builder_get_object (builder, "mainBox"));
	menu_item = gtk_menu_item_new ();
	//gtk_container_add (GTK_CONTAINER (menu_item), box);
	//gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

	gtk_menu_attach_to_widget (GTK_MENU (menu), box, NULL);

	
	app_indicator_set_menu(indicator, GTK_MENU(menu));

	app_indicator_set_label(indicator, "Lalala", "l");
	app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE); 	
			
	gtk_widget_show (menu);
	
	//gtk_widget_show (menu_item);
	gtk_widget_show (box);
	//gtk_widget_show(menu_item2);

	gtk_main();
}

