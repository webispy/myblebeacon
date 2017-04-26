#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "myb_dbus.h"
#include "myb_leadv_object.h"
#include "myb_eddystone.h"

#define TEST_PATH "/tmp1"

static void usage(const char *cmd)
{
  printf("Usage: %s {URL}\n", cmd);
  printf("example) %s https://google.com\n", cmd);
}

int main(int argc, char *argv[])
{
  GMainLoop *loop;
  gchar *adapter;

  if (argc != 2) {
    usage(argv[0]);
    return -1;
  }

  loop = g_main_loop_new(NULL, FALSE);

  myb_dbus_init();

  /**
   * Encode URL to Eddystone URL format
   */
  myb_eddystone_set_url(argv[1]);

  /**
   * Register "org.bluez.LEAdvertisement1" interface to TEST_PATH
   */
  myb_leadv_object_register(TEST_PATH, 1);

  /**
   * Get BLE Adapter object path from BlueZ
   * e.g. "/org/bluez/hci0"
   */
  adapter = myb_dbus_get_ble_adapter();
  if (!adapter) {
    fprintf(stderr, "Can't find BLE adapter\n");
    return -1;
  }

  printf("BLE Adapter = '%s'\n", adapter);

  /**
   * Request 'RegisterAdvertisement' to BlueZ
   */
  myb_dbus_register_to_bluez(adapter, TEST_PATH);

  g_free(adapter);

  /**
   * Mainloop
   */
  printf("\nmainloop started...\n");
  g_main_loop_run(loop);

  return 0;
}
