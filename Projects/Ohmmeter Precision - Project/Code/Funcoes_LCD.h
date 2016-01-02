/* ==================================================================================================================
	Arquivo: Funcoes_LCD.h
	Descrição: Arquivo HEADER para administração das funções que serão passadas.
 ==================================================================================================================*/

#ifndef Funcoes_LCD /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */
#define	Funcoes_LCD /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

#define LimpaDisplay 0x01
#define SegundaLinhaPrimeiraColuna 0xC0

void Inicializando_LCD(void);
void Cmd_LCD(char cmd);
void Escrever_LCD(char caracter);
void Gerador_Frases(const unsigned char vetorFrase[]);
char Converter_Numero_Para_Caracter(char numero);
void Converter_Para_String(unsigned int valor_int, unsigned char digitos);

#endif /* PROTÓTIPOS PARA A PASSAGEM DE FUNÇÕES */

//==================================================================================================================