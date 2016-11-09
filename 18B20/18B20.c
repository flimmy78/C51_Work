#include <reg52.h>        //  ����51��Ƭ���Ĵ��������ͷ�ļ�
#include <intrins.h>      //����_nop_()���������ͷ�ļ�
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//���干���������ʾ������ 
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

 for(j=0;j<57;j++)	  //500us��׼��ʱ���� 
 {
  ;
 } 
}

void beep()	//����1KHZƵ�������ĺ���
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
 unsigned char j,k,l;     //j,k,l�ֱ𴢴��¶ȵİ�λ��ʮλ�͸�λ
 		j=x/100;
		k=(x%100)/10;
		l=x%10; 
		if(k>=3&&l>=1)
		jdq=0;
		else
		jdq=1;

 	while(i--){
		              //ȡ��λ
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
		      dats|=0x80;  //���������������1����1����dat
			else
				dats|=0x00;//���������������0����0����dat
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
	unsigned char TL;     //�����ݴ������¶ȵ�λ
	  unsigned char TH;    //�����ݴ������¶ȸ�λ
      unsigned char TN;      //�����¶ȵ���������
	  unsigned char TD;       //�����¶ȵ�С������
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
		 

		  TL=read();    //�ȶ������¶�ֵ��λ
		TH=read();    //���Ŷ������¶�ֵ��λ
		P0=table[0];
		P2=TL;  
		
		if((TH&0xf8)!=0x00)//�жϸ���λ �õ��¶�������־
		{
			flag=1;
			TL=~TL;	 //ȡ��
			TH=~TH;	  //ȡ��
			tltemp=TL+1; //��λ��1
			TL=tltemp;
			if(tltemp>255) TH++;  //�����8λ����255�����8λ��1
		   TN=TH*16+TL/16;      //ʵ���¶�ֵ=(TH*256+TL)/16,����TH*16+TL/16
									  //�����ó������¶ȵ���������,С�����ֱ�������
		   TD=(TL%16)*10/16;    //�����¶ȵ�С������,����������10�ٳ���16ȡ����
		}
		TN=TH*16+TL/16;      //ʵ���¶�ֵ=(TH*256+TL)/16,����TH*16+TL/16
				                  //�����ó������¶ȵ���������,С�����ֱ�������
	   TD=(TL%16)*10/16;    //�����¶ȵ�С������,����������10�ٳ���16ȡ����
			                  //�����õ������¶�С�����ֵĵ�һλ����(����1λС��)
	   display_temp1(TN);    //��ʾ�¶ȵ���������
	 //  display_temp2(TD);    //��ʾ�¶ȵ�С������
      delaynms(10);               
    }
		// P0=table[0];
		// P2=hei;  
	 
	  


}
