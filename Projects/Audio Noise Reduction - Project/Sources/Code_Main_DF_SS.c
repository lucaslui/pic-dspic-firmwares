/*==============================================================================
 *
 *        PROJETO DE IMPLEMENTAÇÃO DE UM ALGORITMO PARA REDUÇÃO DE RUÍDO
 *                  EM UM PROCESSADOR DE SINAIS DIGITAL - DSP
 *                    DSP utilizado: dsPIC33FJ256GP506
 *     Kit Utilizado: MPLAB Starter Kit for dsPIC® Digital Signal Controllers
 *
 *  Autor: Lucas Lui Motta
 * 
 *  Objetivo: O objetivo desse projeto prático é implementar o algoritmo 
 *  de redução de ruído de Subtração Espectral baseado no artigo de Boll 1979
 *  no processador.
 *
 *  Softwares usados: 	XC16 - Compilador (v1.26)
 *			MPLABX - IDE (v3.35)
 *
 *==============================================================================
 *
 *  Sobre Licença: GNU GPL 2
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */
//==============================================================================

#define FCY 40000000ULL // Definição necessário para func. correta do delay. 

#include <p33FJ256GP506.h>  // Bibl. do DSP - dsPIC33FJ256GP506.
#include <libpic30.h>       // Biblioteca padrão para periféricos do comp. XC16.
#include <dsp.h>            // Biblioteca c/ funções como a FFT e o w.Hamming.
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include "Code_Basic_Functions.h"   // Bibl. das funções básicas de botões.
#include "Code_Config_Bits.h"       // Configurando fusíveis externos do DSP.
#include "Code_Codec_WM8510.h"      // Bibl. do CODEC WM8510.
#include "Code_Memory_SST25VF040.h" // Bibl. da Memória Integrada SST25VF040.
#include "Code_Compresses_G711.h"   // Algoritmo de compressão 16->8 bits.

//==============================================================================

#define FRAME_SIZE          128  // Valor da quantidade de amostras de cada frame do áudio.
#define ANALYZE_SIZE        512  // Valor adotado de amostras do pedação de áudio para ser analisado por vez (1024).  

#define SAMPLING_FREQUENCY  8000 // Valor da frequência de amostragem adotado no projeto (8kHz).
#define WINDOW_SIZE_MS      16   // Valor de tempo selecionado do tamanho da janela no tempo (16ms).
#define OVERLAP             0.5  // Valor de sobreposição adotado para o projeto (50%).
#define SAMPLES_MS          8    // Número de amostras/ms de acordo com a "fs" adotado (SAMPLING_FREQUENCY/1000 - 8kHz/1000 = 8) 
#define REPEAT_ANALYZE_MS   8    // Tempo em milisegundos p/ próx. análise de janela (16*0.5 = 8ms)
#define STEP_NEXT_ANALYZE   64   // Quantidade de amostras em um passo de análise (8*8 = 64 ams).  
#define SAMPLES_FOR_WINDOW  128  // Quantidade de amostras por janela (16*8 = 128 ams).
#define QUANT_OF_WINDOWS    7    // Número de janelas dentro do pedaço de áudio analisado ((1024 - 128) / 64)  + 1 = 15 ou ((512 - 128) / 64)  + 1 = 7). 

#define FFT_BLOCK_LENGTH    128  // Tamanho do bloco de pontos que serão analisados na FFT (FFT_BLOCK_LENGHT > SAMPLES_FOR_WINDOW).
#define LOG2_BLOCK_LENGTH   7    // Valor em formato de potência de 2 adotado para FFT (log2 X = 128 -> X = 7).
#define FFT_ANALYZE_SIZE    896  // Quantidade de espaço para armazenar os espectros das janelas do pedaço de áudio (128*15 = 1920) ou (128*7 = 896).

#define FACTOR_ATTENUATION  2    // Valor de ajuste para previnir a saturação/overflow da entrada de áudio.

#define FCT_UPDATE_NOISE    9.0   // Fator definido para atualização do ruído.     
#define ALFA                0.003 // Fator de atenuação em caso de janelas consideradas ruído.
#define NOISE_MARGIN        0.023 // Valor da empirico da margem de ruído para o VAD.
#define HANGOVER            8.0   // Valor empirico de quantas janelas terão que serem validadas como ruído.

