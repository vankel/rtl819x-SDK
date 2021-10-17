/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2011 GSyC/LibreSoft, Universidad Rey Juan Carlos.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <gdbus.h>

#include "adapter.h"
#include "device.h"
#include "thermometer.h"
#include "manager.h"

#define HEALTH_THERMOMETER_UUID		"00001809-0000-1000-8000-00805f9b34fb"

static DBusConnection *connection = NULL;

static int thermometer_driver_probe(struct btd_device *device, GSList *uuids)
{
	return thermometer_register(connection, device);
}

static void thermometer_driver_remove(struct btd_device *device)
{
	thermometer_unregister(device);
}

static struct btd_device_driver thermometer_device_driver = {
	.name	= "thermometer-device-driver",
	.uuids	= BTD_UUIDS(HEALTH_THERMOMETER_UUID),
	.probe	= thermometer_driver_probe,
	.remove	= thermometer_driver_remove
};

int thermometer_manager_init(DBusConnection *conn)
{
	int ret;

	ret = btd_register_device_driver(&thermometer_device_driver);
	if (ret < 0)
                return ret;

	connection = dbus_connection_ref(conn);
	return 0;
}

void thermometer_manager_exit(void)
{
	btd_unregister_device_driver(&thermometer_device_driver);

	dbus_connection_unref(connection);
	connection = NULL;
}
