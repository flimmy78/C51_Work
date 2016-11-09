/**************************************************
��Դ������
��д��bluefeel
ʱ�䣺10-03-13
��λ������������ѧ
/**************************************************/

#include <reg52.h>
#include <api.h>
#include <uart.h>

#define uchar unsigned char

/***************************************************/
#define TX_ADR_WIDTH   5  // 5�ֽڿ��ȵķ���/���յ�ַ
#define TX_PLOAD_WIDTH 4  // ����ͨ����Ч���ݿ���
#define LED P2

uchar code TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};  // ����һ����̬���͵�ַ
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
����: init_io()

����:
    ��ʼ��IO
/**************************************************/
void init_io(void)
{
	CE  = 0;        // ����
	CSN = 1;        // SPI��ֹ
	SCK = 0;        // SPIʱ���õ�
	IRQ = 1;        // �жϸ�λ
	LED = 0xff;		// �ر�ָʾ��
}
/**************************************************/

/**************************************************
������delay_ms()

������
    �ӳ�x����
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
������SPI_RW()

������
    ����SPIЭ�飬дһ�ֽ����ݵ�nRF24L01��ͬʱ��nRF24L01
	����һ�ֽ�
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar i;
   	for(i=0; i<8; i++)          // ѭ��8��
   	{
   		MOSI = (byte & 0x80);   // byte���λ�����MOSI
   		byte <<= 1;             // ��һλ��λ�����λ
   		SCK = 1;                // ����SCK��nRF24L01��MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
   		byte |= MISO;       	// ��MISO��byte���λ
   		SCK = 0;            	// SCK�õ�
   	}
    return(byte);           	// ���ض�����һ�ֽ�
}
/**************************************************/

/**************************************************
������SPI_RW_Reg()

������
    д����value��reg�Ĵ���
/**************************************************/
uchar SPI_RW_Reg(uchar reg, uchar value)
{
	uchar status;
  	CSN = 0;                   // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);      // ѡ��Ĵ�����ͬʱ����״̬��
  	SPI_RW(value);             // Ȼ��д���ݵ��üĴ���
  	CSN = 1;                   // CSN���ߣ��������ݴ���
  	return(status);            // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Read()

������
    ��reg�Ĵ�����һ�ֽ�
/**************************************************/
uchar SPI_Read(uchar reg)
{
	uchar reg_val;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	SPI_RW(reg);                // ѡ��Ĵ���
  	reg_val = SPI_RW(0);        // Ȼ��ӸüĴ���������
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(reg_val);            // ���ؼĴ�������
}
/**************************************************/

/**************************************************
������SPI_Read_Buf()

������
    ��reg�Ĵ�������bytes���ֽڣ�ͨ��������ȡ����ͨ��
	���ݻ����/���͵�ַ
/**************************************************/
uchar SPI_Read_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	pBuf[i] = SPI_RW(0);    // ����ֽڴ�nRF24L01����
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Write_Buf()

������
    ��pBuf�����е�����д�뵽nRF24L01��ͨ������д�뷢
	��ͨ�����ݻ����/���͵�ַ
/**************************************************/
uchar SPI_Write_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	SPI_RW(pBuf[i]);        // ����ֽ�д��nRF24L01
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������RX_Mode()

������
    �����������nRF24L01Ϊ����ģʽ���ȴ����շ����豸�����ݰ�
/**************************************************/
void RX_Mode(void)
{
	CE = 0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);               // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);           // ʹ�ܽ���ͨ��0
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);                 // ѡ����Ƶͨ��0x40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ���
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);            // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);              // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
  	CE = 1;                                            // ����CE���������豸
}
/**************************************************/

