//==============================================================================
/*
 *  Código para o trasmissor do ônibus.
 *  Data ínicio: 10/09/2015.  
 */
//==============================================================================

#include <pic18f4550.h>
#include "ConfigBits.h"
#include "CodigoBasicos.h"
#include "CodigoLCD.h"
#include "CodigoSerial.h"

#define Linha_1  PORTDbits.RD0
#define Linha_2  PORTDbits.RD1
#define Linha_3  PORTDbits.RD2
#define Linha_4  PORTDbits.RD3

#define seta_direita 11
#define seta_esquerda 12
#define confirma 13
#define limpa 14
#define menu_config 15
#define ida 16
#define volta 17

#define centena 2
#define dezena 1
#define unidade 0



//==============================================================================

unsigned int linha = 0, sentido = ida;
unsigned char unidade_var = 0, dezena_var = 0, centena_var = 0;
unsigned char recebe_serial = 0;

//==============================================================================

int LerTecladoMatricial()
{
    Linha_1 = 0; Linha_2 = 1; Linha_3 = 1; Linha_4 = 1;
    delay_ms(1);
    if((DebBotao(&PORTD, 4, 5, 0)) == 1) return 1;
    else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 2;
         else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 3;
              else if((DebBotao(&PORTD, 7, 5, 0)) == 1) return confirma;

    Linha_1 = 1; Linha_2 = 0; Linha_3 = 1; Linha_4 = 1;
    delay_ms(1);
    if((DebBotao(&PORTD, 4, 5, 0)) == 1) return 4;
    else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 5;
         else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 6;
              else if((DebBotao(&PORTD, 7, 5, 0)) == 1) return limpa;

    Linha_1 = 1; Linha_2 = 1; Linha_3 = 0; Linha_4 = 1; delay_ms(1);
    if((DebBotao(&PORTD, 4, 5, 0)) == 1) return 7;
    else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 8;
         else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return 9;
              else if((DebBotao(&PORTD, 7, 5, 0)) == 1) return menu_config;
        
    Linha_1 = 1; Linha_2 = 1; Linha_3 = 1; Linha_4 = 0;
    delay_ms(1);
    if((DebBotao(&PORTD, 4, 5, 0)) == 1) return seta_esquerda;
    else if((DebBotao(&PORTD, 5, 5, 0)) == 1) return 0;
         else if((DebBotao(&PORTD, 6, 5, 0)) == 1) return seta_direita;
              else if((DebBotao(&PORTD, 7, 5, 0)) == 1) return 255;

    return 255;
}

void Menu_Principal()
{
   Cmd_LCD(0x01);
   Gerador_Frases("Linha: "); 
   Escrever_Lcd(ConverterNumeroParaCaracter(centena_var));
   Escrever_Lcd(ConverterNumeroParaCaracter(dezena_var));
   Escrever_Lcd(ConverterNumeroParaCaracter(unidade_var));
   Cmd_LCD(0xC0);     
   Gerador_Frases("Sentido: "); 
   switch (sentido)
   {
       case ida:
           Gerador_Frases("Ida");
           break;
       case volta:
           Gerador_Frases("Volta");
           break;           
   }
}

void Config_Linha()
{
    Cmd_LCD(0x01);
    Gerador_Frases("- Menu Config -");
    Cmd_LCD(0xC0);
    Gerador_Frases("Linha: ");
    Cmd_LCD(0b00001110); // Liga Cursor
    unsigned char valor_retornado = 255;
    unsigned char digito_selecionado = centena;
    
    while(1) 
    {
        valor_retornado = 255;       
        valor_retornado = LerTecladoMatricial(); 
        
        switch (valor_retornado) 
        {         
            case 0:
                Escrever_Lcd('0');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 0; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 0; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 0; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 1:
                Escrever_Lcd('1');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 1; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 1; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 1; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 2:
                Escrever_Lcd('2');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 2; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 2; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 2; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 3:
                Escrever_Lcd('3');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 3; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 3; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 3; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 4:
                Escrever_Lcd('4');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 4; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 4; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 4; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 5:
                Escrever_Lcd('5');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 5; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 5; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 5; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);
                        
                }
                break;
                
            case 6:
                Escrever_Lcd('6');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 6;  digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 6;  digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 6; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 7:
                Escrever_Lcd('7');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 7; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 7; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 7; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 8:
                Escrever_Lcd('8');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 8; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 8; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 8; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;
                
            case 9:
                Escrever_Lcd('9');
                switch(digito_selecionado)
                {
                    case centena:
                        centena_var = 9; digito_selecionado = dezena;
                        break;
                    case dezena:
                        dezena_var = 9; digito_selecionado = unidade;
                        break;
                    case unidade:
                        unidade_var = 9; digito_selecionado = unidade;
                        Cmd_LCD(0xC9);                        
                }
                break;

            case confirma:  // Botão "COMFIRMA" foi pressionado. 
                linha = ((centena_var*100) + (dezena_var*10) + (unidade_var));
                return;
                
            case limpa: // Botão "Limpa" foi pressionado. 
                centena_var = 0;
                dezena_var = 0;
                unidade_var = 0;
                Cmd_LCD(0xC7);
                for(char j = 0; j < 3; j++) Escrever_Lcd('0');
                digito_selecionado = centena;
                Cmd_LCD(0xC7);
                break;
        }        
    }
}

