/* =============================================================================
 * 
 *  Arquivo: Code_Memory_SST25VF040.h
 *  Portável: Não portável.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#ifndef __SST25LF040ADRV_H__
#define __SST25LF040ADRV_H__

#include <p33FJ256GP506.h>

#define SST25LF040ADRV_FCY							40000000
#define SST25LF040ADRV_SPI1_PRIMARY_PRESCALE		0x2
#define SST25LF040ADRV_SPI1_SECONDARY_PRESCALE		0x6
#define SST25LF040ADRV_BUFFER_SIZE					128
#define SST25LF040ADRV_REALLY_LAST_ADDRESS			0x7FFFF

// Tempo do período TBP 20usec	
#define SST25LF040ADRV_TBP_PERIOD					0x320   

// Endereço final da metade da memória utilizável (524.287 - 131.072 = 393.215 (0x5FFFF)
#define SST25LF040ADRV_MIDDLE_LAST_ADDRESS			0x4FFFF

#define FLASH_VDD_REGULATOR	1

#if (FLASH_VDD_REGULATOR)
	#define REGULATOR_CONTROL_TRIS		TRISBbits.TRISB5
	#define REGULATOR_CONTROL_ANPCFG	AD1PCFGLbits.PCFG5
	#define REGULATOR_CONTROL_LAT		LATBbits.LATB5
	#define REGULATOR_WAKE_UP_DELAY     0x2000
#endif

typedef union u_SST25LF040Address
{
	char addressByte[4];
	long fullAddress;
}SST25LF040Address;	

typedef struct s_SST25LF040AHandle
{
	volatile char * buffer;
	volatile int statusWord;
	volatile int statusFlag;
	volatile int dataCount;
	volatile int dataIndex;
	volatile char* readData;
	SST25LF040Address writeAddress;
	
}SST25LF040AHandle;	

//#define SST25LF040ADRV_CLR_STATE_COUNTER 0xFFF8
#define SST25LF040ADRV_CLR_STATE_COUNTER 0xFFF0
#define SST25LF040ADRV_GET_STATE_COUNTER 0x000F

#define SST25LF040ADRV_SET_RW_DONE		0x0008
#define SST25LF040ADRV_CLR_RW_DONE 		0xFFF7
#define SST25LF040ADRV_GET_RW_DONE      0x0008

#define SST25LF040ADRV_SET_IS_READING	0x0010
#define SST25LF040ADRV_CLR_IS_READING	0xFFEF
#define SST25LF040ADRV_GET_IS_READING	0x0010

#define SST25LF040ADRV_SET_IS_WRITING	0x0020
#define SST25LF040ADRV_CLR_IS_WRITING	0xFFDF
#define SST25LF040ADRV_GET_IS_WRITING	0x0020

/* These are the commands that can be used with 
 * ioctl functions */
 
#define SST25LF040ADRV_CHIP_ERASE			0x1
#define SST25LF040ADRV_READ_FLASH_MFG_ID	0x2
#define SST25LF040ADRV_READ_STATUS			0x3
#define SST25LF040ADRV_WRITE_ENABLE			0x4
#define SST25LF040ADRV_WRITE_STATUS         0x5
#define SST25LF040ADRV_WRITE_DISABLE		0x6
#define SST25LF040ADRV_SECTOR_ERASE         0x7
#define SST25LF040ADRV_BYTE_PROGRAM         0x8
#define SST25LF040ADRV_BLOCK_ERASE			0x9
#define SST25LF040ADRV_WRITE_STATUS_ENABLE	0xA
#define SST25LF040ADRV_DISABLE_SO_DURING_AAI 0xB

//#define SST25LF040A_PROGRAM_CODE				0xAF
#define SST25LF040A_PROGRAM_CODE				0xAD
#define SST25LF040A_BYTE_PROGRAM				0x2
#define SST25LF040A_BLOCK_ERASE_CODE			0x52
#define SST25LF040A_READ_CODE					0x3
#define SST25LF040A_CHIP_ERASE_CODE				0x60
#define SST25LF040A_READ_CHIPID_CODE			0x90
#define SST25LF040A_READ_STATUS_CODE			0x5
#define SST25LF040A_WRITE_ENABLE_CODE			0x6
#define SST25LF040A_WRITE_DISABLE_CODE			0x4
#define SST25LF040A_SECTOR_ERASE_CODE			0x20
#define SST25LF040A_ENABLE_WRITE_STATUS_CODE	0x50
#define SST25LF040A_WRITE_STATUS_CODE			0x1
#define SST25LF040A_DISABLE_SO_DURING_AAI       0x80

#define SST25LF040A_CHIP_SELECT_TRIS		TRISBbits.TRISB2 
#define SST25LF040A_CHIP_SELECT				LATBbits.LATB2 	
#define SST25LF040A_CHIP_SELECT_ANPORT		AD1PCFGLbits.PCFG2

void 	Init_Memory_SST25LF040A	(SST25LF040AHandle* pHandle, char * buffer);
void 	SST25LF040AWrite		(SST25LF040AHandle* pHandle, long address,  char* data,   int count);
void	SST25LF04SST25LF040AWrite0ARead		(SST25LF040AHandle* pHandle, long address,  char* data,int count);
void    SST25LF040ARead     (SST25LF040AHandle* pHandle, long address, char* data,int count);
void 	SST25LF040AIOCtl	(SST25LF040AHandle* pHandle, int command, void *data);
int		SST25LF040AIsBusy	(SST25LF040AHandle* pHandle);
void 	SST25LF040AStart	(SST25LF040AHandle* pHandle);

#endif
