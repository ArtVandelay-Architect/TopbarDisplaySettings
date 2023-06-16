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

#ifndef DISPLAY_CONFIG_API_H
#define DISPLAY_CONFIG_API_H

struct DisplayState {
        guint32                *serial;
        GList                  *monitors;
        GHashTable             *logicalMonitors;
};

//Gets the current state of monitors from DBus in a synchronised manner, updating the 
//`displayState` structure afterwards
void	        get_current_state	(GDBusConnection *connection,
                                         GVariant        *currentStatePointer,
                                         DisplayState    &currentDisplayState);

void            construct_monitors      (DisplayState    &currentDisplayState,
                                         GVariantIter    *monitors,
			                 GVariantIter    *logicalMonitors);

#endif