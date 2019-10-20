//==============================================================================
//	  Arquivo: Config_Bits.h
//	  Descrição: Ajustando registradores de configuração (CONFIG_BITS) somente 
//               ajustados no tempo de gravação(Fusíveis Externos).
//==============================================================================

#ifndef CONFIG_BITS_H
#define	CONFIG_BITS_H

#include <xc.h> // Biblioteca do compilador XC8 v1.34

#pragma config OSC = INTIO67   // CONFIG1H   
#pragma config FCMEN = OFF      
#pragma config IESO = OFF
#pragma config PWRT = ON       // CONFIG2L
#pragma config BOREN = SBORDIS  
#pragma config BORV = 3
#pragma config WDT = OFF       // CONFIG2H
#pragma config WDTPS = 32768
#pragma config CCP2MX = PORTC  // CONFIG3H
#pragma config PBADEN = OFF     
#pragma config LPT1OSC = OFF    
#pragma config MCLRE = ON
#pragma config STVREN = OFF    // CONFIG4L    
#pragma config LVP = OFF        
#pragma config XINST = OFF
#pragma config CP0 = OFF       // CONFIG5L    
#pragma config CP1 = OFF        
#pragma config CP2 = OFF        
#pragma config CP3 = OFF
#pragma config CPB = OFF       // CONFIG5H
#pragma config CPD = OFF
#pragma config WRT0 = OFF      // CONFIG6L  
#pragma config WRT1 = OFF       
#pragma config WRT2 = OFF      
#pragma config WRT3 = OFF
#pragma config WRTC = OFF    // CONFIG6H  
#pragma config WRTB = OFF       
#pragma config WRTD = OFF
#pragma config EBTR0 = OFF   // CONFIG7L 
#pragma config EBTR1 = OFF      
#pragma config EBTR2 = OFF      
#pragma config EBTR3 = OFF      
#pragma config EBTRB = OFF 

#endif
//==============================================================================
