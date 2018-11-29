#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := \
        iot-edge-sdk-embedded-c/src/bsp \
        iot-edge-sdk-embedded-c/include/bsp \
        iot-edge-sdk-embedded-c/include \
        iot-edge-sdk-embedded-c/src/libiotc/control_topic \
        iot-edge-sdk-embedded-c/src/libiotc/datastructures \
        iot-edge-sdk-embedded-c/src/libiotc/debug_extensions/memory_limiter \
        iot-edge-sdk-embedded-c/src/libiotc/event_dispatcher \
        iot-edge-sdk-embedded-c/src/libiotc/event_loop \
        iot-edge-sdk-embedded-c/src/libiotc/io/fs/memory \
        iot-edge-sdk-embedded-c/src/libiotc/io/fs \
        iot-edge-sdk-embedded-c/src/libiotc/io/net \
        iot-edge-sdk-embedded-c/src/libiotc/mqtt/logic \
        iot-edge-sdk-embedded-c/src/libiotc/mqtt/codec \
        iot-edge-sdk-embedded-c/src/libiotc/platform/iotc_thread \
        iot-edge-sdk-embedded-c/src/libiotc/platform/posix/iotc_thread \
        iot-edge-sdk-embedded-c/src/libiotc/tls \
        iot-edge-sdk-embedded-c/src/libiotc/tls/certs \
        iot-edge-sdk-embedded-c/src/libiotc/memory \
        iot-edge-sdk-embedded-c/src/libiotc \
        iot-edge-sdk-embedded-c/third_party/mqtt-protocol-c \
        port/include



COMPONENT_SRCDIRS := \
        iot-edge-sdk-embedded-c/src/bsp/tls/mbedtls \
        iot-edge-sdk-embedded-c/src/libiotc \
        iot-edge-sdk-embedded-c/src/libiotc/control_topic \
        iot-edge-sdk-embedded-c/src/libiotc/datastructures \
        iot-edge-sdk-embedded-c/src/libiotc/debug_extensions/memory_limiter \
        iot-edge-sdk-embedded-c/src/libiotc/event_dispatcher \
        iot-edge-sdk-embedded-c/src/libiotc/event_loop \
        iot-edge-sdk-embedded-c/src/libiotc/io/fs \
        iot-edge-sdk-embedded-c/src/libiotc/io/fs/memory \
        iot-edge-sdk-embedded-c/src/libiotc/io/net \
        iot-edge-sdk-embedded-c/src/libiotc/memory \
        iot-edge-sdk-embedded-c/src/libiotc/mqtt/codec \
        iot-edge-sdk-embedded-c/src/libiotc/mqtt/logic \
        iot-edge-sdk-embedded-c/src/libiotc/mqtt/tls \
        iot-edge-sdk-embedded-c/src/libiotc/tls \
        iot-edge-sdk-embedded-c/src/libiotc/tls/certs \
        iot-edge-sdk-embedded-c/third_party/mqtt-protocol-c \
        port/src

COMPONENT_SUBMODULES := iot-edge-sdk-embedded-c

COMPONENT_OBJEXCLUDE := iot-edge-sdk-embedded-c/src/libiotc/iotc_test.o

CFLAGS += -DIOTC_FS_MEMORY \
          -DIOTC_MEMORY_LIMITER_APPLICATION_MEMORY_LIMIT=524288 \
          -DIOTC_MEMORY_LIMITER_SYSTEM_MEMORY_LIMIT=2024 \
          -DIOTC_MEMORY_LIMITER_ENABLED \
          -DIOTC_TLS_LIB_MBEDTLS \

ifdef CONFIG_GIOT_DEBUG_OUTPUT
CFLAGS += -DIOTC_DEBUG_OUTPUT=1
endif
