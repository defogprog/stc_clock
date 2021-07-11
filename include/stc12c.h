/**
 * @file    stc12c.h
 * @brief   STC12C2052AD Registers description
 * @author  defog psnk42@gmail.com
 */

#define INT0_VEC        0
#define TIM0_VEC        1
#define INT1_VEC        2
#define TIM1_VEC        3
#define UART_VEC        4
#define ADC_SPI_VEC     5
#define PCA_LVD_VEC     6

#define __SFR_P0            0x80
#define __SFR_SP            0x81
#define __SFR_DPL           0x82
#define __SFR_DPH           0x83
#define __SFR_PCON          0x87
#define __SFR_TCON          0x88
#define __SFR_TMOD          0x89
    #define TMOD_T0_M0      (1<<0)
    #define TMOD_T0_M1      (1<<1)
    #define TMOD_T0_CT      (1<<2)
    #define TMOD_T0_GATE    (1<<3)
    #define TMOD_T1_M0      (1<<4)
    #define TMOD_T1_M1      (1<<5)
    #define TMOD_T1_CT      (1<<6)
    #define TMOD_T1_GATE    (1<<7)
#define __SFR_TL0           0x8A
#define __SFR_TL1           0x8B
#define __SFR_TH0           0x8C
#define __SFR_TH1           0x8D
#define __SFR_AUXR          0x8E
    #define AUXR_ELVDI      (1<<2)
    #define AUXR_ESPI       (1<<3)
    #define AUXR_EADCI      (1<<4)
    #define AUXR_UART_M0x6  (1<<5)
    #define AUXR_T1x12      (1<<6)
    #define AUXR_T0x12      (1<<7)
#define __SFR_WAKE_CLKO     0x8F

#define __SFR_P1            0x90
#define __SFR_P1M1          0x91
#define __SFR_P1M0          0x92
#define __SFR_P0M1          0x93
#define __SFR_P0M0          0x94
#define __SFR_P2M1          0x95
#define __SFR_P2M0          0x96
#define __SFR_CLK_DIV       0x97
#define __SFR_SCON          0x98
#define __SFR_SBUF          0x99
#define __SFR_S2CON         0x9A
#define __SFR_S2BUF         0x9B
#define __SFR_BRT           0x9C
#define __SFR_P1ASF         0x9D

#define __SFR_P2            0xA0
#define __SFR_BUS_SPEED     0xA1
#define __SFR_AUXR1         0xA2
#define __SFR_IE            0xA8
#define __SFR_SADDR         0xA9
#define __SFR_IE2           0xAF

#define __SFR_P3            0xB0
#define __SFR_P3M1          0xB1
#define __SFR_P3M0          0xB2
#define __SFR_P4M1          0xB3
#define __SFR_P4M0          0xB4
#define __SFR_IP2           0xB5
#define __SFR_IPH2          0xB6
#define __SFR_IPH           0xB7
#define __SFR_IP            0xB8
#define __SFR_SADEN         0xB9
#define __SFR_P4SW          0xBB
#define __SFR_ADC_CONTR     0xBC
#define __SFR_ADC_RES       0xBD
#define __SFR_ADC_RESL      0xBE

#define __SFR_P4            0xC0
#define __SFR_WDT_CONTR     0xC1
#define __SFR_IAP_DATA      0xC2
#define __SFR_IAP_ADDRH     0xC3
#define __SFR_IAP_ADDRL     0xC4
#define __SFR_IAP_CMD       0xC5
#define __SFR_IAP_TRIG      0xC6
#define __SFR_IAP_CONTR     0xC7
#define __SFR_P5            0xC8
#define __SFR_P5M1          0xC9
#define __SFR_P5M0          0xCA
#define __SFR_SPSTAT        0xCD
#define __SFR_SPCTL         0xCE
#define __SFR_SPDAT         0xCF

#define __SFR_PSW           0xD0
#define __SFR_CCON          0xD8
#define __SFR_CMOD          0xD9
#define __SFR_CCAPM0        0xDA
#define __SFR_CCAPM1        0xDB

#define __SFR_ACC           0xE0
#define __SFR_CL            0xE9
#define __SFR_CCAP0L        0xEA
#define __SFR_CCAP1L        0xEB

