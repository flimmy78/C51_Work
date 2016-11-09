/************************************************
					CAUC
					yeyongan
*************************************************/
#include <reg52.h>
#include <intrins.h>
#define uchar	unsigned char
sbit CS=P1^0;				   
sbit Clk=P1^1;				   //ʱ��
sbit DO=P1^2;				   //ADC0832�������
sbit DI=P1^3;				   //ADC0832��������
sbit key=P3^3;				   //����
bit keydownflg;				   //����λ�Ķ���
bit adc_flg;
uchar dat,channel;			   
uchar key_buffer;				
uchar P2_buffer;				
uchar Beep_cnt;
uchar disp_cnt;
uchar count4ms;
uchar disp_buff[5]; 			 //�������ʾ����
uchar code Tab1[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E};	//���������
uchar code Tab[9]={0x00,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};	   //�����λѡ��

uchar A_D(uchar CH)			   //AD����
{
	uchar i,adval,test;		   //����ֲ���������ʼ��
	adval=0x00;
	test=0x00;
	Clk=0;					   //clk�͵�ƽ
	DI=1;					   //DI��ʼ�ߵ�ƽ�ڵ�һ��ʱ��������½���ǰ���ָߵ�ƽ����ʾ�����ź�
	_nop_();				   
	CS=0;					   //Ƭѡ
	_nop_();
	Clk=1;					   //clk�����أ���ʼλд��
	_nop_();
	if(CH==0x00)			   //ѡ��ͨ��0
	{
		Clk=0;					//clk�͵�ƽ
		 DI=1;					
		_nop_();
		Clk=1;					//clk�����أ�ͨ��0�ĵ�һλд��
		_nop_();
		Clk=0;
		DI=0;
		_nop_();
		Clk=1;					//clk�����أ�ͨ��0�ĵڶ�λд��
		_nop_();
	}
	else
	{
		Clk=0;
		DI=1;
		_nop_();
		Clk=1;				   //clk�����أ�ͨ��1�ĵ�һλд��
		_nop_();
		Clk=0;
		DI=1;
		_nop_();			   //clk�����أ�ͨ��1�ĵڶ�λд��
		Clk=1;
		_nop_();
	}
	Clk=0;
	DI=1;
	for(i=0;i<8;i++)			 //�Ӹ�λ���λ��ȡ��λADֵ
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
		
		test>>=1;				   //�ӵ�λ���λ��ȡ��λADֵ
		
		if(DO)
			test|=0x80;	
		else			
			test|=0x00;
			_nop_();
			Clk=1;
			_nop_();
			Clk=0;
	}
	if(adval==test)	dat=test;		  	//�ж�������ȡֵ�Ƿ������ȾͰѶ�ȡ������ֵ��DAT
	_nop_();
	CS=1;
	DO=1;
	Clk=1;
	return dat;
}				
void FillDispBuffer(void)						//�������ʾ���溯��
{
	disp_buff[0]=channel;						//��ʾͨ��
	disp_buff[1]=12;							//��ʾ"C"
	disp_buff[2]=dat/51;						//��ʾ��λ
	disp_buff[3]=dat%51*10/51;					//��ʾʮ��λ
	disp_buff[4]=((dat%51)*10%51)*10/51; 		//��ʾ�ٷ�λ
	
}
void dealkey(void)						   		//��������
{
	if(keydownflg)	return;				   		//keydownflg����λΪ1�����԰������д���
	key_buffer=P2;
	if((key_buffer&0x80)!=0x80)	channel=0;		//ѡ��ͨ��0
	if((key_buffer&0x40)!=0x40)	channel=1;		//ѡ��ͨ��1
	FillDispBuffer();							//�������ʾ����
	Beep_cnt=0;
	keydownflg=1;								//keydownflg����λ��1
}
void main(void)			   //������
{
	P0=0xff;			   //��ʼ��
	P2=0xff;
	dat=0x00;
	disp_cnt=0;
	count4ms=0;
	channel=0;
	TMOD=0x10;
	TH0=(65535-4000)/256;
	TL0=(65535-4000)%256;
	EA=1;
	TR0=1;
	ET0=1;
	while(1)
	{
		
		if(adc_flg)			 //ADCת������λ����ֹ���������������ͻ
		{
			adc_flg=0;
			A_D(channel);	 //ADC����
			FillDispBuffer();	//�������ʾ����
		}
		if(!key)				//����
			dealkey();
	}
}
void T0_service(void) interrupt 1		//��ʱ��0�ж��Ӻ���
{
	TH0=(65535-4000)/256;
	TL0=(65535-4000)%256;
	P2_buffer=Tab[disp_cnt];			 //�������ܵ�λѡ��

	P2=P2_buffer;						  //�������ʾ���ַ���
	if(disp_cnt==2)						  //����λ�������ʾС����
		P0=Tab1[disp_buff[disp_cnt]]&0x7f;
	else
		P0=Tab1[disp_buff[disp_cnt]];
	disp_cnt++;							   //
	if(disp_cnt==5)	disp_cnt=0;
	count4ms++;
	if(count4ms==50)					 //0.2s ADCת��һ��
	{
		adc_flg=1;
		count4ms=0;
	}
}