#define START_ADDRESS_AUDIO_RECORDED   0x20000
#define START_ADDRESS_AUDIO_FILTERED   0x50000
             
#define YELLOW_LED  LATCbits.LATC13
#define GREEN_LED   LATCbits.LATC14
#define RED_LED     LATCbits.LATC15

#define LED_ON  0
#define LED_OFF 1

//==============================================================================

int 	codecBuffer        [WM8510DRV_DRV_BUFFER_SIZE];
char 	flashMemoryBuffer  [SST25LF040ADRV_BUFFER_SIZE];
int 	samples            [FRAME_SIZE];
char 	encodedSamples     [FRAME_SIZE];
int 	decodedSamples     [FRAME_SIZE];

WM8510Handle codec; 
WM8510Handle *codecHandle = &codec;
SST25LF040AHandle flashMemoryHandle;
SST25LF040AHandle *pFlashMemoryHandle = &flashMemoryHandle;

long currentWriteAddress = 0;
long currentReadAddress = 0; 
long lastAddressWrote = 0;
long address;

// Coeficientes para cálculo da FFT foram armazenados na memória de programa.
const fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2] __attribute__((space(auto_psv))) = 
{   0x7FFF, 0x0000, 0x7FD9, 0xF9B8, 0x7F62, 0xF374, 0x7E9D, 0xED38,
    0x7D8A, 0xE707, 0x7C2A, 0xE0E6, 0x7A7D, 0xDAD8, 0x7885, 0xD4E1,
    0x7642, 0xCF04, 0x73B6, 0xC946, 0x70E3, 0xC3A9, 0x6DCA, 0xBE32,
    0x6A6E, 0xB8E3, 0x66D0, 0xB3C0, 0x62F2, 0xAECC, 0x5ED7, 0xAA0A,
    0x5A82, 0xA57E, 0x55F6, 0xA129, 0x5134, 0x9D0E, 0x4C40, 0x9930,
    0x471D, 0x9592, 0x41CE, 0x9236, 0x3C57, 0x8F1D, 0x36BA, 0x8C4A,
    0x30FC, 0x89BE, 0x2B1F, 0x877B, 0x2528, 0x8583, 0x1F1A, 0x83D6,
    0x18F9, 0x8276, 0x12C8, 0x8163, 0x0C8C, 0x809E, 0x0648, 0x8027,
    0x0000, 0x8000, 0xF9B8, 0x8027, 0xF374, 0x809E, 0xED38, 0x8163,
    0xE707, 0x8276, 0xE0E6, 0x83D6, 0xDAD8, 0x8583, 0xD4E1, 0x877C,
    0xCF04, 0x89BE, 0xC946, 0x8C4A, 0xC3A9, 0x8F1D, 0xBE32, 0x9236,
    0xB8E3, 0x9592, 0xB3C0, 0x9931, 0xAECC, 0x9D0E, 0xAA0A, 0xA129,
    0xA57E, 0xA57E, 0xA129, 0xAA0A, 0x9D0E, 0xAECC, 0x9931, 0xB3C0,
    0x9592, 0xB8E3, 0x9236, 0xBE32, 0x8F1D, 0xC3A9, 0x8C4A, 0xC946,
    0x89BE, 0xCF04, 0x877C, 0xD4E1, 0x8583, 0xDAD8, 0x83D6, 0xE0E6,
    0x8276, 0xE707, 0x8163, 0xED38, 0x809E, 0xF374, 0x8027, 0xF9B8
};

