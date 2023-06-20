#include "display-config-API.h"
#include <gio/gio.h>
#include <glib-object.h>



void get_current_state (GDBusConnection *connection,
                        GVariant        *currentStatePointer)
{
	/*
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
					     */

	
	
}

void update_monitors (GVariantIter *logicalMonitorsIter)
{
	/*
	while (1) {
		GVariant *variant = NULL;
		LogicalMonitor logicalMonitor;
		gboolean status;

		status = g_variant_iter_next (logicalMonitorsIter, LOGICAL_MONITOR_ATFORMAT, &variant);
		if (!status)
			break;

	}
	*/
}