/* =============================================================================
 * 
 *  Arquivo: Code_Compresses_G711.h
 *  Portável: Não portável.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#ifndef	__G711_H__
#define	__G711_H_

#define ZEROTRAP    /* turn on the trap as per the MIL-STD */
#define BIAS 0x84   /* define the add-in bias for 16 bit samples */
#define CLIP 32635

extern void G711Lin2Ulaw (int  * source, char * destination, int size);
extern void G711Ulaw2Lin (char * source, int  * destination, int size);
extern void G711Lin2Alaw (int  * source, char * destination, int size);
extern void G711Alaw2Lin (char * source, int  * destination, int size);

#endif
