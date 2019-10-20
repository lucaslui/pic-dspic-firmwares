/* ==================================================================================================================
	Arquivo: CodigoLCD.h
	Descrição: Arquivo HEADER para administração das funções que serão passadas.
 ==================================================================================================================*/

#ifndef FUNCOES_LCD_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
#define	FUNCOES_LCD_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

void Reset_Obrigatorio_LCD(void);
void Inicializando_LCD(void);
void Cmd_LCD(char cmd);
void Escrever_Lcd(char caracter);
void Gerador_Frases(const unsigned char vetorFrase[]);
char ConverterNumeroParaCaracter(char numero);
void Converter_para_String(unsigned int valor_int, unsigned char digitos);

#endif /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */