#include <reg52.h>        //  包含51单片机寄存器定义的头文件
#include <intrins.h>      //包含_nop_()函数定义的头文件
#include <uart.h>
sbit p10 = P1^0;
sbit p11 = P1^1;
sbit p12 = P1^2;
sbit p13 = P1^3;
uchar has = 0;
uchar nul = 0;
uchar i1;
uchar j;
uint count=1000;
//电机正转：DIR=1 PWM=PWM
//DIR=0 PWM=PWM
//DIR=X PWM=0

void setPWM(uchar val)
{
	 has = val;
	 nul = 100-has;
}
void main()
{
	//P1=11111100;
	//uart_init();
	setPWM(40);
	while(count)
	{
		p11=1;p13=1;p10=1;p12=1;
		for(i1=0;i1<has;i1++)
		{;}
		p11=0;p13=0;
		for(j=0;j<nul;j++)
		{;}
		count--;
	}
	count=2000;
	while(count)
	{
		p11=1;p13=1;p10=0;p12=1;
		for(i1=0;i1<has;i1++)
		{;}
		p11=0;p13=0;
		for(j=0;j<nul;j++)
		{;}
		count--;
	}
	count=1000;	
		while(count)
	{
		p11=1;p13=1;p10=1;p12=1;
		for(i1=0;i1<has;i1++)
		{;}
		p11=0;p13=0;
		for(j=0;j<nul;j++)
		{;}
		count--;
	}
	count=1400;
	while(count)
	{
		p11=1;p13=1;p10=1;p12=0;
		for(i1=0;i1<has;i1++)
		{;}
		p11=0;p13=0;
		for(j=0;j<nul;j++)
		{;}
		count--;
	}
	count=1000;
}