/**************************************************
������TX_Mode()

������
    �����������nRF24L01Ϊ����ģʽ����CE=1��������10us����
	130us���������䣬���ݷ��ͽ����󣬷���ģ���Զ�ת�����
	ģʽ�ȴ�Ӧ���źš�
/**************************************************/
void TX_Mode(uchar * BUF)
{
	CE = 0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // д�뷢�͵�ַ
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
  	SPI_Write_Buf(WR_TX_PLOAD, BUF, TX_PLOAD_WIDTH);                  // д���ݰ���TX FIFO
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);   // ʹ�ܽ���ͨ��0
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);         // ѡ����Ƶͨ��0x40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	CE = 1;
}
/**************************************************/

/**************************************************
������Check_ACK()

������
    �������豸���޽��յ����ݰ����趨û���յ�Ӧ����
	���Ƿ��ط�
/**************************************************/
uchar Check_ACK(bit clear)
{
	while(IRQ);
	sta = SPI_RW(NOP);                    // ����״̬�Ĵ���
	if(MAX_RT)
		if(clear){                         // �Ƿ����TX FIFO��û������ڸ�λMAX_RT�жϱ�־���ط�
			SPI_RW(FLUSH_TX);
			send_string("����ط���");
		}
	SPI_RW_Reg(WRITE_REG + STATUS, sta);  // ���TX_DS��MAX_RT�жϱ�־
	IRQ = 1;
	if(TX_DS)
		return(0x00);
	else
		return(0xff);
}
/**************************************************/

/**************************************************
������CheckButtons()

������
    ��鰴���Ƿ��£���������һ�ֽ�����
/**************************************************/
void CheckButtons()
{
	
			TX_BUF[0] = ~DATA;          // �����͵�����
			TX_Mode(TX_BUF);			// ��nRF24L01����Ϊ����ģʽ����������
			LED = ~DATA;		        // �����͵�LED��ʾ
			Check_ACK(1);               // �ȴ�������ϣ����TX FIFO
			delay_ms(250);
			delay_ms(250);
			LED = 0xff;			        // �ر�LED
			RX_Mode();			        // ����Ϊ����ģʽ
			while(!(P3 & 0x01));
			DATA <<= 1;
			if(!DATA)
				DATA = 0x01;
		
}
///////////////////////////////////////////////////////

#define TX_ADR_WIDTH2   5  // 5�ֽڿ��ȵķ���/���յ�ַ
#define TX_PLOAD_WIDTH2 4  // ����ͨ����Ч���ݿ���


uchar code TX_ADDRESS2[TX_ADR_WIDTH2] = {0x34,0x43,0x10,0x10,0x01};  // ����һ����̬���͵�ַ
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
����: init_io2()

����:
    ��ʼ��IO
/**************************************************/
void init_io2(void)
{
	CE2  = 0;        // ����
	CSN2 = 1;        // SPI��ֹ
	SCK2 = 0;        // SPIʱ���õ�
	IRQ2 = 1;        // �жϸ�λ
}
/**************************************************/

/**************************************************
������delay_ms2()

������
    �ӳ�x����
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
������SPI_RW2()

������
    ����SPIЭ�飬дһ�ֽ����ݵ�nRF24L01��ͬʱ��nRF24L01
	����һ�ֽ�
/**************************************************/
uchar SPI_RW2(uchar byte)
{
	uchar i;
   	for(i=0; i<8; i++)          // ѭ��8��
   	{
   		MOSI2 = (byte & 0x80);   // byte���λ�����MOSI2
   		byte <<= 1;             // ��һλ��λ�����λ
   		SCK2 = 1;                // ����SCK2��nRF24L01��MOSI2����1λ���ݣ�ͬʱ��MISO2���1λ����
   		byte |= MISO2;       	// ��MISO2��byte���λ
   		SCK2 = 0;            	// SCK2�õ�
   	}
    return(byte);           	// ���ض�����һ�ֽ�
}
/**************************************************/

