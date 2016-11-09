/**************************************************
来源：网上
改写：bluefeel
时间：10-03-13
单位：桂林理工大学
/**************************************************/

#include <reg52.h>
#include <api.h>
#include <uart.h>

#define uchar unsigned char

/***************************************************/
#define TX_ADR_WIDTH   5  // 5字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH 4  // 数据通道有效数据宽度
#define LED P2

uchar code TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  // 定义一个静态发送地址
uchar RX_BUF[TX_PLOAD_WIDTH];
uchar TX_BUF[TX_PLOAD_WIDTH];
uchar flag;
uchar DATA = 0x40;
uchar bdata sta;
sbit  RX_DR	 = sta^6;
sbit  TX_DS	 = sta^5;
sbit  MAX_RT = sta^4;
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
	LED = 0xff;		// 关闭指示灯
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
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);                 // 选择射频通道0x40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  // 接收通道0选择和发送通道相同有效数据宽度
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);            // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);              // CRC使能，16位CRC校验，上电，接收模式
  	CE = 1;                                            // 拉高CE启动接收设备
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
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x0a);  // 自动重发延时等待250us+86us，自动重发10次
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);         // 选择射频通道0x40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // CRC使能，16位CRC校验，上电
	CE = 1;
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
	while(IRQ);
	sta = SPI_RW(NOP);                    // 返回状态寄存器
	if(MAX_RT)
		if(clear){                         // 是否清除TX FIFO，没有清除在复位MAX_RT中断标志后重发
			SPI_RW(FLUSH_TX);
			send_string("最大重发了");
		}
	SPI_RW_Reg(WRITE_REG + STATUS, sta);  // 清除TX_DS或MAX_RT中断标志
	IRQ = 1;
	if(TX_DS)
		return(0x00);
	else
		return(0xff);
}
/**************************************************/

/**************************************************
函数：CheckButtons()

描述：
    检查按键是否按下，按下则发送一字节数据
/**************************************************/
void CheckButtons()
{
	
			TX_BUF[0] = ~DATA;          // 数据送到缓存
			TX_Mode(TX_BUF);			// 把nRF24L01设置为发送模式并发送数据
			LED = ~DATA;		        // 数据送到LED显示
			Check_ACK(1);               // 等待发送完毕，清除TX FIFO
			delay_ms(250);
			delay_ms(250);
			LED = 0xff;			        // 关闭LED
			RX_Mode();			        // 设置为接收模式
			while(!(P3 & 0x01));
			DATA <<= 1;
			if(!DATA)
				DATA = 0x01;
		
}
///////////////////////////////////////////////////////

#define TX_ADR_WIDTH2   5  // 5字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH2 4  // 数据通道有效数据宽度


uchar code TX_ADDRESS2[TX_ADR_WIDTH2] = {0x34,0x43,0x10,0x10,0x01};  // 定义一个静态发送地址
uchar RX_BUF2[TX_PLOAD_WIDTH2];
uchar TX_BUF2[TX_PLOAD_WIDTH2];
uchar flag2;
uchar DATA2 = 0x40;
uchar bdata sta2 ;
sbit  RX_DR2	 = sta2^6;
sbit  TX_DS2	 = sta2^5;
sbit  MAX_RT2 = sta2^4;
/**************************************************/

/**************************************************
函数: init_io2()

描述:
    初始化IO
/**************************************************/
void init_io2(void)
{
	CE2  = 0;        // 待机
	CSN2 = 1;        // SPI禁止
	SCK2 = 0;        // SPI时钟置低
	IRQ2 = 1;        // 中断复位
}
/**************************************************/

/**************************************************
函数：delay_ms2()

描述：
    延迟x毫秒
/**************************************************/
void delay_ms2(uchar x)
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
函数：SPI_RW2()

描述：
    根据SPI协议，写一字节数据到nRF24L01，同时从nRF24L01
	读出一字节
/**************************************************/
uchar SPI_RW2(uchar byte)
{
	uchar i;
   	for(i=0; i<8; i++)          // 循环8次
   	{
   		MOSI2 = (byte & 0x80);   // byte最高位输出到MOSI2
   		byte <<= 1;             // 低一位移位到最高位
   		SCK2 = 1;                // 拉高SCK2，nRF24L01从MOSI2读入1位数据，同时从MISO2输出1位数据
   		byte |= MISO2;       	// 读MISO2到byte最低位
   		SCK2 = 0;            	// SCK2置低
   	}
    return(byte);           	// 返回读出的一字节
}
/**************************************************/

