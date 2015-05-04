/*==================================================================================================================
 *
 *             PROJETO DE PI DO CURSO DE ENGENHARIA ELÉTRICA UNISAL - Americana 2014
 *                               MCU utilizado: PIC18F2550
 *
 *  Autor: Lucas Lui Motta.
 *
 *  Projeto: Medidor de Custo - Medição constante do gasto de um aparelho eletroeletrônico diretamente em reais com 
 *	     o decorrer do tempo, usando como periféricos a conversão A/D do próprio PIC, um Amp.OP (LM 358), e um sensor
 *	     de corrente (ACS712).
 *
 *  Softwares usados: XC8 - Compilador.
 *					  MPLABX - IDE.
 *					  PICKIT2 - Gravador.
 *					  Proteus Isis - Simulator.
 *				          Altium Designer Summer 08 - EAD.
 *
 * ==================================================================================================================
 *  Macros/Bibliotecas baseados nos códigos de: Rodrigo Maximiano Antunes de Almeida.
 *  Disponibilizados em: <sites.google.com/site/rmaalmeida/home>
 *
 *  Sobre Licença: GNU GPL 2
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * ==================================================================================================================
 *
 *  Arquivo: Codigo_Main.c
 *
 *	Arquivos do projeto: Codigo_Main.c
 *						 Configurando_Fusiveis_Externos.h
 *						 Definindo_SFRs.h									
 *						 Funcoes_ADC.h
 *						 Funcoes_ADC.c
 *						 Funcoes_Basicas.h
 *						 Funcoes_Basicas.c
 *						 Funcoes_LCD.h			
 *						 Funcoes_LCD.c
 *						 Esquematico_Wattimetro.SchDoc (Altium)	
 *						 PCB_Wattimetro.PcbDoc (Altium)
 *					         ProteusCustímetro.pdsprj (Proteus)
 */
// ==================================================================================================================

#include "Configurando_Fusiveis_Externos.h"
#include "Definindo_SFRs.h"
#include "Funcoes_Basicas.h"
#include "Funcoes_LCD.h"
#include "Funcoes_ADC.h"

// ==================================================================================================================

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit)) // Macros para mascarar os bytes, são feitas operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit)) // para setar, limpar, inverter, e testar bits especificos de um byte.
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

// ==================================================================================================================

double custo = 0;               					// Variáveis Globais utilizadas.                    
long double valor_total_custo = 0;
unsigned int valor_em_corrente_final = 0;

// ==================================================================================================================

void Ajustar_Custo(void)
{
  unsigned char selecionador = 0, unidade = 0, centena = 0, dezena = 0;
  Cmd_LCD(0b00001110); // Liga Cursor.
  while (1)            // Entra em loop infinito, fica aqui no menu de ajustar custo até o botao de break ser precionado.
  {
    if ((DebBotao(&PORTB, 2, 10, 0)) == 1) // Seleciona com qual digito do custo vc quer mexer.
    {
      if (selecionador == 2) selecionador = 0;
      else selecionador++;
    }
    if ((DebBotao(&PORTB, 1, 10, 0)) == 1) break;

    switch (selecionador) // Dependendo do digito que for mexer vc entra em um case diferente.
    {
    case 0:
      Cmd_LCD(0xCF);
      if ((DebBotao(&PORTB, 3, 10, 0)) == 1)
      {
        unidade++;
        if (unidade == 10) unidade = 0;
        Escrever_Lcd(ConverterNumeroParaCaracter(unidade));
        Cmd_LCD(0xCF);
      }
      break;
    case 1:
      Cmd_LCD(0xCE);
      if ((DebBotao(&PORTB, 3, 10, 0)) == 1)
      {
        dezena++;
        if (dezena == 10) dezena = 0;
        Escrever_Lcd(ConverterNumeroParaCaracter(dezena));
        Cmd_LCD(0xCE);
      }
      break;
    case 2:
      Cmd_LCD(0xCC);
      if ((DebBotao(&PORTB, 3, 10, 0)) == 1)
      {
        centena++;
        if (centena == 10) centena = 0;
        Escrever_Lcd(ConverterNumeroParaCaracter(centena));
        Cmd_LCD(0xCD);
      }
      break;
    }
  }
  valor_total_custo = ((centena)+(dezena/10.00)+(unidade/100.000));
  valor_total_custo /= 1000.0000;
  Cmd_LCD(0b00001100); // Desliga Cursor.
}

