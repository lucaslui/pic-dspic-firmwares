/* =============================================================================
 * 
 *  Arquivo: Code_Codec_WM8510.c
 *  Portável: Não portável.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#include "Code_Codec_WM8510.h"

WM8510Handle *thisWM8510Codec;

void Init_Codec_WM8510(WM8510Handle * pHandle, int * pBuffer)
{
	int cleanIt;
	
	DCICON1bits.DCIEN = 0;	// Disable these modules before configuring them
	I2C1CONbits.I2CEN = 0;
	
	I2C1CONbits.I2CSIDL = 0;        // I2C is running during idle
	I2C1CONbits.IPMIEN 	= 0;        // IPMI is disabled	
	I2C1CONbits.A10M	= 0;        // 7 bit slave address
	I2C1CONbits.DISSLW 	= 1;        // No slew control
	I2C1CONbits.SMEN 	= 0;        // No SMBus Enabled
	
	I2C1BRG = WM8510DRV_I2CBAUD;	// 100KHz I2C
	WM8510DRV_I2C_TRISSCL = 1;
	WM8510DRV_I2C_TRISSDA = 1;
	
	DCICON1 = WM8510DRV_DCICON1VAL;
	DCICON2 = WM8510DRV_DCICON2VAL;
	TSCON = WM8510DRV_TSCONVAL;
	RSCON = WM8510DRV_RSCONVAL;	
	
	thisWM8510Codec 		= pHandle;
	pHandle->inputBuffer1 	= pBuffer;
	pHandle->inputBuffer2 	= pBuffer + WM8510DRV_CODEC_FRAME ;
	pHandle->outputBuffer1 	= pBuffer + (2*WM8510DRV_CODEC_FRAME );
	pHandle->outputBuffer2 	= pBuffer + (3*WM8510DRV_CODEC_FRAME );
	
	for (cleanIt = 0; cleanIt <  WM8510DRV_DRV_BUFFER_SIZE; cleanIt ++)
		pBuffer[cleanIt] = 0;
}
	
void WM8510Start(WM8510Handle * pHandle)
{
	pHandle->statusFlag  =   0;
	pHandle->statusFlag	|=  WM8510DRV_SET_READ_BUSY;
	pHandle->statusFlag	|=  WM8510DRV_SET_WRITE_BUSY;
	pHandle->activeInputBuffer  = pHandle->inputBuffer1;
	pHandle->activeOutputBuffer = pHandle->outputBuffer1;
	pHandle->currentSampleIndex	= 0;
	pHandle->currentFrameSize 	= WM8510DRV_CODEC_FRAME - 1;
	pHandle->newFrameSize		= WM8510DRV_CODEC_FRAME - 1;
	TXBUF0 						= pHandle->activeOutputBuffer[0] ;
	
	I2C1CONbits.I2CEN 	= 1;	// Enable the I2C module
	_DCIIF = 0;
	_DCIIE = 1;
	_DCIIP = WM8510DRV_DCI_INTERRUPT_PRIORITY;
	DCICON1bits.DCIEN = 1;		// Enable the DCI module
}
	
void WM8510Stop(WM8510Handle * pHandle)
{
	
	DCICON1bits.DCIEN = 0;	/* Stop the DCI module	*/
	_DCIIE = 0;				/* Stop Interrupts		*/
}

void WM8510Read(WM8510Handle * pHandle, int * data, int size)
{
	/* The buffer indicator bit in the status register determines which	
	 * which buffer can be read. If flag is 0 then buffer 2 should be read
	 * else buffer should be read. */
	 
	int * source;
	int sampleIndex;
	 
	if((pHandle->statusFlag & WM8510DRV_GET_BUFFER_IND) == 0)
	{
        /* This means that buffer 1 is busy and buffer 2 is 
		* available to read	*/
		source = pHandle->inputBuffer2;
	}
	else
	{
		/* This means that buffer 2 is busy and buffer 1 is 
		  * available to read	*/
		source = pHandle->inputBuffer1;
	}
		  
	if (size > WM8510DRV_CODEC_FRAME) size = WM8510DRV_CODEC_FRAME;
		
	for(sampleIndex = 0; sampleIndex < size; sampleIndex++)	data[sampleIndex] = source[sampleIndex];
	
	/* Set the read busy flag indicating that no buffers are 
	 * available for reading	*/
	 
	__asm__ volatile("disi #0x4"); /* disable interrupts */
	pHandle->statusFlag |= WM8510DRV_SET_READ_BUSY;
	__asm__ volatile("disi #0x0"); /* enable interrupts */
}
	
