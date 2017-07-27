#include <nrf24l01.h>

float nrfoldtime=0;
int num = 1;
bool oldstat = false;
#define uchar unsigned char
nrf24l01 nrf;
uchar TX_ADDRESS[] = { 192, 168, 10, 10, 6 };
uchar RX_ADD_P1[] = { 192, 168, 10, 10, 10 };
uchar RX_PW = 10;
uchar tx_data[32];
uchar rx_data[32] = { 0 };
uchar data[5] = { 1 , 6 , 6 , 6 , 6 };

void nrfsent(const char *stat)
{
	bool nstate = strcmp(stat, "ON") ? false : true;
	if (nstate!=oldstat)
	{
		float nrfnewtime = millis();
		if (nrfnewtime - nrfoldtime > 500)
		{
			int whatsend = nstate;
			data[0] = whatsend;
			*nrf.send_data_[0] = data[0];
			*nrf.send_data_[1] = data[1];
			*nrf.send_data_[2] = data[2];
			*nrf.send_data_[3] = data[3];
			*nrf.send_data_[4] = data[4];

			if (nrf.send_data(TX_ADDRESS, tx_data)) {
				for (uchar x = 5; x < RX_PW; ++x) {
					Serial.print(rx_data[x]);
					Serial.print(" : ");
				}
				Serial.println(" ");
			}
			else {
				Serial.println("Time Out!");
			}

			nrfoldtime = millis();
			num++;
		}
	}
	oldstat = nstate;
}
