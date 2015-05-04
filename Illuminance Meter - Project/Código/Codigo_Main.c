/*==================================================================================================================
 *
 *             PROJETO DE PI DO CURSO DE ENGENHARIA ELÉTRICA UNISAL - Americana 2014
 *                               MCU utilizado: PIC18F452
 *
 *  Autor: Lucas Lui Motta.
 *
 *  Projeto: Medidor de Iluminância
 *
 *  Softwares usados: XC8 - Compilador.
 *					  MPLABX - IDE.
 *					  PICKIT2 - Gravador.
 *					  Proteus Isis - Simulator.
 *				      Altium Designer Summer 08 - EAD.
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
 *					     ProteusCustímetro.pdsprj (Proteus)
 */
// ==================================================================================================================

#include "Fusiveis_Externos.h"  // Sequencia de #pragma para configurar os fusiveis externos do PIC18F452.
#include "Enderecando_SFRs.h"   // Identificando os SFRs no mapa de memoria do PIC18F452.
#include "Macros_Basicos.h"     // Macros para trabalhar bit a bit em um byte: Setar, Limpar, Testar e  Inverter bit.

#include "Funcoes_Basicas.h"    // Delay_ms e detector de botao com deboucing.
#include "Funcoes_LCD.h"        // Funcoes do LCD, como inicialização, escrita de dados e comandos.
#include "Funcoes_ADC.h"        // Funcoes do conversor A/D do pic: Inicialização, conversão e leitura.

#define TMR0IF 2 // Definições para facilitar intendimento do programa na parte de tratamento de interrupções.
#define TMR1IF 0
#define INT0IF 1

#define TRUE 1 
#define FALSE 0

unsigned char estadoAtual = 1, VindaPeloTMR0 = 0, ContadorSalvos = 0, ordemSalvada = 0;
unsigned char DentroDeOutroMenu = 0, selecionadorTempoSalve = 0, numeroMaximoSalves = 0;
unsigned int  valor_adc = 0;                            // Variável para armazenar o valor da conversão A/D.
unsigned int  valor_lux = 0;
double valor_lux_provisorio = 0;
unsigned char valor_adc_string[4] = {'0','0','0','0'}; // Vetor para armazenar String do valor da conversao A/D.
unsigned char segundoString[3] = {':','0','0'}, minutoString[3] = {':','0','0'}, horaString[2] = {'0','0'}; // Vetor para armazenar String das variavel do valor do relogio.
unsigned char segundo = 0, minuto = 0, hora = 0;       // Variaveis utilizadas no relogio.
unsigned char modoEconomicoAtivado = 0;
unsigned char const PrimeiroDigitoCodigoSeguranca = 5, SegundoDigitoCodigoSeguranca = 0, TerceiroDigitoCodigoSeguranca = 7, QuartoDigitoCodigoSeguranca = 1;

struct ValoreSalvos
{
  unsigned int  valor_adc[32];
  unsigned char minuto[32];
  unsigned char hora[32];
}ValoresSalvos;

void FuncaoQueSalva(void)
{
  if (ContadorSalvos < numeroMaximoSalves)
  {
    ValoresSalvos.valor_adc[ContadorSalvos] = valor_adc;
    ValoresSalvos.minuto[ContadorSalvos] = minuto;
    ValoresSalvos.hora[ContadorSalvos] = hora;
    ContadorSalvos++;
  }
}

void GeradorFrases(const unsigned char vetorFrase[])
{
  char letras = 0, j = 0 ;
  while(vetorFrase[j] != 0) // Percorre o vetor frase em busca do null(\0) que seria o fim da frase.
  {
    j++;
    letras++;
  }
  for(char i = 0; i < letras; i++) EscreverLcd(vetorFrase[i]); // Percorrendo vetorFrase colocando caracter a caracter
                                                               // no display lcd.
}

void TextoDaInterfacePrincipal(void)
{
  GeradorFrases("Relogio");
  CmdLcd(0b11000000);
  GeradorFrases("Valor: ");
  CmdLcd(0b11001101);
  GeradorFrases("Lux");
}

