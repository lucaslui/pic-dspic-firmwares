#include "Definindo_SFRs.h"
#include "Funcoes_Basicas.h"
#include "Funcoes_LCD.h"

// ===============================================================================================================================================================

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit)) // Definições para operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit)) // para setar,limpar,inverter,e testar bits especificos de um byte.
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

// ===============================================================================================================================================================

#define Pinos_Controle PORTC // Definições da configuração do hardware com as PORTs I/O.
#define Pino_RS 0 
#define Pino_EN 1

#define Barramento_Dados_4bits PORTD // PORT para saida do barramento de dados, usa-se somente Nibble Baixo.

// ===============================================================================================================================================================

void Reset_Obrigatorio_LCD(void)
{
    BitLimpar(Pinos_Controle,Pino_RS);
    delay_ms(30);
    Barramento_Dados_4bits = 0x03;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(30);

    Barramento_Dados_4bits = 0x03;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(30);

    Barramento_Dados_4bits = 0x03; //VEPino_RSAO 2
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(30);

    Barramento_Dados_4bits = 0x02;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(1);

}

// ===============================================================================================================================================================

void Cmd_LCD(char cmd)
{
  BitLimpar(Pinos_Controle,Pino_RS); // Pino Pino_RS ->  L: Configurado para Pino_ENtrada de instruções
                       //             H: configurado para Pino_ENtrada de dados
  Barramento_Dados_4bits = (cmd >> 4);
  BitSetar(Pinos_Controle,Pino_EN);
  BitLimpar(Pinos_Controle,Pino_EN);
  Barramento_Dados_4bits = cmd;
  BitSetar(Pinos_Controle,Pino_EN);
  BitLimpar(Pinos_Controle,Pino_EN);
  delay_ms(5);
}

// ===============================================================================================================================================================

void Inicializando_LCD(void)
{
    Barramento_Dados_4bits = 0x02;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    Barramento_Dados_4bits = 0x08;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(2);

    Barramento_Dados_4bits = 0x00;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    Barramento_Dados_4bits = 0x0C;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(2);
}

// ===============================================================================================================================================================

void Escrever_Lcd(char caracter)
{
    BitSetar(Pinos_Controle,Pino_RS);
    Barramento_Dados_4bits = (caracter >> 4);
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    Barramento_Dados_4bits = caracter;
    BitSetar(Pinos_Controle,Pino_EN);
    BitLimpar(Pinos_Controle,Pino_EN);
    delay_ms(2);
}

// ===============================================================================================================================================================

void Gerador_Frases(const unsigned char vetorFrase[])
{
  char letras = 0, j = 0, i = 0 ;
  while(vetorFrase[j] != 0) // Percorre o vetor frase em busca do null(\0) que seria o fim da frase.
  {
    j++;
    letras++;
  }
  for(i = 0; i < letras; i++) Escrever_Lcd(vetorFrase[i]); // PercorrPino_ENdo vetorFrase colocando caracter a caracter
                                                            // no display lcd.
}

// ===============================================================================================================================================================

char ConverterNumeroParaCaracter(char numero) // Função convePino_RSor de número para o caracter represPino_ENtante.
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

// ===============================================================================================================================================================

void Converter_para_String(unsigned int valor_int, unsigned char digitos) // Função convePino_RSor de número para a string represPino_ENtante.
{
  unsigned char unidade_t = 0, dezena_t = 0, centena_t = 0,  milhar_t = 0;
  unsigned char valor_em_string[4] = {0,0,0,0};
  char indice = 0, i = 0;
  indice = (digitos - 1);
  
  if(valor_int != 0)
  {
    unidade_t = (valor_int % 10);  // arranca o valor da unidade do valor_int.
    valor_em_string[indice] = (ConverterNumeroParaCaracter(unidade_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 10)
  {
    dezena_t = ((valor_int %100)/10); // arranca o valor da dezPino_ENa do valor_int.
    valor_em_string[indice] = (ConverterNumeroParaCaracter(dezena_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 100)
  {
    centena_t = ((valor_int %1000)/100); // arranca o valor da cPino_ENtPino_ENa do valor_int.
    valor_em_string[indice] = (ConverterNumeroParaCaracter(centena_t));
  }else valor_em_string[indice] = '0';
  indice--;
  if(valor_int >= 1000)
  {
    milhar_t = ((valor_int %10000) / 1000); // arranca o valor do milhar do valor_int.
    valor_em_string[indice] = (ConverterNumeroParaCaracter(milhar_t));
  }else valor_em_string[indice] = '0';
 
  for(i = 0; (valor_em_string[i] != 0); i++) Escrever_Lcd(valor_em_string[i]); 

}

// ===============================================================================================================================================================