void Interface_Inicial(void) 
{// Função que escreve a interface de inicialização do sistema.

   Cmd_LCD(0x01);                      
   Gerador_Frases(" Medidor Custo "); 
   Cmd_LCD(0xC0);
   Gerador_Frases(">              <"); 
   delay_ms(400);
   Cmd_LCD(0xC0);
   Gerador_Frases(">>            <<"); 
   delay_ms(400);
   Cmd_LCD(0xC0);
   Gerador_Frases(">>>          <<<"); 
   delay_ms(400);
   Cmd_LCD(0xC0);
   Gerador_Frases(">>>>        <<<<"); 
   delay_ms(400);
   Cmd_LCD(0xC0);
   Gerador_Frases(">>>>>      <<<<<"); 
   delay_ms(400);
   Cmd_LCD(0xC0);
   Gerador_Frases(">>>>>>    <<<<<<");
   delay_ms(400);
}

void Interface_Config_kwh(void) 
{// Função que escreve a interface de configuração do kwh da concessionária.

    Cmd_LCD(0x01);               
    Gerador_Frases(" Ajuste Inicial ");
    Cmd_LCD(0xC0);
    Gerador_Frases("Valor kW.h: ");
    Cmd_LCD(0xCD);
    Escrever_Lcd(',');
    Cmd_LCD(0xCF);
    Escrever_Lcd('0');
    Cmd_LCD(0xCE);
    Escrever_Lcd('0');
    Cmd_LCD(0xCC);
    Escrever_Lcd('0');
    Ajustar_Custo();
}

void Interface_Principal(void) 
{// Função que escreve a interface principal do sistema.

  Cmd_LCD(0x01);                
  Gerador_Frases("Gasto: R$ "); 
  Cmd_LCD(0x8C);
  Gerador_Frases(",");          
  Cmd_LCD(0xC0);
  Gerador_Frases("Corrente: "); 
  Cmd_LCD(0xCC);
  Gerador_Frases(",");
  Cmd_LCD(0xCF);
  Gerador_Frases("A");          
}

void Calculo_Corrente(int valor_ad)
{
    double valor_em_corrente = 0;
    if(valor_ad > 467)
    {
    valor_em_corrente = (double)(0.000012681*valor_ad*valor_ad);
    valor_em_corrente += 0.5969;
    valor_em_corrente -= (double)(0.0072*valor_ad);
    valor_em_corrente_final = (int)(valor_em_corrente*100);
    }else valor_em_corrente_final = 0;
}

