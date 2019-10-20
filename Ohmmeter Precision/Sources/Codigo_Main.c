/*==================================================================================================================
 *
 *         PROJETO DE BOLSA INICIAÇÃO CIÊNTIFICA (BIC-SAL) DA UNISAL DE AMERICANA
 *                  DEPARTAMENTO DE ENGENHARIA ELÉTRICA 2015
 *                             MCU utilizado: PIC18F4520
 *
 *  Autor: Lucas Lui Motta.
 *         Heitor dos Santos Sousa
 *
 *  Projeto: Ohmímetro de Precisão de 0-10 ohms.
 *
 *  Softwares usados: XC8 - Compilador.
 *					  MPLABX - IDE.
 *
 * ==================================================================================================================
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
 *						 Config_Fusiveis_Externos.h									
 *						 Funcoes_Basicas.h
 *						 Funcoes_Basicas.c
 *						 Funcoes_LCD.h			
 *						 Funcoes_LCD.c
 */
// ==================================================================================================================

#include "pic18f4520.h"
#include "Config_Fusiveis_Externos.h"
#include "Funcoes_Basicas.h"
#include "Funcoes_I2C.h"
#include "Funcoes_LCD.h"

// ==================================================================================================================

// ==================================================================================================================

void Interface_Loading(void) 
{
   Cmd_LCD(LimpaDisplay);                      
   Gerador_Frases("Ligando...");
   Cmd_LCD(SegundaLinhaPrimeiraColuna);
   for(char i = 0; i <= 15; i++) // Criando digitos no formato preenchido
   {                             // para dar a impressão de uma barra de loading.
        Escrever_LCD(0xFF);
        delay_ms(90);
   }
   Cmd_LCD(LimpaDisplay);     
}

void Interface_Main(void) 
{// Função que escreve a interface de configuração do kwh da concessionária.

    Cmd_LCD(LimpaDisplay);               
    Gerador_Frases(" Ohmimetro v1.0 ");
    Cmd_LCD(SegundaLinhaPrimeiraColuna);
    Gerador_Frases("Valor: ");
    Cmd_LCD(0xCA);
    Escrever_LCD(',');
}

void Resistance_Calculation(unsigned value_ad_received)
{
    
}

void Separando_e_Imprimindo_Digitos(unsigned int numero)
{
    unsigned char centena = 0, dezena = 0, unidade = 0;
    centena = (int)(numero / 100);
    dezena = (int)((numero - (100*centena)) / 10);
    unidade = (numero - (100*centena) - (10*dezena));
    
    Cmd_LCD(0xC8);
    Escrever_LCD(Converter_Numero_Para_Caracter(centena));
    Escrever_LCD(Converter_Numero_Para_Caracter(dezena));
    Cmd_LCD(0xCB);
    Escrever_LCD(Converter_Numero_Para_Caracter(unidade));
}

void Inicializando_SFRs(void)
{
    //---------------------- CONFIGURAÇÕES INICIAS -----------------------------------------------------------------  
    
    OSCCON  = 0b01110000;  // Oscilador interno configirado como 8Mhz.
    
    //------------------ DESABILITANDO ALGUNS PERIFÉRICOS ----------------------------------------------------------  
    
    CMCON   = 0x07;        // Comparadores desligados, Pinos configurador como digital I/O.
    ADCON1  = 0x07;        // Desabilita entradas analógicas e habilita entradas digitais.
    
    //------------------------ CONFIGURANDO I/O --------------------------------------------------------------------  
    
    TRISA   = 0b11111111;   // PORTA configurada tudo como entrada (Impedância alta).
    TRISB   = 0b11111111;   // PORTB configurada tudo como entrada (Impedância alta).
    TRISC   = 0b11111100;   // RC0, RC1 como saída para os pinos do display RS e EN.
    TRISD   = 0b11110000;   // RD0,RD1,RD2,RD3 configurados como saída para uso no barramento de 4 bits do display.        
 
    //------------------------- INTERRUPÇÕES -----------------------------------------------------------------------  
    
    INTCON = 0x00;
    //INTCON  = 0b11000000;   // Global interrupção habilitado, periféricos interrupções habilitado, TMR0 interrupção desligado.
    //INTCON2 = 0b10000000;   // Desabilitando resistores de pull-up do PORTB.
    //T0CON   = 0b00000010;   // TMR0 desligado, configurado como temporizador 16 bits, referência clock interno, com preescaler 1:8.

    //BitSetar(T0CON, 7);   // Ligando temporizador TIMER 0.
      
}

void main(void)
{
    Inicializando_SFRs();
    Inicializando_LCD();	
    Interface_Loading();
    Interface_Main();
    Inicializando_I2C();
	    
    //BitSetar(INTCON, 5);  // Ligando interrupção Timer0.
        
    unsigned char variavel = 0, variavel_travada = 0;
    unsigned char trava_de_valor = 0;
    unsigned int valor_ad_recebido = 0;
    
    write_msp(0b00011001); // Continous Shot, 15 SPS (16 bits), PGA gain 2 V/V.
    
    while(1)
    {/*
        variavel++;
        if (variavel == 255) variavel = 0;
        delay_ms(300);
        if ((DebBotao(&PORTB, 0, 10, 0)) == 1) // Para travar o ultimo valor no display
        {
            if (trava_de_valor == 0) 
            {
                trava_de_valor = 1;
                variavel_travada = variavel;
            } 
            else trava_de_valor = 0;
        }
        if (trava_de_valor == 0) Separando_e_Imprimindo_Digitos(variavel);
        else Separando_e_Imprimindo_Digitos(variavel_travada);       
    */
        for(int i = 0; i < 2; i++) delay_ms(500);
        valor_ad_recebido = read_msp();
        if (valor_ad_recebido != 0) Separando_e_Imprimindo_Digitos(valor_ad_recebido);
    
    }
}

