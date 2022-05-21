
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_log.h"
#include "wifi.h"
#include "mqtt_control.h"
#include "moisture_sensor.h"
#include "rom/ets_sys.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "mesh_control.h"
#include "DHT22.h"

float cal_abs(float a)
{
    if (a < 0)
        return -1 * a;
    return a;
}
void task_captura_humedad(void *args)
{
    float humedad_ant = 0;
    float humedad = 0;
    long ultima_captura = esp_timer_get_time()/1000000;
    char data[20];

    while (1)
    {
        humedad = calcular_porcentaje_humedad();
        if(cal_abs(humedad_ant - humedad) > minimoHumedad || (esp_timer_get_time()/1000000 - ultima_captura) > minimoTiempoHumedad){
            //ENVIAR
            //Agregar a la cola de envIo
            humedad_ant = humedad;
            ultima_captura = esp_timer_get_time()/1000000;
            ESP_LOGI("t_Hu", "Cambio humedad %.4f", humedad);
            sprintf(data, "%.4f", humedad);
            publish_data("humedad", data);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
//tarea para obtener la temperatura del dht22
 void DHT_task(void *pvParameter)
{
	setDHTgpio( 21 );//setear el puerto en el esp
	//printf( "Starting DHT Task\n\n");
     char  data[20];
     float temp = 0;
     long ultima_captura = esp_timer_get_time()/1000000;
	while(1) {
	
		printf("=== Reading DHT ===\n" );
		int ret = readDHT();
		
		errorHandler(ret);


        if ((esp_timer_get_time()/1000000 - ultima_captura) > minimoTiempoTemperatura)//saca la temperatura en un intervalo de segundos
       {
            temp = getTemperature();//se captura la temperatura
            ultima_captura = esp_timer_get_time()/1000000;
            ESP_LOGI("Temperature", " Tmp: %.1f\n", getTemperature());

            sprintf(data, "%.1f\n", temp);
            publish_data("temperatura", data);//se publica la temperatura en nodered a traves del mosquitto con este llamado a la funcion
        }
        
        
		
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 3000 / portTICK_RATE_MS );
	}
} 
void app_main(void)
{
    mesh_layer = -1;
    iniciar_mesh_red();//se inicia la red mesh
    ESP_ERROR_CHECK(nvs_flash_init());
    //initialize_wifi("Familia Espinoza Salas", "gmse1416");
    //wait_wifi_Connection(); esto es para pruebas sin uso de la red mesh, solo para conecterse a internet
    init_adc_config();
    mqtt_app_start(); //se inicia el mosquitto
    /*
    Aqui se setean las variables globales para usarlas como parametros de configuracion a traves del node red
    */    
    minimoHumedad = 0.5;
    minimoTiempoHumedad = 6;
    minimoTiempoTemperatura = 2;
    while (!is_connected)//para saber que esta conectado a la red
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    subscribe_topic("configparams");//la subscripcion al topic de node red, esto se usa para los parametros de configuracion
    subscribe_topic("configparams2");
    xTaskCreate(task_captura_humedad, "task_captura", 2048, NULL, 1, NULL); // se llaman a las tareas creadas para obtener la humedad en este caso
    xTaskCreate( DHT_task, "DHT_task", 2048, NULL, 5, NULL ); 
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
