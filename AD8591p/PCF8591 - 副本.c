
//-----------------------函数声明，变量定义------------------------
#include <reg52.h>
#include <intrins.h>
sbit SCL=P1^2;                   // 将p1.2口模拟时钟口
sbit SDA=P1^3;                   // 将p1.3口模拟数据口
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//定义共阳数码管显示字型码
bit askflag;
#define uchar unsigned char
#define uint  unsigned int
#define disdata  P0              //显示数据码输出口



uint data dis[4]={0x00,0x00,0x00,0x00}; 
                          //定义3个显示数据单元和1个数据存储单元



#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};
	                 
bit   bdata SystemError;                //从机错误标志位

/*********************** PCF8591专用变量定义 ***********************/

#define	PCF8591_WRITE	0x90 
#define	PCF8591_READ 	0x91
#define  NUM  4 	            //接收和发送缓存区的深度
uchar idata receivebuf[4]={0,0,0,0};    //数据接收缓冲区
uchar idata backs[4]={0,0,0,0}; 
char one=0;

/*******************************************************************/
/*                                                                 */
/*  延时函数                                                       */
/*                                                                 */
/*******************************************************************/

void delay(int ms)
{
    while(ms--)
	{
      uchar i;
	  for(i=0;i<250;i++)  
	   {
	    _nop_();			   
		_nop_();
		_nop_();
		_nop_();
	   }
	}
}		


//-------------------------------------------------------------------
// 函数名称： iic_start()
// 函数功能： 启动I2C总线子程序
//-------------------------------------------------------------------
void iic_start(void)
{ //时钟保持高，数据线从高到低一次跳变，I2C通信开始
	SDA = 1;     
	SCL = 1;
	delayNOP();      // 延时5us 
   	SDA = 0;
	delayNOP();
    SCL = 0;
}
//-------------------------------------------------------------------
// 函数名称： iic_stop()
// 函数功能： 停止I2C总线数据传送子程序
//-------------------------------------------------------------------
void iic_stop(void)
{  	
	SDA = 0;   	   //时钟保持高，数据线从低到高一次跳变，I2C通信停止
	SCL = 1;
	delayNOP();
	SDA = 1;
	delayNOP();
    SCL = 0;
}
//------------------------------------------------------------------
// 函数名称： iicInit_()
// 函数功能： 初始化I2C总线子程序
//------------------------------------------------------------------
 void iicInit(void)
   {
   SCL = 0;
   iic_stop();	
   }  
//-------------------------------------------------------------------
// 函数名称： slave_ACK
// 函数功能： 从机发送应答位子程序
//-------------------------------------------------------------------
void slave_ACK(void)
{
	SDA = 0;  
	SCL = 1;
	delayNOP();	
	SCL = 0;
}
//-------------------------------------------------------------------
// 函数名称： slave_NOACK
// 函数功能： 从机发送非应答位子程序，迫使数据传输过程结束
//-------------------------------------------------------------------
void slave_NOACK(void)
{ 
	SDA = 1;
	SCL = 1;
	delayNOP();
	SDA = 0;
    SCL = 0;  
}
//-------------------------------------------------------------------
// 函数名称： check_ACK
// 函数功能： 主机应答位检查子程序，迫使数据传输过程结束
//-------------------------------------------------------------------
void check_ACK(void)
{ 	    
    SDA = 1;        // 将p1.1设置成输入，必须先向端口写1
	SCL = 1;
	askflag = 0;
	delayNOP();   
	if(SDA == 1)    // 若SDA=1表明非应答，置位非应答标志askflag
    askflag = 1;
   	SCL = 0;
}
//-------------------------------------------------------------------
// 函数名称： IICSendByte
// 入口参数： ch
// 函数功能： 发送一个字节
//-------------------------------------------------------------------
void IICSendByte(uchar ch)
 
{
  	unsigned char idata n=8;     // 向SDA上发送一位数据字节，共八位

	while(n--)
	{ 
	if((ch&0x80) == 0x80)    // 若要发送的数据最高位为1则发送位1
	   {
	 		SDA = 1;    // 传送位1
			SCL = 1;
		    delayNOP();
		//	SDA = 0;	
			SCL = 0; 
	   }
		else
		{  
			SDA = 0;    // 否则传送位0
			SCL = 1;
			delayNOP();
		  	SCL = 0;
		}
		ch = ch<<1;    // 数据左移一位
	}
}
//-------------------------------------------------------------------
// 函数名称： IICreceiveByte
// 返回接收的数据
// 函数功能： 接收一字节子程序
//-------------------------------------------------------------------
uchar IICreceiveByte(void)
{
	uchar idata n=8;     // 从SDA线上读取一上数据字节，共八位
	uchar tdata=0;
	while(n--)
	{
	   SDA = 1;
	   SCL = 1;
	   tdata =tdata<<1;		    //左移一位
	   	if(SDA == 1)
		  tdata = tdata|0x01;   // 若接收到的位为1，则数据的最后一位置1
		else 
		  tdata = tdata&0xfe;   // 否则数据的最后一位置0
	   SCL = 0;
	 }

	 return(tdata);
}
//-------------------------------------------------------------------
// 函数名称： ADC_PCF8591
// 入口参数： controlbyte控制字
// 函数功能： 连续读入4路通道的A/D转换结果到receivebuf
//-------------------------------------------------------------------
void ADC_PCF8591(uchar controlbyte)
{ 
    uchar idata receive_da,i=0;

	iic_start();

	IICSendByte(PCF8591_WRITE);	//控制字
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}

	IICSendByte(controlbyte);	//控制字
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}

    iic_start();                //重新发送开始命令
   	IICSendByte(PCF8591_READ);	//控制字
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}
	 
    IICreceiveByte();   //空读一次，调整读顺序
    slave_ACK();        //收到一个字节后发送一个应答位

	while(i<4)
	{  
	  receive_da=IICreceiveByte();
	  receivebuf[i++]=receive_da;
	  slave_ACK();       //收到一个字节后发送一个应答位
	}
	slave_NOACK();       //收到最后一个字节后发送一个非应答位
	iic_stop();
