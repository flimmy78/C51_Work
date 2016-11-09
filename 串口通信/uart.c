#include<reg52.h>
#include<intrins.h>
#include<uart.h>
uint text[]={0xC0,0xEE,0xD2,0xA2,0xA3,0xBA,0x00};
uint i;
uchar tem;

void main()
{
	uart_init();
	send_string("ÎÒµÄ¶î¶î¶î");					 
	while(1)
	{
		
		send_string(text);
		while(RI)
		{
			tem=read_char();

			send_char(tem);
		}
		
		
	}
}