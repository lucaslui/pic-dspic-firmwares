/* =============================================================================
 * 
 *  Arquivo: Code_Codec_WM8510.c
 *  Portável: Não portável.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#ifndef _WM8510CODECDRV_H_
#define  _WM8510CODECDRV_H_

#if defined(__dsPIC33F__)
#include "p33fxxxx.h"
#elif defined(__dsPIC30F__)
#include "p30fxxxx.h"
#endif

#define WM8510DRV_DCI_MASTER 				1
#define WM8510DRV_CODEC_FRAME 				128
#define WM8510DRV_DRV_BUFFER_SIZE			(WM8510DRV_CODEC_FRAME * 4)
#define WM8510DRV_DCI_INTERRUPT_PRIORITY	4
#define WM8510DRV_I2CBAUD					272
#define WM8510DRV_I2C_ADDR					0b00110100
#define WM8510DRV_DCICON1VAL				0x0700
#define WM8510DRV_DCICON2VAL				0x000F
#define WM8510DRV_TSCONVAL					0x1
#define WM8510DRV_RSCONVAL					0x1

/* These value applicable only if DCI is master	*/
#define WM8510DRV_SAMPLE_RATE			8000 	
#define WM8510DRV_CODEC_DRV_FCY			40000000
#define WM8510DRV_DCI_BRG_VALUE 		8

#define	WM8510DRV_I2C_TRISSCL _TRISG2
#define WM8510DRV_I2C_TRISSDA _TRISG3

typedef union uCommandValueWord
{
	int wordValue;
	unsigned char bytes[2];
}CommandValueWord;	

typedef struct sWM8510Handle
{
	int * 		inputBuffer1;				/* Ping Pong Input Buffer 1	*/
	int * 		inputBuffer2;				/* Ping Pong Input Buffer 2	*/
	int *		outputBuffer1;				/* Ping Pong Output Buffer 1 */	
	int *		outputBuffer2;				/* Ping Pong Output Buffer 2 */
	volatile int 	currentSampleIndex;		/* Tracks the sample being processed */
	volatile int 	currentFrameSize;		/* The size of the current frame being processed - 1	*/
	volatile int 	newFrameSize;           /* The size of the new frame */
	volatile int *	activeInputBuffer;		/* The active ping pong input buffer */
	volatile int *	activeOutputBuffer;		/* The active ping pong output buffer */
	volatile int  	statusFlag;				/* Tracks the state of the driver */

}WM8510Handle;

/* Mask definition for reading and modifying statusFlag */

#define WM8510DRV_GET_READ_BUSY		0x0001
#define WM8510DRV_SET_READ_BUSY		0x0001
#define WM8510DRV_CLR_READ_BUSY		0xFFFE

#define WM8510DRV_GET_WRITE_BUSY	0x0002
#define WM8510DRV_SET_WRITE_BUSY	0x0002
#define WM8510DRV_CLR_WRITE_BUSY	0xFFFD

#define WM8510DRV_GET_BUFFER_IND	0x0004
#define WM8510DRV_TGL_BUFFER_IND	0x0004

/* WM8510 Codec Register Defines	*/
#define WM8510_SOFTWARE_RESET	0
#define WM8510_POWER_MGMT1      1
#define WM8510_POWER_MGMT2      2
#define WM8510_POWER_MGMT3      3
#define WM8510_AUDIO_INTERFACE	4
#define WM8510_COMPANDING_CTRL	5
#define WM8510_CLOCKGEN_CTRL	6
#define WM8510_ADDITIONAL_CTRL	7
#define WM8510_GPIO_STUFF		8
#define WM8510_DAC_CTRL			10
#define WM8510_DAC_VOLUME		11
#define WM8510_ADC_CONTROL		14
#define WM8510_ADC_DIGITAL_VOL	15
#define WM8510_DAC_LIMITER1		24
#define WM8510_DAC_LIMITER2		25
#define WM8510_NOTCH_FILTER1	27
#define WM8510_NOTCH_FILTER2	28
#define WM8510_NOTCH_FILTER3	29
#define WM8510_NOTCH_FILTER4	30
#define WM8510_ALC_CONTROL1     32
#define WM8510_ALC_CONTROL2     33
#define WM8510_ALC_CONTROL3     34
#define WM8510_NOISE_GATE		35
#define WM8510_PLL_N			36
#define WM8510_PLL_K1			37
#define WM8510_PLL_K2			38
#define WM8510_PLL_K3			39
#define WM8510_ATTEN_CTRL		40
#define WM8510_INPUT_CTRL		44
#define WM8510_INP_PGA_CTRL		45
#define WM8510_ADC_BOOST_CTRL	47
#define WM8510_OUTPUT_CTRL		49
#define WM8510_SPK_MIXER_CTRL	50
#define WM8510_SPK_VOLUME_CTRL	54
#define WM8510_MONO_MIXER_CTRL	56

void 	Init_Codec_WM8510 (WM8510Handle * pHandle, int * buffer);
void 	WM8510Start		  (WM8510Handle * pHandle);
void 	WM8510Stop		  (WM8510Handle * pHandle);
void 	WM8510Read		  (WM8510Handle * pHandle, int * source, int size);
void 	WM8510Write		  (WM8510Handle * pHandle, int * dest, int size);
int 	WM8510IOCtl		  (WM8510Handle * pHandle,int command, void * value);
int 	WM8510IsReadBusy  (WM8510Handle * pHandle);
int		WM8510IsWriteBusy (WM8510Handle * pHandle);

/* These are helper functions. Use if you need to	*/

void WM8510SampleRate8KConfig(WM8510Handle *codecHandle);
void WM8510SampleRate16KConfig(WM8510Handle *codecHandle);

#endif