const fractcomplex twiddleFactorsInverse[FFT_BLOCK_LENGTH/2] __attribute__((space(auto_psv))) =
{   0x7FFF, 0x0000, 0x7FD9, 0x0648, 0x7F62, 0x0C8C, 0x7E9D, 0x12C8,
    0x7D8A, 0x18F9, 0x7C2A, 0x1F1A, 0x7A7D, 0x2528, 0x7885, 0x2B1F,
    0x7642, 0x30FC, 0x73B6, 0x36BA, 0x70E3, 0x3C57, 0x6DCA, 0x41CE,
    0x6A6E, 0x471D, 0x66D0, 0x4C40, 0x62F2, 0x5134, 0x5ED7, 0x55F6,
    0x5A82, 0x5A82, 0x55F6, 0x5ED7, 0x5134, 0x62F2, 0x4C40, 0x66D0,
    0x471D, 0x6A6E, 0x41CE, 0x6DCA, 0x3C57, 0x70E3, 0x36BA, 0x73B6,
    0x30FC, 0x7642, 0x2B1F, 0x7885, 0x2528, 0x7A7D, 0x1F1A, 0x7C2A,
    0x18F9, 0x7D8A, 0x12C8, 0x7E9D, 0x0C8C, 0x7F62, 0x0648, 0x7FD9,
    0x0000, 0x7FFF, 0xF9B8, 0x7FD9, 0xF374, 0x7F62, 0xED38, 0x7E9D,
    0xE707, 0x7D8A, 0xE0E6, 0x7C2A, 0xDAD8, 0x7A7D, 0xD4E1, 0x7884,
    0xCF04, 0x7642, 0xC946, 0x73B6, 0xC3A9, 0x70E3, 0xBE32, 0x6DCA,
    0xB8E3, 0x6A6E, 0xB3C0, 0x66CF, 0xAECC, 0x62F2, 0xAA0A, 0x5ED7,
    0xA57E, 0x5A82, 0xA129, 0x55F6, 0x9D0E, 0x5134, 0x9931, 0x4C40,
    0x9592, 0x471D, 0x9236, 0x41CE, 0x8F1D, 0x3C57, 0x8C4A, 0x36BA,
    0x89BE, 0x30FC, 0x877C, 0x2B1F, 0x8583, 0x2528, 0x83D6, 0x1F1A,
    0x8276, 0x18F9, 0x8163, 0x12C8, 0x809E, 0x0C8C, 0x8027, 0x0648
};

fractcomplex buffer[FFT_BLOCK_LENGTH]  __attribute__ ((space(ymemory), far, aligned(FFT_BLOCK_LENGTH*2*2)));

fractional windowCoefs[FFT_BLOCK_LENGTH] __attribute__ ((space(ymemory), far, aligned(FFT_BLOCK_LENGTH*2)));

union unionToMemory_1{  
    float angleValues[FFT_ANALYZE_SIZE]; 
    fractional Buffer1024[ANALYZE_SIZE];       
} u1;

union unionToMemory_2{
    float absMean[FFT_ANALYZE_SIZE];  
    fractcomplex bufferSpect[FFT_ANALYZE_SIZE];      
} u2;

fractional tempBuffer128[FFT_BLOCK_LENGTH];

float bufferIO[FFT_ANALYZE_SIZE];
float noiseMean[FFT_BLOCK_LENGTH];
float nrMaximum[FFT_BLOCK_LENGTH];

int initSilenceFlag, speechFlag, noiseCounter;

//==============================================================================

void Do_OverlapAdd()
{
    fractional bufferAdd[SAMPLES_FOR_WINDOW/2] = {0};
    
    int i, w;
    
    // Zera todas as componentes reais e imaginárias do buffer, para que possa ser utilizado.
    for(i=0; i < FFT_BLOCK_LENGTH; i++) 
    {
        buffer[i].real = 0x0000;
        buffer[i].imag = 0x0000;
    }
    // Carrega o primeiro espectro do sinal janelado no buffer.
    for(i=0; i < FFT_BLOCK_LENGTH; i++) buffer[i] = u2.bufferSpect[i];
    
    // Executa a anti-transformada desse primeiro espectro.
    IFFTComplexIP (LOG2_BLOCK_LENGTH, &buffer[0], (fractcomplex *) __builtin_psvoffset(&twiddleFactorsInverse[0]), (int) __builtin_psvpage(&twiddleFactorsInverse[0]));
    
    // Transporta para a saída a primeira parcela desse primeiro espectro anti-transformado.
    for(i=0; i < STEP_NEXT_ANALYZE; i++) u1.Buffer1024[i] = buffer[i].real;        
    
    // Carrega a segunda parcela dessa janela anti-transformada(64-128) no buffer de soma.
    for(i=STEP_NEXT_ANALYZE; i < SAMPLES_FOR_WINDOW; i++) bufferAdd[i-STEP_NEXT_ANALYZE] = buffer[i].real;

    for(w=1; w < QUANT_OF_WINDOWS; w++) 
    {
        // Carrega o próximo espectro da janela no buffer.
        for(i=0; i < FFT_BLOCK_LENGTH; i++) buffer[i] = u2.bufferSpect[i+(w*FFT_BLOCK_LENGTH)];
        
        // Executa a anti-transformada do espectro da janela.     
        IFFTComplexIP (LOG2_BLOCK_LENGTH, &buffer[0], (fractcomplex *) __builtin_psvoffset(&twiddleFactorsInverse[0]), (int) __builtin_psvpage(&twiddleFactorsInverse[0]));
        
        // Soma as primeiras (0-64) amostras da janela atual com o final da
        // janela anterior (64-128) que está armazenado no buffer de soma.
        for(i=0; i < STEP_NEXT_ANALYZE; i++) bufferAdd[i] += buffer[i].real;
        
        // Transporta o segmento somado de 64 amostras para a saída.
        for(i=0; i < STEP_NEXT_ANALYZE; i++) u1.Buffer1024[i+(w*STEP_NEXT_ANALYZE)] = bufferAdd[i];        
        
        // Carrega a segunda parcela da janela atual (64-128) para o buffer de soma.
        for(i=STEP_NEXT_ANALYZE; i < SAMPLES_FOR_WINDOW; i++) bufferAdd[i-STEP_NEXT_ANALYZE] = buffer[i].real;
    }
    // Transporta a segunda parcela (64-128) da ultima janela para a saída.
    for(i=0; i < STEP_NEXT_ANALYZE; i++) u1.Buffer1024[i+(QUANT_OF_WINDOWS*STEP_NEXT_ANALYZE)] = bufferAdd[i];
}

