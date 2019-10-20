/* =============================================================================
 * 
 *  Arquivo: Code_Basic_Functions.c
 *  Descrição: Biblioteca para inicialização dos LED's e função debouncing para
 *             as duas chaves.
 *  Processador: dsPIC33FJ256GP506. 
 * 
 * ===========================================================================*/

#include "Code_Basic_Functions.h"

int debounceS1 = 0;    // Variável de contagem para o debounce da chave S1
unsigned long int debounceS2 = 0;    // Variável de contagem para o debounce da chave S2

int CheckSwitchS1(void)
{
	int isActive; // Retorna TRUE (1) se a chave for realmente pressionada.
    
	if(SWITCH_S1 == 0)
    {		
		debounceS1++; // Incrementa o contador deboucing da S1.
        
		if (debounceS1 == SWITCH_DEBOUNCE) 
        // Se a chave S1 continua pressionada após a contagem do debounce, faça:
        {			
			debounceS1  = 0;
			isActive = 1;
		}
	}
	else	
    {
        // Se não anula a contagem e retorna falso p/ chave pressionada.
		debounceS1 = 0;
		isActive = 0;
	}	
	return(isActive);
}

int CheckSwitchS2(void)
{
	int isActive;   
    
	if(SWITCH_S2 == 0)
    {
		debounceS2++;  // Incrementa o contador deboucing da S2. 
        
		if (debounceS2 == SWITCH_DEBOUNCE)
        {
        // Se a chave S2 continua pressionada após a contagem do debounce, faça:
			debounceS2  = 0;
			isActive = 1;
		}
	}
	else	
    {
        // Se não, anule a contagem e retorne falso p/ chave pressionada
		debounceS2 = 0;
		isActive = 0;
	}
	return(isActive);
}

int CheckSwitchS2_4secHeldDown(void)
{
    int isActive;
    while(isActive !=0)
    {
        if(SWITCH_S2 == 0)
        {
            debounceS2++;  // Incrementa o contador deboucing da S2. 

            if(debounceS2 == 65000)
            {
                // Se a chave S2 continua pressionada após a contagem do debounce, faça:
                debounceS2  = 0;
                isActive = 1;
                break;
            }
        }
        else	
        {
            // Se não, anule a contagem e retorne falso p/ chave pressionada
            debounceS2 = 0;
            isActive = 0;
        }
    }
    return(isActive);
}
