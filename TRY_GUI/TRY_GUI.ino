#include <dht11.h>

dht11 DHT11;
#define DHT11PIN 3

#define LED_PIN 13

#include "nrf24.h"
#include "menu.h"
#include "menuDrawer.h"


void nrfbegin()
{
	Serial.begin(9600);
	nrf.initialize();//初始化
	nrf.Send_mode();
}
void setup() {
	nrfbegin();

	menuDrawerSetup();

	menuSetup();
}


void loop() {
	menuDrawerLoop();
}









