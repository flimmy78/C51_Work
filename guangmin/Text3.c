#include <reg51.h>
#include <intrins.h>
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//���干���������ʾ������ 

#define	OP_READ	0x90		// ������ַ�Լ���ȡ����,0xa1��Ϊ1010 0001B
#define	OP_WRITE 0x91		// ������ַ�Լ�д�����,0xa1��Ϊ1010 0000B

sbit  Sclock = P3^2;
sbit  Sdata = P3^3;


void WriteCurrent(unsigned char y)
{
	unsigned char i;
	for(i = 0; i < 8; i++)		// ѭ������8��λ
	{
    	Sdata = (bit)(y&0x80);   //ͨ����λ���롱���㽫���λ�����͵�S
		                                  //��Ϊ����ʱ��λ��ǰ����λ�ں�
		_nop_();            //�ȴ�һ����������   
	   Sclock = 1;            //��SCL�������ؽ�����д��AT24Cxx      
   	_nop_();            //�ȴ�һ���������� 
	  _nop_();             //�ȴ�һ����������       
		
	  	Sclock = 0;            //��SCL������Ϊ�͵�ƽ������SC�����γɴ�����������ģ�������
		y <<= 1;           //��y�еĸ�����λ������һλ
               
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
	 Sclock = 0;    //SCLΪ�͵�ƽʱ��SDA�����ݲ�����仯(�������Ժ�����ݴ��ݣ�
}

void control()
{
	
}


void stop()// ֹͣλ
{
	Sdata = 0;     //SDA��ʼ��Ϊ�͵�ƽ��0��	_n
	Sclock = 1;     //�������ݴ���ʱ��Ҫ��SCLΪ�ߵ�ƽ��1��
	sleep();
	Sdata = 1;    //SDA�������ر���Ϊ�ǽ����ź�
	sleep();
	Sdata=0;
	Sclock=0;
}


bit Ask()                  
{
      bit ack_bit;        //����Ӧ��λ 
        Sdata = 1;	 // �����豸��������Ӧ��ʱ������ĸߵ�ƽ�ڼ�(SCL=1)�ͷ�SDA�ߣ�
	                //����SDA��ת�ɽ����豸(AT24Cxx)����
	_nop_();        //�ȴ�һ���������� 
	_nop_();        //�ȴ�һ���������� 
	Sclock = 1;       //���������涨��SCLӦΪ�ߵ�ƽ
	sleep();
	ack_bit = Sdata; //�����豸��AT24Cxx)��SDA�͵͵�ƽ����ʾ�Ѿ����յ�һ���ֽ�
	               //���͸ߵ�ƽ����ʾû�н��յ��������쳣 ��������
	Sclock = 0;       //SCLΪ�͵�ƽʱ��SDA�����ݲ�����仯(�������Ժ�����ݴ��ݣ�
	return  ack_bit;			// ����AT24CxxӦ��λ



}

unsigned char ReadData()
// ��AT24Cxx�������ݵ�MCU
{
	unsigned char i;
	unsigned char x;   //�����AT24Cxx�ж���������
	for(i = 0; i < 8; i++)
	{
		Sclock = 1;                //SCL��Ϊ�ߵ�ƽ
		x<<=1;                  //��x�еĸ�����λ������һλ
		x|=(unsigned char)Sdata;  //��SDA�ϵ�����ͨ����λ�����������x��
		Sclock = 0;                        //��SCL���½��ض�������
	}
	return(x);                //����ȡ�����ݷ���
}

  unsigned char ReadCurrent()
{
	unsigned char x;
	start();               //��ʼ���ݴ���
	WriteCurrent(OP_READ);   //ѡ��Ҫ������AT24CxxоƬ������֪Ҫ��������
	Ask();
	x=ReadData();         //����ȡ�����ݴ���x
	stop();                //ֹͣ���ݴ���
	return x;              //���ض�ȡ������
}
