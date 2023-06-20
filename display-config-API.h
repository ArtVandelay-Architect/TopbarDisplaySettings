/* display-config-API.h
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

#include <gio/gio.h>
#include <vector>

#ifndef DISPLAY_CONFIG_API_H
#define DISPLAY_CONFIG_API_H


#define MODE_BASE_FORMAT "siiddad"
#define MODE_FORMAT "(" MODE_BASE_FORMAT "a{sv})"
#define MODES_FORMAT "a" MODE_FORMAT
#define MONITOR_SPEC_FORMAT "(ssss)"
#define MONITOR_FORMAT "(" MONITOR_SPEC_FORMAT MODES_FORMAT "a{sv})"
#define MONITORS_FORMAT "a" MONITOR_FORMAT

#define LOGICAL_MONITOR_MONITORS_FORMAT "a" MONITOR_SPEC_FORMAT
#define LOGICAL_MONITOR_FORMAT "(iidub" LOGICAL_MONITOR_MONITORS_FORMAT "a{sv})"
#define LOGICAL_MONITORS_FORMAT "a" LOGICAL_MONITOR_FORMAT
#define LOGICAL_MONITOR_ATFORMAT "@" LOGICAL_MONITOR_FORMAT

#define CURRENT_STATE_FORMAT "(u" MONITORS_FORMAT LOGICAL_MONITORS_FORMAT "a{sv})"

struct DisplayState {
	guint32                *serial;
	GList                  *monitors;
	GHashTable             *logicalMonitors;
};

struct LogicalMonitor {
	int    x;               //Resolution X
	int    y;               //Resolution Y
	double scale;           //Display scale
	int    rotation;	//Rotation in degrees, negative = flipped
	int    isPrimary;	//0 = not primary
};

//std::vector<LogicalMonitor> logicalMonitors;
//guint32 globalSerial;

//Gets the current state of monitors from DBus in a synchronised manner, updating the 
//`displayState` structure afterwards
void	        get_current_state	(GDBusConnection *connection,
                                         GVariant        *currentStatePointer);

void            update_monitors      (GVariantIter    *logicalMonitorsIter);

#endif