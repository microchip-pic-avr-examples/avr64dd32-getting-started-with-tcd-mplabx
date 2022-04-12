/*
(c) 2022  Microchip Technology Inc. and its subsidiaries.
    Subject to your compliance with these terms, you may use this software and
    any derivatives exclusively with Microchip products. It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
*/

#include <avr/io.h>
#include <avr/cpufunc.h>

#define DEAD_TIME_A    4   /* 5 us */
#define DEAD_TIME_B    4   /* 5 us */
#define DUTY_CYCLE_A  44   /* 45 us */
#define DUTY_CYCLE_B  44   /* 45 us */

/* Default fuses configuration:
- BOD disabled
- Oscillator in High-Frequency Mode
- UPDI pin active(WARNING: DO NOT CHANGE!)
- RESET pin used as GPIO
- CRC disabled
- MVIO enabled for dual supply
- Watchdog Timer disabled
*/
FUSES =
{
.BODCFG = ACTIVE_DISABLE_gc | LVL_BODLEVEL0_gc | SAMPFREQ_128Hz_gc | SLEEP_DISABLE_gc,
.BOOTSIZE = 0x0,
.CODESIZE = 0x0,
.OSCCFG = CLKSEL_OSCHF_gc,
.SYSCFG0 = CRCSEL_CRC16_gc | CRCSRC_NOCRC_gc | RSTPINCFG_GPIO_gc | UPDIPINCFG_UPDI_gc,
.SYSCFG1 = MVSYSCFG_DUAL_gc | SUT_0MS_gc,
.WDTCFG = PERIOD_OFF_gc | WINDOW_OFF_gc,
};

void TCD0_init(void);
void EVENT_SYSTEM_init(void);
void PORT_init(void);

void TCD0_init(void)
{
    /* write protected register */
    ccp_write_io((void*)&(TCD0.FAULTCTRL), TCD_CMPAEN_bm   /* enable channel A */
                                         | TCD_CMPBEN_bm   /* enable channel B */
                                         | TCD_CMPCEN_bm   /* enable channel C */
                                         | TCD_CMPDEN_bm); /* enable channel D */
    /* set the waveform mode */
    TCD0.CTRLB = TCD_WGMODE_FOURRAMP_gc;
    
    /* set channel D to match channel B */
    TCD0.CTRLC = TCD_CMPDSEL_bm;        
    
    /* set the settling time and duty cycle for the signals*/    
    TCD0.CMPASET = DEAD_TIME_A;
    TCD0.CMPACLR = DUTY_CYCLE_A;
    TCD0.CMPBSET = DEAD_TIME_B;
    TCD0.CMPBCLR = DUTY_CYCLE_B;
    
    TCD0.EVCTRLA = TCD_CFG_FILTER_gc           /* set the anti-spike filter */
                 | TCD_EDGE_FALL_LOW_gc        /* set the 'fault' state */
                 | TCD_TRIGEI_bm;              /* enable input channel A */
    
    /* set the input mode */             
    TCD0.INPUTCTRLA = TCD_INPUTMODE_WAIT_gc;
    
    /* ensure ENRDY bit is set */
    while(!(TCD0.STATUS & TCD_ENRDY_bm))
    {
        ;
    }
    /* TCD input clock is 1MHz (4MHz divided by 4) */
    TCD0.CTRLA =  TCD_CLKSEL_OSCHF_gc        /* choose the timer's clock */
               |  TCD_CNTPRES_DIV4_gc        /* choose the prescaler */
               |  TCD_ENABLE_bm;             /* enable the timer */
}

void EVENT_SYSTEM_init(void)
{
    /*PF6 state as event*/
    EVSYS.CHANNEL4 = EVSYS_CHANNEL4_PORTF_PIN6_gc;
    
    /* Connect user to event channel 4 */
    EVSYS.USERTCD0INPUTA = EVSYS_USER_CHANNEL4_gc;
}

void PORT_init(void)
{
    PORTA.DIRSET = PIN4_bm              /* set pin 4 of port A as output */
                 | PIN5_bm              /* set pin 5 of port A as output */
                 | PIN6_bm              /* set pin 6 of port A as output */
                 | PIN7_bm;             /* set pin 7 of port A as output */
              
    PORTF.DIRCLR   = PIN6_bm;             /* set pin 6 of port F as input */
    PORTF.PIN6CTRL = PORT_PULLUPEN_bm;  /* enable pull-up resistor for pin 6 of port F */         
}

int main(void)
{    
    PORT_init();    
    EVENT_SYSTEM_init();
    TCD0_init();
    
     while (1) 
    {
        ;
    }
}
