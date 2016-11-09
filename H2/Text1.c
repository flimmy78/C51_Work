#include<reg52.h>
#include<intrins.h>
int i=0;
void main()
{
	P1=0xff;
	for(i=0;i<100;i++)
	P0=0xff;
	for(i=0;i<100;i++)
	P0=0x00;	 
		
}