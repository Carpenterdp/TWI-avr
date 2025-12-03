#include "TWI.h"

void TWI::begin(void)
{
    /* Select I2C pins PA1/PA2 */
   // PORTA_DIR = PIN1_bm |PIN2_bm;

    /* Master Baud Rate Control */
    TWI0.MBAUD = TWI_BAUD;

    /* Enable TWI */
    TWI0.MCTRLA = TWI_ENABLE_bm;

    /* Initialize the address register */
    TWI0.MADDR = 0x00;

    /* Initialize the data register */
    TWI0.MDATA = 0x00;

    /* Set bus state idle */
    TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

static TWI_Status TWI_GetStatus(void)
{
    TWI_Status state = TWI_INIT;
    do
    {
        if (TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))
        {
            state = TWI_READY;
        }
        else if (TWI0.MSTATUS & (TWI_BUSERR_bm | TWI_ARBLOST_bm))
        {
            /* get here only in case of bus error or arbitration lost */
            state = TWI_ERROR;
        }
    } while (!state);

    return state;
}

/* Returns 1 if the slave acknowleged the receipt and 0 if not */
static uint8_t RX_acked(void)
{
    // Actual status of the line O means ACK - 1 means NACK. Therefore ! MSTATUS bit
    return (!(TWI0.MSTATUS & TWI_RXACK_bm));
}

/* Returns 1 if address is ACKed */
uint8_t TWI::beginTransmission(uint8_t address, TWI_Direction dir)
{
    /* transmit the slave address */
    TWI0.MADDR = (address << 1) | dir;
    return ((TWI_GetStatus() == TWI_READY) && RX_acked());
}

/* Returns 1 if address is ACKed */
uint8_t TWI::sendByte(uint8_t data)
{
    TWI0.MDATA = data;
    return RX_acked();
}

/* Returns how many bytes have been sent, -1 means bus error */
uint8_t TWI::sendN(uint8_t *pData, uint8_t len)
{
    uint8_t retVal = 0;

    if ((len != 0) && (pData != 0))
    {
        while (len--)
        {
            TWI0.MDATA = *pData;
            if ((TWI_GetStatus() == TWI_READY) && RX_acked())
            {
                retVal++;
                pData++;
                continue;
            }
            else // did not get ACK after slave address
            {
                break;
            }
        }
    }

    return retVal;
}

/* Returns how many bytes have been sent, -1 means bus error*/
uint8_t TWI::sendData(uint8_t address, uint8_t *pData, uint8_t len)
{

    if (!beginTransmission(address, TWI_WRITE))
        return ((uint8_t)-1);

    return sendN(pData, len);
}

/* Returns how many bytes have been read */
uint8_t TWI::readByte(uint8_t *pData)
{
    if (TWI_GetStatus() == TWI_READY)
    {
        *pData = TWI0.MDATA;
        return 1;
    }
    else
        return 0;
}

/* Returns how many bytes have been read */
uint8_t TWI::readN(uint8_t *pData, uint8_t len)
{
    uint8_t retVal = 0;

    if ((len != 0) && (pData != 0)) // if pointer is initialized
    {
        while (--len)
        {
            if (TWI_GetStatus() == TWI_READY)
            {
                *pData = TWI0.MDATA;
                TWI0.MCTRLB = (len == 0) ? TWI_ACKACT_bm | TWI_MCMD_STOP_gc : TWI_MCMD_RECVTRANS_gc;
                retVal++;
                pData++;
                continue;
            }
            else
                break;
        }
    }

    return retVal;
}

/* Returns how many bytes have been received, -1 means NACK at address */
uint8_t TWI::getData(uint8_t address, uint8_t *pData, uint8_t len)
{
    uint8_t retVal = (uint8_t)-1;

    if (!beginTransmission(address, TWI_READ))
        return retVal;

    retVal = 0;
    if ((len != 0) && (pData != 0))
    {
        while (--len)
        {
            if (TWI_GetStatus() == TWI_READY)
            {
                *pData = TWI0.MDATA;
                TWI0.MCTRLB = (len == 0) ? TWI_ACKACT_bm | TWI_MCMD_STOP_gc : TWI_MCMD_RECVTRANS_gc;
                retVal++;
                pData++;
                continue;
            }
            else
                break;
        }
    }

    return retVal;
}

void TWI::endSession(void)
{
    TWI0.MCTRLB = TWI_MCMD_STOP_gc;
}
