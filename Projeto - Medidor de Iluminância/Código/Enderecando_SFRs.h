/* 
 * File:   Endereçando_SFRs.h
 * Author: Lucas
 *
 * Created on 30 de Março de 2014, 12:25
 */

#ifndef ENDERECANDO_SFRS_H
#define	ENDERECANDO_SFRS_H

#define PORTA   (*(volatile near unsigned char*)0xF80) // CONFORME SPECIAL REGISTER MAPS foi pego todos os endereços dos SFR's e identificados
#define PORTB   (*(volatile near unsigned char*)0xF81) // de forma real ao seus nomes.
#define PORTD   (*(volatile near unsigned char*)0xF83) 
#define PORTE   (*(volatile near unsigned char*)0xF84)

#define TRISA   (*(volatile near unsigned char*)0xF92) // SFR's utilizados para modificar as PORTAS como Input ou Output
#define TRISB   (*(volatile near unsigned char*)0xF93)
#define TRISD   (*(volatile near unsigned char*)0xF95)
#define TRISE   (*(volatile near unsigned char*)0xF96)

#define CCP1CON (*(volatile near unsigned char*)0xFBD) // SFR's utilizados para deixar desligado o modulo ccp

#define ADCON0  (*(volatile near unsigned char*)0xFC2) // SFR's para a conversão A/D
#define ADCON1  (*(volatile near unsigned char*)0xFC1)
#define ADRESL  (*(volatile near unsigned char*)0xFC3)
#define ADRESH  (*(volatile near unsigned char*)0xFC4)

#define INTCON  (*(volatile near unsigned char*)0xFF2) // SFR's para ligar interrupção (INT0), e ligar interruoção do TIMER0
#define INTCON2 (*(volatile near unsigned char*)0xFF1)
#define RCON    (*(volatile near unsigned char*)0xFD0)
#define PIR1    (*(volatile near unsigned char*)0xF9E)
#define PIE1    (*(volatile near unsigned char*)0xF9D)


#define TMR0L   (*(volatile near unsigned char*)0xFD6) // SFR's usador para config e monitoramento do TIMER0
#define TMR0H   (*(volatile near unsigned char*)0xFD7)
#define T0CON   (*(volatile near unsigned char*)0xFD5)
#define TMR1L   (*(volatile near unsigned char*)0xFCE) // SFR's usador para config e monitoramento do TIMER1
#define TMR1H   (*(volatile near unsigned char*)0xFCF)
#define T1CON   (*(volatile near unsigned char*)0xFCD)


#endif	/* ENDEREÇANDO_SFRS_H */

