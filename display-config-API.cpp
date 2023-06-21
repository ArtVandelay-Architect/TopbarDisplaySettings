#include "display-config-API.h"
#include <gio/gio.h>
#include <glib-object.h>


void 
update_display_state (DisplayState &displayState) 
{
	GVariant *state;
	state = g_dbus_connection_call_sync (connection,
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

	displayState.serial = serial;
	construct_monitors (monitors, displayState);
	construct_logical_monitors (logicalMonitors, displayState);	

}

void
construct_monitors (GVariantIter *monitors,
                    DisplayState &displayState)
{
	
}

void
construct_logical_monitors (GVariantIter *logicalMonitors,
                            DisplayState &displayState)
{

}

propsmap 
construct_propsmap (GVariantIter *props) 
{
	propsmap newPropsmap;
	const char *cstr;
	GVariant *var = NULL;
	while (g_variant_iter_next(props, "{&sv}", &cstr, &var)) {
		std::string cppstr(cstr);
		newPropsmap[cppstr] = var;
	}
	return newPropsmap;
}

