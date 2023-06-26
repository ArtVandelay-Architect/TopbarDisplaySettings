/* main.cpp
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

#include <iostream>
#include <cstdlib>
#include <vector>
#include "display-config-API.h"
#include <gio/gio.h>
#include <glib.h>
#include "timingFunctions.h"


GDBusConnection *mainDbusConnection;


int
main ()
{
	std::cout << "Getting Bus...\n";
	GError *error = NULL;
	mainDbusConnection = g_bus_get_sync (G_BUS_TYPE_SESSION, 
	                                     NULL, 
				             &error);
	if (error != NULL) {
		g_printerr("Unable to connect to the D-Bus session bus: %s\n", 
		           error->message);
		g_error_free(error);
		g_object_unref (mainDbusConnection);
		return 1;
	}

	std::cout << "Getting State...\n";
	DisplayState displayState;
	update_display_state (displayState);
	std::cout << "State updated!\n\n";

	for (auto& logicalMonitor : displayState.logicalMonitors) {
		std::cout << "Scale: " << logicalMonitor.scale << "\n";
		logicalMonitor.scale = logicalMonitor.scale * 2;
	}

goodclock start = get_start_time ();
	std::cout << "Applying State....\n";
	apply_display_state (displayState);
save_time_to_csv (start, "apply");


	clean_propsmaps (displayState);
	
	g_dbus_connection_close_sync (mainDbusConnection, NULL, &error);
	if (error != NULL) {
		g_printerr("Unable to close to the D-Bus session bus: %s\n", 
		           error->message);
		g_error_free(error);
		g_object_unref (mainDbusConnection);
		return 1;
	}
	g_object_unref (mainDbusConnection);


	return 0;
}

/*

*/

/*
int notmain ()
{
	GVariantBuilder builder;
	GVariant *value;
	g_variant_builder_init (&builder, G_VARIANT_TYPE ("as"));

	g_variant_builder_add (&builder, "s", "when");
	g_variant_builder_add (&builder, "s", "in");
	g_variant_builder_add (&builder, "s", "the");
	g_variant_builder_add (&builder, "s", "course");
	value = g_variant_builder_end(&builder);

	GVariantIter *iter;
	gchar *str;

	g_variant_get (value, "as", &iter);
	while (g_variant_iter_next (iter, "s", &str)) {
		g_print ("Value: %s\n", str);
		//free (gstr);
	}
	g_variant_unref (value);
	g_variant_iter_free (iter);
	
}

*/