/* =============================================================================
 * 
 *  Arquivo: Code_Memory_SST25VF040.c
 *  Portável: Não portável.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#include "Code_Memory_SST25VF040.h"

SST25LF040AHandle * thisFlashMemory;

int variavel_c = 0;

void Init_Memory_SST25LF040A(SST25LF040AHandle* pHandle,  char * buffer)
{
	#if (FLASH_VDD_REGULATOR)
		long vddRegWakeUpDelay;
		REGULATOR_CONTROL_ANPCFG = 1;
		REGULATOR_CONTROL_TRIS	= 0;
		REGULATOR_CONTROL_LAT = 1;
		for(vddRegWakeUpDelay = 0;
		vddRegWakeUpDelay < REGULATOR_WAKE_UP_DELAY;
		vddRegWakeUpDelay++)
		Nop();
	#endif
	
	thisFlashMemory 		= pHandle;
	thisFlashMemory->buffer	= buffer;
	
	SPI1CON1bits.DISSCK	 	= 0;    /* Internal clock enabled      */
	SPI1CON1bits.DISSDO 	= 0;    /* SDO controlled by module	*/
	SPI1CON1bits.MODE16	 	= 0;    /* Byte wide communication     */
	SPI1CON1bits.SMP		= 1;    /* Input sampled at end of data time */
	SPI1CON1bits.CKE		= 1;    /* Data changes from high to low clock*/
	SPI1CON1bits.CKP		= 0;
	SPI1CON1bits.MSTEN		= 1;	/* Master mode enabled			*/
	SPI1CON1bits.SSEN		= 0;	/* SS pin is controlled by port		*/
	
	SPI1CON1bits.SPRE		=  SST25LF040ADRV_SPI1_SECONDARY_PRESCALE;
	SPI1CON1bits.PPRE		=  SST25LF040ADRV_SPI1_PRIMARY_PRESCALE;
	
	SPI1CON2bits.FRMEN		= 0;	/* Framed SPIx is disabled		*/
	
	SST25LF040A_CHIP_SELECT_ANPORT	= 1;	/* SS pin is analog by default. Make it	*/
	SST25LF040A_CHIP_SELECT_TRIS 	= 0;	/* Make SS1/RB2 output			*/
	SST25LF040A_CHIP_SELECT 		= 1;
	SPI1STATbits.SPISIDL 			= 0;
	
	T4CON 	= 0;		/* Timer used for timing the program pulse	*/
	TMR4 	= 0;
	PR4 	=  SST25LF040ADRV_TBP_PERIOD;
	_T4IF = 0;
	_T4IE = 1;
}


void SST25LF040AStart(SST25LF040AHandle* pHandle)
{
	_SPI1IE = 1;						/* Enable the interrupts		*/
	SPI1STATbits.SPIEN		= 1;		/* Enable the module		*/
	pHandle->statusFlag		= 0;
	pHandle->statusWord 	= 0;
	pHandle->dataIndex		= 0;
	pHandle->dataCount 		= 0;
	SST25LF040A_CHIP_SELECT = 1;
}	

int	SST25LF040AIsBusy(SST25LF040AHandle* pHandle)
{
	if ((pHandle->statusFlag & SST25LF040ADRV_GET_IS_WRITING) != 0)
		return(1);
	
	if ((pHandle->statusFlag & SST25LF040ADRV_GET_IS_READING) != 0)
		return(1);
	
	SST25LF040A_CHIP_SELECT = 0;

	SPI1BUF 	= SST25LF040A_READ_STATUS_CODE;		/* Read status register				*/	
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;

	SPI1BUF=0x0;					/* Output dummy words for SPI clock	*/
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;

	SST25LF040A_CHIP_SELECT = 1;
	return((pHandle->statusWord) & 0x01);
}	


void SST25LF040AWrite(SST25LF040AHandle* pHandle,long address,  char* data,  int count)
{	
	int index;
	unsigned char * dest = (unsigned char *) pHandle->buffer;
	
	if(count >  SST25LF040ADRV_BUFFER_SIZE)
	{
		count =SST25LF040ADRV_BUFFER_SIZE;		/* Size cannot be greater than the buffer size	*/
	}
	
	for(index =0; index < count; index ++)		/* Load the data portion of the array	*/
	{
		dest[index] = (unsigned char) data[index];
	}
	
	pHandle->statusFlag |= SST25LF040ADRV_SET_IS_WRITING;	/* Set the write flag 			*/
	pHandle->dataCount 	= count;								/* load the count				*/
	pHandle->writeAddress.fullAddress = address;				/* This is the address to write to.	*/
	pHandle->dataIndex = 0;
	pHandle->statusWord &= SST25LF040ADRV_CLR_STATE_COUNTER;
	
	SST25LF040A_CHIP_SELECT = 0;					/* Enable the chip		*/
	SPI1BUF = SST25LF040A_PROGRAM_CODE;				/* send the program code 		*/						
}

