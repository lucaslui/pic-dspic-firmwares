/* 
 * File:   LibraryLui.h
 * Author: Lucas
 *
 * Created on 30 de Março de 2014, 12:00
 */

#ifndef FUNCOES_BASICAS_H
#define	FUNCOES_BASICAS_H

void delay_ms(int ms);
char DebBotao(volatile near unsigned char *PORT, char nBit, int deboucingTime, char changeState);


#endif	/* LIBRARYLUI_H */