void interrupt TMR0_TMR1(void)
{
   if (BitTestar(INTCON, 2)) // Teste flag TMR0IF para ver se houve interrupção por estouro do TIMER0.
   {
    // Osc. Interno 8Mhz, Clock interno PIC = 8Mhz/4 = 2Mhz ou seja cada ciclo é 500ns, com Preescaler de 1:256, 256x500ns para um incremento
    // ficando assim 128us por incremento, inicializando os registrador high e low do TIMER0 com FF00 (decimal 65280) temos que essa interrupção irá ocorrer
    // com o seguinte tempo, 65536 - 65280 = 256, 256 incrementos para uma interrupção assim 256 x 128 us = 32 ms.
    unsigned int real = 0, centavos = 0, amper = 0, miliamper = 0;
    BitLimpar(INTCON, 2);    // Limpando flag de estouro.    
    TMR0L = 0x00;            // Valor de reinicialização TMR0.
    TMR1H = 0xFF;    
    real = (int) (custo / 100);
    centavos = ((int)custo % 100);
    Cmd_LCD(0x8A);
    Converter_para_String(real,2);
    Cmd_LCD(0x8D);
    Converter_para_String(centavos,2);
    amper = (int)(valor_em_corrente_final / 100);
    miliamper = (int)(valor_em_corrente_final % 100);
    Cmd_LCD(0xCA);
    Converter_para_String(amper,2);
    Cmd_LCD(0xCD);
    Converter_para_String(miliamper,2);
   }
   
   if(BitTestar(PIR1, 0))  // Flag de interrupção de estouro do TIMER 1.
   {
     // Osc. Interno 8Mhz, Clock interno PIC = 8Mhz/4 = 2Mhz ou seja cada ciclo é 500ns, com Preescaler de 1:8, 8x500ns para um incremento
     // ficando assim 4us por incremento do Timer, quantos incrementos para dar 10ms para que a cada 10ms faça o estouro do Timer e faça 
     // a conversão A/D, precisariam de 2500 incrementos, sendo assim em um timer de 16 bits temos 65536 - 2500 = 63036, em hexadecimal F63C.
     static unsigned long int somatorias_valor_adc = 0;
     static unsigned char numerador_media = 0, contador_para_segundo = 0;
     BitLimpar(PIR1, 0);   // Limpando Flag de interrupção.
     TMR1L = 0x3C;         // Valor de reinicialização TMR1.
     TMR1H = 0xF6;
     BitLimpar(INTCON, 5); // Desligando interrupção Timer0, dando altar prioridade a interrupção do TIMER1.
     somatorias_valor_adc += Leitura_ADC();
     numerador_media++;
     contador_para_segundo++;
     if(numerador_media == 50)
     {
        Calculo_Corrente((int)(somatorias_valor_adc / 50));
        somatorias_valor_adc = 0;
        numerador_media = 0;
     }
     if(contador_para_segundo == 100)
     {
       double watts = 0;
       watts = (1.27*valor_em_corrente_final);
       custo += (watts*valor_total_custo);
       contador_para_segundo = 0;
     }
     BitSetar(INTCON, 5);  // Ligando novamente interrupção Timer0.
   }
}

void Inicializando_SFRs(void)
{
    OSCCON  = 0b01110000;  // Oscilador interno configirado como 8Mh.
    CMCON   = 0b00000111;  // Comparadores desligados, Pinos configurador como digital I/O.
    CCP2CON = 0x00;        // Módulo CCP (Capture/Compare/PWM) desligados .
    TRISA   = 0b11111111;  // PORTA configurada tudo como entrada (Impedância alta).
    TRISB   = 0b11111111;  // PORTB configurada tudo como entrada (Impedância alta).
    TRISC   = 0b11111100;  // RC0, RC1 como saída para os pinos do display RS e EN.
    TRISD   = 0b11110000;  // RD0,RD1,RD2,RD3 configurados como saída para uso no barramento de 4 bits do display.        
    INTCON  = 0b11000000;  // Global interrupção habilitado, periféricos interrupções habilitado, TMR0 interrupção desligado.
    INTCON2 = 0b10000000;  // Desabilitando resistores de pull-up do PORTB.
    T0CON   = 0b00000010;  // TMR0 desligado, configurado como temporizador 16 bits, referência clock interno, com preescaler 1:8.
    T1CON   = 0b00110000;  // Preescaler de 1:8, internal clock (fosc/4), TMR1 desligado.    
    BitSetar(T0CON, 7);    // Ligando temporizador TIMER 0.
    BitSetar(T1CON, 0);    // Ligando temporizador TIMER 1.
}

void main(void)
{
    Inicializando_SFRs();
    Reset_Obrigatorio_LCD();
    Inicializando_LCD();
    Inicializando_ADC();
	
    Interface_Inicial();
    Interface_Config_kwh();
    Interface_Principal();
	
    BitSetar(INTCON, 5);  // Ligando interrupção Timer0.
    BitSetar(PIE1, 0);    // Ligando interrupção Timer1.
    while(1);
}

