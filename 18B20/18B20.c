#include <reg52.h>        //  包含51单片机寄存器定义的头文件
#include <intrins.h>      //包含_nop_()函数定义的头文件
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//定义共阳数码管显示字型码 
#define uchar unsigned char
#define uint  unsigned int
sbit fmq=P3^6;
 sbit jdq=P3^7;
sbit DQ = P3^3;
uchar time;
uchar hei;
uchar low;
unsigned char flag,tltemp;
void delay500us()
{

unsigned char j;

 for(j=0;j<57;j++)	  //500us基准延时程序 
 {
  ;
 } 
}

void beep()	//产生1KHZ频率声音的函数
{
 	fmq=0;
	delay500us();
	fmq=1;
	delay500us();	

}
 
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

 void display_temp1(unsigned char x)
{
			  unsigned char i=255;
 unsigned char j,k,l;     //j,k,l分别储存温度的百位、十位和个位
 		j=x/100;
		k=(x%100)/10;
		l=x%10; 
		if(k>=3&&l>=1)
		jdq=0;
		else
		jdq=1;

 	while(i--){
		              //取百位
		P0=table[j];
		P2=0xfe;
		delaynms(5);
		 
		P0=table[k];
		P2=0xfd;
		delaynms(5);
		
		P0=table[l];
		P2=0xfb; 
		delaynms(5);
		  if(k>=3&&l>=1)
	  	beep();
	}
	
	P2=0xff;
	
}

bit start()
{
	bit res = 1;

	DQ=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();

	DQ=0;
	for(time=0;time<240;time++);
	DQ=1;
	for(time=0;time<10;time++);
	res=DQ;
	/* if(DQ==0)
	{
		P0=table[0];
		P2=0;
		
	}else
	{
		P0=table[1];
		P2=0;
		
	}
	while(1);  */
	
	for(time=0;time<50;time++);
	  
	return res;
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
		      dats|=0x80;  //如果读到的数据是1，则将1存入dat
			else
				dats|=0x00;//如果读到的数据是0，则将0存入dat
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
		for(time=0;time<5;time++);
		DQ=1;
		for(time=0;time<15;time++);
	}
}

void main()
{
	unsigned char TL;     //储存暂存器的温度低位
	  unsigned char TH;    //储存暂存器的温度高位
      unsigned char TN;      //储存温度的整数部分
	  unsigned char TD;       //储存温度的小数部分
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
		 

		  TL=read();    //先读的是温度值低位
		TH=read();    //接着读的是温度值高位
		P0=table[0];
		P2=TL;  
		
		if((TH&0xf8)!=0x00)//判断高五位 得到温度正负标志
		{
			flag=1;
			TL=~TL;	 //取反
			TH=~TH;	  //取反
			tltemp=TL+1; //低位加1
			TL=tltemp;
			if(tltemp>255) TH++;  //如果低8位大于255，向高8位进1
		   TN=TH*16+TL/16;      //实际温度值=(TH*256+TL)/16,即：TH*16+TL/16
									  //这样得出的是温度的整数部分,小数部分被丢弃了
		   TD=(TL%16)*10/16;    //计算温度的小数部分,将余数乘以10再除以16取整，
		}
		TN=TH*16+TL/16;      //实际温度值=(TH*256+TL)/16,即：TH*16+TL/16
				                  //这样得出的是温度的整数部分,小数部分被丢弃了
	   TD=(TL%16)*10/16;    //计算温度的小数部分,将余数乘以10再除以16取整，
			                  //这样得到的是温度小数部分的第一位数字(保留1位小数)
	   display_temp1(TN);    //显示温度的整数部分
	 //  display_temp2(TD);    //显示温度的小数部分
      delaynms(10);               
    }
		// P0=table[0];
		// P2=hei;  
	 
	  


}
