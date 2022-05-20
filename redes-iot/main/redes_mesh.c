
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
    long ultima_captura = esp_timer_get_time()/1000;
    char data[20];
    while (1)
    {
        humedad = calcular_porcentaje_humedad();
        if(cal_abs(humedad_ant - humedad) > 0.5 || (esp_timer_get_time()/1000 - ultima_captura) > 60000){
            //ENVIAR
            //Agregar a la cola de envIo
            humedad_ant = humedad;
            ultima_captura = esp_timer_get_time()/1000;
            ESP_LOGI("t_Hu", "Cambio humedad %.4f", humedad);
            sprintf(data, "%.4f", humedad);
            publish_data("humedad", data);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}
 void DHT_task(void *pvParameter)
{
	setDHTgpio( 5 );
	printf( "Starting DHT Task\n\n");
     char  data[20];
     float temp = 0;
	while(1) {
	
		printf("=== Reading DHT ===\n" );
		int ret = readDHT();
		
		errorHandler(ret);
        temp = getTemperature();
		//printf( "Hum %.1f\n", getHumidity() );
		printf( "Tmp %.1f\n", getTemperature() );
        ESP_LOGI("Temperature", " Tmp: %.1f\n", getTemperature());

        sprintf(data, "%.1f\n", temp);
        publish_data("temperatura", data);
		
		// -- wait at least 2 sec before reading again ------------
		// The interval of whole process must be beyond 2 seconds !! 
		vTaskDelay( 3000 / portTICK_RATE_MS );
	}
} 
void app_main(void)
{
    mesh_layer = -1;
    iniciar_mesh_red();
    ESP_ERROR_CHECK(nvs_flash_init());
    //initialize_wifi("Familia Espinoza Salas", "gmse1416");
    //wait_wifi_Connection();
    init_adc_config();
    mqtt_app_start();

    while (!is_connected)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    xTaskCreate(task_captura_humedad, "task_captura", 2048, NULL, 1, NULL);
    xTaskCreate( DHT_task, "DHT_task", 2048, NULL, 5, NULL ); 
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
