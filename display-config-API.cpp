/* display-config-API.cpp
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
#include <gio/gio.h>
#include <glib-object.h>
#include <iostream>

void 
update_display_state (DisplayState &displayState) 
{
	std::cout << "Calling Dbus\n";
	GVariant *state;
	state = g_dbus_connection_call_sync (mainDbusConnection,
	                                     "org.gnome.Mutter.DisplayConfig",
	                                     "/org/gnome/Mutter/DisplayConfig",
	                                     "org.gnome.Mutter.DisplayConfig",
	                                     "GetCurrentState",
	                                     NULL,
	                                     NULL,
	                                     G_DBUS_CALL_FLAGS_NO_AUTO_START,
	                                     -1,
	                                     NULL,
	                                     NULL);

	if (!state)
		return;
	
	guint32 serial;
	GVariantIter *monitors;
	GVariantIter *logicalMonitors;
	GVariantIter *props;

	std::cout << "Dissecting variant\n";
	g_variant_get (state,
	               CURRENT_STATE_FORMAT,
		       &serial,
		       &monitors,
		       &logicalMonitors,
		       &props);

	std::cout << "Constructing serial\n";
	displayState.serial = (int) serial;
	std::cout << "Constructing monitors\n";
	construct_monitors (monitors, displayState);
	std::cout << "Constructing logical monitors\n";
	construct_logical_monitors (logicalMonitors, displayState);	
	std::cout << "Constructing props\n";
	displayState.props = construct_propsmap (props);

	g_variant_iter_free (monitors);
	g_variant_iter_free (logicalMonitors);
	g_variant_iter_free (props);
	g_variant_unref (state);

	std::cout << "State constructed!\n";
}

void
construct_monitors (GVariantIter *monitors,
                    DisplayState &displayState)
{
	GVariant *monitorPtr = NULL;
	gchar *connector, *vendor, *product, *serial;
	GVariantIter *modes = NULL;
	GVariantIter *props = NULL;
	while (g_variant_iter_next (monitors, "@" MONITOR_FORMAT, &monitorPtr)) {
		//std::cout << "Dissecting monitor variant\n";
		g_variant_get (monitorPtr,
		               MONITOR_FORMAT,
			       &connector,
			       &vendor,
			       &product,
			       &serial,
			       &modes,
			       &props);
		//std::cout << "Done dissecting monitor variant\n";
		Monitor monitor;
		monitor.connector = std::string (connector);
		monitor.vendor = std::string (vendor);
		monitor.product = std::string (product);
		monitor.serial = std::string (serial);
		construct_modes (modes, monitor);
		monitor.props = construct_propsmap (props);

		displayState.monitors.push_back (monitor);
		displayState.monitorsHash[monitor.connector] = monitor;

		g_variant_unref (monitorPtr);
		g_variant_iter_free (modes);
		g_variant_iter_free (props);
		free (connector);
		free (vendor);
		free (product);
		free (serial);
	}

}

void
construct_modes (GVariantIter *modes,
                 Monitor      &monitor)
{
	GVariant *modePtr = NULL;
	gchar *id;
	int width, height;
	double refRate, prefScale;
	GVariantIter *supportedScales = NULL;
	GVariantIter *props = NULL;
	
	while (g_variant_iter_next (modes, "@" MODE_FORMAT, &modePtr)) {
		//std::cout << "Dissecting mode variant\n";
		//printf("modePtr: %d\n", modePtr);
		g_variant_get (modePtr,
		               MODE_FORMAT,
		               &id,
			       &width,
			       &height,
			       &refRate,
			       &prefScale,
			       &supportedScales,
			       &props);
		//std::cout << "Done dissecting mode variant\n";
		Mode mode;
		mode.id = std::string(id);
		mode.width = width;
		mode.height = height;
		mode.refreshRate = refRate;
		mode.preferredScale = prefScale;

		double scale;
		while (g_variant_iter_next (supportedScales, "d", &scale)) {
			mode.supportedScales.push_back(scale);
		}

		mode.props = construct_propsmap (props);

		monitor.modes.push_back(mode);

		g_variant_unref (modePtr);
		g_variant_iter_free (supportedScales);
		g_variant_iter_free (props);
		free (id);
	}	
}
	

void
construct_logical_monitors (GVariantIter *logicalMonitors,
                            DisplayState &displayState)
{
	GVariant *logicalMonitorPtr = NULL;
	int x, y;
	double scale;
	guint32 transform;
	bool primary;
	GVariantIter *monitorSpecs = NULL;
	while (g_variant_iter_next (logicalMonitors, LOGICAL_MONITOR_ATFORMAT, &logicalMonitorPtr)) {
		g_variant_get (logicalMonitorPtr,
		               LOGICAL_MONITOR_FORMAT,
			       &x,
			       &y,
			       &scale,
			       &transform,
			       &primary,
			       &monitorSpecs,
			       NULL); //No props for logical monitors
		LogicalMonitor logicalMonitor;
		logicalMonitor.x = x;
		logicalMonitor.y = y;
		logicalMonitor.scale = scale;
		logicalMonitor.transform = (int) transform;
		logicalMonitor.primary = primary;
		
		GVariant *monitorSpecPtr = NULL;
		gchar *connector, *vendor, *product, *serial;
		while (g_variant_iter_next (monitorSpecs, "@" MONITOR_SPEC_FORMAT, &monitorSpecPtr)) {
			g_variant_get (monitorSpecPtr,
			               MONITOR_SPEC_FORMAT,
				       &connector,
				       &vendor,
				       &product,
				       &serial);
			MonitorSpec monitorSpec;
			monitorSpec.connector = std::string (connector);
			monitorSpec.vendor = std::string (vendor);
			monitorSpec.product = std::string (product);
			monitorSpec.serial = std::string (serial); 
 
 
			logicalMonitor.monitors.push_back(monitorSpec);

			g_variant_unref (monitorSpecPtr);
			free (connector);
			free (vendor);
			free (product);
			free (serial);
		}
		displayState.logicalMonitors.push_back(logicalMonitor);	

		g_variant_iter_free (monitorSpecs);
		g_variant_unref (logicalMonitorPtr);	
	}
	
}

propsmap 
construct_propsmap (GVariantIter *props) 
{
	propsmap newPropsmap;
	gchar *cstr;
	GVariant *var = NULL;
	while (g_variant_iter_next (props, "{&sv}", &cstr, &var)) {
		std::string cppstr(cstr);
		newPropsmap[cppstr] = g_variant_ref(var);
		free (cstr);
		g_variant_unref(var);
	}
	return newPropsmap;
}

void 
apply_display_state (const DisplayState &displayState)
{
	std::cout << "Configuring Logical Monitors\n";
	GVariant * logicalMonitorParameters = config_logical_monitors(displayState);

	//Config props
	std::cout << "Configuring props\n";
	GVariantBuilder propsBuilder;
	g_variant_builder_init (&propsBuilder, G_VARIANT_TYPE("a{sv}"));
	bool sclm = false; //supports changing layout mode
	std::string key("supports-changing-layout-mode");
	if (displayState.props.count(key) > 0) 
		sclm = g_variant_get_boolean (displayState.props.at(key));
	if (sclm) {
		guint32 layoutMode = g_variant_get_uint32 (displayState.props.at("layout-mode"));
		g_variant_builder_add (&propsBuilder, "{sv}", "layout-mode", g_variant_new_uint32 (layoutMode));
	}
	GVariant * propsParameters = g_variant_builder_end (&propsBuilder);

	//Try to Verify first
	std::cout << "Verifying parameters\n";
	GVariant * displayParametersVerify = g_variant_new (APPLY_MONITOR_CONFIG_PARAMETER,
	                                                    displayState.serial,
	                                                    APPLY_MONITOR_CONFIG_METHOD_VERIFY,
	                                                    logicalMonitorParameters,
	                                                    propsParameters);
	std::cout << "Constructing Perma\n";
	GVariant * displayParametersPerma = g_variant_new (APPLY_MONITOR_CONFIG_PARAMETER,
			                                   displayState.serial,
			                                   APPLY_MONITOR_CONFIG_METHOD_PERSISTENT,
			                                   logicalMonitorParameters,
			                                   propsParameters);

	GVariant * result = NULL;
	GError *error = NULL;
	result = g_dbus_connection_call_sync (mainDbusConnection,
	                                      "org.gnome.Mutter.DisplayConfig",
	                                      "/org/gnome/Mutter/DisplayConfig",
	                                      "org.gnome.Mutter.DisplayConfig",
	                                      "ApplyMonitorsConfig",
	                                      displayParametersVerify,
	                                      NULL,
	                                      G_DBUS_CALL_FLAGS_NO_AUTO_START,
	                                      -1,
	                                      NULL,
	                                      &error);


	if (error != NULL || result == NULL) {
		g_warning ("Verify ApplyMonitorsConfig parameters failed: %d %s\n", error->code, error->message);
	} else { //Verification successful
		std::cout << "Verification successful\n";						   
		result = g_dbus_connection_call_sync (mainDbusConnection,
		                                      "org.gnome.Mutter.DisplayConfig",
		                                      "/org/gnome/Mutter/DisplayConfig",
		                                      "org.gnome.Mutter.DisplayConfig",
		                                      "ApplyMonitorsConfig",
		                                      displayParametersPerma,
		                                      NULL,
		                                      G_DBUS_CALL_FLAGS_NO_AUTO_START,
		                                      -1,
		                                      NULL,
		                                      &error);
		std::cout << "sync called\n";
		//g_variant_unref (displayParametersPerma);
	}


	//g_variant_unref (logicalMonitorParameters);
	//g_variant_unref (propsParameters);
	//g_variant_unref (displayParametersVerify);
}

GVariant * 
config_logical_monitors (const DisplayState &displayState)
{
	GVariantBuilder builder;
	g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(" LOGICAL_MONITOR_CONF ")"));

	for (const auto& logicalMonitor : displayState.logicalMonitors) {
		g_variant_builder_add (&builder,
		                       LOGICAL_MONITOR_CONF_AT,
				       logicalMonitor.x,
				       logicalMonitor.y,
				       logicalMonitor.scale,
				       logicalMonitor.transform,
				       logicalMonitor.primary,
				       config_monitors_conf (displayState, logicalMonitor));
	}
	return g_variant_builder_end (&builder);
}


GVariant * 
config_monitors_conf (const DisplayState   &displayState, 
                      const LogicalMonitor &logicalMonitor)
{
	std::cout << "Configuring monitors conf\n";
	GVariantBuilder builder;
	g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(" MONITOR_CONF ")"));

	for (const auto & monitorSpec : logicalMonitor.monitors) {
		std::string sConnector = monitorSpec.connector;

		// Mode
		std::cout << "Configuring modes\n";
		std::string currentModeID = "", preferredModeID = "", modeID;
		for (const auto& mode : displayState.monitorsHash.at(sConnector).modes) {
			if (mode.props.count ("is-current") > 0) {
				if (g_variant_get_boolean(mode.props.at ("is-current"))) {
					currentModeID = mode.id;
					break;
				}
			}
			if (mode.props.count ("is-preferred") > 0) {
				if (g_variant_get_boolean(mode.props.at ("is-preferred")))
					preferredModeID = mode.id;
			}
		}
		if (currentModeID != "") { // Current mode exists
			modeID = currentModeID;
		}else {
			if (preferredModeID != "") { // Preferred mode exists
				modeID = preferredModeID;
			} else {
				// At least some mode exists?
				if (displayState.monitorsHash.at(sConnector).modes.size() != 0)
					modeID = displayState.monitorsHash.at(sConnector).modes.at(0).id;
				else
					g_warning ("No mode found for connector %s\n", sConnector.c_str());
			}
				
		}

		// Props
		std::cout << "Configuring monitors conf props\n";
		GVariantBuilder propsBuilder;
		g_variant_builder_init (&propsBuilder, G_VARIANT_TYPE("a{sv}"));
		bool ens = false; //enable underscanning
		std::string key("enable_underscanning");
		if (displayState.props.count(key) > 0) {
			ens = g_variant_get_boolean (displayState.props.at(key));
			g_variant_builder_add (&propsBuilder, 
			                       "{sv}", 
					       "enable_underscanning", 
					       g_variant_new_boolean (ens));
		} else {
			std::cout << "No underscanning, adding place holder\n";
			g_variant_builder_add (&propsBuilder, 
			                       "{sv}", 
					       "enable_underscanning", 
					       g_variant_new_boolean (false));			
		}

		const gchar *gConnector = sConnector.c_str();
		const gchar *gModeID = modeID.c_str();
		printf ("pp %s\n", gModeID);
		std::cout << "(" MONITOR_CONF ")" << "Assembling monitors conf\n";
		g_variant_builder_add (&builder,
		                       MONITOR_CONF_AT,
				       gConnector,
				       gModeID,
				       g_variant_builder_end (&propsBuilder));

		
		std::cout << "Finished assembling monitors conf\n";
		//g_variant_unref (propsParameters);
	}
	std::cout << "Monitors conf: ready to return\n";
	GVariant *res = g_variant_builder_end (&builder);
	return res;
}

void
clean_propsmaps (DisplayState &displayState)
{
	clean_propsmap (displayState.props);
	for (auto& monitor : displayState.monitors) {
		clean_propsmap (monitor.props);
		for (auto& mode : monitor.modes) {
			clean_propsmap (mode.props);
		}
	}
}


void
clean_propsmap (propsmap &newPropsmap)
{
	for (const auto& kv : newPropsmap) { //Key : Value
		g_variant_unref(kv.second);
	}
}