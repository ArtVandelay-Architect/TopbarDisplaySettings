#include "display-config-API.h"
#include <gio/gio.h>
#include <glib-object.h>

#define MODE_BASE_FORMAT "siiddad"
#define MODE_FORMAT "(" MODE_BASE_FORMAT "a{sv})"
#define MODES_FORMAT "a" MODE_FORMAT
#define MONITOR_SPEC_FORMAT "(ssss)"
#define MONITOR_FORMAT "(" MONITOR_SPEC_FORMAT MODES_FORMAT "a{sv})"
#define MONITORS_FORMAT "a" MONITOR_FORMAT

#define LOGICAL_MONITOR_MONITORS_FORMAT "a" MONITOR_SPEC_FORMAT
#define LOGICAL_MONITOR_FORMAT "(iidub" LOGICAL_MONITOR_MONITORS_FORMAT "a{sv})"
#define LOGICAL_MONITORS_FORMAT "a" LOGICAL_MONITOR_FORMAT

#define CURRENT_STATE_FORMAT "(u" MONITORS_FORMAT LOGICAL_MONITORS_FORMAT "a{sv})"


void get_current_state (GDBusConnection *connection,
                        GVariant        *currentStatePointer,
                        DisplayState    *currentDisplayState)
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

	guint32 *serial = NULL;
	GVariantIter *monitors = NULL;
	GVariantIter *logicalMonitors = NULL;
	GVariantIter *props = NULL;
	GError *error = NULL;

	g_variant_get (state,
	               CURRENT_STATE_FORMAT,
		       &serial,
		       &monitors,
		       &logicalMonitors,
		       &props);

	currentDisplayState->serial = serial;
	construct_monitors(currentDisplayState, monitors, logicalMonitors);
	

	
	
}

void construct_monitors (DisplayState *currentDisplayState,
                         GVariantIter *monitors,
			 GVariantIter *logicalMonitors)
{
	while (1) {
		
	}
}