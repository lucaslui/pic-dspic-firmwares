/* 
 * File:   DefinesOfMcu.h
 * Author: Lucas
 * Created on 30 de Março de 2014, 12:04
 */

#ifndef MACROS_BASICOS_PASSADOS_PARA_MAIN_H
#define	MACROS_BASICOS_PASSADOS_PARA_MAIN_H

#define BitSetar(arg,nBit)   ((arg) |=  (1<<nBit)) // definições para operações bit a bit
#define BitLimpar(arg,nBit)   ((arg) &= ~(1<<nBit))// para setar,limpar,inverter,e testar bits especificos de um byte.
#define BitInverter(arg,nBit)   ((arg) ^=  (1<<nBit))
#define BitTestar(arg,nBit)   ((arg) &   (1<<nBit))

#endif

