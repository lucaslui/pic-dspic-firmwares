/*
 * Projeto do estudante do 3º ano de engenharia
 * Autor Código: Lucas Lui Motta
 * Data: 15/05/2015
 * Assunto Projeto: Código do Transmissor.
 */

// =======================================================================================================================

#include <xc.h>         // Biblioteca das definições como SFR's bit a bit (struct - campo de bits) Ex: PORTXbits.RAX
#include <pic16f688.h>  // Bibliotecas para o mapeamento e nomeações dos SFR's.
#include "SERIAL.h"
#include "CONFIG_BITS.h"
// =======================================================================================================================

void delay_ms(int ms)
{
  /* 14 instruções em assembly para uma incrementação
   * 1/4MHz = 250ns, 4 ciclos para efetuar uma instrução no PIC
   * Assim 250ns x 4 = 1us por instrução,
   * ocorrem 14 por incrementação sendo assim 14us por incremento
   * para gerar 0,5s de delay precisaram 35714 incrementos.
   * logo ms 1 ms equivale a 71,4285 incrementos. Sendo assim:  */
  unsigned int numeroIncrementos , i = 0;
  numeroIncrementos = (ms * 71);
  for (i = 0; i < numeroIncrementos; i++);
}

// =======================================================================================================================

void Inicializando_ADC(void)
{
  ADCON0 = 0b10000000; // Justificativa a direita, Tensao referencia Vdd, canal 0 habilitado, ADON E GO/DONE desligados.
  ADCON1 = 0b00100000; // A/D Conversion Clock = FOSC/32.(010)

  ADCON0bits.ADON = 1;     //  ADON: bit liga/desliga ADC
                           // 1 = A/D - Habilita a possibilidade de ligar o ADC.
                           // 0 = A/D - Desliga o ADC, não a mais comsumo de corrente.
}

// =======================================================================================================================

unsigned int Leitura_ADC(void)
{
  unsigned int valor_adc_temporario = 0;
  ADCON0bits.GO_nDONE = 1;     // GO/DONE: dando start de conversão.

  while (ADCON0bits.GO_nDONE); // Esperando conversão A/D acabar ou seja status do bit GO_nDone igual a 0.
  valor_adc_temporario =  ADRESH ;
  valor_adc_temporario <<=  8 ;
  valor_adc_temporario += ADRESL;
  return (valor_adc_temporario); //retorna valor inteiro.
}

// =======================================================================================================================

void Inicializando_SFRs()
{
    OSCCON = 0b01100101;      // IRCF(7:5) 111: Internal Oscillator  = 4Mhz, Bit 0 - 1 = Internal oscillator is used for system clock.
    INTCONbits.GIE = 0;
    OPTION_REG = 0b10000000; // 1 = PORTA pull-ups are disabled. 0 = Internal instruction cycle clock (CLKOUT).
    CMCON0 = 0x07;            // CM(0:2) 111 - Comparators Off.
    ANSEL = 0x01;             // Somente RA0/AN0 configurado como analógica input.
    TRISC = 0xFF;             // PORTC inteiro configurado como entrada (alta impedância).
    WPUA = 0x00;              // Weak Pull-up Register bits - disabled.
}

// =======================================================================================================================

unsigned char calculo_valor_pressao(unsigned int valor)
{
    unsigned char pressao = 0;
    pressao = (char)((valor - 163.84)/7.9);
    return pressao;
}

void main()
{
    unsigned int acumulador_valor_adc = 0, valor_adc_media = 0;
    unsigned char quantidade = 0;
    Inicializando_SFRs();
    Inicializando_ADC();
    InicializaSerial();
    while(1)
    {
        for(int i=0; i <= 1000; i++)
        {
            acumulador_valor_adc += Leitura_ADC();
            quantidade++;
            if(quantidade == 50)
            {
                valor_adc_media = (int)(acumulador_valor_adc / 50);
                quantidade = 0;
                acumulador_valor_adc = 0;
                EnviaSerial(calculo_valor_pressao(valor_adc_media));
            }
            if((i % 5) == 0)
            {
                EnviaSerial(calculo_valor_pressao(valor_adc_media));
            }
            delay_ms(2);
        }

    }
}

// =======================================================================================================================