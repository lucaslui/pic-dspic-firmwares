/* ==================================================================================================================
	Arquivo: Funcoes_LCD.c
	Descrição: Biblioteca para LCD - HD44780 e barrramento 4 bits.
	Portável: Não portável, necessita ajustes dependendo do microcontrolador utilizado.
	Microcontrolador Utilizado: PIC18F4520.
 ==================================================================================================================*/

#include "pic18f4520.h"
#include "Config_Fusiveis_Externos.h"
#include "Funcoes_Basicas.h"
#include "Funcoes_LCD.h"

// ==================================================================================================================

#define Pino_RS PORTCbits.RC0   //    <- Pino RS do display.
#define Pino_EN PORTCbits.RC1   //    <- Pino EN do display.

#define Barramento_Dados_4bits PORTD // PORT para saida do barramento de dados, usa-se somente Nibble Baixo.

// ==================================================================================================================

void Reset_Obrigatorio_LCD(void)
{
    Pino_RS = 0; // Pino Pino_RS ->  L: Configurado para entrada de instruções
                 //                  H: configurado para entrada de dados
    delay_ms(30);
    Barramento_Dados_4bits = 0x03;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(30);

    Barramento_Dados_4bits = 0x03;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(30);

    Barramento_Dados_4bits = 0x03;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(30);

    Barramento_Dados_4bits = 0x02;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(1);
}

// ==================================================================================================================

void Cmd_LCD(char cmd)
{
  Pino_RS = 0; // Pino Pino_RS ->  L: Configurado para entrada de instruções
               //                  H: configurado para entrada de dados
  Barramento_Dados_4bits = (cmd >> 4);
  Pino_EN = 1;
  Pino_EN = 0;
  Barramento_Dados_4bits = cmd;
  Pino_EN = 1;
  Pino_EN = 0;
  delay_ms(5);
}

// ==================================================================================================================

void Inicializando_LCD(void)
{
    Reset_Obrigatorio_LCD();
    Barramento_Dados_4bits = 0x02;
    Pino_EN = 1;
    Pino_EN = 0;
    Barramento_Dados_4bits = 0x08;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(2);

    Barramento_Dados_4bits = 0x00;
    Pino_EN = 1;
    Pino_EN = 0;
    Barramento_Dados_4bits = 0x0C;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(2);
}

// ==================================================================================================================

void Escrever_LCD(char caracter)
{
    Pino_RS = 1;   // Pino Pino_RS ->  L: Configurado para entrada de instruções
                   //                  H: configurado para entrada de dados
    Barramento_Dados_4bits = (caracter >> 4);
    Pino_EN = 1;
    Pino_EN = 0;
    Barramento_Dados_4bits = caracter;
    Pino_EN = 1;
    Pino_EN = 0;
    delay_ms(2);
}

// ==================================================================================================================

void Gerador_Frases(const unsigned char vetorFrase[])
{
  char letras = 0, j = 0, i = 0 ;
  while(vetorFrase[j] != 0) // Percorre o vetor frase em busca do null(\0) que seria o fim da frase.
  {
    j++;
    letras++;
  }
  for(i = 0; i < letras; i++) Escrever_LCD(vetorFrase[i]); // Percorrendo vetorFrase colocando caracter a caracter
                                                          // no display lcd.
}

// ==================================================================================================================

char Converter_Numero_Para_Caracter(char numero) // Função conversora de uma unidade númerica para o caracter correspondente.
{
  switch (numero)
  {
  case 0:
    return ('0');
  case 1:
    return ('1');
  case 2:
    return ('2');
  case 3:
    return ('3');
  case 4:
    return ('4');
  case 5:
    return ('5');
  case 6:
    return ('6');
  case 7:
    return ('7');
  case 8:
    return ('8');
  case 9:
    return ('9');
  }
  return ('0');
}

// ==================================================================================================================

void Converter_Para_String(unsigned int valor_int, unsigned char digitos)  // Função converte número inteiro em string e com "n" digitos, e 
{                                                                        // em seguida imprime no LCD.
  unsigned char unidade_t = 0, dezena_t = 0, centena_t = 0,  milhar_t = 0;
  unsigned char valor_em_string[4] = {0,0,0,0};
  char indice = 0, i = 0;
  indice = (digitos - 1);
  
  if(valor_int != 0)
  {
    unidade_t = (valor_int % 10);           // arranca o valor da unidade do valor_int.
    valor_em_string[indice] = (Converter_Numero_Para_Caracter(unidade_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 10)
  {
    dezena_t = ((valor_int %100)/10);       // arranca o valor da dezena do valor_int.
    valor_em_string[indice] = (Converter_Numero_Para_Caracter(dezena_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 100)
  {
    centena_t = ((valor_int %1000)/100);    // arranca o valor da centena do valor_int.
    valor_em_string[indice] = (Converter_Numero_Para_Caracter(centena_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 1000)
  {
    milhar_t = ((valor_int %10000) / 1000); // arranca o valor do milhar do valor_int.
    valor_em_string[indice] = (Converter_Numero_Para_Caracter(milhar_t));
  }else valor_em_string[indice] = '0';
 
  for(i = 0; (valor_em_string[i] != 0); i++) Escrever_LCD(valor_em_string[i]); 
}

// ==================================================================================================================
