#include "display-config-API.h"
#include <gio/gio.h>
#include <glib-object.h>


void 
update_display_state (DisplayState &displayState) 
{
	GVariant *state;
	state = g_dbus_connection_call_sync (dbusConnection,
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
	
	guint32 serial;
	GVariantIter *monitors;
	GVariantIter *logicalMonitors;
	GVariantIter *props;

	g_variant_get (state,
	               CURRENT_STATE_FORMAT,
		       &serial,
		       &monitors,
		       &logicalMonitors,
		       &props);

	displayState.serial = (int) serial;
	construct_monitors (monitors, displayState);
	construct_logical_monitors (logicalMonitors, displayState);	

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
		g_variant_get (monitorPtr,
		               MONITOR_FORMAT,
			       &connector,
			       &vendor,
			       &product,
			       &serial,
			       &modes,
			       &props);
		Monitor monitor;
		monitor.connector = std::string (connector);
		monitor.vendor = std::string (vendor);
		monitor.product = std::string (product);
		monitor.serial = std::string (serial);
		construct_modes (modes, monitor);
		monitor.props = construct_propsmap (props);

		displayState.monitors.push_back (monitor);
	}

	g_variant_unref (monitorPtr);
	g_variant_iter_free (modes);
	g_variant_iter_free (props);
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
	
	while (g_variant_iter_next (modes, "@" MODE_FORMAT, modePtr)) {
		g_variant_get (modePtr,
		               MODE_FORMAT,
		               &id,
			       &width,
			       &height,
			       &refRate,
			       &prefScale,
			       &supportedScales,
			       &props);
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
	}

	g_variant_unref (modePtr);
	g_variant_iter_free (supportedScales);
	g_variant_iter_free (props);
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
		logicalMonitor.transform = transform;
		logicalMonitor.primary = primary;
		
		GVariant *monitorSpecPtr = NULL;
		gchar *connector, *vendor, *product, *serial;
		while (g_variant_iter_next (monitorSpecs, "@" MONITOR_SPEC_FORMAT, &monitorSpecPtr)) {
			g_variant_get (monitorSpecPtr,
			               MONITOR_SPEC_FORMAT,
				       connector,
				       vendor,
				       product,
				       serial);
			MonitorSpec monitorSpec;
			monitorSpec.connector = std::string (connector);
			monitorSpec.vendor = std::string (vendor);
			monitorSpec.product = std::string (product);
			monitorSpec.serial = std::string (serial);
			logicalMonitor.monitors.push_back(monitorSpec);
		}

		displayState.logicalMonitors.push_back(logicalMonitor);		
	}
}

propsmap 
construct_propsmap (GVariantIter *props) 
{
	propsmap newPropsmap;
	const char *cstr;
	GVariant *var = NULL;
	while (g_variant_iter_next (props, "{&sv}", &cstr, &var)) {
		std::string cppstr(cstr);
		newPropsmap[cppstr] = var;
	}

	g_variant_unref(var);
	return newPropsmap;
}

