/* display-settings-wrappers.cpp
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
#include "display-settings-wrappers.h"
#include <iostream>

void set_display_scaling (double scale) 
{
	DisplayState displayState;
	update_display_state (displayState);
	for (auto& logicalMonitor : displayState.logicalMonitors) {
		logicalMonitor.scale = scale;
	}
	apply_display_state (displayState);
}

void set_display_scaling_relative (double scaleFactor)
{
	DisplayState displayState;
	update_display_state (displayState);
	for (auto& logicalMonitor : displayState.logicalMonitors) {
		logicalMonitor.scale *= scaleFactor;
	}
	apply_display_state (displayState);
}

double get_display_scaling ()
{
	DisplayState displayState;
	update_display_state (displayState);
	if (displayState.logicalMonitors.size() < 1) {
		std::cout << "No monitor found!\n";
		return -1;
	}
	return displayState.logicalMonitors[0].scale;	
}

void set_taskbar_visibility (bool visible)
{
	int err;
	if (visible) {
		err = system ("gsettings set org.gnome.shell.extensions.dash-to-dock manualhide false");
	} else {
		err = system ("gsettings set org.gnome.shell.extensions.dash-to-dock manualhide true");
	}
	if (err) {
		std::cout << "Error executing command in set_task_bar_visibility\n";
	}
}

void send_modprobe_reset_commands ()
{
	int err;
        err = system ("sudo modprobe -r wacom");
	err = system ("sudo modprobe wacom");
	if (err) {
		std::cout << "Error executing command in send_modprobe_reset_commands\n";
	}
}