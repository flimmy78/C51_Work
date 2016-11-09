
//-----------------------������������������------------------------
#include <reg52.h>
#include <intrins.h>
sbit SCL=P1^2;                   // ��p1.2��ģ��ʱ�ӿ�
sbit SDA=P1^3;                   // ��p1.3��ģ�����ݿ�

bit askflag;
#define uchar unsigned char
#define uint  unsigned int
#define disdata  P0              //��ʾ�����������
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//���干���������ʾ������
uchar code Tab[]={0x00,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};	   //�����λѡ��


uint data dis[4]={0x00,0x00,0x00,0x00}; 
                          //����3����ʾ���ݵ�Ԫ��1�����ݴ洢��Ԫ
uint data dat[4]={0x00,0x00,0x00,0x00}; 


#define delayNOP(); {_nop_();_nop_();_nop_();_nop_();};
	                 
bit   bdata SystemError;                //�ӻ������־λ

/*********************** PCF8591ר�ñ������� ***********************/

#define	PCF8591_WRITE	0x90 
#define	PCF8591_READ 	0x91
#define  NUM  4 	            //���պͷ��ͻ����������
uchar idata receivebuf[4]={0x00,0x00,0x00,0x00};    //���ݽ��ջ�����
uchar idata backs[4]={0x00,0x00,0x00,0x00}; 


/*******************************************************************/
/*                                                                 */
/*  ��ʱ����                                                       */
/*                                                                 */
/*******************************************************************/

void delay(uint ms)
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
// �������ƣ� iic_start()
// �������ܣ� ����I2C�����ӳ���
//-------------------------------------------------------------------
void iic_start(void)
{ //ʱ�ӱ��ָߣ������ߴӸߵ���һ�����䣬I2Cͨ�ſ�ʼ
	SDA = 1;     
	SCL = 1;
	delayNOP();      // ��ʱ5us 
   	SDA = 0;
	delayNOP();
    SCL = 0;
}
//-------------------------------------------------------------------
// �������ƣ� iic_stop()
// �������ܣ� ֹͣI2C�������ݴ����ӳ���
//-------------------------------------------------------------------
void iic_stop(void)
{  	
	SDA = 0;   	   //ʱ�ӱ��ָߣ������ߴӵ͵���һ�����䣬I2Cͨ��ֹͣ
	SCL = 1;
	delayNOP();
	SDA = 1;
	delayNOP();
    SCL = 0;
}
//------------------------------------------------------------------
// �������ƣ� iicInit_()
// �������ܣ� ��ʼ��I2C�����ӳ���
//------------------------------------------------------------------
 void iicInit(void)
   {
   SCL = 0;
   iic_stop();	
   }  
//-------------------------------------------------------------------
// �������ƣ� slave_ACK
// �������ܣ� �ӻ�����Ӧ��λ�ӳ���
//-------------------------------------------------------------------
void slave_ACK(void)
{
	SDA = 0;  
	SCL = 1;
	delayNOP();	
	SCL = 0;
}
//-------------------------------------------------------------------
// �������ƣ� slave_NOACK
// �������ܣ� �ӻ����ͷ�Ӧ��λ�ӳ�����ʹ���ݴ�����̽���
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
// �������ƣ� check_ACK
// �������ܣ� ����Ӧ��λ����ӳ�����ʹ���ݴ�����̽���
//-------------------------------------------------------------------
void check_ACK(void)
{ 	    
    SDA = 1;        // ��p1.1���ó����룬��������˿�д1
	SCL = 1;
	askflag = 0;
	delayNOP();   
	if(SDA == 1)    // ��SDA=1������Ӧ����λ��Ӧ���־askflag
    askflag = 1;
   	SCL = 0;
}
//-------------------------------------------------------------------
// �������ƣ� IICSendByte
// ��ڲ����� ch
// �������ܣ� ����һ���ֽ�
//-------------------------------------------------------------------
void IICSendByte(uchar ch)
 
{
  	unsigned char idata n=8;     // ��SDA�Ϸ���һλ�����ֽڣ�����λ

	while(n--)
	{ 
	if((ch&0x80) == 0x80)    // ��Ҫ���͵��������λΪ1����λ1
	   {
	 		SDA = 1;    // ����λ1
			SCL = 1;
		    delayNOP();
		//	SDA = 0;	
			SCL = 0; 
	   }
		else
		{  
			SDA = 0;    // ������λ0
			SCL = 1;
			delayNOP();
		  	SCL = 0;
		}
		ch = ch<<1;    // ��������һλ
	}
}
//-------------------------------------------------------------------
// �������ƣ� IICreceiveByte
// ���ؽ��յ�����
// �������ܣ� ����һ�ֽ��ӳ���
//-------------------------------------------------------------------
uchar IICreceiveByte(void)
{
	uchar idata n=8;     // ��SDA���϶�ȡһ�������ֽڣ�����λ
	uchar tdata=0;
	while(n--)
	{
	   SDA = 1;
	   SCL = 1;
	   tdata =tdata<<1;		    //����һλ
	   	if(SDA == 1)
		  tdata = tdata|0x01;   // �����յ���λΪ1�������ݵ����һλ��1
		else 
		  tdata = tdata&0xfe;   // �������ݵ����һλ��0
	   SCL = 0;
	 }

	 return(tdata);
}
//-------------------------------------------------------------------
// �������ƣ� ADC_PCF8591
// ��ڲ����� controlbyte������
// �������ܣ� ��������4·ͨ����A/Dת�������receivebuf
//-------------------------------------------------------------------
void ADC_PCF8591(uchar controlbyte)
{ 
    uchar idata receive_da,i=0;

	iic_start();

	IICSendByte(PCF8591_WRITE);	//������
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}

	IICSendByte(controlbyte);	//������
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}

    iic_start();                //���·��Ϳ�ʼ����
   	IICSendByte(PCF8591_READ);	//������
	check_ACK();
	if(askflag == 1)
	{
		SystemError = 1;
		return;
	}
	 
    IICreceiveByte();   //�ն�һ�Σ�������˳��
    slave_ACK();        //�յ�һ���ֽں���һ��Ӧ��λ

	while(i<4)
	{  
	  receive_da=IICreceiveByte();
	  receivebuf[i++]=receive_da;
	  slave_ACK();       //�յ�һ���ֽں���һ��Ӧ��λ
	}
	slave_NOACK();       //�յ����һ���ֽں���һ����Ӧ��λ
	iic_stop();