int WM8510IsWriteBusy(WM8510Handle *pHandle)
{
	return( ((pHandle->statusFlag & WM8510DRV_GET_WRITE_BUSY) == 0) ? 0 : 1); 
}

int  WM8510IsReadBusy(WM8510Handle *pHandle)
{
	return( ((pHandle->statusFlag & WM8510DRV_GET_READ_BUSY) == 0) ? 0 : 1); 
}	

void WM8510Write(WM8510Handle * pHandle, int * data, int size)
{
	/* The buffer indicator bit in the status register determines which	
	 * which buffer can be written to. If flag is 0 then buffer 2 should be 
	 * written to since the driver is writing buffer 1 to the output. */
	 
	 int* destination;
	 int  sampleIndex;
	 
	 if((pHandle->statusFlag & WM8510DRV_GET_BUFFER_IND) == 0)
	 {
		 /* This means that buffer 1 is busy and buffer 2 is 
		  * available to write to	*/
		 destination = pHandle->outputBuffer2;
	}
	else
	{
		/* This means that buffer 2 is busy and buffer 1 is 
		 * available to write to	*/
		destination = pHandle->outputBuffer1;
	}
		  
	if (size > WM8510DRV_CODEC_FRAME) size = WM8510DRV_CODEC_FRAME;
	
	/* Update the frame size. The actual switch will happen
	 * in the ISR	*/	
	pHandle->newFrameSize = size;
		
	for(sampleIndex = 0; sampleIndex < size; sampleIndex++)	destination[sampleIndex] = data[sampleIndex];
	
	/* Set the write busy flag indicating that no buffers are 
	 * available for writing	*/
	__asm__ volatile("disi #0x4"); /* disable interrupts */
	pHandle->statusFlag |= WM8510DRV_SET_WRITE_BUSY;
	__asm__ volatile("disi #0x0"); /* enable interrupts */
}
	
int WM8510IOCtl(WM8510Handle * pHandle,int command, void * value)
{
	/* Use the I2C module to send the control data to the codec
	 * Send the device address first, the upper control word next
	 * which consists of register address and bit B8 of control data
	 * and finally send the control data bits B7-B0	*/
	
	CommandValueWord commandValue;
	
	commandValue.wordValue = *((int *)value);
	commandValue.bytes[1] &= 0x1;
	command = command << 1;
	command = command | commandValue.bytes[1];
	
	
	I2C1CONbits.SEN = 1;					/* Send the start condition 			*/
	while(I2C1CONbits.SEN == 1);			/* Wait for the condition to complete	*/
	
	I2C1TRN = WM8510DRV_I2C_ADDR;			/* Address of the WM8510			*/
	while(I2C1STATbits.TRSTAT);			/* Wait till this has been tranmitted	*/
	if (I2C1STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{									/* was recieved					*/
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);			/* Send the stop condition if a NACK	*/
		return(-1);						/* was received					*/		
	
	}

	I2C1TRN = command;					/* The address of the register		*/
	while(I2C1STATbits.TRSTAT);			/* Wait till this has been tranmitted	*/
	if (I2C1STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{									/* was recieved					*/
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);			/* Send the stop condition if a NACK	*/
		return(-1);						/* was received					*/		
	}
	
	I2C1TRN = commandValue.bytes[0];			/* The value of the register			*/
	while(I2C1STATbits.TRSTAT);			/* Wait till this has been tranmitted	*/
	if (I2C1STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{									/* was recieved					*/
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);			/* Send the stop condition if a NACK	*/
		return(-1);						/* was received					*/		
	}
	
	I2C1CONbits.PEN = 1;
	while(I2C1CONbits.PEN);				/* Send the stop condition			*/
	return(1);
}

void __attribute__((__interrupt__,no_auto_psv)) _DCIInterrupt(void)
{
	_DCIIF = 0;;
	
	/* Send and Recieve Samples */
	
	thisWM8510Codec->activeInputBuffer[thisWM8510Codec->currentSampleIndex] = RXBUF0;
	TXBUF0 = thisWM8510Codec->activeOutputBuffer[thisWM8510Codec->currentSampleIndex];
	thisWM8510Codec->currentSampleIndex++;
	
	if(thisWM8510Codec->currentSampleIndex == thisWM8510Codec->currentFrameSize)
	{
		/* Transmitted one frame of data.
		 * Recieved one frame of data.
		 * Toggle the buffer indicator bit */
	
		thisWM8510Codec->statusFlag ^= WM8510DRV_TGL_BUFFER_IND;
		if((thisWM8510Codec->statusFlag &	WM8510DRV_GET_BUFFER_IND) != 0)
		{
			/* Buffer indicator is 1 means use buffer2	*/
			thisWM8510Codec->activeInputBuffer = thisWM8510Codec->inputBuffer2;
			thisWM8510Codec->activeOutputBuffer = thisWM8510Codec->outputBuffer2;
		}
		else
		{
			/* Buffer indicator is 0 means use buffer1	*/
			thisWM8510Codec->activeInputBuffer = thisWM8510Codec->inputBuffer1;
			thisWM8510Codec->activeOutputBuffer = thisWM8510Codec->outputBuffer1;
		}
		/* Reset the sample index and update the sample count	*/
		thisWM8510Codec->currentSampleIndex = 0;		
		thisWM8510Codec->currentFrameSize = thisWM8510Codec->newFrameSize ;
		thisWM8510Codec->statusFlag &= WM8510DRV_CLR_READ_BUSY;
		thisWM8510Codec->statusFlag &= WM8510DRV_CLR_WRITE_BUSY;
	}
}