//==============================================================================

float Max_Float_Value(float a, float b)
{
    if(a > b) return a;
    else return b;
}

//==============================================================================

float Min_Float_Value(float a, float b, float c)
{
    float min = a;
    
    if(b < min) min = b;
    if(c < min) min = c;
    
    return min;
}

//==============================================================================

void Update_Noise(int w)
{          
    int i;            
    float resultSub[FFT_BLOCK_LENGTH] = {0};
 
    for(i=0; i < FFT_BLOCK_LENGTH; i++) 
    {
        noiseMean[i] = ((FCT_UPDATE_NOISE*noiseMean[i])+ u2.absMean[i+(w*FFT_BLOCK_LENGTH)])/(FCT_UPDATE_NOISE + 1.0);
    }
    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) 
    {
        resultSub[i] = u2.absMean[i+(w*FFT_BLOCK_LENGTH)] - noiseMean[i]; 
        nrMaximum[i] = Max_Float_Value(resultSub[i], nrMaximum[i]);  
        bufferIO[i+(w*FFT_BLOCK_LENGTH)] = ALFA * u2.absMean[i+(w*FFT_BLOCK_LENGTH)];
    }
}

//==============================================================================

void Spectral_Subtraction(int w)
{
    int i;    
    float y_previous = 0, y_current = 0, y_after = 0;
    
    // Execução da subtração espectral - D[n] = Y_mean[n] - N_mean[n]
    for(i=0; i < FFT_BLOCK_LENGTH; i++) bufferIO[i+(w*FFT_BLOCK_LENGTH)] = u2.absMean[i+(w*FFT_BLOCK_LENGTH)] - (noiseMean[i]);
    
    // Retificação de meia-onda.
    for(i=0; i < FFT_BLOCK_LENGTH; i++) if(bufferIO[i+(w*FFT_BLOCK_LENGTH)] < 0) bufferIO[i+(w*FFT_BLOCK_LENGTH)] = 0;  
    
    if((w > 0) && (w < (QUANT_OF_WINDOWS-1)))
    {
        for(i=0; i < FFT_BLOCK_LENGTH; i++)
        {
            if(bufferIO[i+(w*FFT_BLOCK_LENGTH)] < nrMaximum[i])
            {
                y_previous = u2.absMean[i+((w-1)*FFT_BLOCK_LENGTH)] - noiseMean[i];
                y_current = bufferIO[i+(w*FFT_BLOCK_LENGTH)]; 
                y_after = u2.absMean[i+((w+1)*FFT_BLOCK_LENGTH)] - noiseMean[i];
                bufferIO[i+(w*FFT_BLOCK_LENGTH)] = Min_Float_Value(y_previous, y_current, y_after);
            }            
        }
    }  
}

//==============================================================================

