Algoritmo Subtração Espectral para redução de ruído em sinais de voz implementado em DSP
-------------------------------------------------------------

Resumo:

     Este trabalho foca o estudo de algoritmos atuais de redução de ruído e a implementação de um deles em DSP (Digital Signal Processor
     Processador de Sinais Digitais) de forma que possa ser empregado em sistemas de controle de ruído em tempo real. O controle de
     ruído pode ser utilizado na redução de ruído em sinais de áudio e fala, o que permite melhorar a qualidade e inteligibilidade
     destes sinais em sistemas de comunicação, assim como em diversos outros equipamentos como, por exemplo, fones de ouvido, telefones
     celulares e sistemas de reconhecimento automático de fala.

Hardware e Infraestrutura:

      A plataforma escolhida para implementação do algoritmo de redução de ruído é a MPLAB Starter Kit for dsPIC DSC da empresa
      Microchip. Foi escolhida depois de um estudo das placas com DSP disponíveis no mercado destinadas a processamento de áudio. 
      Nesse estudo foram levadas em conta as características da placa, preço, facilidade de aquisição, familiaridade do autor deste
      trabalho com outros processadores da empresa, suporte da comunidade que aborta ideias e soluções online, as bibliotecas
      disponíveis, o acesso grátis ao ambiente de desenvolvimento integrado MPLAB X IDE (Integrated Development Environment) e ao
      compilador de linguagem C MPLAB XC16 (MICROCHIP, 2016). Sua alimentação é de +5 [V] através de um conector mini USB
      (Universal Serial Bus). A plataforma permite, através de uma porta USB, a gravação e/ou depuração do código gerado para o
      processador DSP, ou seja, não há necessidade de outro dispositivo para essas funções. O DSP incorporado na placa é o
      dsPIC33FJ256GP506 da família dsPIC33F da Microchip e é um DSP de ponto-fixo de 16 bits. Contém 256 [kB] de memória de programa e
      16 [kB] de memória RAM (Random Access Memory). Este DSP é considerado também um DSC (Digital Signal Controller), pois contém
      características de processamento de um DSP com a adição de alguns componentes comumente encontrados em microcontroladores.Por se
      tratar de uma placa para processamento de áudio, a plataforma contém um CODEC (acrônimo de codificador/decodificador) com
      conversor A/D com resolução de até 24 bits e frequência máxima de amostragem de 48 [kHz], uma entrada para microfone e uma saída
      para fone de ouvido com impedância de 32[Ω]. O kit também contém uma memória flash externa com comunicação serial do tipo SPI
      (Serial Peripheral Interface) e capacidade de 4 [Mb] (512 [kB]) que pode ser utilizada para armazenar qualquer tipo de dado, como
      por exemplo, um sinal de áudio capturado pelo microfone. O processador se comunica com o CODEC através de um barramento de dados
      do tipo DCI (Data Converter Interface) e outro de controle do tipo serial I2C (Inter-Integrated Circuit). Através do barramento de
      controle é possível ordenar a transferência de dados e configurar alguns parâmetros do CODEC como, por exemplo, o protocolo de
      transferência de dados que será utilizado, a frequência de amostragem desejada, a resolução do conversor A/D, o volume do sinal de
      saída, a filtragem do sinal de entrada etc.
      
Autor:
-------------------------------------------------------------

    nome: Lucas Lui Motta
    email: lucasluimotta@gmail.com
    github: github.com/lucaslui
    linkdin: br.linkedin.com/pub/lucas-lui-motta/86/195/b63/
    facebook: www.facebook.com/lucas.lui.71
