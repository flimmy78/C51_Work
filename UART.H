
#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define XTAL 11059200    // CUP 晶振频率
#define baudrate 1200    // 通信波特率

uint s=0;

extern void send_string(uchar *ps)
{		
	s=0;
	while(ps[s]!='\0'){
		SBUF = ps[s];
		s++;            // 要发送的字符放入缓冲区
		while(TI == 0);    // 一直等等，直到发送完数据为止
		TI = 0;
	}
}

extern void send_char(uchar ps)
{			
		SBUF = ps;           // 要发送的字符放入缓冲区
		while(TI == 0);    // 一直等等，直到发送完数据为止
		TI = 0;
		
}

uchar read_char(void)   //调用此函数前，要先调用RI,查看是否收到数据，确定已收到的话，再调用此函数
{
RI=0;                       //要先将RI=0再返回，否则，返回后，RI=0这句就不执行了。
return SBUF;
}

extern void uart_init(void)
{
TMOD = 0x20;   // 定时器1工作于8位自动重载模式, 用于产生波特率
TH1=(unsigned char)(256 - (XTAL / (32L * 12L * baudrate)));
TL1=(unsigned char)(256 - (XTAL / (32L * 12L * baudrate)));                // 定时器1赋初值
SM0 = 0; //串口工作方式控制
SM1 = 1; //串口工作方式控制 //这两个共同控制了串口工作方式为 0;
REN = 1; //串口接收允许
PCON = 0x00;
TR1 = 1; //允许串口接收
ES = 0;   //关闭串口中断
}

