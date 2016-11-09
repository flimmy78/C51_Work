/************************************************
					CAUC
					yeyongan
*************************************************/
#include <reg52.h>
#include <intrins.h>
#define uchar	unsigned char
sbit CS=P1^0;				   
sbit Clk=P1^1;				   //时钟
sbit DO=P1^2;				   //ADC0832输出引脚
sbit DI=P1^3;				   //ADC0832输入引脚
//sbit key=P3^3;				   //按键
//bit keydownflg;				   //操作位的定义
bit adc_flg;
uchar dat,channel;			   
uchar key_buffer;				
uchar P2_buffer;				
uchar Beep_cnt;
uchar disp_cnt;
uchar count4ms;
uchar disp_buff[5]; 			 //数码管显示缓存
uchar disp_buff2[5]; 			 //数码管显示缓存
uchar code Tab1[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E};	//共阳数码表
uchar code Tab[5]={0x7f,0xbf,0xdf,0xef,0xf7};	   //数码管位选表
uchar test;
uchar adval;


uchar A_D(uchar CH)			   //AD函数
{
uchar i,adval,test;
		   //定义局部变量并初始化

	adval=0x00;
	test=0x00;
	Clk=0;					   //clk低电平
	DI=1;					   //DI初始高电平在第一个时钟脉冲的下降沿前保持高电平，表示启动信号
	_nop_();				   
	CS=0;					   //片选
	_nop_();
	Clk=1;					   //clk上升沿，起始位写入
	_nop_();
	if(CH==0x00)			   //选择通道0
	{
		Clk=0;					//clk低电平
		 DI=1;					
		_nop_();
		Clk=1;					//clk上升沿，通道0的第一位写入
		_nop_();
		Clk=0;
		DI=0;
		_nop_();
		Clk=1;					//clk上升沿，通道0的第二位写入
		_nop_();
	}
	else
	{
		Clk=0;
		DI=1;
		_nop_();
		Clk=1;				   //clk上升沿，通道1的第一位写入
		_nop_();
		Clk=0;
		DI=1;
		_nop_();			   //clk上升沿，通道1的第二位写入
		Clk=1;
		_nop_();
	}
	Clk=0;
	DI=1;
	for(i=0;i<8;i++)			 //从高位向低位读取八位AD值
	{
		_nop_();
		adval<<=1;
		Clk=1;
		_nop_();
		Clk=0;
		if(DO)
			adval|=0x01;
		else
			adval|=0x00;
	}
	for(i=0;i<8;i++)
	{
		
		test>>=1;				   //从低位向高位读取八位AD值
		
		if(DO)
			test|=0x80;	
		else			
			test|=0x00;
			_nop_();
			Clk=1;
			_nop_();
			Clk=0;
	}
	if(adval==test)	
	dat=test;		  	//判断两个读取值是否相等相等就把读取的数赋值给DAT
	_nop_();
	CS=1;
	DO=1;
	Clk=1;
	return dat;
}				
void FillDispBuffer(uchar datas[])						//数码管显示缓存函数
{
	datas[0]=channel;						//显示通道
	datas[1]=12;							//显示"C"
	datas[2]=dat/100;						//显示个位
	datas[3]=dat%100*10/100;				//显示十分位 
	datas[4]=((dat%51)*10%51)*10/51; 		//显示百分位

	
	
}

void delay(int c)     //延时，C不能小于5
    {
    int i,j;
    for(i=0;i<c;i++)
        for(j=0;j<10;j++)
            ;
     }
void main(void)			   //主函数
{
	P0=0xff;			   //初始化
	P2=0xff;
	dat=0x00;
	disp_cnt=0;
	count4ms=0;
	channel=0;
	TMOD=0x01;
	TH0=(65535-10000)/256;
	TL0=(65535-10000)%256;
	EA=1;
	TR0=1;
	ET0=1;
	while(1)
	{
		  	A_D(0x00);	 //ADC函数
		FillDispBuffer(disp_buff);	//数码管显示缓存
			P0=Tab1[disp_buff[2]];
			P2=0xfe;
			delay(1);
			P0=Tab1[disp_buff[3]];
			P2=0xfd;
			delay(1);
			P0=Tab1[disp_buff[4]];
			P2=0xfb;
			delay(1);
	
			
		/*	A_D(0xff);	 //ADC函数
		FillDispBuffer(disp_buff2);	//数码管显示缓存	
			P0=Tab1[disp_buff2[2]];
			P2=0xdf;
			delay(1);
			P0=Tab1[disp_buff2[3]];
			P2=0xbf;
			delay(1);
			P0=Tab1[disp_buff2[4]];
			P2=0x7f;
			delay(1);	 */

				
			
			dealkey(5);
		
		}
	
}
void T0_service(void) interrupt 1		//定时器0中断子函数
{
	TH0=(65535-10000)/256;
	TL0=(65535-10000)%256;
			
	
	
		
	
}
