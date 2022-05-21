/* 

	DHT22 temperature sensor driver

*/

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

// == function prototypes =======================================

void 	setDHTgpio(int gpio); //setear el puerto en el esp
void 	errorHandler(int response);
int 	readDHT();//se usa para iniciarlizar el sensor
float 	getHumidity();//obtiene la humedad
float 	getTemperature();//obtiene la temperatura
int 	getSignalLevel( int usTimeOut, bool state );

#endif
