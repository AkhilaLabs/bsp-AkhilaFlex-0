#ifndef YOTTA_CFG_MAPPINGS_H
#define YOTTA_CFG_MAPPINGS_H

/**
  * This header file contains mappings from a yotta config.json file for the DAL,
  * to DAL specific #defines used in the DAL.
  */

//DAL mappings

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_FIBER_USER_DATA
    #define AKHILAFLEX_FIBER_USER_DATA YOTTA_CFG_AKHILAFLEX_DAL_FIBER_USER_DATA
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_MAXIMUM_FIBER_POOL_SIZE
    #define AKHILAFLEX_FIBER_MAXIMUM_FIBER_POOL_SIZE YOTTA_CFG_AKHILAFLEX_DAL_MAXIMUM_FIBER_POOL_SIZE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_HEAP_ALLOCATOR
    #define AKHILAFLEX_HEAP_ALLOCATOR YOTTA_CFG_AKHILAFLEX_DAL_HEAP_ALLOCATOR
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_NESTED_HEAP_PROPORTION
    #define AKHILAFLEX_NESTED_HEAP_SIZE YOTTA_CFG_AKHILAFLEX_DAL_NESTED_HEAP_PROPORTION
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_FIBER_USER_DATA
    #define AKHILAFLEX_FIBER_USER_DATA YOTTA_CFG_AKHILAFLEX_DAL_FIBER_USER_DATA
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_REUSE_SD
    #define AKHILAFLEX_HEAP_REUSE_SD YOTTA_CFG_AKHILAFLEX_DAL_REUSE_SD
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_GATT_TABLE_SIZE
    #define AKHILAFLEX_SD_GATT_TABLE_SIZE YOTTA_CFG_AKHILAFLEX_DAL_GATT_TABLE_SIZE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_SYSTEM_TICK_PERIOD
    #define SYSTEM_TICK_PERIOD_MS YOTTA_CFG_AKHILAFLEX_DAL_SYSTEM_TICK_PERIOD
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_SYSTEM_COMPONENTS
    #define AKHILAFLEX_SYSTEM_COMPONENTS YOTTA_CFG_AKHILAFLEX_DAL_SYSTEM_COMPONENTS
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_IDLE_COMPONENTS
    #define AKHILAFLEX_IDLE_COMPONENTS YOTTA_CFG_AKHILAFLEX_DAL_IDLE_COMPONENTS
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ENABLED
    #define AKHILAFLEX_BLE_ENABLED YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ENABLED
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_USE_ACCEL_LSB
    #define USE_ACCEL_LSB YOTTA_CFG_AKHILAFLEX_DAL_USE_ACCEL_LSB
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_FULL_RANGE_PITCH_CALCULATION
#define AKHILAFLEX_FULL_RANGE_PITCH_CALCULATION YOTTA_CFG_AKHILAFLEX_DAL_FULL_RANGE_PITCH_CALCULATION 
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_MIN_DISPLAY_BRIGHTNESS
    #define AKHILAFLEX_DISPLAY_MINIMUM_BRIGHTNESS YOTTA_CFG_AKHILAFLEX_DAL_MIN_DISPLAY_BRIGHTNESS
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_MAX_DISPLAY_BRIGHTNESS
    #define AKHILAFLEX_DISPLAY_MAXIMUM_BRIGHTNESS YOTTA_CFG_AKHILAFLEX_DAL_MAX_DISPLAY_BRIGHTNESS
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_SCROLL_SPEED
    #define AKHILAFLEX_DEFAULT_SCROLL_SPEED YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_SCROLL_SPEED
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_SCROLL_STRIDE
    #define AKHILAFLEX_DEFAULT_SCROLL_STRIDE YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_SCROLL_STRIDE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_PRINT_SPEED
    #define AKHILAFLEX_DEFAULT_PRINT_SPEED YOTTA_CFG_AKHILAFLEX_DAL_DISPLAY_PRINT_SPEED
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_DEFAULT_PULLMODE
    #define AKHILAFLEX_DEFAULT_PULLMODE YOTTA_CFG_AKHILAFLEX_DAL_DEFAULT_PULLMODE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_HEAP_ENABLED
    #define AKHILAFLEX_HEAP_ENABLED YOTTA_CFG_AKHILAFLEX_DAL_HEAP_ENABLED
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_PANIC_ON_HEAP_FULL
    #define AKHILAFLEX_PANIC_HEAP_FULL YOTTA_CFG_AKHILAFLEX_DAL_PANIC_ON_HEAP_FULL
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_DEBUG
    #define AKHILAFLEX_DBG YOTTA_CFG_AKHILAFLEX_DAL_DEBUG
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_HEAP_DEBUG
    #define AKHILAFLEX_HEAP_DBG YOTTA_CFG_AKHILAFLEX_DAL_HEAP_DEBUG
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_STACK_SIZE
    #define AKHILAFLEX_STACK_SIZE YOTTA_CFG_AKHILAFLEX_DAL_STACK_SIZE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_SRAM_BASE
    #define AKHILAFLEX_SRAM_BASE YOTTA_CFG_AKHILAFLEX_DAL_SRAM_BASE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_SRAM_END
    #define AKHILAFLEX_SRAM_END YOTTA_CFG_AKHILAFLEX_DAL_SRAM_END
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_SD_LIMIT
    #define AKHILAFLEX_SD_LIMIT YOTTA_CFG_AKHILAFLEX_DAL_SD_LIMIT
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_GATT_TABLE_START
    #define AKHILAFLEX_SD_GATT_TABLE_START YOTTA_CFG_AKHILAFLEX_DAL_GATT_TABLE_START
