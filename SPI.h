#ifndef SPI_H_
#define SPI_H_

#include "STD_TYPES.h"

void SPI_Init(void);
u8 SPI_Transceive(u8 data);

#endif