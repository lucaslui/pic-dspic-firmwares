// ===============================================================================================================================================================

#include <PIC16F628A.h>
#include "Funcoes_PWM.h"

// ===============================================================================================================================================================

void SetaPWM(unsigned char porcento)
{
    // Formula do duty cycle:
    // DC_porcento = V / ((PR2+1)*4;
    // V = DC/100 * (PR2+1) * 4
    // V = DC * (PR2+1) /25
    unsigned int val = ((unsigned int)porcento) *(PR2+1);
    val = val / 25;     // Garante que tem apenas 10 bits
    val &= 0x03ff;      // Os 8 primeiros bits sao colocados no CCPR1L.
    CCPR1L = val >> 2;  // Os ultimos dois são colocados na posição 5 e 4 do CCP1CON.
    CCP1CON |= (val & 0x0003) << 4;
}

// ===============================================================================================================================================================

void SetaFreqPWM(unsigned int freq)
{
    // PR2 = fosc/(fpwm*4*prescaler) - 1;
    // PR2 = (4000000/(freq*4*1)) - 1;
    PR2 = (1000000/(freq)) - 1;
}

// ===============================================================================================================================================================

void InicializaPWM(void)
{
    CCP1CON = 0b00001100;   // Configura CCP1 como um PWM mode.
    T2CON = 0b00000000;     // Configura o prescale do timer 2 para 1:1
    T2CONbits.TMR2ON = 1;   // Liga o timer 2
    TRISBbits.TRISB3 = 0;   // Configura PORTA RB3/CCP1 como saída.
}

// ===============================================================================================================================================================