/* ==================================================================================================================
	Arquivo: SFR.h
	Descrição: Mapeando registradores com ponteiros duplos, usando diretivas do pré-processador (DEFINE).
====================================================================================================================*/

#ifndef DEFININDO_SFRS_LUI /* DEFININDO_SFRS_H */
#define	DEFININDO_SFRS_LUI /* DEFININDO_SFRS_H */

#define TRISA   (*(volatile near unsigned char*)0x85)
#define TRISB   (*(volatile near unsigned char*)0x86)

#define PORTA   (*(volatile near unsigned char*)0x05)
#define PORTB   (*(volatile near unsigned char*)0x06)

#define INTCON  (*(volatile near unsigned char*)0x0B)
#define PIE1    (*(volatile near unsigned char*)0x8C)

#define CCP1CON (*(volatile near unsigned char*)0x17)
#define CMCON   (*(volatile near unsigned char*)0x01)
#define PCON    (*(volatile near unsigned char*)0x8E)
#define OPTION_REG  (*(volatile near unsigned char*)0x81)


/*#define T0CON   (*(volatile near unsigned char*)0xFD5)
#define TMR0L   (*(volatile near unsigned char*)0xFD6)
#define TMR0H   (*(volatile near unsigned char*)0xFD7)

#define T1CON   (*(volatile near unsigned char*)0xFCD)
#define TMR1L   (*(volatile near unsigned char*)0xFCE)
#define TMR1H   (*(volatile near unsigned char*)0xFCF)
*/
#endif	/* DEFININDO_SFRS_H */

// ==================================================================================================================

