/*
 * File:   Funcoes_LCD.h
 * Author: Lucas
 */

#ifndef FUNCOES_LCD_Lui
#define	FUNCOES_LCD_Lui

void Reset_Obrigatorio_LCD(void);
void Inicializando_LCD(void);
void Cmd_LCD(char cmd);
void Escrever_Lcd(char caracter);
void Gerador_Frases(const unsigned char vetorFrase[]);
char ConverterNumeroParaCaracter(char numero);
void Converter_para_String(unsigned int valor_int, unsigned char digitos);

#endif

