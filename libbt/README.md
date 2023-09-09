# SLSI libbt

This module is configured through soong namespace defined vendor variables in `BoardConfig.mk`,
and `BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR` also in `BoardConfig.mk`

To enable this libbt the following should be in the device `BoardConfig.mk`

```
SOONG_CONFIG_NAMESPACES += libbt-vendor
SOONG_CONFIG_libbt-vendor := slsi
SOONG_CONFIG_libbt-vendor_slsi := true
```

These lines do the following:

* Add libbt-vendor to soong config namespaces
* Define all the keys for this namespace (only slsi so far)
* Set the value for the key "slsi"

Additionally `BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR` can be
defined as "`hardware/samsung_slsi/libbt/include`" to use vendor
specific defines when building the BT stack.
