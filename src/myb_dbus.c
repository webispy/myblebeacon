#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "myb_dbus.h"

static GDBusConnection *conn;

const char *myb_dbus_init(void)
{
  GError *error = NULL;
  const gchar *uniq_name;

  conn = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, &error);
  if (!conn) {
    fprintf(stderr, "g_bus_get_sync() failed. (%s)\n", error->message);
    g_error_free(error);
    return NULL;
  }

  uniq_name = g_dbus_connection_get_unique_name(conn);

  printf("dbus unique name = '%s'\n", uniq_name);

  return uniq_name;
}

GDBusConnection *myb_dbus_get_connection()
{
  return conn;
}

gchar *myb_dbus_get_ble_adapter()
{
  GError *error = NULL;
  GVariant *result;
  GVariant *ifaces;
  GVariantIter *iter = NULL;
  gchar *obj_name;
  gchar *adapter = NULL;

  result = g_dbus_connection_call_sync(conn, "org.bluez", "/",
      "org.freedesktop.DBus.ObjectManager", "GetManagedObjects",
      NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
  if (error) {
    fprintf(stderr, "failed. (%s)\n", error->message);
    g_error_free(error);
    return NULL;
  }

  if (!result)
    return NULL;

  g_variant_get(result, "(a{oa{sa{sv}}})", &iter);

  while (g_variant_iter_loop(iter, "{&o@a{sa{sv}}}", &obj_name, &ifaces)) {
    GVariantIter *iface_iter = NULL;
    gchar *iface_name;

    g_variant_get(ifaces, "a{sa{sv}}", &iface_iter);

    while(g_variant_iter_loop(iface_iter, "{&sa{sv}}", &iface_name, NULL)) {
      if (!g_strcmp0(iface_name, "org.bluez.LEAdvertisingManager1")) {
        printf("LE interface found!\n");
        g_free(adapter);
        adapter = g_strdup(obj_name);
      }
    }

    g_variant_iter_free(iface_iter);
  }

  g_variant_iter_free(iter);
  g_variant_unref(result);

  return adapter;
}

static void _register_to_bluez_cb(GObject *source_object, GAsyncResult *res,
    gpointer user_data)
{
  GVariant *result;
  GError *error = NULL;

  result = g_dbus_connection_call_finish(G_DBUS_CONNECTION(source_object),
      res, &error);
  if (error) {
    fprintf(stderr, "method call failed. (%s)\n", error->message);
    g_error_free(error);
    return;
  }

  if (result)
    g_variant_unref(result);

  printf("method call ok\n");
}

void myb_dbus_register_to_bluez(const char *adapter_path, const char *le_path)
{
  GError *error = NULL;
  GVariant *param = g_variant_new("(oa{sv})", le_path, NULL);

  g_dbus_connection_call(conn, "org.bluez", adapter_path,
      "org.bluez.LEAdvertisingManager1", "RegisterAdvertisement", param,
      NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL, _register_to_bluez_cb, &error);
  if (error) {
    fprintf(stderr, "method call failed. (%s)\n", error->message);
    g_error_free(error);
    return;
  }
}