void do_VAD(int w)
{
    int i;
    float specDist[FFT_BLOCK_LENGTH] = {0};
    float distance = 0, pSignal = 0, pNoise = 0;
    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) 
    {
        pSignal = 20*(log10f((1.0 + u2.absMean[i+(w*FFT_BLOCK_LENGTH)])));
        pNoise = 20*(log10f(1.0 + noiseMean[i]));
        specDist[i] = pSignal - pNoise;
    }
    for(i=0; i < FFT_BLOCK_LENGTH; i++) if(specDist[i] < 0) specDist[i] = 0;  
    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) distance += specDist[i];
    distance /= (float)FFT_BLOCK_LENGTH;
    
    if (distance < (float) NOISE_MARGIN) noiseCounter++;
    else noiseCounter = 0;
     
    if (noiseCounter > HANGOVER) speechFlag = 0;    
    else speechFlag = 1; 
}

//==============================================================================

void Algorithm_Filtering()
{
    int i, w;
    
    // Converte os pontos das FFT de retangular para polar.
    for(w=0; w < QUANT_OF_WINDOWS; w++)
    {   
        for(i=0; i < FFT_BLOCK_LENGTH; i++) buffer[i] = u2.bufferSpect[i+(w*FFT_BLOCK_LENGTH)];;
    
        SquareMagnitudeCplx(FFT_BLOCK_LENGTH, &buffer[0], &tempBuffer128[0]);
        
        for(i=(FFT_BLOCK_LENGTH/2)+1; i < FFT_BLOCK_LENGTH; i++) tempBuffer128[i] = 0;

        for(i=0; i < FFT_BLOCK_LENGTH; i++) bufferIO[i+(w*FFT_BLOCK_LENGTH)] = sqrtf(Fract2Float(tempBuffer128[i]));

        for(i=0; i < FFT_BLOCK_LENGTH; i++) 
        {
            errno = 0;
            u1.angleValues[i+(w*FFT_BLOCK_LENGTH)] = atan2f(Fract2Float(buffer[i].imag),Fract2Float(buffer[i].real));
            if (errno) u1.angleValues[i+(w*FFT_BLOCK_LENGTH)] = 0;
        }
    }
    
    // Adota que os primeiros quatro trechos do sinal é de silêncio:
    // IS(ams)=(512ams * 4 repetições) = 2048ams -> IS(ms)=2048/8000 = 256ms
    if(initSilenceFlag < 3)
    {
        for(i=0; i < FFT_BLOCK_LENGTH; i++)
        {
            for(w=0; w < QUANT_OF_WINDOWS; w++) noiseMean[i] += bufferIO[i+(w*FFT_BLOCK_LENGTH)]; 
            if(noiseMean[i] != 0.0) noiseMean[i] /= (float)QUANT_OF_WINDOWS;
        }    
        initSilenceFlag++;
    }
    
    // Calculo da média das componentes das janelas próximas(Magnitude Average).    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) u2.absMean[i] = bufferIO[i]; 
    for(w=1; w < (QUANT_OF_WINDOWS-1); w++)
    {
        for(i=0; i < FFT_BLOCK_LENGTH; i++)
        {
            u2.absMean[i+(w*FFT_BLOCK_LENGTH)] = 
                                (bufferIO[i+((w-1)*FFT_BLOCK_LENGTH)]
                               + bufferIO[i+(w*FFT_BLOCK_LENGTH)]
                               + bufferIO[i+((w+1)*FFT_BLOCK_LENGTH)])/3.0;
        }
    }    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) u2.absMean[i+((QUANT_OF_WINDOWS-1)*FFT_BLOCK_LENGTH)] = bufferIO[i+((QUANT_OF_WINDOWS-1)*FFT_BLOCK_LENGTH)];
      
    if(initSilenceFlag >= 3)
    {
        for(w=0; w < QUANT_OF_WINDOWS; w++)
        {
            // Algoritmo VAD para ruídos periódicos, utilizando SNR para teste.
            do_VAD(w);

            if(speechFlag == 0) Update_Noise(w);
            else Spectral_Subtraction(w);
        }
    }
    else 
    {
        for(w=0; w < QUANT_OF_WINDOWS; w++)
        {
            for(i=0; i < FFT_BLOCK_LENGTH; i++)
            {
                bufferIO[i+(w*FFT_BLOCK_LENGTH)] = ALFA * u2.absMean[i+(w*FFT_BLOCK_LENGTH)];
            }
        }      
    }
    
    // Converte os módulos com a fase preservada na forma retangular.
    for(w=0; w < QUANT_OF_WINDOWS; w++)
    {
        for(i=0; i < FFT_BLOCK_LENGTH; i++)
        {
            buffer[i].real = Float2Fract(bufferIO[i+(w*FFT_BLOCK_LENGTH)]*(cosf(u1.angleValues[i+(w*FFT_BLOCK_LENGTH)])));
            buffer[i].imag = Float2Fract(bufferIO[i+(w*FFT_BLOCK_LENGTH)]*(sinf(u1.angleValues[i+(w*FFT_BLOCK_LENGTH)])));
        }
        for(i=0; i < FFT_BLOCK_LENGTH; i++) u2.bufferSpect[i+(w*FFT_BLOCK_LENGTH)] = buffer[i];
    }
}