void TextoDeEntrada(void) // Somente frases de interface com o usuário.
{
  GeradorFrases("Ligando...");
  CmdLcd(0b11000000);
  for(char i = 0; i <= 15; i++) // Criando digitos no formato preenchido
  {                             // para dar a impressão de uma barra de loading.
     EscreverLcd(0b11111111);
     delay_ms(90);
  }
  CmdLcd(0x01);                 // Limpa display lcd
  TextoDaInterfacePrincipal();
  BitSetar(T0CON, 7); // Liga TMR0 e consequentemente TMR1.
}

char ConverterNumeroParaCaracter(char numero) // conversor de numero para string.
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

void ConversorStringValorADC(unsigned int valor)
{
  char unidade = 0, dezena = 0, centena = 0,  milhar = 0, i = 3; // i = 3, para ordem certa dos numb no sist decimal.
  if(valor != 0)
  {
    unidade = (valor % 10); //arranca o valor da unidade do valor(valor_adc).
    valor_adc_string[i] = (ConverterNumeroParaCaracter(unidade)); // e convertendo para string e armazenando em um vetor.
  }else valor_adc_string[i] = '0';
  i--;
  if(valor >= 10)
  {
    dezena = ((valor %100)/10);//arranca o valor da decimal do valor(valor_adc).
    valor_adc_string[i] = (ConverterNumeroParaCaracter(dezena));// e convertendo para string e armazenando em um vetor.
  }else valor_adc_string[i] = '0';
  i--;
  if(valor>=100)
  {
    centena = ((valor %1000)/100);//arranca o valor da decimal do valor(valor_adc).
    valor_adc_string[i] = (ConverterNumeroParaCaracter(centena));// e convertendo para string e armazenando em um vetor.
  }else valor_adc_string[i] = '0';
  i--;
  if(valor >= 1000)
  {
    milhar = ((valor %10000) / 1000);
    valor_adc_string[i] = (ConverterNumeroParaCaracter(milhar));
  }else valor_adc_string[i] = '0';
}

void ConversorStringRelogio(unsigned int valor, unsigned char *Stringtemp, char indicePrimeiraLeitura)
{
  char unidade = 0, decimal = 0, i = indicePrimeiraLeitura; //i = indicePrimeiraLeitura, para começar o vetor na posição certa
  if(valor != 0)
  {
    unidade = (valor % 10);                                 // Absorve o valor da unidade do numero e
    Stringtemp[i] = (ConverterNumeroParaCaracter(unidade)); // convertendo para string e armazenando em um vetor.
  }else Stringtemp[i] = '0';
  i--; // Decrecendo a variavel "i" para caminhar pelo vetor.
  if(valor >= 10)
  {
    decimal = ((valor %100)/10);                            // Absorve o valor da decimal do numero e
    Stringtemp[i] = (ConverterNumeroParaCaracter(decimal)); // convertendo para string e armazenando em um vetor.
  }else Stringtemp[i] = '0';
}

void EscritaRelogio(void) // Função utilizada escrever as variaveis segundo, minuto, e hora no display
{
    ConversorStringRelogio(segundo, &segundoString[0], 2); // Conversor para string do valor da variavel é mandado nessa função
    CmdLcd(0b10001101);                                    // a variavel, o vetor onde será armazenado, e o tamanho desse vetor.
    for (unsigned char i = 0; i < 3; i++) EscreverLcd(segundoString[i]);
    ConversorStringRelogio(minuto, &minutoString[0], 2);
    CmdLcd(0b10001010);
    for (unsigned char i = 0; i < 3; i++) EscreverLcd(minutoString[i]);
    ConversorStringRelogio(hora, &horaString[0], 1);
    CmdLcd(0b10001000);
    for (unsigned char i = 0; i < 2; i++) EscreverLcd(horaString[i]);
}

