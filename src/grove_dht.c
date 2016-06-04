// Example for using grovepi digital humidity and temp sensor
// gcc -o dht grove_dht.c grovepi.c -Wall
// gprado 05/06/2016

#include "grovepi.h"

#define	dht_temp_cmd	40
float *dht(int pin, int module_type);

int main(void)
{		
	float *ht;
	
	//Exit on failure to start communications with the GrovePi
	if(init()==-1)
		exit(1);
	
	//Set pin mode to input
	pinMode(7,0);
	while(1)
	{
		ht=dht(7,0);
		printf("humidity = %.02f%% temp = %.02f C\n", ht[0], ht[1]);
		//Sleep for 50ms
		pi_sleep(50);
	}
   	return 1;
}

float *dht(int pin, int module_type)
{
	int data;
	write_block(dht_temp_cmd, pin, module_type, 0);
	read_byte();
	read_block();

	static float ht[2];
	unsigned char h[]={r_buf[5], r_buf[6], r_buf[7], r_buf[8]};
	memcpy(&ht[0], &h, sizeof(ht[0]));
	unsigned char t[]={r_buf[1], r_buf[2], r_buf[3], r_buf[4]};
	memcpy(&ht[1], &t, sizeof(ht[1]));

	return ht;
}