//==============================================================================

void Do_Segment_Windowing()
{ 
    int i, w;
        
    for(w=0; w < QUANT_OF_WINDOWS; w++) 
    {   
        // Carrega a nova janela do vetor de áudio, cada janela 64 ams a frente da outra.
        for(i=0; i < SAMPLES_FOR_WINDOW; i++) 
        {
            buffer[i].real = u1.Buffer1024[i+(w*STEP_NEXT_ANALYZE)];
            buffer[i].imag = 0x0000;
        }
        // Após completar a janela com amostras é feito hamming da mesma.
        VectorWindow(SAMPLES_FOR_WINDOW, &buffer[0].real, &buffer[0].real, &windowCoefs[0]);
        
        // Após o hamming do frame é então feita a FFT dele.
        FFTComplexIP (LOG2_BLOCK_LENGTH, &buffer[0], (fractcomplex *) __builtin_psvoffset(&twiddleFactors[0]), (int) __builtin_psvpage(&twiddleFactors[0]));
        
        // Reverte a sequência dos bits, pois a função FFT implementada inverte elas.
        BitReverseComplex(LOG2_BLOCK_LENGTH, &buffer[0]);
        
        for(i=0; i< FFT_BLOCK_LENGTH; i++) buffer[i].real <<= LOG2_BLOCK_LENGTH;
        for(i=0; i< FFT_BLOCK_LENGTH; i++) buffer[i].imag <<= LOG2_BLOCK_LENGTH;
                
        // Descarrega a janela transformada pela FFT no vetor de saída do tipo fractcomplex.
        for(i=0; i < FFT_BLOCK_LENGTH; i++) u2.bufferSpect[i+(w*FFT_BLOCK_LENGTH)] = buffer[i];
    }  
}

//==============================================================================

void ReadFilterWrite()
{
    int i, j, m; 
    
    long quant1024Analyzes; 
        
    initSilenceFlag = 0;
    speechFlag = 0;        
    noiseCounter = 0;
    
    for(i=0; i < FFT_BLOCK_LENGTH; i++) 
    {
        noiseMean[i] = 0.0;
        nrMaximum[i] = 0.0;
    }
    
    RED_LED = LED_ON;
    
    currentReadAddress = START_ADDRESS_AUDIO_RECORDED;
	currentWriteAddress = START_ADDRESS_AUDIO_FILTERED;
        
    quant1024Analyzes = ceil((lastAddressWrote - currentReadAddress) / ANALYZE_SIZE);

    for(m=0; m < quant1024Analyzes; m++)
    {   
        for(j=0; j < (ANALYZE_SIZE/FRAME_SIZE); j++)
        {
            while(SST25LF040AIsBusy(pFlashMemoryHandle));				
            SST25LF040ARead(pFlashMemoryHandle,currentReadAddress,encodedSamples,FRAME_SIZE);
            while(SST25LF040AIsBusy(pFlashMemoryHandle));
            G711Ulaw2Lin(encodedSamples,decodedSamples,FRAME_SIZE);
            currentReadAddress += FRAME_SIZE;

            for(i=0; i < FRAME_SIZE; i++) u1.Buffer1024[i+(j*FRAME_SIZE)] = (decodedSamples[i] >> FACTOR_ATTENUATION);
        }    
         
        Do_Segment_Windowing();        
        
        Algorithm_Filtering();
        
        Do_OverlapAdd();
        
        for(j=0; j < (ANALYZE_SIZE/FRAME_SIZE); j++)
        {
            for(i=0; i < FRAME_SIZE; i++) decodedSamples[i] = (u1.Buffer1024[i+(j*FRAME_SIZE)] << FACTOR_ATTENUATION);
            
            G711Lin2Ulaw(decodedSamples,encodedSamples,FRAME_SIZE);
            while(SST25LF040AIsBusy(pFlashMemoryHandle));
            SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_WRITE_ENABLE,0);
            SST25LF040AWrite(pFlashMemoryHandle,currentWriteAddress,encodedSamples,FRAME_SIZE);
            currentWriteAddress += FRAME_SIZE;
        }
    }
    RED_LED = LED_OFF;
}

