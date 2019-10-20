Projetos de sistemas embarcados com microcontroladores e microprocessadores
-------------------------------------------------------------

Padrão do conteúdo dos projetos:

     Sources
     Simulation
     Schematic
     Output.

Significado:

     Sources: destinado aos códigos produzidos em Linguagem C/C++ para o sistema embarcado do Projeto.
     Simulation: arquivo criado para simulação do projeto (Proteus).
     Schematic: arquivos feitos para a confecção do aparelho físico (esquemático, pcb).
     Output: arquivos de saída como .hex gerado pela compilação dos códigos, destinado a produção.
      
Alguns passos para a programação de sistemas embarcados do tipo bare-metal:

    - Escolher o microcontrolador entre as diversas familias no mercado (PIC - microchip e AVR - atmel).
    - Adotar a IDE que será utilizada, junto com a cadeia de ferramentas (toolchain) para a compilação do programa.
    - Escolher o gravador/programador e o tipo de conexão dele (USB, serial, ethernet). 
    - Configurar o depurador para nível de simulação virtual, ou então nível hardware (JTAG).
      
Programação:

    - Configurar/Tratar registradores de funções especiais (SFR's) do microcontrolador.
    - Configurar os registradores de configuração em tempo de gravação (Fusíveis Externos).
    - Criar/Adicionar bibliotecas para a aplicação.
    - Escolher entre as técnicas de programação para sistemas embarcados.(pooling, máquina de estado, etc).
      
Autor:
-------------------------------------------------------------

    nome: Lucas Lui Motta
    email: lucasluimotta@gmail.com
    github: github.com/lucaslui
    linkedin: www.linkedin.com/in/lucas-lui-motta-eng/
    facebook: www.facebook.com/lucas.lui.71
