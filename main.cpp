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

int
main ()
{
	GDBusConnection *connection;
	GError *error = NULL;

	connection = g_bus_get_sync (G_BUS_TYPE_SESSION, 
	                             NULL, 
				     &error);
	if (error != NULL) {
		g_printerr("Unable to connect to the D-Bus session bus: %s\n", 
		           error->message);
		g_error_free(error);
		return 1;
	}


	return EXIT_SUCCESS;
}