void EscritaAjustarHoras(void) // Função utilizada escrever as variaveis segundo, minuto, e hora no display
{
    ConversorStringRelogio(segundo, &segundoString[0], 2); // Conversor para string do valor da variavel é mandado nessa função
    CmdLcd(0b11001001);                                    // a variavel, o vetor onde será armazenado, e o tamanho desse vetor.
    for (unsigned char i = 0; i < 3; i++) EscreverLcd(segundoString[i]);
    ConversorStringRelogio(minuto, &minutoString[0], 2);
    CmdLcd(0b11000110);
    for (unsigned char i = 0; i < 3; i++) EscreverLcd(minutoString[i]);
    ConversorStringRelogio(hora, &horaString[0], 1);
    CmdLcd(0b11000100);
    for (unsigned char i = 0; i < 2; i++) EscreverLcd(horaString[i]);
}

void EscreverListaLCD(void)
{
  char unidade = 0, dezena = 0;
  CmdLcd(0b10000110);
  if (ordemSalvada != 0)
  {
    dezena = ((ordemSalvada % 100) / 10); //arranca o valor da unidade do valor(valor_adc).
    EscreverLcd(ConverterNumeroParaCaracter(dezena)); // e convertendo para string e armazenando em um vetor.
  }
  else EscreverLcd('0');

  if (ordemSalvada != 0)
  {
    unidade = (ordemSalvada % 10); //arranca o valor da unidade do valor(valor_adc).
    EscreverLcd(ConverterNumeroParaCaracter(unidade)); // e convertendo para string e armazenando em um vetor.
  }
  else EscreverLcd('0');

  ConversorStringRelogio(ValoresSalvos.minuto[ordemSalvada], &minutoString[0], 2);
  CmdLcd(0b10001101);
  for (unsigned char i = 0; i < 3; i++) EscreverLcd(minutoString[i]);
  ConversorStringRelogio(ValoresSalvos.hora[ordemSalvada], &horaString[0], 1);
  CmdLcd(0b10001011);
  for (unsigned char i = 0; i < 2; i++) EscreverLcd(horaString[i]);
  CmdLcd(0b11000111);
  ConversorStringValorADC(ValoresSalvos.valor_adc[ordemSalvada]); // Procedimento feito para conversão do valor_adc inteiro para o equivalente em string.
  for (unsigned char i = 0; i < 4; i++) EscreverLcd(valor_adc_string[i]); // Percorre o vetor "vetor_adc_string" colocando string de numero a numero no lcd.
}

void ModoEconomico(void)
{
 DentroDeOutroMenu = TRUE;
 BitSetar(T0CON, 7); // Liga TMR0 e consequentemente TMR1.
 CmdLcd(0x01);       // Limpa display lcd
 CmdLcd(0b00001100); // Desliga Cursor.
 GeradorFrases("Modo Economico");
 CmdLcd(0b11000000);
 for (char i = 0; i <= 15; i++) // Criando digitos no formato preenchido
 {                              // para dar a impressão de uma barra de loading.
   EscreverLcd(0b11111111);
   delay_ms(90);
 }
 CmdLcd(0x08);        // Limpar totalmente dados do display.
 BitLimpar(PORTB, 3); // Desligando backlight.
 TRISD = 0xFF;        // Deixando todas as PORTAS como entrada (high impedance)
 TRISE = 0b11101111;  // assim deixando o mcu com consumo minímo.
 while (1)
 {
  if ((DebBotao(&PORTB, 0, 10, 0)) == 1) break;
 }
 InicializandoLCD(); // Reinicializando funções do LCD.
 CmdLcd(0x01);       // Limpa display lcd
 BitSetar(PORTB, 3); // Ligando backlight.
 TextoDaInterfacePrincipal();
 TMR0H = 0b11111111;   // Inicializa o TMR0 com com valor baixo para aparecer o relogio no display
 TMR0L = 0b11111101;   // na maneira mais rapida possivel.
 BitSetar(T0CON, 7); // Liga TMR0 e consequentemente TMR1.
 DentroDeOutroMenu = FALSE;
}

