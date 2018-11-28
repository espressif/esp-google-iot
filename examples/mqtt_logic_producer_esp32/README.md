# Google IoT Core Example
___________________________________________

## Setup

### Getting Started

Follow this [getting-started-guide ](https://cloud.google.com/iot/docs/how-tos/getting-started) to do the initial setup.

### Create Public/Private Key pair

As of now, only Elliptic curve algorithm for device authentication is supported.
So, navigate to `Generating an ES256 key` sub section in this [guide](https://cloud.google.com/iot/docs/how-tos/credentials/keys) and follow the instructions given there.

### Create Registries and Devices

Step-by-step guide for creating registries and devices is given [here](https://cloud.google.com/iot/docs/how-tos/devices). While creating a device, use the public key generated in the previous step.

*NOTE: location of the registry, registry ID and device ID will be used for ESP32 configuration later on*

## Configuring your device

### Installing Private Key

Copy the private key created in the earlier step to `main/certs/private_key.pem` subdirectory.

*NOTE: Don't accidentally commit private key to public source*

### Set Project ID and Device Path

Under `make menuconfig` -> `Example Configuration`, set these details.

`project_id` of your project can be fetched from [resources page](https://console.cloud.google.com/cloud-resource-manager)

`device_path` is `projects/<project_id>/locations/<registry_location>/registries/<registry_id>/devices/<device_id>`

*NOTE: Fetch `registry_location`, `registry_id` and `device_id` from the registry created earlier.*

## MQTT Publish/Subscribe

After flashing the example to your ESP32, it should connect to Google IoT Core and start subscribing/publishing MQTT data.

This example publishes data to the topic `/devices/{device-id}/events`

You can send commands to the device by following instruction given [here](https://cloud.google.com/iot/docs/how-tos/commands), as a subscription to the wildcard topic `/devices/{device-id}/commands/#` is established in this example. 