/**************************************************
函数：SPI_RW2_Reg2()

描述：
    写数据value到reg寄存器
/**************************************************/
uchar SPI_RW2_Reg2(uchar reg, uchar value)
{
	uchar status;
  	CSN2 = 0;                   // CSN2置低，开始传输数据
  	status = SPI_RW2(reg);      // 选择寄存器，同时返回状态字
  	SPI_RW2(value);             // 然后写数据到该寄存器
  	CSN2 = 1;                   // CSN2拉高，结束数据传输
  	return(status);            // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Read2()

描述：
    从reg寄存器读一字节
/**************************************************/
uchar SPI_Read2(uchar reg)
{
	uchar reg_val;
  	CSN2 = 0;                    // CSN2置低，开始传输数据
  	SPI_RW2(reg);                // 选择寄存器
  	reg_val = SPI_RW2(0);        // 然后从该寄存器读数据
  	CSN2 = 1;                    // CSN2拉高，结束数据传输
  	return(reg_val);            // 返回寄存器数据
}
/**************************************************/

/**************************************************
函数：SPI_Read2_Buf()

描述：
    从reg寄存器读出bytes个字节，通常用来读取接收通道
	数据或接收/发送地址
/**************************************************/
uchar SPI_Read2_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN2 = 0;                    // CSN2置低，开始传输数据
  	status = SPI_RW2(reg);       // 选择寄存器，同时返回状态字
  	for(i=0; i<bytes; i++)
    	pBuf[i] = SPI_RW2(0);    // 逐个字节从nRF24L01读出
  	CSN2 = 1;                    // CSN2拉高，结束数据传输
  	return(status);             // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：SPI_Write_Buf2()

描述：
    把pBuf缓存中的数据写入到nRF24L01，通常用来写入发
	射通道数据或接收/发送地址
/**************************************************/
uchar SPI_Write_Buf2(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN2 = 0;                    // CSN2置低，开始传输数据
  	status = SPI_RW2(reg);       // 选择寄存器，同时返回状态字
  	for(i=0; i<bytes; i++)
    	SPI_RW2(pBuf[i]);        // 逐个字节写入nRF24L01
  	CSN2 = 1;                    // CSN2拉高，结束数据传输
  	return(status);             // 返回状态寄存器
}
/**************************************************/

/**************************************************
函数：RX_Mode2()

描述：
    这个函数设置nRF24L01为接收模式，等待接收发送设备的数据包
/**************************************************/
void RX_Mode2(void)
{
	CE2 = 0;
  	SPI_Write_Buf2(WRITE_REG + RX_ADDR_P0, TX_ADDRESS2, TX_ADR_WIDTH2);  // 接收设备接收通道0使用和发送设备相同的发送地址
  	SPI_RW2_Reg2(WRITE_REG + EN_AA, 0x01);               // 使能接收通道0自动应答
  	SPI_RW2_Reg2(WRITE_REG + EN_RXADDR, 0x01);           // 使能接收通道0
  	SPI_RW2_Reg2(WRITE_REG + RF_CH, 40);                 // 选择射频通道0x40
  	SPI_RW2_Reg2(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH2);  // 接收通道0选择和发送通道相同有效数据宽度
  	SPI_RW2_Reg2(WRITE_REG + RF_SETUP, 0x07);            // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW2_Reg2(WRITE_REG + CONFIG, 0x0f);              // CRC使能，16位CRC校验，上电，接收模式
  	CE2 = 1;                                            // 拉高CE2启动接收设备
}
/**************************************************/

/**************************************************
函数：TX_Mode2()

描述：
    这个函数设置nRF24L01为发送模式，（CE2=1持续至少10us），
	130us后启动发射，数据发送结束后，发送模块自动转入接收
	模式等待应答信号。
/**************************************************/
void TX_Mode2(uchar * BUF)
{
	CE2 = 0;
  	SPI_Write_Buf2(WRITE_REG + TX_ADDR, TX_ADDRESS2, TX_ADR_WIDTH2);     // 写入发送地址
  	SPI_Write_Buf2(WRITE_REG + RX_ADDR_P0, TX_ADDRESS2, TX_ADR_WIDTH2);  // 为了应答接收设备，接收通道0地址和发送地址相同
  	SPI_Write_Buf2(WR_TX_PLOAD, BUF, TX_PLOAD_WIDTH2);                  // 写数据包到TX FIFO
  	SPI_RW2_Reg2(WRITE_REG + EN_AA, 0x01);       // 使能接收通道0自动应答
  	SPI_RW2_Reg2(WRITE_REG + EN_RXADDR, 0x01);   // 使能接收通道0
  	SPI_RW2_Reg2(WRITE_REG + SETUP_RETR, 0x0a);  // 自动重发延时等待250us+86us，自动重发10次
  	SPI_RW2_Reg2(WRITE_REG + RF_CH, 40);         // 选择射频通道0x40
  	SPI_RW2_Reg2(WRITE_REG + RF_SETUP, 0x07);    // 数据传输率1Mbps，发射功率0dBm，低噪声放大器增益
  	SPI_RW2_Reg2(WRITE_REG + CONFIG, 0x0e);      // CRC使能，16位CRC校验，上电
	CE2 = 1;
}
/**************************************************/

/**************************************************
函数：Check_ACK2()

描述：
    检查接收设备有无接收到数据包，设定没有收到应答信
	号是否重发
/**************************************************/
uchar Check_ACK2(bit clear)
{
	while(IRQ2);
	sta2 = SPI_RW2(NOP);                    // 返回状态寄存器
	if(MAX_RT2)
		if(clear){                         // 是否清除TX FIFO，没有清除在复位MAX_RT2中断标志后重发
			SPI_RW2(FLUSH_TX);
			send_string("最大重发2");
			}
	SPI_RW2_Reg2(WRITE_REG + STATUS, sta2);  // 清除TX_DS2或MAX_RT2中断标志
	IRQ2 = 1;
	if(TX_DS2)
		return(0x00);
	else
		return(0xff);
}

void CheckButtons2()
{
	
			TX_BUF2[0] = ~DATA;          // 数据送到缓存
			TX_Mode2(TX_BUF2);			// 把nRF24L01设置为发送模式并发送数据
			Check_ACK2(1);               // 等待发送完毕，清除TX FIFO
			RX_Mode2();			        // 设置为接收模式
		
		
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
	init_io2();		              // 初始化IO
	RX_Mode2();		              // 设置为接收模式
	init_io();		              // 初始化IO
	RX_Mode();		              // 设置为接收模式
	while(1)
	{
		
			CheckButtons();
			send_string("完成了");
		
		           // 按键扫描
		sta2 = SPI_Read(STATUS);	  // 读状态寄存器
	    if(RX_DR)				  // 判断是否接受到数据
		{
			SPI_Read_Buf(RD_RX_PLOAD, RX_BUF, TX_PLOAD_WIDTH);  // 从RX FIFO读出数据
			flag = 1;
		}
		SPI_RW_Reg(WRITE_REG + STATUS, sta2);  // 清除RX_DS中断标志
		if(flag)		           // 接受完成
		{
			flag = 0;		       // 清标志
			LED = RX_BUF[0];	   // 数据送到LED显示
		//	send_char(LED);
			delay_ms(250);
			delay_ms(250);
  			LED = 0xff;		       // 关闭LED
		}

		CheckButtons2();
		////////////////////////////////////////////
		sta2 = SPI_Read2(STATUS);	  // 读状态寄存器
	    if(RX_DR2)				  // 判断是否接受到数据
		{
			SPI_Read2_Buf(RD_RX_PLOAD, RX_BUF2, TX_PLOAD_WIDTH2);  // 从RX FIFO读出数据
			flag2 = 1;
		}
		SPI_RW2_Reg2(WRITE_REG + STATUS, sta2);  // 清除RX_DS中断标志
		if(flag2)		           // 接受完成
		{
			flag2 = 0;		       // 清标志
			//LED = RX_BUF2[0];	   // 数据送到LED显示
			send_char(RX_BUF2[0]);
			delay_ms2(250);
			delay_ms2(250);
  		//	LED = 0xff;		       // 关闭LED
		}
	}
}
/**************************************************/