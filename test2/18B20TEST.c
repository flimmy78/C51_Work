#include <reg52.h>        //  ����51��Ƭ���Ĵ��������ͷ�ļ�
#include <intrins.h>      //����_nop_()���������ͷ�ļ�
unsigned char code table[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//���干���������ʾ������ 
#define uchar unsigned char
#define uint  unsigned int

main()
{
	while(1)
	{
		P1=0x0a;
	}
}