void Config_Sentido() 
{
    unsigned char valor_retornado = 255;
    Cmd_LCD(0x01);
    Gerador_Frases("- Menu Config -");
    Cmd_LCD(0xC0);
    Gerador_Frases("Sentido: ");    
    sentido = ida;
    Gerador_Frases("Ida ");
    Escrever_Lcd(0b01111110); // Seta para direita.
    Gerador_Frases("  ");
    
    while(1)
    {
        valor_retornado = 255;       
        valor_retornado = LerTecladoMatricial();
        
        switch(valor_retornado)
        {
            case seta_direita:
                if(sentido == ida) 
                {
                    sentido = volta;
                    Cmd_LCD(0xC9);
                    Escrever_Lcd(0b01111111); // Seta para esquerda.                    
                    Gerador_Frases(" Volta");
                }
                break;
                
            case seta_esquerda:
                if(sentido == volta) 
                {
                    sentido = ida;
                    Cmd_LCD(0xC9);
                    Gerador_Frases("Ida ");
                    Escrever_Lcd(0b01111110); // Seta para direita.
                    Gerador_Frases("  ");
                }
                break;
                
            case confirma:
                Cmd_LCD(0b00001100); // Desliga Cursor
                return;
        }               
    }
}

void Frase_Inicio()
{
   Cmd_LCD(0x01);
   Gerador_Frases("Emissor de Sinal"); 
   for(unsigned char loading = 0; loading < 4; loading ++)
   {
        Cmd_LCD(0xC0);   
        Gerador_Frases("   versao 1.3   "); 
        delay_ms(500);
        Cmd_LCD(0xC0);
        Gerador_Frases(">  versao 1.3  <"); 
        delay_ms(500);
        Cmd_LCD(0xC0);
        Gerador_Frases(">> versao 1.3 <<");
        delay_ms(500);
        Cmd_LCD(0xC0);
        Gerador_Frases(">>>versao 1.3<<<"); 
   }
}

void Inicializando_SFR()
{
    OSCCON = 0b01110011; // Habilitando oscilador interno como 8Mhz.
    CMCON = 0b00000111; // Desabilitando comparadores.
    TRISA = 0b11110000; // Habilitando como saída os pinos de dados para o LCD (RA0,RA1,RA2,RA3).
    TRISC = 0b11111100; // Habilitando como saída os pinos de controle para LCD (RC0(RS), RC1(EN)).
    TRISD = 0b11110000; // Habilitando os botoes do teclado matricial.
    INTCON = 0x00; // Desabilitando todo tipo de interrupção.
    PORTEbits.RDPU = 0;
    TRISCbits.RC0 = 0; // Saída para o led de indicação.
}

void main()
{
    unsigned char valor_retornado = 255;
    unsigned char recebe_serial = 0;
    Inicializando_SFR();
    Reset_Obrigatorio_LCD();
    Inicializando_LCD();
    Frase_Inicio();
    Config_Linha();
    Config_Sentido();  
    Menu_Principal();
    InicializaSerial(); // Inicializa porta serial.    
    
    while(1)
    {

        valor_retornado = 255;
        valor_retornado = LerTecladoMatricial();
        
        if (valor_retornado == menu_config)
        {
            Config_Linha();
            Config_Sentido();
            Menu_Principal();
        }
        recebe_serial = RecebeSerial();
        if(linha == recebe_serial) PORTCbits.RC0 = 1;
        EnviaSerial(linha);
        

        /*
        valor_retornado = 255;
        valor_retornado = LerTecladoMatricial();
        if (valor_retornado == 15) 
        {
            Config_Linha();
            Config_Sentido();  
            Menu_Principal();
        }
        */
    }    
}

//==============================================================================