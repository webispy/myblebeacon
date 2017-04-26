#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "myb_dbus.h"
#include "myb_leadv_object.h"
#include "myb_eddystone.h"

#define UUID "feaa"

static void _method_call(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name,
    const gchar *method_name, GVariant *parameters,
    GDBusMethodInvocation *invoc, gpointer user_dsta)
{
  printf("invocated\n");
  g_dbus_method_invocation_return_value(invoc, NULL);
}

static GVariant *_get_property(GDBusConnection *conn, const gchar *sender,
    const gchar *object_path, const gchar *interface_name,
    const gchar *prop_name, GError **error, gpointer user_data)
{
  GVariant *ret = NULL;
  gint old_api = GPOINTER_TO_INT(user_data);

  printf("property get: '%s'\t", prop_name);

  if (!g_strcmp0(prop_name, "Type")) {
    ret = g_variant_new_string("broadcast");
  }
  else if (!g_strcmp0(prop_name, "ServiceUUIDs")) {
    GVariantBuilder *b = g_variant_builder_new(G_VARIANT_TYPE("as"));

    g_variant_builder_add(b, "s", UUID);
    ret = g_variant_new("as", b);
    g_variant_builder_unref(b);
  }
  else if (!g_strcmp0(prop_name, "ManufacturerData")) {
  }
  else if (!g_strcmp0(prop_name, "ServiceData")) {
    GVariantBuilder *b, *by;
    const unsigned char *sdata;
    unsigned int sdata_len = 0;
    unsigned int i;

    sdata = myb_eddystone_peek_payload(&sdata_len);

    by = g_variant_builder_new(G_VARIANT_TYPE("ay"));
    for (i = 0; i < sdata_len; i++)
      g_variant_builder_add(by, "y", sdata[i]);

    if (old_api) {
      b = g_variant_builder_new(G_VARIANT_TYPE("a{say}"));
      g_variant_builder_add(b, "{s@ay}", UUID, g_variant_new("ay", by));
    }
    else {
      b = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
      g_variant_builder_add(b, "{sv}", UUID, g_variant_new("ay", by));
    }
    ret = g_variant_builder_end(b);
  }
  else if (!g_strcmp0(prop_name, "SolicitUUIDs")) {
  }
  else if (!g_strcmp0(prop_name, "IncludeTxPower")) {
    ret = g_variant_new_boolean(TRUE);
  }

  if (ret)
    printf("- value: <type=%s> %s\n", g_variant_get_type_string(ret),
        g_variant_print(ret, TRUE));
  else
    printf("- value: none\n");

  return ret;
}

static const GDBusInterfaceVTable vtable = {
  .method_call = _method_call,
  .get_property = _get_property,
  .set_property = NULL
};

void myb_leadv_object_register(const char *path, gint old_api)
{
  GDBusNodeInfo *ni;
  GError *error = NULL;
  gchar *xml = NULL;
  gsize size = 0;
  guint oid;
  GDBusConnection *conn = myb_dbus_get_connection();

  if (old_api)
    g_file_get_contents("../introspection/old_adv.xml", &xml, &size, &error);
  else
    g_file_get_contents("../introspection/adv.xml", &xml, &size, &error);

  if (error) {
    fprintf(stderr, "failed. (%s)\n", error->message);
    g_error_free(error);
    return;
  }

  ni = g_dbus_node_info_new_for_xml(xml, NULL);
  if (!ni)
    return;

  g_free(xml);

  oid = g_dbus_connection_register_object(conn, path, ni->interfaces[0],
      &vtable, GINT_TO_POINTER(old_api), NULL, &error);
  if (error) {
    fprintf(stderr, "failed. (%s)\n", error->message);
    g_error_free(error);
  }

  printf("registered '%s' object. (object_id=%u)\n", path, oid);
}

