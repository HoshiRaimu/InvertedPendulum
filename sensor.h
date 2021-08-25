/* 
 * File:   sensor.h
 * Author: raimu
 *
 * Created on 2021/08/25, 22:30
 */

#include <stdint.h>

#ifndef SENSOR_H
#define	SENSOR_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SENSOR_H */

uint8_t spiExchange(uint8_t);
void spiSendData(uint8_t, uint8_t);
uint8_t spiReceiveData(uint8_t);
void spiContReceiveData(uint8_t *, uint8_t, uint8_t);

