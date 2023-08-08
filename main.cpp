/* main.cpp
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
#include "timingFunctions.h"
#include "menu-config.h"

#include <iostream>
#include <cstdlib>


GDBusConnection *mainDbusConnection;


int main(int argc, char *argv[])
{
	gtk_init(NULL, NULL);

	SystemTrayMenu systemTrayMenu ("./testIcon.png", "./menuUI.ui");

	systemTrayMenu.status_icon_show ();

	gtk_main();

	return 0;
}
