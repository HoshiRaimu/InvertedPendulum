/*
 * File:   main.c
 * Author: raimu
 *
 * Created on 2021/08/04, 22:31
 */

// CONFIG1
#pragma config FEXTOSC = OFF    // External Oscillator mode selection bits->Oscillator not enabled
#pragma config RSTOSC = HFINT1    // Power-up default value for COSC bits->HFINTOSC (1MHz)
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; i/o or oscillator function on OSC2
#pragma config CSWEN = ON    // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable bit->FSCM timer enabled

// CONFIG2
#pragma config MCLRE = OFF    // Master Clear Enable bit->MCLR pin function is port defined function
#pragma config PWRTE = ON    // Power-up Timer Enable bit->PWRT enabled
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON    // Brown-out reset enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices
#pragma config ZCD = OFF    // Zero-cross detect disable->Zero-cross detect circuit is disabled at POR.
#pragma config PPS1WAY = ON    // Peripheral Pin Select one-way control->The PPSLOCK bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset
#pragma config DEBUG = OFF    // Background Debugger->Background Debugger disabled

// CONFIG3
#pragma config WDTCPS = WDTCPS_31    // WDT Period Select bits->Divider ratio 1:65536; software control of WDTPS
#pragma config WDTE = OFF    // WDT operating mode->WDT Disabled, SWDTEN is ignored
#pragma config WDTCWS = WDTCWS_7    // WDT Window Select bits->window always open (100%); software control; keyed access not required
#pragma config WDTCCS = SC    // WDT input clock selector->Software Control

// CONFIG4
#pragma config WRT = OFF    // UserNVM self-write protection bits->Write protection off
#pragma config SCANE = available    // Scanner Enable bit->Scanner module is available for use
#pragma config LVP = OFF    // Low Voltage Programming Enable bit->High Voltage on MCLR/Vpp must be used for programming

// CONFIG5
#pragma config CP = OFF    // UserNVM Program memory code protection bit->Program Memory code protection disabled
#pragma config CPD = OFF    // DataNVM code protection bit->Data EEPROM code protection disabled

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "AQM0802.h"

#define _XTAL_FREQ 32000000


#define A1PHASE LATB0
#define A1ENBL  LATB1
#define B1PHASE LATB2
#define B1ENBL  LATB3

#define A2PHASE LATB4
#define A2ENBL  LATB5
#define B2PHASE LATB6
#define B2ENBL  LATB7


void init(void);


void forward1(int t)
{
  static uint8_t cnt = 0;
  uint8_t order[4] = {12, 6, 3, 9};

  A1PHASE = ((order[cnt] & 0x08) >> 3);
  B1PHASE = ((order[cnt] & 0x04) >> 2);
  A1ENBL  = ((order[cnt] & 0x02) >> 1);
  B1ENBL  = (order[cnt] & 0x01);
  __delay_us(3000);

  cnt++;
  if(cnt > 3) cnt = 0;
}

void main(void) {
    init();
    lcdInitialize();
    __delay_ms(100);
    while(1)
    {
        lcdLocateCursor(1, 1);
        ADGO = 1;
        while(ADGO);
        int data = ADRES;
        __delay_ms(1);
        printf("%4d\n",data);
    }
    return;
}

void init()
{
    //動作周波数設定
    OSCCON1bits.NOSC = 0b110;       //内部クロック使用
    OSCCON1bits.NDIV = 0b0000;      //分周1:1
    OSCFRQbits.HFFRQ = 0b110;       //32MHz
    
    //ピンの設定
    ANSELA = 0b00000000;        //
    ANSELB = 0b00000000;        //
    ANSELC = 0b10000000;        //
    TRISA  = 0b00000000;        //
    TRISB  = 0b00000011;        //RB0,RB1を入力に設定
    TRISC  = 0b10010000;        //RC5を入力に設定
    
    //SPIの設定
    SSP1CON1 = 0b00110000;          //SPIモードにし、Fosc / 4のクロック周波数を設定
    SSP1STAT = 0b00000000;          //受信サンプルを中央、立ち上がりエッジで送信、BFを設定
    
    //SSP1IF= 0 ;                      //SPIの割込みフラグを初期化
    //SSP1IE= 1 ;                      //SPIの割込みを許可
    //PEIE = 1 ;                      //周辺機器の割込みを許可
    //GIE  = 1 ;                      //全体の割り込みを許可
    
    //I2Cの設定
    SSP2STAT = 0x80;   //クロック信号は100kHzを使用
    SSP2CON1 = 0x28;   //I2C通信のマスターモードを有効化
    SSP2CON3 = 0x00;   //CON3はデフォルト設定
    SSP2ADD  = 0x09;   //クロック信号速度を100kHzに設定
    
    //ADCの設定
    ADCON0 = 0b10000100;
    ADCLKbits.ADCCS = 0x111111;
    ADPCH = 0b010111;   //RC7を選択
    ADREFbits.ADPREF = 0x00;
    
    
    //解除
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00;
    
    //SPI
    //SCL:RC6, SDI:RC4, SDO:RC5
    SSP1CLKPPS = 0x16;
    RC6PPS = 0x14;
    SSP1DATPPS = 0x14;
    RC5PPS = 0x15;
    
    //I2C
    //SCL:RB1, SDI:RB0
    SSP2DATPPS = 0x08;
    RB0PPS = 0x17;
    SSP2CLKPPS = 0x09;
    RB1PPS = 0x16;
    
    //UART
    RXPPS  = 0x10;              //RC0をRXに設定
    RC1PPS = 0x10;              //RC1をTXに設定
    
    //ロック
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0x00;
    
    //EUSARTの設定
    TX1STA   = 0x20;              //非同期モードで動作し、で1バイトを送信する
    TX1STAbits.BRGH = 1;
    RC1STA   = 0x90;              //シリアルポートを許可し、CRENビットがクリアになるまで継続的な受信を許可する
    BAUD1CON = 0x08;
    SP1BRG = 832;
    
    
}

