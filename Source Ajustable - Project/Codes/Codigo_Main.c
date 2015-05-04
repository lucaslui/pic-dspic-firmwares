/* =======================================================================================================================
 * Arquivo:   Main_Code.c
 * Autor: Lucas
 * Criado: 7 de Abril de 2015, 00:18
 =======================================================================================================================*/

#include <xc.h>         // Biblioteca das definições como SFR's bit a bit (struct - campo de bits) Ex: PORTXbits.RAX
#include <pic16f688.h>  // Bibliotecas para o mapeamento e nomeações dos SFR's.

// =======================================================================================================================

#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = 0FF      // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

// =======================================================================================================================

unsigned char unidade = 0, dezena = 0, display_alvo = 1, quantidade = 0; // Variáveis Globais <- Gambiarra.
unsigned int acumulador_valor_adc = 0, valor_adc_media = 0;

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

void Gerando_saida(unsigned char numero)
{
  switch(numero)
  {
   case 0:
     PORTC = 0b00111111; // G desligado,  numero 0 no display.
     PORTAbits.RA4 = 0;
     break;
   case 1:
     PORTC = 0b00000110; // B e C ligado, numero 1 no display.
     PORTAbits.RA4 = 0;
     break;
   case 2:
     PORTC = 0b00011011; // F e C desligado, numero 2 no display.
     PORTAbits.RA4 = 1;
     break;
   case 3:
     PORTC = 0b00001111; // F e E desligado, numero 3 no display
     PORTAbits.RA4 = 1;
     break;
   case 4:
     PORTC = 0b00100110; // A, D, E desligado, numero 4 no display.
     PORTAbits.RA4 = 1;
     break;
   case 5:
     PORTC = 0b00101101; // B e E desligado, numero 5 no display.
     PORTAbits.RA4 = 1;
     break;
   case 6:
     PORTC = 0b00111101; // B desligado, numero 6 no display.
     PORTAbits.RA4 = 1;
     break;
   case 7:
     PORTC = 0b00000111; // A, B, C ligado, numero 7 no display.
     PORTAbits.RA4 = 0;
     break;
   case 8:
     PORTC = 0b00111111; // Tudo ligado,  numero 8 no display.
     PORTAbits.RA4 = 1;
     break;
   case 9:
     PORTC = 0b00101111; // E desligado,  numero 9 no display
     PORTAbits.RA4 = 1;
     break;
   default:
     PORTC = 0b00111001; // Erro = B e C desligado.
     PORTAbits.RA4 = 1;
     break;
  }
}

// =======================================================================================================================

void ComutacaoDisplay(void) // Procedimento de multiplexação dos display.
{                           
  switch(display_alvo)
  {
   case 1:
     PORTAbits.RA2 = 0;
     PORTC = 0;
     PORTAbits.RA4 = 0;
     Gerando_saida(unidade);
     PORTAbits.RA1= 1;
     display_alvo = 2;
     break;
   case 2:
     PORTAbits.RA1 = 0;
     PORTC = 0;
     PORTAbits.RA4 = 0;
     Gerando_saida(dezena);
     PORTAbits.RA2 = 1;
     display_alvo = 1;
     break;
  }
}

// =======================================================================================================================

void Serarador_digitos(double tensao_saida)
{
    if(tensao_saida > 9.9)  // Acima de 10v o display mostra o valor de tensão somente de unidade a unidade.
    {
        PORTAbits.RA5 = 0;  // Desliga o ponto do decimal.
        unidade = (int)((int)tensao_saida % 10);            // Arranca o valor da unidade do resultado da conversão A/D.
        dezena = (int)(((int)tensao_saida % 100) / 10);     // Arranca o valor da dezena  do resultado da conversão A/D.
    }
    else // Abaixo de 10v o display mostra o valor de tensão nas casas decimais.
    {
        PORTAbits.RA5 = 1;  // Liga o ponto do decimal.
        tensao_saida *= 10; // Faz o avanço de uma casa decimal para que possa usar o conversor.
        unidade = (int)((int)tensao_saida % 10);            // Arranca o valor da unidade do resultado da conversão A/D.
        dezena = (int)(((int)tensao_saida % 100) / 10);     // Arranca o valor da dezena  do resultado da conversão A/D.
    }
}