void QuantidadeSalves(void)
{
  if (selecionadorTempoSalve == 0) numeroMaximoSalves = 32;
  else if (selecionadorTempoSalve == 1) numeroMaximoSalves = 16;
  else if (selecionadorTempoSalve == 2) numeroMaximoSalves = 8;
}

void MenuListaDeValoresSalvo(void)
{
  DentroDeOutroMenu = TRUE;
  BitLimpar(T1CON,0); // Desliga  TMR1.
  CmdLcd(0x01);
  CmdLcd(0b10000000);
  GeradorFrases("Salvo(");
  CmdLcd(0b10001000);
  GeradorFrases("):");
  CmdLcd(0b10001101);
  GeradorFrases(":00");
  CmdLcd(0b11000000);
  GeradorFrases("Valor: ");
  CmdLcd(0b11001101);
  GeradorFrases("Lux");
  ordemSalvada = 0;
  EscreverListaLCD();
  QuantidadeSalves();
  while (1)
  {
    if ((DebBotao(&PORTA, 2, 10, 0)) == 1)
    {
      ordemSalvada++;
      if (ordemSalvada >= numeroMaximoSalves) ordemSalvada = 0;
      EscreverListaLCD();
    }
    if ((DebBotao(&PORTA, 3, 10, 0)) == 1)
    {
      if (ordemSalvada == 0) ordemSalvada = numeroMaximoSalves;
      ordemSalvada--;
      EscreverListaLCD();
    }
    if ((DebBotao(&PORTB, 1, 10, 0)) == 1) break; // Mesmo Botao que entra é o que sai desse Menu.
    if ((DebBotao(&PORTB, 0, 10, 0)) == 1)
    {
        modoEconomicoAtivado = 1;
        break;
    }
  }
  if(modoEconomicoAtivado == 0)
  {
   CmdLcd(0x01); // Limpa display lcd.
   TextoDaInterfacePrincipal();
   DentroDeOutroMenu = FALSE;
   TMR0H = 0b11111111; // Necessario para volta rapida a escrita da interface principal.
   TMR0L = 0b11111101; 
  }
  else
  {
   modoEconomicoAtivado = 0;
   ModoEconomico();
  }
}


void MenuAjustarRelogio(void) // Função feita para o ajuste do relogio.
{
  BitLimpar(T0CON, 7); // Desliga TMR0.
  BitLimpar(T1CON,0);  // Desliga  TMR1.
  CmdLcd(0x01);
  GeradorFrases("Ajuste de Horas:");
  EscritaAjustarHoras();
  char selecionador = 0, temp = 0;
  CmdLcd(0b00001110); // Liga Cursor.
  for(char i=0; i<5;i++)CmdLcd(0b00010100); // Desloca cursor 5x para direita para alcancar a parte do segundo.
  while (1) // Entra em loop infinito, fica aqui no menu de ajustar relogio até o botao de break ser precionado.
  {
    if ((DebBotao(&PORTA, 3, 10, 0)) == 1)
    {
      if (selecionador == 2) selecionador = 0;
      else selecionador++;
    }
    if ((DebBotao(&PORTA, 2, 10, 0)) == 1)
    {
      if (selecionador == 0) selecionador = 2;
      else selecionador--;
    }

    switch (selecionador)  //
    {
    case 0:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1)
      {
        segundo++;
        if (segundo == 60) segundo = 0;
        EscritaAjustarHoras();
        for (char i = 0; i < 5; i++)CmdLcd(0b00010100);
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1)
      {
        segundo--;
        if(segundo == 0) segundo = 59;
        EscritaAjustarHoras();
        for (char i = 0; i < 5; i++)CmdLcd(0b00010100);
      }
      break;
    case 1:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1)
      {
        minuto++;
        if (minuto == 60) minuto = 0;
        EscritaAjustarHoras();
        for (char i = 0; i < 2; i++)CmdLcd(0b00010100);
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1)
      {
        if(minuto == 0) minuto = 60;
        minuto--;
        EscritaAjustarHoras();
        for (char i = 0; i < 2; i++)CmdLcd(0b00010100);
      }
      break;
    case 2:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1)
      {
        hora++;
        if (hora == 24) hora = 0;
        EscritaAjustarHoras();
        CmdLcd(0b00010000);
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1)
      {
        if(hora == 0) hora = 24;
        hora--;
        EscritaAjustarHoras();
        CmdLcd(0b00010000);
      }
      break;

    }

    if (selecionador != temp)
    {
      temp = selecionador;
      if (selecionador == 0) CmdLcd(0b11001011);
      else if (selecionador == 1) CmdLcd(0b11001000);
      else CmdLcd(0b11000101);
    }

    if ((DebBotao(&PORTA, 1, 10, 0)) == 1) break; // Mesmo botao para sair do menu.
    if ((DebBotao(&PORTB, 0, 10, 0)) == 1)
    {
        modoEconomicoAtivado = 1;
        break;
    }
  }
  if(modoEconomicoAtivado == 0)
  {
   CmdLcd(0b00001100); // Desliga Cursor.
   CmdLcd(0x01);       // Limpa display lcd.
   TextoDaInterfacePrincipal();
   TMR0H = 0b11111111; // Necessario para volta rapida a escrita da interface principal.
   TMR0L = 0b11111101; //
   BitSetar(T0CON, 7); // Liga TMR0 e consequentemente TMR1.
  }
  else
  {
   modoEconomicoAtivado = 0;
   ModoEconomico();
  }
}

