// Example for using the grovepi digital ultrasound sensor
// gcc -o us grove_ultrasonic.c grovepi.c -Wall
// gprado 05/06/2016

#include "grovepi.h"

#define	uRead_cmd	7
int ultrasonicRead(int pin);

int main(void)
{		
	int dval;
	
	//Exit on failure to start communications with the GrovePi
	if(init()==-1)
		exit(1);
	
	//Set pin mode to input
	pinMode(8,0);
	while(1)
	{
		dval=ultrasonicRead(8);
		printf("Digital read %d\n", dval);
		//Sleep for 50ms
		pi_sleep(50);
	}
   	return 1;
}

int ultrasonicRead(int pin)
{
	int data;
	write_block(uRead_cmd, pin, 0, 0);
	read_byte();
	read_block();
	data = r_buf[1]* 256 + r_buf[2];
	if (data==65535)
		return -1;
	return data;
}
