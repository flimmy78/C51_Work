#include <reg52.h>        //  ����51��Ƭ���Ĵ��������ͷ�ļ�
#include <intrins.h>      //����_nop_()���������ͷ�ļ�
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//���干���������ʾ������ 
#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P3^3;
uchar time;
uchar hei;
uchar low;
	void delay1ms()
{
   unsigned char i,j;	
	 for(i=0;i<4;i++)
	  for(j=0;j<33;j++)
	   ;		 
 }

 void delaynms(unsigned char n)
 {
   unsigned char i;
	for(i=0;i<n;i++)
	   delay1ms();
 }

bit start()
{
	bit res = 1;

	DQ=1;
	_nop_();
	DQ=0;
	for(time=0;time<240;time++);
	DQ=1;
	for(time=0;time<10;time++);
	res=DQ;
	for(time=0;time<50;time++);
	return res;
	/*if(DQ==0)
	{
		P0=table[0];
		P2=0;
		while(1);
	}else
	{
		P0=table[1];
		P2=0;
		while(1);
	}
	*/
}
	  
uchar read()
{
	uchar dats;
	uchar i=8;
	while(i--){
		DQ=1;
		_nop_();
		DQ=0;
		_nop_();
		DQ=1;
		for(time=0;time<3;time++);
		dats>>=1;
		   if(DQ==1)
		      dats|=0x80;  //���������������1����1����dat
			else
				dats|=0x00;//���������������0����0����dat
	    for(time=0;time<17;time++);

	}
	return dats;
}

void write(uchar dats)
{	  	
	uchar i=8;
	while(i--){
		DQ=1;
		_nop_();
		DQ=0;
		_nop_();
		DQ=(bit)(dats&0x01);
		dats>>=1;
		for(time=0;time<20;time++);
	}
}

void main()
{
	while(1)
	{
 	if(start())
		return;
	 write(0xCC);
	 write(0x44);
	 delaynms(200);

	 	if(start())
		return;
	 write(0xCC);
	 write(0xBE);
	 hei=read();
	 low=read();
	 P0=table[0];
	 P2=hei;  
	 }
	 


}