void EscreverTempoSalveDesejado(char selecionador)
{
  CmdLcd(0x01);
  GeradorFrases("Tempo de Salve:");
  CmdLcd(0b11000011);
  switch(selecionador)
  {
  case 0:
    GeradorFrases("15 minutos");
    CmdLcd(0b11000000);
    EscreverLcd(0b01111111);
    CmdLcd(0b11001111);
    EscreverLcd(0b01111110);
    break;
  case 1:
    GeradorFrases("30 minutos");
    CmdLcd(0b11000000);
    EscreverLcd(0b01111111);
    CmdLcd(0b11001111);
    EscreverLcd(0b01111110);
    break;
  case 2:
    GeradorFrases("60 minutos");
    CmdLcd(0b11000000);
    EscreverLcd(0b01111111);
    CmdLcd(0b11001111);
    EscreverLcd(0b01111110);
    break;
  }
}

void MenuAjusteTempoSalve(void)
{
  DentroDeOutroMenu = TRUE;
  BitLimpar(T1CON,0); // Desliga  TMR1.
  CmdLcd(0x01);
  GeradorFrases("Tempo de Salve:");
  CmdLcd(0b11000000);
  EscreverTempoSalveDesejado(selecionadorTempoSalve);
  while(1)
  {
    if ((DebBotao(&PORTA, 2, 10, 0)) == 1) // Escolha do tempo de salve movimenta a direita.
    {
      selecionadorTempoSalve++;
      if(selecionadorTempoSalve == 3) selecionadorTempoSalve = 0;
      EscreverTempoSalveDesejado(selecionadorTempoSalve);
    }
    if ((DebBotao(&PORTA, 3, 10, 0)) == 1) // Escolha do tempo de salve movimenta a esquerda.
    {
      if(selecionadorTempoSalve == 0) selecionadorTempoSalve = 3;
      selecionadorTempoSalve--;
      EscreverTempoSalveDesejado(selecionadorTempoSalve);
    }
    if ((DebBotao(&PORTB, 2, 10, 0)) == 1) break;
    if ((DebBotao(&PORTB, 0, 10, 0)) == 1) 
    {
        modoEconomicoAtivado = 1;
        break;
    }
  }
  if(modoEconomicoAtivado == 0)
  {
   CmdLcd(0x01); //limpa display lcd
   TextoDaInterfacePrincipal();
   DentroDeOutroMenu = FALSE;
   TMR0H = 0b11111111; // Inicializa o TMR0 com com valor baixo para aparecer o relogio no display
   TMR0L = 0b11111101; // na maneira mais rapida possivel.
  }
  else
  {
   modoEconomicoAtivado = 0;
   ModoEconomico();
  }
}

