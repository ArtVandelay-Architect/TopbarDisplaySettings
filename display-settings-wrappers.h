/* display-settings-wrappers.h
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

#ifndef DISPLAY_SETTINGS_WRAPPERS_H
#define DISPLAY_SETTINGS_WRAPPERS_H

// Sets the display scaling to `scale`, will be rounded to nearest 0.25
void   set_display_scaling          (double scale);
// Multiply the current scaling by `scale factor`
void   set_display_scaling_relative (double scaleFactor);
// Get the display scaling of one of the monitors
double get_display_scaling          ();
// Set the visibility of the taskbar
void   set_taskbar_visibility       (bool visible);
// Send reset commands to modprobe for wacom `sudo modprobe -r wacom && sudo modprobe wacom`
void   send_modprobe_reset_commands ();

#endif