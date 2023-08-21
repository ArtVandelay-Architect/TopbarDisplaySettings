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
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

void set_display_scaling (double scale) 
{
	// No longer needed, exact scaling is now handled in get_supported_scales
	/* 
	// Approximate the scale to nearest 0.25
	bool scaleSet = false;
	for (double s = 1; s <= 3.1; s+=0.25){
		double absDiff = (scale > s) ? (scale - s) : (s - scale);
		if (absDiff < 0.25) {
			scale = s;
			scaleSet = true;
			break;
		}
	}
	if (!scaleSet) {
		std::cout << "Invalid scale!\n";
		scale = 1;
	}

	if (isWholeNumber (scale)) {
		set_fractional_scaling (false);
	} else {
		set_fractional_scaling (true);
	}
	*/

	DisplayState displayState;
	update_display_state (displayState);
	for (auto& logicalMonitor : displayState.logicalMonitors) {
		logicalMonitor.scale = scale;
	}

	apply_display_state (displayState);
	//return scale;
}

double set_display_scaling_relative (double scaleFactor) //Buggy!
{
	DisplayState displayState;
	update_display_state (displayState);
	for (auto& logicalMonitor : displayState.logicalMonitors) {
		logicalMonitor.scale *= scaleFactor;
	}
	apply_display_state (displayState);
	return scaleFactor;
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

bool get_fractional_scaling ()
{
	// Get all the enabled experimental features
	const char * command = "gsettings get org.gnome.mutter experimental-features";
	FILE* pipe = popen (command, "r");
	char buffer[128];
	std::vector<std::string> outputLines;
	while (fgets (buffer, sizeof (buffer), pipe) != nullptr) {
		outputLines.emplace_back (buffer);
	}
	pclose(pipe);
	std::string features = outputLines[0]; //Likely only one line

	if (features == "@as []\n") //empty
		return false;
	// Otherwise, features will be like ['x11-randr-fractional-scaling', 'aaa']
	features = features.substr (1, features.length () - 3); //Remove brackets

	// Split the string based on commas
	std::istringstream ss (features);
	std::string token;
	
	while (std::getline (ss, token, ',')) {
		if (token == "'x11-randr-fractional-scaling'")
			return true;
	}
	
	return false;
}

void set_fractional_scaling (bool mode)
{
	// Get all the enabled experimental features
	const char * command = "gsettings get org.gnome.mutter experimental-features";
	FILE* pipe = popen (command, "r");
	char buffer[128];
	std::vector<std::string> outputLines;
	while (fgets (buffer, sizeof (buffer), pipe) != nullptr) {
		outputLines.emplace_back (buffer);
	}
	pclose(pipe);
	std::string features = outputLines[0]; //Likely only one line

	std::string mainCommand = "gsettings set org.gnome.mutter experimental-features \"";
	//std::cout << "Features: " << features << " " << (features == "@as []") << "\n";
	if (features == "@as []\n") { //No current features
		if (mode) {
			mainCommand = mainCommand + "['x11-randr-fractional-scaling']";
		} else { //Already false, no need to set
			return;
		}
	} else { //features like ['x11-randr-fractional-scaling', 'aaa']
		features = features.substr (1, features.length () - 3); //Remove brackets
		// Split the string based on commas
		std::istringstream ss (features);
		std::string token;
		std::vector<std::string> elements;
		
		while (std::getline (ss, token, ',')) {
			if (token == "'x11-randr-fractional-scaling'") {//Already exists
				if (mode) {
					return;
				}
				continue;
			}
			elements.push_back (token);
		}
		if (mode) { //Does not exist, need to be added
			elements.push_back ("'x11-randr-fractional-scaling'");
		}

		if (elements.size() > 0) { //If there are any features to be added
			std::string key = "[";
			for (const auto& e : elements) {
				key = key + e + ",";
			}
			key = key.substr (0, key.length () - 1); //Remove final comma
			mainCommand = mainCommand + key + "]";
		} else { //Otherwise, add a square bracket
			mainCommand = mainCommand + "[]";
		}
	}
	mainCommand = mainCommand + "\"";
	std::cout << "set_fractional_scaling: Engaging command: " << mainCommand << "\n";
	int err = system (mainCommand.c_str());
	if (err) {
		std::cout << "Set fractional scaling failed\n";
	}
		
}

ScalesMap get_supported_scales (std::vector<int> &keys, long unsigned int &keyIndex)
{
	// scales supported by the main monitor, keys are scaling percentage rounded to 25%
	ScalesMap supportedScales;

	DisplayState displayState;
	update_display_state (displayState);

	// Only consider the first monitor for now
	const auto monitor = displayState.monitors[0];
	
	//Find the corresponding logical monitor and the current scale
	double currScale = 1;
	for (const auto& logicalMonitor : displayState.logicalMonitors) {
		for (const auto& monitorSpec : logicalMonitor.monitors) {
			if (monitorSpec.connector == monitor.connector) {
				currScale = logicalMonitor.scale;
			}
		}
	}

	// There does not exist a straight-forward way to find modes
	// We must guess
	Mode currMode;
	bool currentModeFound = false, preferredModeFound = false;
	for (const auto& mode : monitor.modes) {
		// If they are nice enough to include properties, great
		if (mode.props.count ("is-current") > 0) { //is-current should be priotised
			if (g_variant_get_boolean(mode.props.at ("is-current"))) {
				currentModeFound = true;
				currMode = mode;
				break;
			}
		}
		if (mode.props.count ("is-preferred") > 0) { //is-preferred will do
			if (g_variant_get_boolean(mode.props.at ("is-preferred"))) {
				preferredModeFound = true;
				currMode = mode;
			}
		}
	}
	if ((!currentModeFound) && (!preferredModeFound)) {
		if (monitor.modes.size () > 0) {
			currMode = monitor.modes.at(0);
		} else {
			g_warning ("Cannot find mode in update_supported_scales");
			return supportedScales;
		}
	}
	
	keys.clear();
	for (const double scale : currMode.supportedScales) {
		// The keys are the nearest 25% of scales 
		int minDiff = 1000, temp;
		for (int key = 100; key <= 500; key+=25) {
			temp = static_cast<int> ((key > scale * 100) ? (key - scale * 100) : (scale * 100 - key));
			if (temp > minDiff) {
				key -= 25;
				supportedScales[key] = scale;
				keys.push_back (key);
				break;
			}
			minDiff = temp;
		}
	}
	std::sort (keys.begin(), keys.end());

	for (long unsigned int i = 0; i < keys.size(); i++) {
		if (supportedScales[keys[i]] == currScale) {
			keyIndex = i;
			break;
		}
	}
	return supportedScales;
}