void WM8510SampleRate8KConfig(WM8510Handle *codecHandle)
{
	int commandValue,result;
	commandValue = 1;		/* Any value can be written to reset the codec	*/
	result = WM8510IOCtl(codecHandle,WM8510_SOFTWARE_RESET,	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b001101111;		
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT1,     (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000010101;
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT2,     (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010001001;
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT3,     (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000011000;
	WM8510IOCtl(codecHandle,WM8510_AUDIO_INTERFACE, (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b110101001;
	WM8510IOCtl(codecHandle,WM8510_CLOCKGEN_CTRL,   (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000001010;
	WM8510IOCtl(codecHandle,WM8510_ADDITIONAL_CTRL, (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(codecHandle,WM8510_GPIO_STUFF, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b100001000;
	WM8510IOCtl(codecHandle,WM8510_ADC_CONTROL	, 	(void *) &commandValue);
	if (result == -1) while(1);
		
	commandValue = 0b001000000;
	WM8510IOCtl(codecHandle,WM8510_NOTCH_FILTER1, 	(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b000000101;
	WM8510IOCtl(codecHandle,WM8510_NOTCH_FILTER2, 	(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b000111111;
	WM8510IOCtl(codecHandle,WM8510_NOTCH_FILTER3, 	(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b001111101;
	WM8510IOCtl(codecHandle,WM8510_NOTCH_FILTER4, 	(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b100000000;
	WM8510IOCtl(codecHandle,WM8510_NOTCH_FILTER4, 	(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b000011000;
	WM8510IOCtl(codecHandle,WM8510_PLL_N, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000001100;
	WM8510IOCtl(codecHandle,WM8510_PLL_K1, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010010011;
	WM8510IOCtl(codecHandle,WM8510_PLL_K2, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b011101001;
	WM8510IOCtl(codecHandle,WM8510_PLL_K3, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(codecHandle,WM8510_INPUT_CTRL, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000000;
	WM8510IOCtl(codecHandle,WM8510_ADC_BOOST_CTRL, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000001;
	WM8510IOCtl(codecHandle,WM8510_MONO_MIXER_CTRL,	 (void *) &commandValue);
	if (result == -1) while(1);
}

void WM8510SampleRate16KConfig(WM8510Handle *codecHandle)
{
	int commandValue,result;
	commandValue = 1;		/* Any value can be written to reset the codec	*/
	result = WM8510IOCtl(codecHandle,WM8510_SOFTWARE_RESET,	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b001101111;		
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT1, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000010101;
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT2, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010001001;
	WM8510IOCtl(codecHandle,WM8510_POWER_MGMT3, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000011000;
	WM8510IOCtl(codecHandle,WM8510_AUDIO_INTERFACE, (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b101101001;
	WM8510IOCtl(codecHandle,WM8510_CLOCKGEN_CTRL, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000110;
	WM8510IOCtl(codecHandle,WM8510_ADDITIONAL_CTRL, (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(codecHandle,WM8510_GPIO_STUFF, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000011000;
	WM8510IOCtl(codecHandle,WM8510_PLL_N, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000001100;
	WM8510IOCtl(codecHandle,WM8510_PLL_K1, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010010011;
	WM8510IOCtl(codecHandle,WM8510_PLL_K2, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b011101001;
	WM8510IOCtl(codecHandle,WM8510_PLL_K3, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(codecHandle,WM8510_INPUT_CTRL, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000000;
	WM8510IOCtl(codecHandle,WM8510_ADC_BOOST_CTRL,  (void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000001;
	WM8510IOCtl(codecHandle,WM8510_MONO_MIXER_CTRL,	(void *) &commandValue);
	if (result == -1) while(1);
}
