#include "sensor.h"
#include <stdint.h>

uint8_t spiExchange(uint8_t data)
{
    uint8_t dumy;
    
    dumy    = SSP1BUF;          //ダミー読み出し
    SSP1BUF = data;             //データをセットし、送信完了待ち
    while(!SSP1STATbits.BF);
    
    return SSP1BUF;
}

void spiSendData(uint8_t addr, uint8_t data)
{
    spiExchange(addr);
    spiExchange(data);
}

uint8_t spiReceiveData(uint8_t addr)
{ 
    addr |= 0x80;
    spiExchange(addr);
    return spiExchange(0);
}

void spiContReceiveData(uint8_t *data, uint8_t addr, uint8_t count)
{
    addr |= 0x80;
    addr |= 0x40;
    spiExchange(addr);
    
    for(uint8_t i = 0; i < count; i++)
    {
        data[i] = spiExchange(0);
    }
}