#endif

// Radio Mappings
#ifdef YOTTA_CFG_AKHILAFLEX_DAL_RADIO_DEFAULT_FREQUENCY
    #define AKHILAFLEX_RADIO_DEFAULT_FREQUENCY YOTTA_CFG_AKHILAFLEX_DAL_RADIO_DEFAULT_FREQUENCY
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_RADIO_LOWER_FREQ_BAND
    #define AKHILAFLEX_RADIO_LOWER_FREQ_BAND YOTTA_CFG_AKHILAFLEX_DAL_RADIO_LOWER_FREQ_BAND
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_RADIO_UPPER_FREQ_BAND
    #define AKHILAFLEX_RADIO_UPPER_FREQ_BAND YOTTA_CFG_AKHILAFLEX_DAL_RADIO_UPPER_FREQ_BAND
#endif

//Bluetooth mappings
#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PAIRING_MODE
    #define AKHILAFLEX_BLE_PAIRING_MODE YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PAIRING_MODE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PRIVATE_ADDRESSING
    #define AKHILAFLEX_BLE_PRIVATE_ADDRESSES YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PRIVATE_ADDRESSING
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_OPEN
    #define AKHILAFLEX_BLE_OPEN YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_OPEN
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ADVERTISING_INTERVAL
#define AKHILAFLEX_BLE_ADVERTISING_INTERVAL YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ADVERTISING_INTERVAL
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PARTIAL_FLASHING
#define AKHILAFLEX_BLE_PARTIAL_FLASHING YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_PARTIAL_FLASHING
#endif

//we check if the user has requested open mode, otherwise we will double def!
#if (YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_OPEN == 0)

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_WHITELIST
    #define AKHILAFLEX_BLE_WHITELIST YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_WHITELIST
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ADVERTISING_TIMEOUT
    #define AKHILAFLEX_BLE_ADVERTISING_TIMEOUT YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_ADVERTISING_TIMEOUT
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_TX_POWER
    #define AKHILAFLEX_BLE_DEFAULT_TX_POWER YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_TX_POWER
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_SECURITY_LEVEL
    #define AKHILAFLEX_BLE_SECURITY_LEVEL YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_SECURITY_LEVEL
#endif

#endif


#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_DFU_SERVICE
    #define AKHILAFLEX_BLE_DFU_SERVICE YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_DFU_SERVICE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EDDYSTONE_URL
    #define AKHILAFLEX_BLE_EDDYSTONE_URL YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EDDYSTONE_URL
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EDDYSTONE_UID
    #define AKHILAFLEX_BLE_EDDYSTONE_UID YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EDDYSTONE_UID
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EVENT_SERVICE
    #define AKHILAFLEX_BLE_EVENT_SERVICE YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_EVENT_SERVICE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_DEVICE_INFO_SERVICE
    #define AKHILAFLEX_BLE_DEVICE_INFORMATION_SERVICE YOTTA_CFG_AKHILAFLEX_DAL_BLUETOOTH_DEVICE_INFO_SERVICE
#endif

#ifdef YOTTA_CFG_AKHILAFLEX_DAL_MESSAGE_BUS_CONCURRENCY_MODE
    #define MESSAGE_BUS_CONCURRENCY_MODE YOTTA_CFG_AKHILAFLEX_DAL_MESSAGE_BUS_CONCURRENCY_MODE
#endif

#endif