void SistemaNumericoRelogio(void)
{
    segundo++;           // Cada incremento dessa variavel contator equivale a 1 segundo.
    if (segundo == 60)   // Conversão basica para transformar em um relógio.
    {
      segundo = 0;
      minuto++;
    }
    if (minuto == 60)    // Conversão basica para transformar em um relógio.
    {
      minuto = 0;
      hora++;
      FuncaoQueSalva();  // Todo incremento na hora fara com que salve.
    }
    if (hora == 24) hora = 0;
}

void SalvarParaTempoDesejado(void)
{
  switch (selecionadorTempoSalve)
  {
  case 0:
    if (((minuto != 0) && ((minuto % 15) == 0) && (segundo == 0))) FuncaoQueSalva();
    break;
  case 1:
    if (((minuto != 0) && ((minuto % 30) == 0) && (segundo == 0))) FuncaoQueSalva();
    break;
  case 2:
    break;
  }
}

void MenuResetandoValoresSalvos(void)
{
  DentroDeOutroMenu = TRUE;
  BitLimpar(T1CON,0); // Desliga  TMR1.
  CmdLcd(0x01);
  GeradorFrases("CODIGO SEGURANCA");
  CmdLcd(0b11000110);
  GeradorFrases("****");
  CmdLcd(0b00001110); // Liga Cursor.
  CmdLcd(0b00010000); // Desloca cursor para primeiro digito do codigo segurança.
  unsigned char digitoCodigo = 0, CodigoPrimeiroDigito = 0, CodigoSegundoDigito = 0, CodigoTerceiroDigito = 0, CodigoQuartoDigito = 0;
  while(1)
  {
    if ((DebBotao(&PORTA, 3, 10, 0)) == 1) // Botao LEFT
    {
      if (digitoCodigo == 3) 
      {
          digitoCodigo = 0;
          CmdLcd(0b11001001); // Move para o primeiro digito do codigo.
      }
      else
      {
          digitoCodigo++;
          CmdLcd(0b00010000); // Desloca curso para esquerda.
      }
    }
    if ((DebBotao(&PORTA, 2, 10, 0)) == 1) // Botao RIGHT
    {
      if (digitoCodigo == 0)
      {
          digitoCodigo = 3;
          CmdLcd(0b11000110); // Move para o primeiro digito do codigo.
      }
      else
      {
          digitoCodigo--;
          CmdLcd(0b00010100); // Desloca curso para direita.
      }
    }

    switch (digitoCodigo)  //
    {
    case 0: // Caso do primeiro digito.
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1) // Botão de mover para cima.
      {
        CodigoPrimeiroDigito++;
        if (CodigoPrimeiroDigito == 10) CodigoPrimeiroDigito = 0;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoPrimeiroDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1) // Botão de mover para baixo.
      {
        if(CodigoPrimeiroDigito == 0) CodigoPrimeiroDigito = 9;
        else CodigoPrimeiroDigito--;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoPrimeiroDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      break;
    case 1:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1) // Botão de mover para cima.
      {
        CodigoSegundoDigito++;
        if (CodigoSegundoDigito == 10) CodigoSegundoDigito = 0;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoSegundoDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1) // Botão de mover para baixo.
      {
        if(CodigoSegundoDigito == 0) CodigoSegundoDigito = 9;
        else CodigoSegundoDigito--;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoSegundoDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      break;
    case 2:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1) // Botão de mover para cima.
      {
        CodigoTerceiroDigito++;
        if (CodigoTerceiroDigito == 10) CodigoTerceiroDigito = 0;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoTerceiroDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1) // Botão de mover para baixo.
      {
        if(CodigoTerceiroDigito == 0) CodigoTerceiroDigito = 9;
        else CodigoTerceiroDigito--;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoTerceiroDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      break;
    case 3:
      if ((DebBotao(&PORTA, 4, 10, 0)) == 1) // Botão de mover para cima.
      {
        CodigoQuartoDigito++;
        if (CodigoQuartoDigito == 10) CodigoQuartoDigito = 0;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoQuartoDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      if ((DebBotao(&PORTA, 5, 10, 0)) == 1) // Botão de mover para baixo.
      {
        if(CodigoQuartoDigito == 0) CodigoQuartoDigito = 9;
        else CodigoQuartoDigito--;
        EscreverLcd(ConverterNumeroParaCaracter(CodigoQuartoDigito));
        CmdLcd(0b00010000); // Desloca para esquerda devolta.
      }
      break;
    }
    if ((DebBotao(&PORTB, 4, 10, 0)) == 1) break; // Botao de menu ajuste denovo para sair.
    if ((DebBotao(&PORTB, 0, 10, 0)) == 1)
    {
        modoEconomicoAtivado = 1;
        break;
    }
  }

  if(modoEconomicoAtivado == 0)
  {
   CmdLcd(0b00001100); // Desligado cursor.
   if ((PrimeiroDigitoCodigoSeguranca == CodigoPrimeiroDigito)&& (SegundoDigitoCodigoSeguranca == CodigoSegundoDigito) &&
      (TerceiroDigitoCodigoSeguranca == CodigoTerceiroDigito) && (QuartoDigitoCodigoSeguranca == CodigoQuartoDigito))
   {
    ContadorSalvos = 0;
    for (unsigned char i = 0; i < 32; i++)
    {
     ValoresSalvos.valor_adc[i] = 0;
     ValoresSalvos.minuto[i] = 0;
     ValoresSalvos.hora[i] = 0;
    }
    CmdLcd(0x01);
    GeradorFrases("Apagando Valores");
    CmdLcd(0b11000000);
    for (char i = 0; i <= 15; i++) // Criando digitos no formato preenchido
    {                              // para dar a impressão de uma barra de loading.
      EscreverLcd(0b11111111);
      delay_ms(90);
    }
   }
   else
   {
    CmdLcd(0x01);
    GeradorFrases(">>>> CODIGO <<<<");
    CmdLcd(0b11000000);
    GeradorFrases(">>> INVALIDO <<<");
    delay_ms(500);
    delay_ms(500);
   }
   CmdLcd(0x01); //limpa display lcd
   TextoDaInterfacePrincipal();
   DentroDeOutroMenu = FALSE;
   TMR0H = 0b11111111; // Necessario para volta rapida a escrita da interface principal.
   TMR0L = 0b11111101; //
  }
  else
  {
   modoEconomicoAtivado = 0;
   ModoEconomico();
  }
}

void interrupt INT0_TMR0() 
{                           
  if (BitTestar(INTCON, TMR0IF)) // Flag para sinalizar estou TMR0, temporizador para assemelhar ao clock real.
  {
    BitLimpar(INTCON, TMR0IF); // Status bit- limpar estouro do timer 0
    BitLimpar(T1CON,0); // Desliga TMR1.
    TMR0H = 0b11110000;
    TMR0L = 0b10111101; // Inicializa com valor 61629. em binario  msbyte11110000 lsbyte 10111101 p/contar 3096 com preescaler 1:256 =~ 1 sec
    SistemaNumericoRelogio();  // Incrementa segundo++ e converte para sistema numerico de um relógio.
    SalvarParaTempoDesejado(); // Salva no tempo de salve escolhido.
    if(DentroDeOutroMenu == FALSE)
    {
      EscritaRelogio();
      BitSetar(T1CON,0); // Liga TMR1.
    }
  }

  if (BitTestar(PIR1, TMR1IF)) // Flag para sinilizar estou do TMR1, assim escrevendo valor da conversão A/D no LCD.
  {
    BitLimpar(PIR1, TMR1IF);
    TMR1H = 0b11100110;   // Reinicia Contador TMR1.
    TMR1L = 0b10011011;
    if(DentroDeOutroMenu == FALSE)
    {
        if(valor_adc < 200 )
        {
         CmdLcd(0b11000111);   // String aparecera na linha 1 e coluna 7 somente.
         EscreverLcd('E');
         EscreverLcd('r');
         EscreverLcd('r');
         EscreverLcd('0');
        }
        else
        {
         valor_lux_provisorio = (0.000025428*((double)valor_adc*valor_adc*valor_adc));
         valor_lux_provisorio = ((0.0668*((double)valor_adc*valor_adc))- valor_lux_provisorio);
         valor_lux_provisorio = (valor_lux_provisorio + 17899);
         valor_lux_provisorio = (valor_lux_provisorio - ((double)59.222*(valor_adc)));
         valor_lux = (int)valor_lux_provisorio;
         ConversorStringValorADC(valor_lux); // Procedimento feito para conversão do valor_adc inteiro para o equivalente em string.
         CmdLcd(0b11000111);                 // Valor em string aparecera na linha 1 e coluna 7 somente.
         for (unsigned char i = 0; i < 4; i++) EscreverLcd(valor_adc_string[i]); // Percorre o vetor "vetor_adc_string" colocando string do valor adc no lcd.
        }     
    }
  }
}

void InicializandoSFRs(void)
{
  TRISA = 0xFF;         // PORTA configuradas como entrada (High Impendance).
  TRISB = 0b11110111;         // PORTA configuradas como entrada (High Impendance).
  CCP1CON = 0x00;       // Desligando modo CCP, pinos configurados como digital I/O.
  INTCON = 0b11100000;  // Todas prioridades de interrupções ligadas, INT0 ativado, TMR0 ativado;
  INTCON2= 0x00;        // Interrupção do INT0 na borda de descida.
  T0CON = 0b00000111;   // TMR0 configurado como 16 bit timer, referencia oscilador interno, 1:256 prescaler.
  TMR0H = 0b11111111;   // Inicializa o TMR0 com com valor baixo para aparecer o relogio no display
  TMR0L = 0b11111101;   // na maneira mais rapida possivel.
  T1CON = 0b10110000;   // TMR1 configurado como 16 bit timer, referencia oscilador interno, 1:8 prescaler.
  TMR1H = 0b11100110;   // Inicializa o TMR1, valor do contador do TMR1 com 59035 para q tenha somente incremento de 6500
  TMR1L = 0b10011011;   // assim tendo 50 ms de tempo de cada estouro.
  BitSetar(PIE1,0);     // Habilita interrupção do TMR1.
  BitSetar(PORTB, 3);   // Ligando backlight.
}

void main(void)
{
  InicializandoSFRs(); //Inicializações do Sistema.
  InicializandoLCD();
  InicializandoADC();
  QuantidadeSalves();
  TextoDeEntrada();
  while (1)
  {
    for (unsigned int i = 0; i < 65000; i++)         // Funções feitas para evitar o efeito de pooling da leitura ADC.
    {
      if ((i % 1250) == 0) valor_adc = LeituraADC(); // Somente lerá a entrada ADC a cada 15,5 ms.
      if ((DebBotao(&PORTA, 1, 10, 0)) == 1) MenuAjustarRelogio();          // Monitora o botao de ajuste de relógio.
      if ((DebBotao(&PORTB, 1, 10, 0)) == 1) MenuListaDeValoresSalvo();     // Monitora o botao de ajuste de relógio.
      if ((DebBotao(&PORTB, 2, 10, 0)) == 1) MenuAjusteTempoSalve();    // Monitora o botao de ajuste de tempo de salve.
      if ((DebBotao(&PORTB, 4, 10, 0)) == 1) MenuResetandoValoresSalvos();  // Monitora o botao de ajuste de tempo de salve.
      if ((DebBotao(&PORTB, 0, 10, 0)) == 1) ModoEconomico();
    }
  }
}

//==================================================================================================================
