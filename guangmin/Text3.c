#include <reg51.h>
#include <intrins.h>
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//定义共阳数码管显示字型码 

#define	OP_READ	0x90		// 器件地址以及读取操作,0xa1即为1010 0001B
#define	OP_WRITE 0x91		// 器件地址以及写入操作,0xa1即为1010 0000B

sbit  Sclock = P3^2;
sbit  Sdata = P3^3;


void WriteCurrent(unsigned char y)
{
	unsigned char i;
	for(i = 0; i < 8; i++)		// 循环移入8个位
	{
    	Sdata = (bit)(y&0x80);   //通过按位“与”运算将最高位数据送到S
		                                  //因为传送时高位在前，低位在后
		_nop_();            //等待一个机器周期   
	   Sclock = 1;            //在SCL的上升沿将数据写入AT24Cxx      
   	_nop_();            //等待一个机器周期 
	  _nop_();             //等待一个机器周期       
		
	  	Sclock = 0;            //将SCL重新置为低电平，以在SCＬ线形成传送数据所需的８个脉冲
		y <<= 1;           //将y中的各二进位向左移一位
               
	}
}




void sleep()
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
void main()
{

	
}
void start()
{
	 Sclock = 1;
	 Sdata = 1;
	 sleep();
	 Sclock = 0;
	 sleep();
	 Sclock = 0;    //SCL为低电平时，SDA上数据才允许变化(即允许以后的数据传递）
}

void control()
{
	
}


void stop()// 停止位
{
	Sdata = 0;     //SDA初始化为低电平“0”	_n
	Sclock = 1;     //结束数据传送时，要求SCL为高电平“1”
	sleep();
	Sdata = 1;    //SDA的上升沿被认为是结束信号
	sleep();
	Sdata=0;
	Sclock=0;
}


bit Ask()                  
{
      bit ack_bit;        //储存应答位 
        Sdata = 1;	 // 发送设备（主机）应在时钟脉冲的高电平期间(SCL=1)释放SDA线，
	                //以让SDA线转由接收设备(AT24Cxx)控制
	_nop_();        //等待一个机器周期 
	_nop_();        //等待一个机器周期 
	Sclock = 1;       //根据上述规定，SCL应为高电平
	sleep();
	ack_bit = Sdata; //接受设备（AT24Cxx)向SDA送低电平，表示已经接收到一个字节
	               //若送高电平，表示没有接收到，传送异常 结束发送
	Sclock = 0;       //SCL为低电平时，SDA上数据才允许变化(即允许以后的数据传递）
	return  ack_bit;			// 返回AT24Cxx应答位



}

unsigned char ReadData()
// 从AT24Cxx移入数据到MCU
{
	unsigned char i;
	unsigned char x;   //储存从AT24Cxx中读出的数据
	for(i = 0; i < 8; i++)
	{
		Sclock = 1;                //SCL置为高电平
		x<<=1;                  //将x中的各二进位向左移一位
		x|=(unsigned char)Sdata;  //将SDA上的数据通过按位“或“运算存入x中
		Sclock = 0;                        //在SCL的下降沿读出数据
	}
	return(x);                //将读取的数据返回
}

  unsigned char ReadCurrent()
{
	unsigned char x;
	start();               //开始数据传递
	WriteCurrent(OP_READ);   //选择要操作的AT24Cxx芯片，并告知要读其数据
	Ask();
	x=ReadData();         //将读取的数据存入x
	stop();                //停止数据传递
	return x;              //返回读取的数据
}