void SST25LF040ARead(SST25LF040AHandle* pHandle, long address, char* data,int count)
{
	char upperAddress,highAddress,lowAddress;

	
	upperAddress = (char)((address & 0x00FF0000) >> 16);	/* Address A23-A16	*/
	highAddress = (char)((address & 0x0000FF00) >> 8);	 /* Address A15-A8	*/
	lowAddress = (char)(address & 0x000000FF);			 /* Address A7-A0		*/
	
	if(count >  SST25LF040ADRV_BUFFER_SIZE)
	{
		pHandle->dataCount = SST25LF040ADRV_BUFFER_SIZE;	/* Size cannot be greater than the buffer size	*/
	}
	else
	{
		pHandle->dataCount = (unsigned int)count;
	}
	
	pHandle->readData = data;
	pHandle->dataIndex = 0;
	
	SST25LF040A_CHIP_SELECT = 0;
	SPI1BUF = SST25LF040A_READ_CODE;
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;

	SPI1BUF = upperAddress;
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
	SPI1BUF = highAddress;
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
	SPI1BUF = lowAddress;
	while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
	pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
	SPI1BUF = 0;
	pHandle->statusFlag |= SST25LF040ADRV_SET_IS_READING;
	
}	
	
void SST25LF040AIOCtl(SST25LF040AHandle* pHandle,int command, void * data)
{
	if(command == SST25LF040ADRV_BLOCK_ERASE)
	{
		char * address = (char *)data;
		SST25LF040A_CHIP_SELECT = 0;
		SPI1BUF = SST25LF040A_BLOCK_ERASE_CODE;
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		SPI1BUF = address[2];
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		SPI1BUF = address[1];
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		SPI1BUF = address[0];
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		SST25LF040A_CHIP_SELECT = 1;
	}
		
	
	if(command == SST25LF040ADRV_READ_FLASH_MFG_ID)
	{
		int count;
		char * target = (char*)data;
		
		SST25LF040A_CHIP_SELECT = 0;
		SPI1BUF = SST25LF040A_READ_CHIPID_CODE;
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		
		for(count = 0;count < 3; count ++)
		{
			SPI1BUF = 0;
			while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
			pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		}
		
		for(count = 0;count <= 3; count ++)
		{
			SPI1BUF = 0;
			while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
			pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
			target[count] = pHandle->statusWord;
		}
		
		SST25LF040A_CHIP_SELECT = 1;
	}
	else if(command == SST25LF040ADRV_READ_STATUS)
	{	
		char * target = (char*)data;
		
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_READ_STATUS_CODE;		/* Read status register				*/	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SPI1BUF=0x0;					/* Output dummy words for SPI clock	*/
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		*target = pHandle->statusWord;
	
		SST25LF040A_CHIP_SELECT = 1;
	}
	else if(command == SST25LF040ADRV_WRITE_STATUS_ENABLE)
	{
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_ENABLE_WRITE_STATUS_CODE;		/* Read status register				*/	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SST25LF040A_CHIP_SELECT = 1;
	}
	else if(command == SST25LF040ADRV_WRITE_ENABLE)
	{
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_WRITE_ENABLE_CODE; /* Read status register */	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SST25LF040A_CHIP_SELECT = 1;
	}
    else if(command == SST25LF040ADRV_DISABLE_SO_DURING_AAI)
	{
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_DISABLE_SO_DURING_AAI; /* Read status register */	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SST25LF040A_CHIP_SELECT = 1;
	}    
	else if(command == SST25LF040ADRV_WRITE_STATUS)
	{
		char * value = (char *)data;
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_WRITE_STATUS_CODE;		/* Read status register				*/	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SPI1BUF= *value;					/* Output dummy words for SPI clock	*/
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
	
		SST25LF040A_CHIP_SELECT = 1;
		
	}
	else if(command == SST25LF040ADRV_CHIP_ERASE)
	{
		SST25LF040A_CHIP_SELECT = 0;
	
		SPI1BUF = SST25LF040A_CHIP_ERASE_CODE;		/* Read status register				*/	
		while((pHandle->statusFlag & SST25LF040ADRV_GET_RW_DONE ) == 0);
		pHandle->statusFlag &= SST25LF040ADRV_CLR_RW_DONE;
		
		SST25LF040A_CHIP_SELECT = 1;
	}
}