//==============================================================================

void ReadPlaybackAudioFiltered()
{
    currentReadAddress = START_ADDRESS_AUDIO_FILTERED;
				
    GREEN_LED = LED_ON;
    RED_LED = LED_ON;
								
    WM8510Start(codecHandle);
                
    while(currentReadAddress < (lastAddressWrote + 0x2FFFF))
    {                                 
        while(SST25LF040AIsBusy(pFlashMemoryHandle));				
        SST25LF040ARead(pFlashMemoryHandle,currentReadAddress,encodedSamples,FRAME_SIZE);
        while(SST25LF040AIsBusy(pFlashMemoryHandle));
        currentReadAddress += FRAME_SIZE;
                                        
        G711Ulaw2Lin(encodedSamples,decodedSamples,FRAME_SIZE);                
        while(WM8510IsWriteBusy(codecHandle));	
        WM8510Write(codecHandle,decodedSamples,FRAME_SIZE);
    }
                                
    WM8510Stop(codecHandle);                
                
    GREEN_LED = LED_OFF;
    RED_LED = LED_OFF;
}

//==============================================================================

void ReadPlaybackAudioRecorded()
{
    currentReadAddress = START_ADDRESS_AUDIO_RECORDED;
				
    GREEN_LED = LED_ON;
								
    WM8510Start(codecHandle);
                
    while(currentReadAddress < lastAddressWrote)
    {                                 
        while(SST25LF040AIsBusy(pFlashMemoryHandle));				
        SST25LF040ARead(pFlashMemoryHandle,currentReadAddress,encodedSamples,FRAME_SIZE);
        while(SST25LF040AIsBusy(pFlashMemoryHandle));
        currentReadAddress += FRAME_SIZE;
                                        
        G711Ulaw2Lin(encodedSamples,decodedSamples,FRAME_SIZE);                
        while(WM8510IsWriteBusy(codecHandle));	
        WM8510Write(codecHandle,decodedSamples,FRAME_SIZE);
    }
                                
    WM8510Stop(codecHandle);                
                
    GREEN_LED = LED_OFF;
}

//==============================================================================

void CleanMemorySST25VF040B()
{
    RED_LED = LED_ON;
    
    for(address = START_ADDRESS_AUDIO_RECORDED; address < SST25LF040ADRV_REALLY_LAST_ADDRESS; address += 0x8000)
    {
        SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_WRITE_ENABLE,0);	
        SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_BLOCK_ERASE,(void *)&address);	
        while(SST25LF040AIsBusy(pFlashMemoryHandle));
    }    
    
    RED_LED = LED_OFF;
}

//==============================================================================

void RecordWrite()
{    
    __delay_ms(200);
    
    CleanMemorySST25VF040B();
    
    currentWriteAddress = START_ADDRESS_AUDIO_RECORDED;
            
    YELLOW_LED = LED_ON;
                	
    WM8510Start(codecHandle);                
                
    while(currentWriteAddress < SST25LF040ADRV_MIDDLE_LAST_ADDRESS)
    {
        while(WM8510IsReadBusy(codecHandle));
        WM8510Read(codecHandle,samples,FRAME_SIZE);      
        G711Lin2Ulaw(samples,encodedSamples,FRAME_SIZE);
                                        
        while(SST25LF040AIsBusy(pFlashMemoryHandle));
        SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_WRITE_ENABLE,0);
        SST25LF040AWrite(pFlashMemoryHandle,currentWriteAddress,encodedSamples,FRAME_SIZE);
        currentWriteAddress += FRAME_SIZE;
        
        if((CheckSwitchS1()) == 1) break;
    }
    
    lastAddressWrote = currentWriteAddress; 
	
    WM8510Stop(codecHandle);
                                
    YELLOW_LED = LED_OFF;
    
    __delay_ms(200);
}

