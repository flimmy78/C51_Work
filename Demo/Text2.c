#include<reg52.h>
			sbit aa = P3^2;
 void main()
 { 	
 EA	= 1;
EX0 =1;
 IT0=1;    //选择负跳变来触发外中断
 aa= 0;
 	while(1)
	{
 		P0=0x00;
		 
	}
 }

 void test() interrupt 0
 { 	 if(P1==0x00)
 	{
		P1=0xff;
		aa=1;
	}else
	{	 aa=1;
		P1=0x00;
	}
 }