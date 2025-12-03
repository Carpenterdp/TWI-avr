#ifndef TWI_H
#define TWI_H 

#define F_CPU 16000000L

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#define TWI_SCL_FREQ 100000 

#define TWI_BAUD ((((float) F_CPU / (float) TWI_SCL_FREQ) - 10)/2)

typedef enum {
    TWI_INIT = 0, 
    TWI_READY,
    TWI_ERROR
} TWI_Status;

typedef enum{
    TWI_WRITE = 0,
    TWI_READ = 1
} TWI_Direction;

class TWI {
 
public:
void    begin(void);
uint8_t beginTransmission(uint8_t address, TWI_Direction dir);
uint8_t sendByte(uint8_t data); 
uint8_t sendN(uint8_t *pData, uint8_t len); 
uint8_t readByte(uint8_t *data); 
uint8_t readN(uint8_t *pData, uint8_t len); 
uint8_t sendData(uint8_t address, uint8_t *pData, uint8_t len);
uint8_t getData(uint8_t address, uint8_t *pData, uint8_t len); 
void    endSession(void);

};

#endif
