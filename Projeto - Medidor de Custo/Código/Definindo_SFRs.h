/* ==================================================================================================================

	Arquivo: Definindo_SFRs.h 
	Descrição: Mapeando registradores com ponteiros duplos, usando diretivas do pré-processador (DEFINE).

====================================================================================================================*/

#ifndef DEFININDO_SFRS_LUI /* DEFININDO_SFRS_H */
#define	DEFININDO_SFRS_LUI /* DEFININDO_SFRS_H */

#define TRISA   (*(volatile near unsigned char*)0xF92)
#define TRISB   (*(volatile near unsigned char*)0xF93)
#define TRISC   (*(volatile near unsigned char*)0xF94)
#define TRISD   (*(volatile near unsigned char*)0xF95)
#define TRISE   (*(volatile near unsigned char*)0xF96)

#define PORTA   (*(volatile near unsigned char*)0xF80)
#define PORTB   (*(volatile near unsigned char*)0xF81)
#define PORTC   (*(volatile near unsigned char*)0xF82)
#define PORTD   (*(volatile near unsigned char*)0xF83)
#define PORTE   (*(volatile near unsigned char*)0xF84)

#define INTCON   (*(volatile near unsigned char*)0xFF2)
#define INTCON2  (*(volatile near unsigned char*)0xFF1)

#define ADCON0   (*(volatile near unsigned char*)0xFC2)
#define ADCON1   (*(volatile near unsigned char*)0xFC1)
#define ADCON2   (*(volatile near unsigned char*)0xFC0)
#define ADRESL   (*(volatile near unsigned char*)0xFC3)
#define ADRESH   (*(volatile near unsigned char*)0xFC4)

#define CMCON   (*(volatile near unsigned char*)0xFB4)
#define CCP2CON (*(volatile near unsigned char*)0xFBA)
#define OSCCON  (*(volatile near unsigned char*)0xFD3)
#define PIE1    (*(volatile near unsigned char*)0xF9D)
#define PIR1    (*(volatile near unsigned char*)0xF9E)

#define T0CON   (*(volatile near unsigned char*)0xFD5)
#define TMR0L   (*(volatile near unsigned char*)0xFD6)
#define TMR0H   (*(volatile near unsigned char*)0xFD7)

#define T1CON   (*(volatile near unsigned char*)0xFCD)
#define TMR1L   (*(volatile near unsigned char*)0xFCE)
#define TMR1H   (*(volatile near unsigned char*)0xFCF)

#endif	/* DEFININDO_SFRS_H */

// ==================================================================================================================

