# myblebeacon
Eddystone(https://github.com/google/eddystone) protocol implementation using Bluez DBus API.

## Prerequisites
* BlueZ (Enable Experimental option: ```bluetoothd -E```)
  * The BlueZ DBus API has changed, so you need to check the BlueZ version. For older version, use ```myb_leadv_object_register(..., 1)``` otherwise, use ```myb_leadv_object_register(..., 0)```
* Physical Web application
  * Download from App Store or Play Store

## Build

### Install build dependency packages
* Ubuntu
```sh
apt install libglib2.0-dev cmake
```
* Fedora
```sh
dnf install glib2-devel cmake
```

### Build
```sh
git clone https://github.com/webispy/myblebeacon
cd myblebeacon
mkdir build
cd build
cmake ..
make
```

## Test
```sh
./myble https://google.com
```
