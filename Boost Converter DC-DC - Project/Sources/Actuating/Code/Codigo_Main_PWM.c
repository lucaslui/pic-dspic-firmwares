// ===============================================================================================================================================================

#include <PIC16F628A.h>
#include "Config_bits_PWM.h"
#include "Funcoes_SERIAL.h"
#include "Funcoes_PWM.h"
#include "Funcoes_Basicas.h"

// ===============================================================================================================================================================

 char porcentagem_pwm = 50, valor_recebido_serial = 0;

// ===============================================================================================================================================================
 
void inicializando_registradores()
{
    OPTION_REG = 0b00000000; // Refência de clock dos timer igual a interno, pull-up resistores do PORTB configurados como HABILITADOS.
    INTCON = 0x00;           // Desabilita todas as interrupções.
    PCON = 0x08;             // Clock do microcontrolador configurado como 4MHZ vindo do oscilador interno.
    CMCON = 0x07;            // Modulo comparador off.
    TRISA = 0xFF;            // Todos os pinos PORTA configurados como entrada (alta impedância).
    TRISB = 0xFF;            // Todos os pinos PORTB configurados como entrada (alta impedância).
}

// ===============================================================================================================================================================

void main()
{
    inicializando_registradores();
    InicializaSerial();
    InicializaPWM();
    SetaFreqPWM(50000);
    InicializaPWM();
    SetaPWM(porcentagem_pwm);
    while(1)
    {
        valor_recebido_serial = RecebeSerial();
        if(valor_recebido_serial != 0)
        {
            porcentagem_pwm = valor_recebido_serial;
            SetaPWM(porcentagem_pwm);
        }
        if ((DebBotao(&PORTB, 4, 10, 0) == 1) && (porcentagem_pwm <= 82))
        {
            porcentagem_pwm += 1;
            SetaPWM(porcentagem_pwm);
        }

        if ((DebBotao(&PORTB, 7, 10, 0) == 1) && (porcentagem_pwm >= 12))
        {          
            porcentagem_pwm -= 1;
            SetaPWM(porcentagem_pwm);
        }
    }
}

// ===============================================================================================================================================================