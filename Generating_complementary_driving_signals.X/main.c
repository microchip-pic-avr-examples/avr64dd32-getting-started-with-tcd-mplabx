/*
(c) 2022 Microchip Technology Inc. and its subsidiaries.
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


#define PERIOD          19  /* f TCD cycle = 20MHz / (2 * (1 + 19) ) = 500kHz */
#define DUTY_CYCLE      10  /* 1 clk = 50ns, duty cycle = 2 * 10 * 50ns = 1us */
#define DEAD_TIME        2  /* 1 clk = 50ns, dead time = 2 * 2 * 50ns = 200ns */

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

void CLKCTRL_init(void);
void TCD0_init(void);
void PORT_init(void);


void CLKCTRL_init(void)
{
    /* RUNSTDBY disabled; FREQSEL 20M; AUTOTUNE disabled */
    ccp_write_io((void*)&(CLKCTRL.OSCHFCTRLA),0x20);
    /* system clock is configured to 20MHz */
}

void TCD0_init(void)
{
    /* write protected register */
    ccp_write_io((void*)&(TCD0.FAULTCTRL), TCD_CMPAEN_bm   /* enable channel A */
                                         | TCD_CMPBEN_bm); /* enable channel B */
  
    /* set the waveform mode */
    TCD0.CTRLB = TCD_WGMODE_DS_gc;
    
    /* set the signal period */
    TCD0.CMPBCLR = PERIOD;
    
    /* the signals are alternatively active and a
       symmetric dead time is added */
    TCD0.CMPASET = DUTY_CYCLE - DEAD_TIME;
    TCD0.CMPBSET = DUTY_CYCLE + DEAD_TIME;

    /* ensure ENRDY bit is set */
    while(!(TCD0.STATUS & TCD_ENRDY_bm))
    {
        ;
    }
    /* TCD input clock is 20MHz (20MHz divided by 1) */
    TCD0.CTRLA =  TCD_CLKSEL_OSCHF_gc        /* choose the timer's clock */
               |  TCD_CNTPRES_DIV1_gc        /* choose the prescaler */
               |  TCD_ENABLE_bm;             /* enable the timer */
}

void PORT_init(void)
{
    PORTA.DIRSET = PIN4_bm          /* set pin 4 as output */
                 | PIN5_bm;         /* set pin 5 as output */
}

int main(void)
{
    CLKCTRL_init();
    PORT_init();
    TCD0_init();
    
    while (1) 
    {
        ;
    }
}