/**************************************************
������SPI_RW2_Reg2()

������
    д����value��reg�Ĵ���
/**************************************************/
uchar SPI_RW2_Reg2(uchar reg, uchar value)
{
	uchar status;
  	CSN2 = 0;                   // CSN2�õͣ���ʼ��������
  	status = SPI_RW2(reg);      // ѡ��Ĵ�����ͬʱ����״̬��
  	SPI_RW2(value);             // Ȼ��д���ݵ��üĴ���
  	CSN2 = 1;                   // CSN2���ߣ��������ݴ���
  	return(status);            // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Read2()

������
    ��reg�Ĵ�����һ�ֽ�
/**************************************************/
uchar SPI_Read2(uchar reg)
{
	uchar reg_val;
  	CSN2 = 0;                    // CSN2�õͣ���ʼ��������
  	SPI_RW2(reg);                // ѡ��Ĵ���
  	reg_val = SPI_RW2(0);        // Ȼ��ӸüĴ���������
  	CSN2 = 1;                    // CSN2���ߣ��������ݴ���
  	return(reg_val);            // ���ؼĴ�������
}
/**************************************************/

/**************************************************
������SPI_Read2_Buf()

������
    ��reg�Ĵ�������bytes���ֽڣ�ͨ��������ȡ����ͨ��
	���ݻ����/���͵�ַ
/**************************************************/
uchar SPI_Read2_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN2 = 0;                    // CSN2�õͣ���ʼ��������
  	status = SPI_RW2(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	pBuf[i] = SPI_RW2(0);    // ����ֽڴ�nRF24L01����
  	CSN2 = 1;                    // CSN2���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Write_Buf2()

������
    ��pBuf�����е�����д�뵽nRF24L01��ͨ������д�뷢
	��ͨ�����ݻ����/���͵�ַ
/**************************************************/
uchar SPI_Write_Buf2(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN2 = 0;                    // CSN2�õͣ���ʼ��������
  	status = SPI_RW2(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	SPI_RW2(pBuf[i]);        // ����ֽ�д��nRF24L01
  	CSN2 = 1;                    // CSN2���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������RX_Mode2()

������
    �����������nRF24L01Ϊ����ģʽ���ȴ����շ����豸�����ݰ�
/**************************************************/
void RX_Mode2(void)
{
	CE2 = 0;
  	SPI_Write_Buf2(WRITE_REG + RX_ADDR_P0, TX_ADDRESS2, TX_ADR_WIDTH2);  // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
  	SPI_RW2_Reg2(WRITE_REG + EN_AA, 0x01);               // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	SPI_RW2_Reg2(WRITE_REG + EN_RXADDR, 0x01);           // ʹ�ܽ���ͨ��0
  	SPI_RW2_Reg2(WRITE_REG + RF_CH, 40);                 // ѡ����Ƶͨ��0x40
  	SPI_RW2_Reg2(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH2);  // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ���
  	SPI_RW2_Reg2(WRITE_REG + RF_SETUP, 0x07);            // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW2_Reg2(WRITE_REG + CONFIG, 0x0f);              // CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
  	CE2 = 1;                                            // ����CE2���������豸
}
/**************************************************/

/**************************************************
������TX_Mode2()

������
    �����������nRF24L01Ϊ����ģʽ����CE2=1��������10us����
	130us���������䣬���ݷ��ͽ����󣬷���ģ���Զ�ת�����
	ģʽ�ȴ�Ӧ���źš�
/**************************************************/
void TX_Mode2(uchar * BUF)
{
	CE2 = 0;
  	SPI_Write_Buf2(WRITE_REG + TX_ADDR, TX_ADDRESS2, TX_ADR_WIDTH2);     // д�뷢�͵�ַ
  	SPI_Write_Buf2(WRITE_REG + RX_ADDR_P0, TX_ADDRESS2, TX_ADR_WIDTH2);  // Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
  	SPI_Write_Buf2(WR_TX_PLOAD, BUF, TX_PLOAD_WIDTH2);                  // д���ݰ���TX FIFO
  	SPI_RW2_Reg2(WRITE_REG + EN_AA, 0x01);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	SPI_RW2_Reg2(WRITE_REG + EN_RXADDR, 0x01);   // ʹ�ܽ���ͨ��0
  	SPI_RW2_Reg2(WRITE_REG + SETUP_RETR, 0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
  	SPI_RW2_Reg2(WRITE_REG + RF_CH, 40);         // ѡ����Ƶͨ��0x40
  	SPI_RW2_Reg2(WRITE_REG + RF_SETUP, 0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW2_Reg2(WRITE_REG + CONFIG, 0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	CE2 = 1;
}
/**************************************************/

/**************************************************
������Check_ACK2()

������
    �������豸���޽��յ����ݰ����趨û���յ�Ӧ����
	���Ƿ��ط�
/**************************************************/
uchar Check_ACK2(bit clear)
{
	while(IRQ2);
	sta2 = SPI_RW2(NOP);                    // ����״̬�Ĵ���
	if(MAX_RT2)
		if(clear){                         // �Ƿ����TX FIFO��û������ڸ�λMAX_RT2�жϱ�־���ط�
			SPI_RW2(FLUSH_TX);
			send_string("����ط�2");
			}
	SPI_RW2_Reg2(WRITE_REG + STATUS, sta2);  // ���TX_DS2��MAX_RT2�жϱ�־
	IRQ2 = 1;
	if(TX_DS2)
		return(0x00);
	else
		return(0xff);
}

void CheckButtons2()
{
	
			TX_BUF2[0] = ~DATA;          // �����͵�����
			TX_Mode2(TX_BUF2);			// ��nRF24L01����Ϊ����ģʽ����������
			Check_ACK2(1);               // �ȴ�������ϣ����TX FIFO
			RX_Mode2();			        // ����Ϊ����ģʽ
		
		
}
/**************************************************/

/**************************************************
������main()

������
    ������
/**************************************************/
void main(void)
{
	uart_init();
	init_io2();		              // ��ʼ��IO
	RX_Mode2();		              // ����Ϊ����ģʽ
	init_io();		              // ��ʼ��IO
	RX_Mode();		              // ����Ϊ����ģʽ
	while(1)
	{
		
			CheckButtons();
			send_string("�����");
		
		           // ����ɨ��
		sta2 = SPI_Read(STATUS);	  // ��״̬�Ĵ���
	    if(RX_DR)				  // �ж��Ƿ���ܵ�����
		{
			SPI_Read_Buf(RD_RX_PLOAD, RX_BUF, TX_PLOAD_WIDTH);  // ��RX FIFO��������
			flag = 1;
		}
		SPI_RW_Reg(WRITE_REG + STATUS, sta2);  // ���RX_DS�жϱ�־
		if(flag)		           // �������
		{
			flag = 0;		       // ���־
			LED = RX_BUF[0];	   // �����͵�LED��ʾ
		//	send_char(LED);
			delay_ms(250);
			delay_ms(250);
  			LED = 0xff;		       // �ر�LED
		}

		CheckButtons2();
		////////////////////////////////////////////
		sta2 = SPI_Read2(STATUS);	  // ��״̬�Ĵ���
	    if(RX_DR2)				  // �ж��Ƿ���ܵ�����
		{
			SPI_Read2_Buf(RD_RX_PLOAD, RX_BUF2, TX_PLOAD_WIDTH2);  // ��RX FIFO��������
			flag2 = 1;
		}
		SPI_RW2_Reg2(WRITE_REG + STATUS, sta2);  // ���RX_DS�жϱ�־
		if(flag2)		           // �������
		{
			flag2 = 0;		       // ���־
			//LED = RX_BUF2[0];	   // �����͵�LED��ʾ
			send_char(RX_BUF2[0]);
			delay_ms2(250);
			delay_ms2(250);
  		//	LED = 0xff;		       // �ر�LED
		}
	}
}
/**************************************************/