//	delay(100);
}
 	uchar iden=0;



	 void run()
{
	 iden=0;
		iicInit();	              //I2C���߳�ʼ��
   		ADC_PCF8591(0x04);
	

	if(SystemError == 1)	  //�д���������
	  {
	  	iicInit();				  //I2C���߳�ʼ��
	    ADC_PCF8591(0x04);
	   }  
}



	/******************************************************************/
/*                                                                */
/* ���ݴ�������ʾ                                                 */
/* ���ɼ��������ݽ���16����ת��ΪASCLL�롣                        */
/*                                                                */
/******************************************************************/
 

	#define N 12
	char A=5;

void conver(uchar val)
{
	//  dis[2]=val/26;   //ADֵת��Ϊ3ΪBCD�룬���Ϊ5.00V��
   
    // dis[3]=val%26;   //�����ݴ�
    // dis[3]=dis[3]*10;    //����С����һλ
    // dis[1]=dis[3]/26;
   
    // dis[3]=dis[3]%26;
    // dis[3]=dis[3]*10;    //����С���ڶ�λ
    // dis[0]=dis[3]/26;    
	  dis[2]=val/100;
	 
	  dis[1]=val/10%10;
	  dis[0]=val%10;                                                                        //
         
}
char show_value(char n)

{  
 /*  uchar val;
   uchar count,i,j;
   uchar value_buf[N];
   int  sum=0;
   for  (count=0;count<N;count++)
   {
   		run();
      value_buf[count] = receivebuf[n];
      delay(1);
   }
   for (j=0;j<N-1;j++)
   {
      for (i=0;i<N-j;i++)
      {
         if ( value_buf[i]>value_buf[i+1] )
         {
		    uchar temp ;
            temp = value_buf[i];
            value_buf[i] = value_buf[i+1];
             value_buf[i+1] = temp;
         }
      }
   }
   for(count=1;count<N-1;count++)
   {
      sum += value_buf[count];
	  }
   val = (uchar)(sum/(N-2));

	 if(n==0)
	 {
	 	A=3;
	 }else if(n==1)
	 {
	 	A=5;
	 }else if(A==2)
	 {
	 	A=5;
	 }
   	if(backs[n]+A>val&&backs[n]-A<val)
	{
		val=backs[n];
		
	}else
	{
		   backs[n] = val;
	}	  */
	run();
    return receivebuf[n];	
	//return val;	
	
	
	
	 
  
} 

//-------------------------------------------------------------------
// �������ƣ� main
// �������ܣ� ������
//-------------------------------------------------------------------
main()
{
   
	TMOD=0x01;                //��ʱ
	EA=1;
	ET0=1;
	TR0=1;
	TH0=(65535-10000)/256;
	TL0=(65535-10000)%256;
	
  while(1)
  {
  	    conver(dat[0]);       
	    P0=table[dis[2]];//����λ��ʾ
        P2=Tab[1];
		delay(1);
        P0=table[dis[1]];        //��һλС����ʾ 
       	P2=Tab[2];
		delay(1);
		P0=table[dis[0]];        //��һλС����ʾ 
       	P2=Tab[3];
		delay(1);
	
      
	  conver(dat[1]); 
	    P0=table[dis[2]];//����λ��ʾ
        P2=Tab[5];
		delay(1);
        P0=table[dis[1]];        //��һλС����ʾ 
       	P2=Tab[6];
		delay(1);
		P0=table[dis[0]];        //��һλС����ʾ 
       	P2=Tab[7];
		delay(1);
	 /* conver(dat[2]);          	 
	    P0=table[dis[2]];//����λ��ʾ
        P2=0xbf;
		delay(1);
        P0=table[dis[1]];        //��һλС����ʾ 
        P2=0x7f;
	    delay(1);
		 */
	 // iicInit();	       	      //I2C���߳�ʼ��      
     // delay(50);                       //��ʱ
  }
}


void timer0() interrupt 1
{
	  
	if(iden==40)
{
		  dat[0]=show_value(0); //��ʾͨ��0
		  dat[1]=show_value(1); //��ʾͨ��1
		  dat[2]=show_value(2); //��ʾͨ��2
		  dat[3]=show_value(3); //��ʾͨ��3	
iden=0;	 	
} 
iden++;
	TH0=(65535-10000)/256;
	TL0=(65535-10000)%256;
}

