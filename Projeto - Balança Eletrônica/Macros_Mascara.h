/* ==================================================================================================================
*
* Arquivo: Macros_Mascara.h
* Descrição: Macros para mascarar os bytes, são feitas operações bit a bit, para setar, limpar, inverter, e testar bits 
* especificos de um byte.
*
*====================================================================================================================*/

#ifndef MACROS_MASCARA_H /* MACROS_MASCARA_H */
#define	MACROS_MASCARA_H /* MACROS_MASCARA_H */

#define BitSetar(arg,nBit)    ((arg) |=  (1<<nBit))
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit))
#define BitInverter(arg,nBit) ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

#endif	/* MACROS_MASCARA_H */

// ======================================================================================================================
