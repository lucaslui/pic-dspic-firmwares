/* =============================================================================
 * 
 *  Arquivo: Code_Basic_Functions.h
 *  Descrição: Biblioteca para inicialização dos leds e função debouncing para
 *             as duas chaves.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#ifndef __LEDS_BUTTONS_H__
#define __LEDS_BUTTONS_H__

#include <p33FJ256GP506.h> 

/* Wait for this many times before switch press is valid		
* For this application, each increment is 								
* (FRAME_SIZE/ADC_FSAMP) seconds */

#define SWITCH_DEBOUNCE		12

#define SWITCH_S1	PORTDbits.RD8
#define SWITCH_S2	PORTDbits.RD9

int CheckSwitchS1(void);
int CheckSwitchS2(void);
int CheckSwitchS2_4secHeldDown(void);

#endif