#define __SFR_B             0xF0
#define __SFR_PCA_PWM0      0xF2
#define __SFR_PCA_PWM1      0xF3
#define __SFR_CH            0xF9
#define __SFR_CCAP0H        0xFA
#define __SFR_CCAP1H        0xFB


__sfr __at(0xE0) ACC;
__sfr __at(0xF0) B;
__sfr __at(0xD0) PSW;
__sfr __at(0x81) SP;
// __sfr16 __at(0x82) DPTR;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x87) PCON;
__sfr __at(0x8E) AUXR;
__sfr __at(0xA2) AUXR1;
__sfr __at(0x8F) WAKE_CLKO;
__sfr __at(0x97) CLK_DIV;
__sfr __at(0xA1) BUS_SPEED;
__sfr __at(0xA8) IE;
__sfr __at(0xAF) IE2;
__sfr __at(0xB8) IP;
__sfr __at(0xB7) IPH;
__sfr __at(0xB5) IP2;
__sfr __at(0xB6) IPH2;
__sfr __at(0x80) P0;
__sfr __at(0x94) P0M0;
__sfr __at(0x93) P0M1;
__sfr __at(0x90) P1;
__sfr __at(0x92) P1M0;
__sfr __at(0x91) P1M1;
__sfr __at(0x9D) P1ASF;
__sfr __at(0xA0) P2;
__sfr __at(0x96) P2M0;
__sfr __at(0x95) P2M1;
__sfr __at(0xB0) P3;
__sfr __at(0xB2) P3M0;
__sfr __at(0xB1) P3M1;
__sfr __at(0xC0) P4;
__sfr __at(0xB4) P4M0;
__sfr __at(0xB3) P4M1;
__sfr __at(0xBB) P4SW;
__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0x98) SCON;
__sfr __at(0x99) SBUF;
__sfr __at(0x9A) S2CON;
__sfr __at(0x9B) S2BUF;
__sfr __at(0x9C) BRT;
__sfr __at(0xA9) SADDR;
__sfr __at(0xB9) SADEN;
__sfr __at(0xBC) ADC_CONTR;
__sfr __at(0xBD) ADC_RES;
__sfr __at(0xBE) ADC_RESL;
__sfr __at(0xC1) WDT_CONTR;
__sfr __at(0xC8) P5;
__sfr __at(0xC9) P5M1;
__sfr __at(0xCA) P5M0;
__sfr __at(0xCE) SPCTL;
__sfr __at(0xCD) SPSTAT;
__sfr __at(0xCF) SPDAT;
__sfr __at(0xC2) IAP_DATA;
__sfr __at(0xC3) IAP_ADDRH;
__sfr __at(0xC4) IAP_ADDRL;
__sfr __at(0xC5) IAP_CMD;
__sfr __at(0xC6) IAP_TRIG;
__sfr __at(0xC7) IAP_CONTR;
__sfr __at(0xD8) CCON;
__sfr __at(0xD9) CMOD;
__sfr __at(0xDA) CCAPM0;
__sfr __at(0xDB) CCAPM1;
__sfr __at(0xE9) CL;
__sfr __at(0xEA) CCAP0L;
__sfr __at(0xEB) CCAP1L;
__sfr __at(0xF2) PCA_PWM0;
__sfr __at(0xF3) PCA_PWM1;
__sfr __at(0xF9) CH;
__sfr __at(0xFA) CCAP0H;
__sfr __at(0xFB) CCAP1H;

// 0x88
__sbit __at(__SFR_TCON^7) TCON_TF1;
__sbit __at(__SFR_TCON^6) TCON_TR1;
__sbit __at(__SFR_TCON^5) TCON_TF0;
__sbit __at(__SFR_TCON^4) TCON_TR0;
__sbit __at(__SFR_TCON^3) TCON_IE1;
__sbit __at(__SFR_TCON^2) TCON_IT1;
__sbit __at(__SFR_TCON^1) TCON_IE0;
__sbit __at(__SFR_TCON^0) TCON_IT0;