// =======================================================================================================================

double Calculando_tensao(unsigned int valor_adc_media)
{
    //*********************************************************************************************************
    //
    // Supondo uma resposta linear da tensão no pino AN0 e o valor da tensão na saída da fonte, temos:
    // Parâmetros para a equação da reta:
    //
    //                    RA0/AN0          SAÍDA
    //                     4.287V          28,53V
    //                     0.209V          1.261V
    //
    // Convertendo tensão RA0/AN0 para valores digitais, resolução conversor 10 bits = 1024.
    //
    //                      Valor A/D              SAÍDA
    //                     876(876,2439)           28,53V
    //                     43 (42,7186)            1,261V
    // Equação da reta:
    //
    //   m = (28,56 - 1,261 / 876,2439 - 42,7186) = 0,03275
    //   y-yo = m.(x-xo) ->   y = 0,03275.(x-42,7186) + 1,261
    //                        y = 0,03275x - 1,399 + 1,261
    //                        y = 0,03275x - 0,138                         (RUIMMMMMMMM)
    // return ((0.03275*valor_adc_media) - 0.138);
    //
    //*********************************************************************************************************
    //
    // Supondo uma resposta com uma curva diferente de uma reta, temos:
    //
    //    ADC	        SAIDA					RA0
    //    42,71868132	1,26					0,209
    //    198,4681319	5,86					0,971
    //    393,8703297	11,62					1,927
    //    648,5472527	19,17					3,173
    //    790,1934066	24,05					3,866
    //    875,4263736	28,5					4,283
    //
    // Equação feita no excel: y = 8E-06x2 + 0,024x + 0,4893. (EXCEL)
    // Com valor R² = 0,9979.
    //
    // resultado_temp += (0.000008*valor_adc_media*valor_adc_media);       (RUIMMMMMMMM)
    // resultado_temp += (0.024*valor_adc_media);
    // resultado_temp += (0,4893);
    //
    //*********************************************************************************************************
    //
    // Supondo um polinomial de 4° ordem, temos:
    // Y= -0.27089 + 0.036922*X - 0.28129E-04*X^2 + 0.26658E-07*X^3  (LABFIT)     (BOM)
    //
     //*********************************************************************************************************

    double resultado_temp = 0;

    resultado_temp += ((0.000026658*valor_adc_media)*valor_adc_media*valor_adc_media);
    resultado_temp += (36.922*valor_adc_media);
    resultado_temp -= (270.89);
    resultado_temp -= (0.028129*valor_adc_media*valor_adc_media);
    resultado_temp /= 1000;

    return(resultado_temp);
}
// =======================================================================================================================
void Inicializando_SFRs()
{
    OSCCON = 0b01110001;      // IRCF(7:5) 111: Internal Oscillator  = 8Mhz, Bit 0 - 1 = Internal oscillator is used for system clock.
    INTCON = 0x00;
    OPTION_REGbits.nRAPU = 1; // 1 = PORTA pull-ups are disabled.
    OPTION_REGbits.T0CS = 1;  // 0 = Internal instruction cycle clock (CLKOUT).
    CMCON0 = 0x07;            // CM(0:2) 111 - Comparators Off.
    ANSEL = 0x01;             // Somente RA0/AN0 configurado como analógica input.
    TRISC = 0x00;             // PORTC inteiro configurado como saída, para serem usados no display.
    TRISA = 0b11001001;       // RA1, RA2, RA4, RA5 configurados como saída.
    WPUA = 0x00;              // Weak Pull-up Register bits - disabled.
}

// =======================================================================================================================

void main()
{
    Inicializando_SFRs();
    Inicializando_ADC();
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
            }
            if((i % 300) == 0)
            {
                Serarador_digitos(Calculando_tensao(valor_adc_media));
            }
            if((i % 3) == 0)
            {
                ComutacaoDisplay();
            }
            delay_ms(2);            
        }
    }
}

// =======================================================================================================================