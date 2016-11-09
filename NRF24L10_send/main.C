/**************************************************
来源：网上
改写：bluefeel
时间：10-03-13
单位：桂林理工大学

STATUS	 
7 默认为0
6 接收数据中断。当接收到有效数据后置1.写1清除中断
5 数据发送完成中断。当数据发送完成后产生中断。如果工作在自动应答模式下只有当接收到应答信号后置1
4 达到最多次重发中断。写1清除中断，如果此位置1了则必须清除才能继续通讯
3：1 接收数据通道号 000-101 数据通道号 110 未使用 111：RX FIFO寄存器为空
0 TX FIFO寄存器满标志 1：TX FIFO 寄存器满 0：TX FIFO寄存器未满


/**************************************************/

#include <reg52.h>
#include <intrins.h>
#include <api.h>
#include <uart.h>

#define uchar unsigned char

/***************************************************/
#define TX_ADR_WIDTH   4  // 4字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH 20  // 数据通道有效数据宽度（fifo字节）

uchar code TX_ADDRESS[TX_ADR_WIDTH] = {0xC0,0xEE,0xD2,0xA2};  // 定义一个静态发送地址
uchar RX_BUF[TX_PLOAD_WIDTH];
uchar TX_BUF[TX_PLOAD_WIDTH];
uchar flag;
uchar DATA = 0x26;
uchar bdata sta;
sbit  RX_DR	 = sta^6;
sbit  TX_DS	 = sta^5;
sbit  MAX_RT = sta^4;
char  delay = 1500;
/**************************************************/

/**************************************************
函数: init_io()

描述:
    初始化IO
/**************************************************/
void init_io(void)
{
	CE  = 0;        // 待机
	CSN = 1;        // SPI禁止
	SCK = 0;        // SPI时钟置低
	IRQ = 1;        // 中断复位
}
/**************************************************/

/**************************************************
函数：delay_ms()

描述：
    延迟x毫秒
/**************************************************/
void delay_ms(uchar x)
{
    uchar i, j;
    i = 0;
    for(i=0; i<x; i++)
    {
       j = 250;
       while(--j);
	   j = 250;
       while(--j);
    }
}
/**************************************************/

/**************************************************
函数：SPI_RW()

描述：
    根据SPI协议，写一字节数据到nRF24L01，同时从nRF24L01
	读出一字节
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar i;
   	for(i=0; i<8; i++)          // 循环8次
   	{
   		MOSI = (byte & 0x80);   // byte最高位输出到MOSI
   		byte <<= 1;             // 低一位移位到最高位
   		SCK = 1;                // 拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
   		byte |= MISO;       	// 读MISO到byte最低位
   		SCK = 0;            	// SCK置低
   	}
    return(byte);           	// 返回读出的一字节
}
/**************************************************/

/**************************************************
函数：SPI_RW_Reg()

描述：
    写数据value到reg寄存器
/**************************************************/
uchar SPI_RW_Reg(uchar reg, uchar value)
{
	uchar status;
  	CSN = 0;                   // CSN置低，开始传输数据
  	status = SPI_RW(reg);      // 选择寄存器，同时返回状态字
  	SPI_RW(value);             // 然后写数据到该寄存器
  	CSN = 1;                   // CSN拉高，结束数据传输
  	return(status);            // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Read()

描述：
    从reg寄存器读一字节
/**************************************************/
uchar SPI_Read(uchar reg)
{
	uchar reg_val;
  	CSN = 0;                    // CSN置低，开始传输数据
  	SPI_RW(reg);                // 选择寄存器
  	reg_val = SPI_RW(0);        // 然后从该寄存器读数据
  	CSN = 1;                    // CSN拉高，结束数据传输
  	return(reg_val);            // 返回寄存器数据
}
/**************************************************/

/**************************************************
函数：SPI_Read_Buf()

描述：
    从reg寄存器读出bytes个字节，通常用来读取接收通道
	数据或接收/发送地址
/**************************************************/
uchar SPI_Read_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN置低，开始传输数据
  	status = SPI_RW(reg);       // 选择寄存器，同时返回状态字
  	for(i=0; i<bytes; i++)
    	pBuf[i] = SPI_RW(0);    // 逐个字节从nRF24L01读出
  	CSN = 1;                    // CSN拉高，结束数据传输
  	return(status);             // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Write_Buf()

描述：
    把pBuf缓存中的数据写入到nRF24L01，通常用来写入发
	射通道数据或接收/发送地址
/**************************************************/
uchar SPI_Write_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN置低，开始传输数据
  	status = SPI_RW(reg);       // 选择寄存器，同时返回状态字
  	for(i=0; i<bytes; i++)
    	SPI_RW(pBuf[i]);        // 逐个字节写入nRF24L01
  	CSN = 1;                    // CSN拉高，结束数据传输
  	return(status);             // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：RX_Mode()

描述：
    这个函数设置nRF24L01为接收模式，等待接收发送设备的数据包
/**************************************************/
void RX_Mode(void)
{
		CE = 0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 接收设备接收通道0使用和发送设备相同的发送地址
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);               // 使能接收通道0自动应答
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);           // 使能接收通道0
  	SPI_RW_Reg(WRITE_REG + SETUP_AW, 0x02);	 		   //设置地址宽度 1:0 00-无效 01-3字节 10-4字节 11 5字节
		SPI_RW_Reg(WRITE_REG + RF_CH, 0);                 // 选择射频通道0x40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  // 接收通道0选择和发送通道相同有效数据宽度
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0F);            // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);              // CRC使能，16位CRC校验，上电，接收模式
  	CE = 1;                                            // 拉高CE启动接收设备
	while(delay--);
	delay = 150;

}
/**************************************************/

