#include <reg52.h>        //  包含51单片机寄存器定义的头文件
#include <intrins.h>      //包含_nop_()函数定义的头文件
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//定义共阳数码管显示字型码 
#define uchar unsigned char
#define uint  unsigned int
 uchar count =0;
main()
{
	
	EA=1;
	EX0=1;
	IT0=1;
	TMOD=0x01;
	 ET1=1;
	 TR1=1;
	TH1=(65536-50000)/256;  //定时器T0的高8位赋初值
	TL1=(65536-50000)%256;  //定时器T0的高8位赋初值
	while(1)
	{
	   P0 = table[count];
	   P2 = 0xfe;
	}
}
void timer0() interrupt 0
{	 count++;
	
	if(count>9)
	{
		count=0;
	}

} 
uchar time = 0;
sbit fmq=P3^7;
void timer1() interrupt 3
{
	 //P1=~P1;
	  time++;
	if(time>20)
	{
	//	fmq=~fmq;	
	  time=0;
		   TH1=(65536-50000)/256;  //定时器T0的高8位赋初值
			TL1=(65536-50000)%256;  //定时器T0的高8位赋初值
	}

} 