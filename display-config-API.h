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
#include <unordered_map>
#include <string>

#ifndef DISPLAY_CONFIG_API_H
#define DISPLAY_CONFIG_API_H

/*
MODE_FORMAT: (siiddada{sv})
MONITOR_SPEC_FORMAT: (ssss)
MONITOR_FORMAT: ((ssss)a(siiddada{sv})a{sv})
LOGICAL_MONITOR_FORMAT: (iiduba(ssss)a{sv})
LOGICAL_MONITORS_FORMAT: a(iiduba(ssss)a{sv})
LOGICAL_MONITOR_ATFORMAT: @(iiduba(ssss)a{sv})
CURRENT_STATE_FORMAT: (ua((ssss)a(siiddada{sv})a{sv})a(iiduba(ssss)a{sv})a{sv})
MONITOR_CONF: ssa{sv}
LOGICAL_MONITOR_CONF: iiduba(ssa{sv})
APPLY_MONITOR_CONFIG_PARAMETER: (uua(iiduba(ssa{sv}))a{sv})
*/

//Formats associated with `getCurrentState`
#define MODE_FORMAT "(siiddada{sv})"

#define MONITOR_SPEC_FORMAT "(ssss)"
#define MONITOR_FORMAT "(" MONITOR_SPEC_FORMAT "a" MODE_FORMAT "a{sv})"

#define LOGICAL_MONITOR_FORMAT "(iidub" "a" MONITOR_SPEC_FORMAT "a{sv})"
#define LOGICAL_MONITORS_FORMAT "a" LOGICAL_MONITOR_FORMAT
#define LOGICAL_MONITOR_ATFORMAT "@" LOGICAL_MONITOR_FORMAT

#define CURRENT_STATE_FORMAT "(u" "a" MONITOR_FORMAT LOGICAL_MONITORS_FORMAT "a{sv})"

//Formats associated with `applyMonitorsConfig`
#define MONITOR_CONF "ssa{sv}"
#define MONITOR_CONF_AT "(ss@a{sv})"

#define LOGICAL_MONITOR_CONF "iiduba(" MONITOR_CONF ")"
#define LOGICAL_MONITOR_CONF_AT "(iidub@a(" MONITOR_CONF "))"

#define APPLY_MONITOR_CONFIG_PARAMETER "(uu@a(" LOGICAL_MONITOR_CONF ")@a{sv})"

#define APPLY_MONITOR_CONFIG_METHOD_VERIFY 0
#define APPLY_MONITOR_CONFIG_METHOD_TEMPORARY 1
#define APPLY_MONITOR_CONFIG_METHOD_PERSISTENT 2

typedef std::unordered_map<std::string, GVariant *> propsmap;

extern GDBusConnection *mainDbusConnection;

// ---
// Structures responsible for processing `getCurrentState`

struct Mode {
	std::string         id;
	int                 width;
	int                 height;
	double              refreshRate;
	double              preferredScale;
	std::vector<double> supportedScales;
	propsmap            props;

	Mode() 
		: id(), width(0), height(0), refreshRate(0.0), preferredScale(0.0), supportedScales(), props() 
	{}
};

struct MonitorSpec {
	std::string connector;
	std::string vendor;
	std::string product;
	std::string serial;

	MonitorSpec()  
		: connector(), vendor(), product(), serial() 
	{}
};

struct LogicalMonitor {
	int                      x;
	int                      y;
	double                   scale;
	int                      transform;
	bool                     primary;
	std::vector<MonitorSpec> monitors;
	propsmap                 props;

	LogicalMonitor() 
		: x(0), y(0), scale(0.0), transform(0), primary(false), monitors(), props() 
	{}
};

struct Monitor {
	std::string       connector;
	std::string       vendor;
	std::string       product;
	std::string       serial;
	std::vector<Mode> modes;
	propsmap          props;

	Monitor()
		: connector(), vendor(), product(), serial(), modes(), props()
	{}
};

struct DisplayState {
	int                                      serial;
	std::vector<Monitor>                     monitors;
	std::vector<LogicalMonitor>              logicalMonitors;
	propsmap                                 props;

	std::unordered_map<std::string, Monitor> monitorsHash;

	DisplayState()
		: serial(0), monitors(), logicalMonitors(), props(), monitorsHash()
	{}
};

// ---
// Structures responsible for processing `applyMonitorsConfig`

struct MonitorConf {
	std::string connector;
	std::string modeID;
	propsmap    props;

	MonitorConf()
		: connector(), modeID(), props()
	{}
};

struct LogicalMonitorConf {
	int                      x;
	int                      y;
	double                   scale;
	int                      transform;
	bool                     primary;
	std::vector<MonitorConf> monitors;

	LogicalMonitorConf()
		: x(0), y(0), scale(0.0), transform(0), primary(false), monitors()
	{}
};

struct DisplayConfig {
	int                             serial;
	int                             method;
	std::vector<LogicalMonitorConf> logicalmonitors;
	propsmap                        props;

	DisplayConfig()
		: serial(0), method(0), logicalmonitors(), props()
	{}
};

// ---
// Functions

//Updates the `DisplayState` passed into the function by calling `getCurrentState` in dBus
void       update_display_state       (DisplayState         &displayState);
void       construct_monitors         (GVariantIter         *monitors,
                                       DisplayState         &displayState);
void       construct_modes            (GVariantIter         *modes,
                                       Monitor              &monitor);
void       construct_logical_monitors (GVariantIter         *logicalMonitors,
                                       DisplayState         &displayState);
propsmap   construct_propsmap         (GVariantIter         *props);

//Applies a given `DisplayState` using `applyMonitorsConfig` in dBus
void       apply_display_state        (const DisplayState   &displayState);
GVariant * config_logical_monitors    (const DisplayState   &displayState);
GVariant * config_monitors_conf       (const DisplayState   &displayState, 
                                       const LogicalMonitor &logicalMonitor);

void       clean_propsmaps            (DisplayState         &displayState);
void       clean_propsmap             (propsmap             &newPropsmap);
#endif