// 0x90
__sbit __at(__SFR_P1^7) P1_7;
__sbit __at(__SFR_P1^6) P1_6;
__sbit __at(__SFR_P1^5) P1_5;
__sbit __at(__SFR_P1^4) P1_4;
__sbit __at(__SFR_P1^3) P1_3;
__sbit __at(__SFR_P1^2) P1_2;
__sbit __at(__SFR_P1^1) P1_1;
__sbit __at(__SFR_P1^0) P1_0;

// 0x98
__sbit __at(__SFR_SCON^7) SCONSM0_FE;
__sbit __at(__SFR_SCON^6) SCONSM1;
__sbit __at(__SFR_SCON^5) SCONSM2;
__sbit __at(__SFR_SCON^4) SCONREN;
__sbit __at(__SFR_SCON^3) SCONTB8;
__sbit __at(__SFR_SCON^2) SCONRB8;
__sbit __at(__SFR_SCON^1) SCONTI;
__sbit __at(__SFR_SCON^0) SCONRI;

// 0xA8
__sbit __at(__SFR_IE^7) IE_EA;
__sbit __at(__SFR_IE^6) IE_ELVD;
__sbit __at(__SFR_IE^5) IE_EADC;
__sbit __at(__SFR_IE^4) IE_ES;
__sbit __at(__SFR_IE^3) IE_ET1;
__sbit __at(__SFR_IE^2) IE_EX1;
__sbit __at(__SFR_IE^1) IE_ET0;
__sbit __at(__SFR_IE^0) IE_EX0;

// 0xB0
__sbit __at(__SFR_P3^7) P3_7;
__sbit __at(__SFR_P3^6) P3_6;
__sbit __at(__SFR_P3^5) P3_5;
__sbit __at(__SFR_P3^4) P3_4;
__sbit __at(__SFR_P3^3) P3_3;
__sbit __at(__SFR_P3^2) P3_2;
__sbit __at(__SFR_P3^1) P3_1;
__sbit __at(__SFR_P3^0) P3_0;

// 0xB8
__sbit __at(__SFR_IP^7) IP_PPCA;
__sbit __at(__SFR_IP^6) IP_PLVD;
__sbit __at(__SFR_IP^5) IP_PADC;
__sbit __at(__SFR_IP^4) IP_PS;
__sbit __at(__SFR_IP^3) IP_PT1;
__sbit __at(__SFR_IP^2) IP_PX1;
__sbit __at(__SFR_IP^1) IP_PT0;
__sbit __at(__SFR_IP^0) IP_PX0;

// 0xD0
__sbit __at(__SFR_PSW^7) PSW_CY;
__sbit __at(__SFR_PSW^6) PSW_AC;
__sbit __at(__SFR_PSW^5) PSW_F0;
__sbit __at(__SFR_PSW^4) PSW_RS1;
__sbit __at(__SFR_PSW^3) PSW_RS0;
__sbit __at(__SFR_PSW^2) PSW_OV;
__sbit __at(__SFR_PSW^1) PSW_F1;
__sbit __at(__SFR_PSW^0) PSW_P;

// 0xD8
__sbit __at(__SFR_CCON^7) CCON_CF;
__sbit __at(__SFR_CCON^6) CCON_CR;
__sbit __at(__SFR_CCON^1) CCON_CCF1;
__sbit __at(__SFR_CCON^0) CCON_CCF0;

// 0xE0
__sbit __at(__SFR_ACC^7) ACC_7;
__sbit __at(__SFR_ACC^6) ACC_6;
__sbit __at(__SFR_ACC^5) ACC_5;
__sbit __at(__SFR_ACC^4) ACC_4;
__sbit __at(__SFR_ACC^3) ACC_3;
__sbit __at(__SFR_ACC^2) ACC_2;
__sbit __at(__SFR_ACC^1) ACC_1;
__sbit __at(__SFR_ACC^0) ACC_0;

// 0xF0
__sbit __at(__SFR_B^7) B_7;
__sbit __at(__SFR_B^6) B_6;
__sbit __at(__SFR_B^5) B_5;
__sbit __at(__SFR_B^4) B_4;
__sbit __at(__SFR_B^3) B_3;
__sbit __at(__SFR_B^2) B_2;
__sbit __at(__SFR_B^1) B_1;
__sbit __at(__SFR_B^0) B_0;
