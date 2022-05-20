#include "mqtt_control.h"

static const char *TAG = "MQTTS_EXAMPLE";

static const uint8_t mqtt_arenalconnected_pem_start[] =     "-----BEGIN CERTIFICATE-----\n"
                                                            "MIIDsTCCApmgAwIBAgIUeavAwhmHPmI+jbIh1jj/tqQE24swDQYJKoZIhvcNAQEL\n"
                                                            "BQAwaDELMAkGA1UEBhMCQ1IxETAPBgNVBAgMCEFsYWp1ZWxhMQ4wDAYDVQQHDAVQ\n"
                                                            "aXRhbDETMBEGA1UECgwKQ29uc3VsdGluZzEhMB8GA1UEAwwYYmlsbS5hcmVuYWxj\n"
                                                            "b25uZWN0ZWQuY29tMB4XDTIyMDUxOTIxMjQ0MloXDTI3MDUxOTIxMjQ0MlowaDEL\n"
                                                            "MAkGA1UEBhMCQ1IxETAPBgNVBAgMCEFsYWp1ZWxhMQ4wDAYDVQQHDAVQaXRhbDET\n"
                                                            "MBEGA1UECgwKQ29uc3VsdGluZzEhMB8GA1UEAwwYYmlsbS5hcmVuYWxjb25uZWN0\n"
                                                            "ZWQuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1Akumz0yNFFM\n"
                                                            "Q00s7PBHoVvqeCjOCOazKDh7l/8vJqUhD5Y4hD5+ARvx5bVkH6yJe47JCLkxK2fO\n"
                                                            "g/Z6Rjc1wNbeyKdXRY9vHwlls4ZF+077br6DvekEWiMIRQdb7FPzGyqzfj7jd9Qg\n"
                                                            "fvUXFa9ZBcBiLuOrYNr6lYUN6EUr0RE5zU6h6Ju6Z3XoyVZP+Aey0dCOU8Oglxcu\n"
                                                            "FdYVBX5MDIaFU9Ytbh/NFlYozduUWL9NhYi/WJh0QJshml/37CAG/bdf7V2woZk1\n"
                                                            "30O1S0v3LvnZWlR3FrsBF5WDqsPZd6Omq2pMc7gvsLWHUqbQK5YjD+byEbW5sRUb\n"
                                                            "eWGdJbjbMwIDAQABo1MwUTAdBgNVHQ4EFgQUzjzzMXlbAIRMHFfDelFKgBpbaNow\n"
                                                            "HwYDVR0jBBgwFoAUzjzzMXlbAIRMHFfDelFKgBpbaNowDwYDVR0TAQH/BAUwAwEB\n"
                                                            "/zANBgkqhkiG9w0BAQsFAAOCAQEArBVB8a/6AcqCPG7eIqrzMKux+EgFMLCRwRIm\n"
                                                            "EmI5WrqcFLsnKdH7u+s4uGUMijyO7HL0zmvd1GJgtPdyrRG7vTvVfHus3TX+DnjD\n"
                                                            "L7UAxnHO7JdQHYLyX+ptyoL5QAcCsEcBvKCNfu/1kYRI4nqsT2bzasfHZy8hS4r4\n"
                                                            "lcxqTEUC0WjynQKc8YIXxY7266kO1oiOEs55mvpeWzaj96hzEcslTjo3lF+0q+Zl\n"
                                                            "vwKRT49NGuMFVx/FPYN13HzdqYTf1LdYaF3zLktk21tJYMvlmmU3Q9X9ZIleYfZd\n"
                                                            "hQkcRUtaJ7F2SCxxOF6UuMeLUuUrkkNViKBgY0Y/hh1vE+EZ2w=="
                                                            "\n-----END CERTIFICATE-----";
// Note: this function is for testing purposes only publishing part of the active partition
//       (to be checked against the original binary)
//
void send_binary(esp_mqtt_client_handle_t client)
{
    spi_flash_mmap_handle_t out_handle;
    const void *binary_address;
    const esp_partition_t *partition = esp_ota_get_running_partition();
    esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &binary_address, &out_handle);
    // sending only the configured portion of the partition (if it's less than the partition size)
    int binary_size = partition->size;
    int msg_id = esp_mqtt_client_publish(client, "/topic/binary", binary_address, binary_size, 0, 0);
    ESP_LOGI(TAG, "binary sent with msg_id=%d", msg_id);
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    //TODO create a global variable for topic and message... copy with strncpy ()
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        is_connected = true;

        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        is_connected = false;

        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        //TODO Here you can handle all the incoming info
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    is_connected = false;
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://bilm.arenalconnected.com:8883",
        .cert_pem = (const char *)mqtt_arenalconnected_pem_start,
        .username = "redes",
        .password = "redes",
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void publish_data(const char* topic, const char* message){
    if (!is_connected)
        return;
    esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
    ESP_LOGI("MQTT", "publish_data (%s) -> %s", topic, message);
}

void subscribe_topic(const char *topic){
    if (!is_connected)
        return;
    int msg_id = esp_mqtt_client_subscribe(client, topic, 1);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}