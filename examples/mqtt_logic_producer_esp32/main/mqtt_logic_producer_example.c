/* MQTT LOGIC Producer Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <time.h>
#include "lwip/apps/sntp.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include <iotc.h>

extern const uint8_t ec_pv_key_start[] asm("_binary_private_key_pem_start");
extern const uint8_t ec_pv_key_end[] asm("_binary_private_key_pem_end");

static const char *TAG = "APP";

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;

#define IOTC_UNUSED(x) (void)(x)

static const char *iotc_publish_topic = "/devices/esp32-devkitc-v4/events";
static const char *iotc_subscribe_topic = "/devices/esp32-devkitc-v4/commands/#";
static const char *iotc_publish_message = "Hello From Your ESP32 IoTC client!";
iotc_mqtt_qos_t iotc_example_qos = IOTC_MQTT_QOS_AT_LEAST_ONCE;
static iotc_timed_task_handle_t delayed_publish_task =
     IOTC_INVALID_TIMED_TASK_HANDLE;
iotc_context_handle_t iotc_context = IOTC_INVALID_CONTEXT_HANDLE;

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "time.google.com");
    sntp_init();
}

static void obtain_time(void)
{
    initialize_sntp();
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    while (timeinfo.tm_year < (2016 - 1900))
    {
        ESP_LOGI(TAG, "Waiting for system time to be set...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
    ESP_LOGI(TAG, "Time is set...");
}

void publish_function(iotc_context_handle_t context_handle,
                      iotc_timed_task_handle_t timed_task, void *user_data)
{
    IOTC_UNUSED(timed_task);
    IOTC_UNUSED(user_data);
    printf("publishing msg \"%s\" to topic: \"%s\"\n", iotc_publish_message,
           iotc_publish_topic);

    iotc_publish(context_handle, iotc_publish_topic, iotc_publish_message,
                 iotc_example_qos,
                 /*callback=*/NULL, /*user_data=*/NULL);
}

void iotc_itest_mqttlogic_subscribe_callback(
    iotc_context_handle_t in_context_handle, iotc_sub_call_type_t call_type,
    const iotc_sub_call_params_t *const params, iotc_state_t state,
    void *user_data)
{
    IOTC_UNUSED(in_context_handle);
    IOTC_UNUSED(call_type);
    IOTC_UNUSED(params);
    IOTC_UNUSED(state);
    IOTC_UNUSED(user_data);
}

void subscribe_function(iotc_context_handle_t context_handle,
                        iotc_timed_task_handle_t timed_task, void *user_data)
{
    IOTC_UNUSED(timed_task);
    IOTC_UNUSED(user_data);
    printf("subscribe to topic: \"%s\"\n", iotc_subscribe_topic);
    iotc_subscribe(context_handle, iotc_subscribe_topic, IOTC_MQTT_QOS_AT_LEAST_ONCE,
                   &iotc_itest_mqttlogic_subscribe_callback, /*user_data=*/NULL);
}

void on_connection_state_changed(iotc_context_handle_t in_context_handle,
                                 void *data, iotc_state_t state)
{
    iotc_connection_data_t *conn_data = (iotc_connection_data_t *)data;

    switch (conn_data->connection_state)
    {
    /* IOTC_CONNECTION_STATE_OPENED means that the connection has been
       established and the IoTC Client is ready to send/recv messages */
    case IOTC_CONNECTION_STATE_OPENED:
        printf("connected!\n");

        /* Publish immediately upon connect. 'publish_function' is defined
           in this example file and invokes the IoTC API to publish a
           message. */
        publish_function(in_context_handle, IOTC_INVALID_TIMED_TASK_HANDLE,
                         /*user_data=*/NULL);
        subscribe_function(in_context_handle, IOTC_INVALID_TIMED_TASK_HANDLE,
                           /*user_data=*/NULL);
        /* Create a timed task to publish every 5 seconds. */
        delayed_publish_task = iotc_schedule_timed_task(in_context_handle,
                                                        publish_function, 5,
                                                        15, /*user_data=*/NULL);
        break;

    /* IOTC_CONNECTION_STATE_OPEN_FAILED is set when there was a problem
       when establishing a connection to the server. The reason for the error
       is contained in the 'state' variable. Here we log the error state and
       exit out of the application. */
    case IOTC_CONNECTION_STATE_OPEN_FAILED:
        printf("ERROR!\tConnection has failed reason %d\n\n", state);

        /* exit it out of the application by stopping the event loop. */
        iotc_events_stop();
        break;

    /* IOTC_CONNECTION_STATE_CLOSED is set when the IoTC Client has been
       disconnected. The disconnection may have been caused by some external
       issue, or user may have requested a disconnection. In order to
       distinguish between those two situation it is advised to check the state
       variable value. If the state == IOTC_STATE_OK then the application has
       requested a disconnection via 'iotc_shutdown_connection'. If the state !=
       IOTC_STATE_OK then the connection has been closed from one side. */
    case IOTC_CONNECTION_STATE_CLOSED:
        /* When the connection is closed it's better to cancel some of previously
           registered activities. Using cancel function on handler will remove the
           handler from the timed queue which prevents the registered handle to be
           called when there is no connection. */
        if (IOTC_INVALID_TIMED_TASK_HANDLE != delayed_publish_task)
        {
            iotc_cancel_timed_task(delayed_publish_task);
            delayed_publish_task = IOTC_INVALID_TIMED_TASK_HANDLE;
        }

        if (state == IOTC_STATE_OK)
        {
           /* The connection has been closed intentionally. Therefore, stop
              the event processing loop as there's nothing left to do
              in this example. */
            iotc_events_stop();
        }
        else
        {
            printf("connection closed - reason %d!\n", state);
            /* The disconnection was unforeseen.  Try reconnect to the server
           with previously set configuration, which has been provided
           to this callback in the conn_data structure. */
            iotc_connect(
                in_context_handle, conn_data->project_id, conn_data->device_path,
                conn_data->private_key_data, conn_data->jwt_expiration_period_sec,
                conn_data->connection_timeout, conn_data->keepalive_timeout,
                &on_connection_state_changed);
        }
        break;
    
    default:
        printf("wrong value\n");
        break;
    }
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void wifi_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "start the WIFI SSID:[%s]", CONFIG_ESP_WIFI_SSID);
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Waiting for wifi");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

static void mqtt_task(void *pvParameters)
{
    /* initialize iotc library and create a context to use to connect to the
    * GCP IoT Core Service. */
    const iotc_state_t error_init = iotc_initialize();

    if (IOTC_STATE_OK != error_init)
    {
        printf(" iotc failed to initialize, error: %d\n", error_init);
        vTaskDelete(NULL);
    }

    /*  Create a connection context. A context represents a Connection
        on a single socket, and can be used to publish and subscribe
        to numerous topics. */
    iotc_context = iotc_create_context();
    if (IOTC_INVALID_CONTEXT_HANDLE >= iotc_context)
    {
        printf(" iotc failed to create context, error: %d\n", -iotc_context);
        vTaskDelete(NULL);
    }

    /*  Queue a connection request to be completed asynchronously.
        The 'on_connection_state_changed' parameter is the name of the
        callback function after the connection request completes, and its
        implementation should handle both successful connections and
        unsuccessful connections as well as disconnections. */
    const uint16_t connection_timeout = 0;
    const uint16_t keepalive_timeout = 20;

    iotc_crypto_private_key_data_t key_data;
    key_data.private_key_signature_algorithm =
        IOTC_JWT_PRIVATE_KEY_SIGNATURE_ALGORITHM_ES256;
    key_data.private_key_union_type = IOTC_CRYPTO_KEY_UNION_TYPE_PEM;
    key_data.private_key_union.key_pem.key = ec_pv_key_start;

    iotc_connect(iotc_context, CONFIG_PROJECT_ID, CONFIG_DEVICE_PATH, &key_data,
                 /*{jwt_expiration_period_sec=*/3600, connection_timeout,
                 keepalive_timeout, &on_connection_state_changed);

    /* The IoTC Client was designed to be able to run on single threaded devices.
        As such it does not have its own event loop thread. Instead you must
        regularly call the function iotc_events_process_blocking() to process
        connection requests, and for the client to regularly check the sockets for
        incoming data. This implementation has the loop operate endlessly. The loop
        will stop after closing the connection, using iotc_shutdown_connection() as
        defined in on_connection_state_change logic, and exit the event handler
        handler by calling iotc_events_stop(); */
    iotc_events_process_blocking();

    iotc_delete_context(iotc_context);

    iotc_shutdown();

    vTaskDelete(NULL);
}

void app_main()
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init();
    obtain_time();
    xTaskCreate(&mqtt_task, "mqtt_task", 8192, NULL, 5, NULL);
}
