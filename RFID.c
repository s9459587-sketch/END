#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "SPI.h"
#include "RFID.h"

#define RFID_SS_PIN   2
#define RFID_RST_PIN  7

#define CommandReg    0x01
#define ComIrqReg     0x04
#define ErrorReg      0x06
#define FIFODataReg   0x09
#define FIFOLevelReg  0x0A
#define ControlReg    0x0C
#define BitFramingReg 0x0D
#define ModeReg       0x11
#define TxControlReg  0x14
#define TxASKReg      0x15
#define TModeReg      0x2A
#define TPrescalerReg 0x2B
#define TReloadHReg   0x2C
#define TReloadLReg   0x2D

#define PCD_Idle      0x00
#define PCD_Transceive 0x0C

#define PICC_REQIDL   0x26
#define PICC_ANTICOLL 0x93

#define MI_OK         0
#define MI_ERR        1

static void RFID_Write(u8 reg, u8 data)
{
	CLR_BIT(PORTB, RFID_SS_PIN);

	SPI_Transceive((reg << 1) & 0x7E);
	SPI_Transceive(data);

	SET_BIT(PORTB, RFID_SS_PIN);
}

static u8 RFID_Read(u8 reg)
{
	u8 data;

	CLR_BIT(PORTB, RFID_SS_PIN);

	SPI_Transceive(((reg << 1) & 0x7E) | 0x80);
	data = SPI_Transceive(0x00);

	SET_BIT(PORTB, RFID_SS_PIN);

	return data;
}

static u8 RFID_Request(u8 command, u8 *buffer)
{
	u8 status;

	RFID_Write(BitFramingReg, 0x07);

	RFID_Write(CommandReg, PCD_Idle);

	RFID_Write(FIFOLevelReg, 0x80);

	RFID_Write(FIFODataReg, command);

	RFID_Write(CommandReg, PCD_Transceive);

	status = RFID_Read(ComIrqReg);

	if(status & 0x30)
	return MI_OK;

	return MI_ERR;
}

static u8 RFID_Anticollision(u8 *uid)
{
	u8 i;

	RFID_Write(BitFramingReg, 0x00);
	RFID_Write(FIFOLevelReg, 0x80);

	RFID_Write(FIFODataReg, PICC_ANTICOLL);

	RFID_Write(FIFODataReg, 0x20);

	RFID_Write(CommandReg, PCD_Transceive);

	for(i = 0; i < 5; i++)
	uid[i] = RFID_Read(FIFODataReg);

	return MI_OK;
}

void RFID_Init(void)
{
	SET_BIT(DDRB, RFID_SS_PIN);
	SET_BIT(DDRD, RFID_RST_PIN);

	SET_BIT(PORTB, RFID_SS_PIN);
	SET_BIT(PORTD, RFID_RST_PIN);

	_delay_ms(50);

	RFID_Write(TModeReg, 0x8D);
	RFID_Write(TPrescalerReg, 0x3E);

	RFID_Write(TReloadLReg, 30);
	RFID_Write(TReloadHReg, 0);

	RFID_Write(TxASKReg, 0x40);
	RFID_Write(ModeReg, 0x3D);

	RFID_Write(TxControlReg, 0x03);
}

u8 RFID_IsCardPresent(void)
{
	u8 buffer[2];

	if(RFID_Request(PICC_REQIDL, buffer) == MI_OK)
	return 1;

	return 0;
}

u8 RFID_ReadUID(RFID_Card *card)
{
	u8 uid[5];
	u8 i;

	if(RFID_Anticollision(uid) != MI_OK)
	return 0;

	card->size = 4;

	for(i = 0; i < 4; i++)
	card->uid[i] = uid[i];

	return 1;
}

void RFID_Halt(void)
{
	RFID_Write(CommandReg, PCD_Idle);
}