/* ==================================================================================================================
	Arquivo: Funcoes_LCD.h
	Descrição: Arquivo HEADER para administração das funções que serão passadas.
 ==================================================================================================================*/

#ifndef FUNCOES_LCD_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
#define	FUNCOES_LCD_Lui /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

const char LIMPAR_DISPLAY = 0x01;
const char SEGUNDA_LINHA = 0xC0;

void Reset_Obrigatorio_LCD(void);
void Inicializando_LCD(void);
void Cmd_LCD(char cmd);
void Escrever_Lcd(char caracter);
void Gerador_Frases(const unsigned char vetorFrase[]);
char ConverterNumeroParaCaracter(char numero);
void Converter_para_String(unsigned int valor_int, unsigned char digitos);

#endif /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
