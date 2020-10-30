# This file is no longer auto-generated to make the repository builds with GCC
# and ARMCC no matter what.

cmake_minimum_required(VERSION 2.8.12)

enable_language(ASM)

set(YOTTA_AUTO_AKHILAFLEX-DAL_CPP_FILES
    "AKHILAFLEX.cpp"
    "core/MemberFunctionCallback.cpp"
    "core/AKHILAFLEXCompat.cpp"
    "core/AKHILAFLEXDevice.cpp"
    "core/AKHILAFLEXFiber.cpp"
    "core/AKHILAFLEXFont.cpp"
    "core/AKHILAFLEXHeapAllocator.cpp"
    "core/AKHILAFLEXListener.cpp"
    "core/AKHILAFLEXSystemTimer.cpp"
    "core/AKHILAFLEXUtil.cpp"

    "types/CoordinateSystem.cpp"
    "types/ManagedString.cpp"
    "types/AKHILAFLEXEvent.cpp"
    "types/AKHILAFLEXImage.cpp"
    "types/PacketBuffer.cpp"
    "types/RefCounted.cpp"

    "drivers/DynamicPwm.cpp"
    "drivers/AKHILAFLEXAccelerometer.cpp"
    "drivers/AKHILAFLEXButton.cpp"
    "drivers/AKHILAFLEXCompass.cpp"
    "drivers/AKHILAFLEXCompassCalibrator.cpp"
    "drivers/AKHILAFLEXDisplay.cpp"
    "drivers/AKHILAFLEXI2C.cpp"
    "drivers/AKHILAFLEXIO.cpp"
    "drivers/AKHILAFLEXLightSensor.cpp"
    "drivers/AKHILAFLEXMessageBus.cpp"
    "drivers/AKHILAFLEXMultiButton.cpp"
    "drivers/AKHILAFLEXPin.cpp"
    "drivers/AKHILAFLEXQuadratureDecoder.cpp"
    "drivers/AKHILAFLEXRadio.cpp"
    "drivers/AKHILAFLEXRadioDatagram.cpp"
    "drivers/AKHILAFLEXRadioEvent.cpp"
    "drivers/AKHILAFLEXSerial.cpp"
    "drivers/AKHILAFLEXStorage.cpp"
    "drivers/AKHILAFLEXThermometer.cpp"
    "drivers/TimedInterruptIn.cpp"
    "drivers/AKHILAFLEXFlash.cpp"
    "drivers/AKHILAFLEXFile.cpp"
    "drivers/AKHILAFLEXFileSystem.cpp"
    "drivers/FXOS8700.cpp"
    "drivers/MMA8653.cpp"
    "drivers/MAG3110.cpp"
    "drivers/LSM303Accelerometer.cpp"
    "drivers/LSM303Magnetometer.cpp"
    "drivers/AKHILAFLEXMemoryMap.cpp"

    "bluetooth/AKHILAFLEXAccelerometerService.cpp"
    "bluetooth/AKHILAFLEXBLEManager.cpp"
    "bluetooth/AKHILAFLEXButtonService.cpp"
    "bluetooth/AKHILAFLEXDFUService.cpp"
    "bluetooth/AKHILAFLEXEddystone.cpp"
    "bluetooth/AKHILAFLEXEventService.cpp"
    "bluetooth/AKHILAFLEXIOPinService.cpp"
    "bluetooth/AKHILAFLEXLEDService.cpp"
    "bluetooth/AKHILAFLEXMagnetometerService.cpp"
    "bluetooth/AKHILAFLEXTemperatureService.cpp"
    "bluetooth/AKHILAFLEXUARTService.cpp"
    "bluetooth/AKHILAFLEXPartialFlashingService.cpp"

)

execute_process(WORKING_DIRECTORY "../../yotta_modules/${PROJECT_NAME}" COMMAND "git" "log" "--pretty=format:%h" "-n" "1" OUTPUT_VARIABLE git_hash)
execute_process(WORKING_DIRECTORY "../../yotta_modules/${PROJECT_NAME}" COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD" OUTPUT_VARIABLE git_branch OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${git_branch}" STREQUAL "master")
    set(AKHILAFLEX_DAL_VERSION_STRING "${YOTTA_AKHILAFLEX_DAL_VERSION_STRING}")
else()
    set(AKHILAFLEX_DAL_VERSION_STRING "${YOTTA_AKHILAFLEX_DAL_VERSION_STRING}-${git_branch}-g${git_hash}")
endif()

set(AKHILAFLEX_DAL_VERSION_FLAGS "-DAKHILAFLEX_DAL_VERSION=\\\"${AKHILAFLEX_DAL_VERSION_STRING}\\\"")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${AKHILAFLEX_DAL_VERSION_FLAGS}")

if (YOTTA_CFG_AKHILAFLEX_CONFIGFILE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${YOTTA_FORCE_INCLUDE_FLAG} \"${YOTTA_CFG_AKHILAFLEX_CONFIGFILE}\"")
endif ()

if(CMAKE_COMPILER_IS_GNUCC)
  file(REMOVE "asm/CortexContextSwitch.s")
  configure_file("asm/CortexContextSwitch.s.gcc" "asm/CortexContextSwitch.s" COPYONLY)
else()
  file(REMOVE "asm/CortexContextSwitch.s")
  configure_file("asm/CortexContextSwitch.s.armcc" "asm/CortexContextSwitch.s" COPYONLY)
endif()

set(YOTTA_AUTO_AKHILAFLEX-DAL_S_FILES
    "asm/CortexContextSwitch.s"
)

add_library(AKHILAFLEX
    ${YOTTA_AUTO_AKHILAFLEX-DAL_CPP_FILES}
    ${YOTTA_AUTO_AKHILAFLEX-DAL_S_FILES}
)

yotta_postprocess_target(LIBRARY AKHILAFLEX)

target_link_libraries(AKHILAFLEX
    mbed-classic
    ble
    ble-nrf51822
)

if(CMAKE_COMPILER_IS_GNUCC)
    message("suppressing ALL warnings from mbed-classic, ble, ble-nrf51822 & nrf51-sdk")
    target_compile_options(mbed-classic PRIVATE "-w")
    target_compile_options(ble PRIVATE "-w")
    target_compile_options(ble-nrf51822 PRIVATE "-w")
    target_compile_options(nrf51-sdk PRIVATE "-w")
endif()