void __attribute__((__interrupt__,no_auto_psv)) _SPI1Interrupt(void)
{
	int statusFlag;
	_SPI1IF = 0;
		
	if((thisFlashMemory->statusFlag & SST25LF040ADRV_GET_IS_WRITING) != 0)
	{
		/* The driver is performing a write operation	
		 * Perform a dummy read to prevent a read overflow
		 * condition */
		thisFlashMemory->statusWord = SPI1BUF;
		statusFlag = thisFlashMemory->statusFlag & SST25LF040ADRV_GET_STATE_COUNTER;
		
		switch(statusFlag){
			case 0:
				SPI1BUF = thisFlashMemory->writeAddress.addressByte[2];
				thisFlashMemory->statusFlag ++;
				break;
			case 1:
				SPI1BUF = thisFlashMemory->writeAddress.addressByte[1];
				thisFlashMemory->statusFlag++;
				break;
			case 2:
				SPI1BUF = thisFlashMemory->writeAddress.addressByte[0];
				thisFlashMemory->statusFlag ++;
				break;
			case 3:
                SPI1BUF = thisFlashMemory->buffer[thisFlashMemory->dataIndex++];
				thisFlashMemory->statusFlag ++;
				break;
			case 4:
				SPI1BUF = thisFlashMemory->buffer[thisFlashMemory->dataIndex++];
				thisFlashMemory->statusFlag ++;
				break;            
            case 5:
				SST25LF040A_CHIP_SELECT = 1;	
				TMR4 = 0;
				T4CONbits.TON = 1;
				break;
			case 6:
                SPI1BUF = thisFlashMemory->buffer[thisFlashMemory->dataIndex++];
				thisFlashMemory->statusFlag++;
				break;
            case 7:
                SPI1BUF = thisFlashMemory->buffer[thisFlashMemory->dataIndex++];
				thisFlashMemory->statusFlag--;
                thisFlashMemory->statusFlag--;
				break;
			case 8:
				SST25LF040A_CHIP_SELECT = 1;
				thisFlashMemory->dataCount = 0;
				thisFlashMemory->dataIndex = 0;
				thisFlashMemory->statusFlag &= SST25LF040ADRV_CLR_STATE_COUNTER;
				thisFlashMemory ->statusFlag &= SST25LF040ADRV_CLR_IS_WRITING;
				break;
			}
	}
	else if((thisFlashMemory->statusFlag & SST25LF040ADRV_GET_IS_READING) != 0)
	{
		/* The driver is performing a read operation	
		 * Perform a dummy read to prevent a read overflow
		 * condition */
		
		(thisFlashMemory->readData)[thisFlashMemory->dataIndex] = SPI1BUF;
		thisFlashMemory->dataIndex ++;
		if(thisFlashMemory->dataIndex < thisFlashMemory->dataCount)
		{
			SPI1BUF = 0;
		}else{
		
			thisFlashMemory->statusFlag &= SST25LF040ADRV_CLR_IS_READING;
			thisFlashMemory->dataCount = 0;
			thisFlashMemory->dataIndex = 0;
			SST25LF040A_CHIP_SELECT = 1;
		}
	}	
	else
	{
		thisFlashMemory->statusFlag |= SST25LF040ADRV_SET_RW_DONE;
		thisFlashMemory->statusWord = SPI1BUF;
	}
}
	
void __attribute__((__interrupt__,no_auto_psv)) _T4Interrupt(void)
{
	/* At this point the program pulse period is complete	
	 * Stop the timer. Check if the last data word has been sent.
	 * Yes then send  WRDI code and clear the 
	 * IS_WRITING flag	*/
	
	_T4IF = 0;
	T4CONbits.TON = 0;	
	
	if(thisFlashMemory->dataIndex >= thisFlashMemory->dataCount)
	{
		SST25LF040A_CHIP_SELECT = 0;
		SPI1BUF = SST25LF040A_WRITE_DISABLE_CODE;
        thisFlashMemory->statusFlag &= SST25LF040ADRV_CLR_STATE_COUNTER;
		thisFlashMemory->statusFlag += 8;
	}
	else
	{
		SST25LF040A_CHIP_SELECT = 0;
		SPI1BUF =  SST25LF040A_PROGRAM_CODE;
		thisFlashMemory->statusFlag++;
	}
}	
