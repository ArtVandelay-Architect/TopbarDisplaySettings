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

#include "display-config-API.h"
#include "timingFunctions.h"
#include "menu-config.h"
#include "display-settings-wrappers.h"

#include <iostream>
#include <cstdlib>


GDBusConnection *mainDbusConnection;

//1, 1.25, 1.49533

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

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

	//SystemTrayMenu systemTrayMenu ("./testIcon.png", "./menuUI.ui");

	//systemTrayMenu.status_icon_show ();

	//set_display_scaling (1.5);

	DisplayState displayState;

	update_display_state (displayState);
	
	for (const auto& monitor : displayState.monitors) {
		std::cout << "\nNumber of Modes: " << monitor.modes.size() << "\n";
		for (const auto& mode : monitor.modes) {
			std::cout << mode.id <<": Preferred scale: " << mode.preferredScale << "\n";
			for (const auto& scale : mode.supportedScales) {
				std::cout << scale << " ";
			}
			std::cout << "\n";
		}
	}

	gtk_main();

	return 0;
}
