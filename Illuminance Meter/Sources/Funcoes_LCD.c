#include "Funcoes_LCD.h"
#include "Funcoes_Basicas.h"
#include "Macros_Basicos.h"
#include "Enderecando_SFRs.h"

#define RS 0
#define RW 1
#define EN 2

void InicializandoLCD(void)
{
  delay_ms(10); // tempo de estabilização da alimentação do LCD

  TRISD = 0x00;       // todos os pinos do PORTD configurados como saída
  TRISE = 0b11101000; // RE0, RE1, RE2 configurados como saida: usados p/ RS,RW,EN.
  BitLimpar(PORTE,RW); // Pino RW -> H: leitura de dados
                      //            L: escrita de dados

  CmdLcd(0b00111000); //modo de barramento de dados -8bits, 2 linhas, fonte 5x8.
  CmdLcd(0x06); //Config a moviment d cursor e o modo de shift do display, modo incremento, display n acompanha deslocamento
  CmdLcd(0b00001100);//display ligado, sinalização cursor desligado e blinking.
  CmdLcd(0x03); //Config o endereço para 0. Retorna o display para o início se houve alguma operação de shift.
  CmdLcd(0x01);//limpa display e configura endereço para 0
  CmdLcd(0x80);//desloca o cursor para o inicio
}

void CmdLcd(char cmd)
{
  BitLimpar(PORTE,RS); // Pino RS ->  L: Configurado para entrada de instruções
                       //             H: configurado para entrada de dados
  PORTD = cmd;
  BitSetar(PORTE,EN);// bit de inicialização do display, chip select do display.
  delay_ms(2);// maior tempo de processamento de um comando 1,92ms logo 2ms para processar todos os tipos de comando.
  BitLimpar(PORTE,EN); //desliga o chip select
  delay_ms(2); // segurança para esperar algum processamento interno do lcd antes de perdir pra executar mais comandos.
}

void delay_somente_usado_aqui_50us(void)
{
  for(char i = 0; i<7; i++);
}
void EscreverLcd(char caracter)
{
  BitSetar(PORTE,RS); // Pin RS ->  L: instruction code input
                      //            H: data input
  PORTD = caracter;
  BitSetar(PORTE,EN);
  delay_somente_usado_aqui_50us();
  //delay_ms(3); //tempo de absorção de cada caracter até 40us, mas foi colocado 1ms. (preguiça)
  BitLimpar(PORTE,EN);
  delay_somente_usado_aqui_50us();
  //delay_ms(3); // tempo de seguração para algum processamento interno do lcd, antes de pedir algo denovo.
}