/**************************************************
函数：TX_Mode()

描述：
    这个函数设置nRF24L01为发送模式，（CE=1持续至少10us），
	130us后启动发射，数据发送结束后，发送模块自动转入接收
	模式等待应答信号。
/**************************************************/
void TX_Mode(uchar * BUF)
{
	CE = 0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同
  	SPI_Write_Buf(WR_TX_PLOAD, BUF, TX_PLOAD_WIDTH);                  // 写数据包到TX FIFO
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);       // 使能接收通道0自动应答
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);   // 使能接收通道0
  	SPI_RW_Reg(WRITE_REG + SETUP_AW, 0x02);	 	//设置地址宽度 1:0 00-无效 01-3字节 10-4字节 11 5字节
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x0a);  // 自动重发延时等待250us+86us，自动重发10次
  	SPI_RW_Reg(WRITE_REG + RF_CH, 0);         // 选择射频通道0x40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x0F);    // 数据传输率2Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // CRC使能，16位CRC校验，上电
	CE = 1;
	while(delay--);
	delay = 150;
}
/**************************************************/

/**************************************************
函数：Check_ACK()

描述：
    检查接收设备有无接收到数据包，设定没有收到应答信
	号是否重发
/**************************************************/
uchar Check_ACK(bit clear)
{
	send_string("等待irq中断");
	while(IRQ);
	send_string("中断后");
	sta = SPI_Read(STATUS);
	//sta = SPI_RW(NOP);
	if(TX_DS)//数据发送完成中断。当数据发送完成后产生中断。如果工作在自动应答模式下只有当接收到应答信号后置1
		send_char('T');
	else
		send_char('F');                    // 返回状态寄存器
	if(MAX_RT){
		if(clear){    					// 是否清除TX FIFO，没有清除在复位MAX_RT中断标志后重发
		send_string("达到最大重发数");
		//send_char(SPI_Read(OBSERVE_TX));                     
			SPI_RW(FLUSH_TX);
		}
	}
	SPI_RW_Reg(WRITE_REG + STATUS, sta);  // 清除TX_DS或MAX_RT中断标志
	IRQ = 1;   
	if(TX_DS)//数据发送完成中断。当数据发送完成后产生中断。如果工作在自动应答模式下只有当接收到应答信号后置1
		return(0x54);
	else
		return(0x46);
}
/**************************************************/

/**************************************************
函数：CheckButtons()

描述：
    检查按键是否按下，按下则发送一字节数据
/**************************************************/
void CheckButtons()
{

			TX_BUF[0] = DATA;          // 数据送到缓存
			TX_Mode(TX_BUF);			// 把nRF24L01设置为发送模式并发送数据
			//LED = ~DATA;		        // 数据送到LED显示
			send_char(Check_ACK(1));               // 等待发送完毕，清除TX FIFO
			//delay_ms(250);
			//delay_ms(250);
			//LED = 0xff;			        // 关闭LED
			RX_Mode();			        // 设置为接收模式
			//while(!(P3 & 0x01));
			//DATA <<= 1;
			//if(!DATA)
			//	DATA = 0x26;
	
}
/**************************************************/

/**************************************************
函数：main()

描述：
    主函数
/**************************************************/
void main(void)
{
	uart_init();
	init_io();		              // 初始化IO
	RX_Mode();		
	while(1)
	{
		 
		while(RI)
		{	  DATA = read_char();
			  CheckButtons();           // 按键扫描
		}
		
		sta = SPI_Read(STATUS);	  // 读状态寄存器
	 	 if(RX_DR)				  // 判断是否接受到数据
		{
			//send_string("收到数据");
			send_char('&');
			SPI_Read_Buf(RD_RX_PLOAD, RX_BUF, TX_PLOAD_WIDTH);  // 从RX FIFO读出数据
			flag = 1;
		}
		SPI_RW_Reg(WRITE_REG + STATUS, sta);  // 清除RX_DS中断标志
		if(flag)		           // 接受完成
		{
			flag = 0;		       // 清标志
			send_char(RX_BUF[0]);
			send_char(';');
		}
	}
}
/**************************************************/