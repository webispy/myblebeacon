#ifndef __MYB_DBUS_H__
#define __MYB_DBUS_H__

#include <glib.h>
#include <gio/gio.h>

const char*      myb_dbus_init(void);
GDBusConnection* myb_dbus_get_connection(void);
gchar*           myb_dbus_get_ble_adapter(void);
void             myb_dbus_register_to_bluez(const char *adapter_path,
                                            const char *le_path);

#endif