//	delay(100);
}
 	uchar iden=0;
	 void run()
{
	 iden=0;
		iicInit();	              //I2C总线初始化
   		ADC_PCF8591(0x04);
	

	if(SystemError == 1)	  //有错误，重新来
	  {
	  	iicInit();				  //I2C总线初始化
	    ADC_PCF8591(0x04);
	   }  
}



	/******************************************************************/
/*                                                                */
/* 数据处理与显示                                                 */
/* 将采集到的数据进行16进制转换为ASCLL码。                        */
/*                                                                */
/******************************************************************/
 	long data tem;
	char data n=60000;
show_value(uchar back ,uchar ad_data,char n)

{  	
	while(n--)
	{
		run();
		tem+=receivebuf[n];
	}
	 // tem=0;
	 n=60000;
	 tem/=n;
	if(tem<back+13&&tem>back-13)
	{
	
		 dis[2]=back/26;   //AD值转换为3为BCD码，最大为5.00V。
   // dis[2]=dis[2]+0x30;	 //转换为ACSII码
    	dis[3]=back%18;   //余数暂存
    	dis[3]=dis[3]*26;    //计算小数第一位
    	dis[1]=dis[3]/26;
	}else
	{
		backs[n]=tem;
	
		 dis[2]=ad_data/26;   //AD值转换为3为BCD码，最大为5.00V。
   // dis[2]=dis[2]+0x30;	 //转换为ACSII码
    	dis[3]=ad_data%26;   //余数暂存
    	dis[3]=dis[3]*10;    //计算小数第一位
    	dis[1]=dis[3]/26;

	}
	
	
	  	
 	
	 
  //  dis[2]=ad_data/26;   //AD值转换为3为BCD码，最大为5.00V。
   // dis[2]=dis[2]+0x30;	 //转换为ACSII码
    //dis[3]=ad_data%26;   //余数暂存
   // dis[3]=dis[3]*10;    //计算小数第一位
   // dis[1]=dis[3]/26;
    //dis[1]=dis[1]+0x30;	 //转换为ACSII码
    //dis[3]=dis[3]%100;
   // dis[3]=dis[3]*10;    //计算小数第二位
   // dis[0]=dis[3]/100;                                                                             //
    //dis[0]=dis[0]+0x30;  //转换为ACSII码           
} 

//-------------------------------------------------------------------
// 函数名称： main
// 函数功能： 主程序
//-------------------------------------------------------------------
main()
{
    uchar i,l;
	TMOD=0x01;                //延时
	EA=1;
	ET0=1;
	TR0=1;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;





	
  while(1)
  {		   
  	
	 show_value(backs[0],receivebuf[0],0); //显示通道0       
	    P0=table[dis[2]];//整数位显示
        P2=0xfe;
		delay(1);
        P0=table[dis[1]];        //第一位小数显示 
       	P2=0xfd;
		delay(1);
	//	P0=table[dis[0]];
	//	P2=0xfb;
	//	delay(1);
      show_value(backs[1],receivebuf[1],1); //显示通道1	 
	    P0=table[dis[2]];//整数位显示
        P2=0xf7;
		delay(1);
        P0=table[dis[1]];//第一位小数显示 
        P2=0xef;
	    delay(1);
		//P0=table[dis[0]];//第一位小数显示

	  show_value(backs[2],receivebuf[2],2); //显示通道2         	 
	    P0=table[dis[2]];//整数位显示
        P2=0xbf;
		delay(1);
        P0=table[dis[1]];        //第一位小数显示 
        P2=0x7f;
	    delay(1);

      show_value(backs[3],receivebuf[3],3); //显示通道3		 
	
	
	   

	 // iicInit();	       	      //I2C总线初始化  
     
     // delay(50);                       //延时
	 	  

   
  }
}


void timer0() interrupt 1
{
	  
	if(iden==2)
	{
		  	
	 } 
	  iden++;
	TH0=(65535-50000)/256;
	TL0=(65535-50000)%256;
}

