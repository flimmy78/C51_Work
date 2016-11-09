#include<reg52.h>

#include <intrins.h>      //包含_nop_()函数定义的头文件
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//定义共阳数码管显示字型码 

sbit a=P1^0;
sbit b=P1^1;
sbit c=P1^2;
sbit d=P1^3;		 //数据左边第一个
sbit key=P1^4;
int count=0;
char bi=0;
 void delay1ms(char cou)
{
   unsigned char i,j,c;
   for(c=0;c<cou;c++)
   {	
	 for(i=0;i<4;i++)
	  for(j=0;j<33;j++)
	   ;
	 }		 
 }
main()
{		 
	EA=1;
	ET0=1;
	TMOD=0x01;

	EX1=1;
	IT1=1;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
	TR0=0;

while(1)
{	

	char i=0;
	char j=0;
	if(bi>70)
	{
		bi=0;
	}
	for(;i<100-bi;i++)
	{
		a=1;
		b=0;
		c=1;
		d=0;
	}
	for(;j<bi;j++)
	{
		a=0;
		a=0;
		c=0;
		d=0;
	}
	P0=table[bi/10];
	P2=0xfe;
	P0=table[bi%10];
	P2=0xfd;
	if(key==0)
		{ 
		delay1ms(15);
		if(key==0)
		{
		 bi++;
		P0=~P0;
		 }
		while(!key);
	}
		count=count+1;

}	  
}
 char co=0;
void time0() interrupt 1
{
	  //c=0;
	 // d=0;
	//P1=~P1;
	 if(co==20)
	 { 
		while(1)
		{
		/*P0=table[count/1000];	
		P2=0xfe;
		P0=table[count%1000*10/100];
		P2=0xfd;
		P0=table[count%1000%100/10];
		P2=0xfb;*/
		P0=table[count/100];	
		P2=0xfe;
		P0=table[count%100/10];
		P2=0xfd;
			}  
	}
	co++;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;

	
}

void time1() interrupt 2
{
	

}