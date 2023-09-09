// As per cortex-m4 Generic User guide doc, cache polic and shareability do not affect the system behavior.
// To make codes more portable, following attributes are applied.
//
// Flash memory : Normal, Non-shareable, write-through
// Internal RAM : Normal, Shareable, write-through
// External RAM : Normal, Shareable, write-back, write-allocate
// Peri : Device, Shareable
//
static MpuRegionType regions[] = {
    // Customized background region to prevent software from accessing to regions that are not mapped
    {MPU_REGION_0, 0x00000000, MPU_AP_NA | MPU_NORMAL_WB_WA_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_4GB, 1},

    // CHUB Internal SRAM --> Do To!!! Need to split code and data into sub-rebions with different attributes
    {MPU_REGION_1, 0x00000000, MPU_AP_RW | MPU_NORMAL_WB_WA_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},

    // CHUB SFR
    {MPU_REGION_2, 0x40100000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},

    // APM & CMGP
    {MPU_REGION_3, 0x40300000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},
    {MPU_REGION_4, 0x40400000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},
    {MPU_REGION_5, 0x40500000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},
    {MPU_REGION_6, 0x40600000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},
    {MPU_REGION_7, 0x40700000, MPU_NOT_EXECUTABLE | MPU_AP_RW | MPU_DEVICE_TEX_C_B_S | MPU_SUBREGION_DISABLE | MPU_REGION_ENABLE, MPU_SIZE_1MB, 1},
    // !!!! Need to check -> According to UM, 0x40800000 is required.
};
