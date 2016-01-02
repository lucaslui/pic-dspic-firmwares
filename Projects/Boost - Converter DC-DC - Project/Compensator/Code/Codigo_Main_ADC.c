// ===============================================================================================================================================================

#include <PIC16F688.h>
#include "Config_bits_ADC.h"
#include "Funcoes_SERIAL.h"
#include "Funcoes_ADC.h"
#include "Funcoes_Basicas.h"

// ===============================================================================================================================================================

unsigned int valor_adc = 0;
unsigned char valor_pwm = 50;
unsigned char quantidade = 0;
unsigned int acumulador_valor_adc = 0;

// ===============================================================================================================================================================

void inicializando_registradores()
{
    OSCCON = 0b01110101;      // IRCF(7:5) 111: Oscilador Interno = 8Mhz, Bit 0 - 1 = Oscilador interno usado como clock do microcontrolador.
    INTCON = 0x00;
    OPTION_REG = 0b10000000;  // 1 = PORTA pull-ups estão desabilitados, 0 = Referência dos timers usando clock do microcontrolador.
    CMCON0 = 0x07;            // CM(0:2) 111 - Desligando comparadores.
    ANSEL = 0x01;             // Somente RA0/AN0 configurado como analógica input.
    TRISA = 0xFF;             // PORTA inteiro configurado como entrada (alta impedância).
    TRISC = 0xFF;             // PORTC inteiro configurado como entrada (alta impedância).
    WPUA = 0x00;              // Desabilita também a possibilidade de pinos no PORTA estarem em pull-up.
}

// ===============================================================================================================================================================

void Compensador_proporcional(int valor_adc_media)
{
    if ((valor_adc_media >= 144) && (valor_adc_media <= 181) && (valor_pwm < 84)) {
        valor_pwm += 1;
        
    } else if ((valor_adc_media < 144) && (valor_pwm < 80)) {
        valor_pwm += 2;
        
    } else if ((valor_adc_media >= 195) && (valor_adc_media <= 240) && (valor_pwm > 10)) {
        valor_pwm -= 1;
        
    } else if ((valor_adc_media > 240) && (valor_pwm > 10)) {
        valor_pwm -= 2;
        
    }
    EnviaSerial(valor_pwm);
}

// ===============================================================================================================================================================

void main()
{
    inicializando_registradores();
    Inicializando_ADC();
    InicializaSerial();
    
    while(1)
    {
        for(int i = 0; i <= 1000; i++)
        {
            acumulador_valor_adc += Leitura_ADC();
            quantidade++;
            if(quantidade == 80)
            {
                valor_adc = (int)(acumulador_valor_adc / 80);
                quantidade = 0;
                acumulador_valor_adc = 0;
                Compensador_proporcional(valor_adc);
                delay_ms(200);
            }
        }
    }
}

// ===============================================================================================================================================================