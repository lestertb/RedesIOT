#ifndef __MQTT_CONTROL_H__
#define __MQTT_CONTROL_H__

#include "esp_err.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_mesh.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>

/*
variables globales para usar como parametros de configuracion
*/
esp_mqtt_client_handle_t client;
#define MAX_MSG_LEN 1024
char incoming_topic[MAX_MSG_LEN];
char incoming_msg[MAX_MSG_LEN];


bool is_connected;


float minimoHumedad;
float minimoTiempoHumedad;
float minimoTiempoTemperatura;
void (*process_data_prt)(const char *topic, const char *msg);//procesa la data recibida

void mqtt_app_start(void);//inicie el server  mqtt

void publish_data(const char* topic, const char* message);//publica en el node red

void subscribe_topic(const char *topic); //se suscribe a un topic del node red

#endif