//==============================================================================

void Init_SFRs()
{
   // Configurando oscilador interno (FRC - Fast RC) com PLL (Phase Locked Loop)
   // para trabalhar com 40Mhz.
   // Fosc = Fin*(M/(N1*N2)), Fcy=Fosc/2 - (7,37MHz Circuito FRC interno)
   // Fosc = 7.37MHz*43/(2*2) = 79,2Mhz aprox. 80Mhz -> Fcy = 40Mhz
	
	PLLFBDbits.PLLDIV = 0b00101001;  // Multiplicador - 41+2 - "M" = 43
	CLKDIVbits.PLLPOST = 0;          // Fator de divisão posterior "N2" = 2
	CLKDIVbits.PLLPRE = 0;           // Fator Pré-divisão "N1" = 2 

	OSCTUN = 0;     // Registrador responsável pelo ajuste fino de freq.
                    // baseado em possíveis mudanças de temperatura,
                    // nesse caso está centralizado em 7,37Mhz sem ajuste
	
   // dsPIC permite o chaveamento do oscilador a qualquer momento do código, 
   // para isso é escrito no registrador OSCCONH (NOSC - byte-high do 
   // registrador) o novo tipo de oscilador escolhido e setar com OSCCONL 
   // (OSWEN - byte-low do registrador, conforme seguência em:    
	__builtin_write_OSCCONH(0x01);  
	__builtin_write_OSCCONL(0x01);
    
	while(OSCCONbits.COSC != 0b01); // Espera pela troca de tipo de oscilação
	while(!OSCCONbits.LOCK);        // e pelo flag da trava de segurança.

    TRISCbits.TRISC13 = 0; // Pino digital config. como saída para LED Amarelo.
    TRISCbits.TRISC14 = 0; // Pino digital config. como saída para LED Verde.
    TRISCbits.TRISC15 = 0; // Pino digital config. como saída para LED Vermelho.
    
    TRISDbits.TRISD8 = 1; // Pino digital config. como entrada para chave S1.
    TRISDbits.TRISD9 = 1; // Pino digital config. como entrada para chave S2.
    
    TRISGbits.TRISG0 = 0;
    TRISGbits.TRISG1 = 0;
    
    LATGbits.LATG0 = 0;
    LATGbits.LATG1 = 0;

    YELLOW_LED = LED_OFF;
    RED_LED = LED_OFF;
    GREEN_LED = LED_OFF;
}

//==============================================================================

int main(void)
{
    int commandValue;
    
    Init_SFRs();
    Init_Codec_WM8510(codecHandle,codecBuffer);
	Init_Memory_SST25LF040A(pFlashMemoryHandle,flashMemoryBuffer);

    // Inicializando estilo de janela tipo Hamming (pref. Overlap pref. 1/2).
    HammingInit(FFT_BLOCK_LENGTH, &windowCoefs[0]);      

    // Inicilizando memória flash externa.
    SST25LF040AStart(pFlashMemoryHandle); 
    
    // Inicilizando CODEC, funções de entrada de áudio e funções de saída.
	WM8510Start(codecHandle); 
		
	// Configura o CODEC para trabalhar no modo de 8K de sample rate.
	WM8510SampleRate8KConfig(codecHandle);
	
	// Habilita a memória flash serial para escrita
	commandValue = 0;
	SST25LF040AIOCtl(pFlashMemoryHandle, SST25LF040ADRV_WRITE_STATUS_ENABLE,0);
	SST25LF040AIOCtl(pFlashMemoryHandle, SST25LF040ADRV_WRITE_STATUS,(void *)(&(commandValue)));
    
    while(1)
	{									
		if((CheckSwitchS1()) == 1) 
        {
            RecordWrite();
            ReadFilterWrite();
        }
		if((CheckSwitchS2()) == 1) 
        {
            __delay_ms(200);
            if(CheckSwitchS2_4secHeldDown() == 1) ReadPlaybackAudioFiltered();
            else ReadPlaybackAudioRecorded();
        }
    }
}
//==============================================================================
