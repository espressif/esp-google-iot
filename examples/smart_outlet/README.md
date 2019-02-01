# Smart Outlet Example

## Setup

### Getting Started

Follow this [getting-started-guide ](https://cloud.google.com/iot/docs/how-tos/getting-started) to do the initial setup.

### Create Public/Private Key pair

As of now, only Elliptic curve algorithm for device authentication is supported.
So, navigate to `Generating an ES256 key` sub section in this [guide](https://cloud.google.com/iot/docs/how-tos/credentials/keys#generating_an_es256_key) and follow the instructions given there.

### Create Registries and Devices

Step-by-step guide for creating registries and devices is given [here](https://cloud.google.com/iot/docs/how-tos/devices). While creating a device, use the public key generated in the previous step.

*NOTE: location of the registry, registry ID and device ID will be used for ESP32 configuration later on*

## Configuring your device

### Installing Private Key

Copy the private key created in the earlier step to `main/certs/private_key.pem` subdirectory.

*NOTE: Don't accidentally commit private key to public source*

### Device Configuration

#### Wifi Setting

Under `make menuconfig` -> `Example Configuration`, set `WiFi SSID` and `WiFI Password`

#### Google IoT Core Device Setting

Set Project ID, Location, Registry ID and Device ID under `make menuconfig` -> `Component config` -> `Google IoT Core Configuration`

`project_id` of your project can be fetched from [resources page](https://console.cloud.google.com/cloud-resource-manager)

Fetch `location`, `registry_id` and `device_id` from the registry created earlier.

## MQTT Publish/Subscribe

After flashing the example to your ESP32, it should connect to Google IoT Core and start subscribing/publishing MQTT data.

### Telemetry Event

This example publishes temperature data every 10 seconds to the topic `/devices/{device-id}/events`

### Device Configuration

To know more about configuring devices from Google IoT Core visit [here](https://cloud.google.com/iot/docs/how-tos/config/configuring-devices)

A subscription to the topic `/devices/{device-id}/config` is established on start-up, thus you recieve configuration details in the registered subscribe callback `iotc_mqttlogic_subscribe_callback`

### Send Device Command
As a subscription to the wildcard topic `/devices/{device-id}/commands/#` is established in this example, you can send commands to the device by following instruction given [here](https://cloud.google.com/iot/docs/how-tos/commands).

- Set Output GPIO under `make menuconfig` -> `Example Configuration` -> `Output GPIO`
- Send `{"outlet": 0}` or `{"outlet": 1}` from Google Cloud IoT Core Device Console to change GPIO output.

