🅿️ Estacionamento Inteligente
Sistema de monitoramento de vagas em tempo real com sensores ultrassônicos, LEDs de sinalização e painel via Serial Monitor.
UNISATC — Engenharia de Computação | Programação Aplicada  
Profa. Dra. Kajiana Nuernberg Sartor Vidotto | 2026
---
📋 Descrição do Projeto
O sistema monitora 4 vagas de estacionamento em tempo real. Cada vaga possui:
Um sensor ultrassônico HC-SR04 que detecta presença de veículo
Um LED verde (vaga livre) e um LED vermelho (vaga ocupada)
Atualização automática a cada 2 segundos
O painel exibe o status de cada vaga e a contagem total de vagas disponíveis no Serial Monitor.
---
🔧 Componentes de Hardware
Componente	Quantidade	Função
Arduino (Wokwi)	1	Microcontrolador principal
HC-SR04	4	Sensor ultrassônico por vaga
LED Verde	4	Sinalização: vaga livre
LED Vermelho	4	Sinalização: vaga ocupada
Resistor 220Ω	8	Limitador de corrente dos LEDs
---
📌 Mapeamento de Pinos
Vaga	LED Verde	LED Vermelho	Sensor TRIG	Sensor ECHO
1	2	3	22	23
2	4	5	24	25
3	7	8	26	27
4	9	10	28	29
---
🏗️ Arquitetura do Projeto (POO)
```
Estacionamento
├── TEM → Veiculo[4]   (composição)
│           └── HERDA → Vaga   (herança)
├── TEM → Sensor[4]    (composição)
└── TEM → Display      (composição)
```
Classes
Classe	Responsabilidade
`Sensor`	Mede distância via HC-SR04, detecta presença
`Vaga`	Classe base: estado da vaga + LEDs (encapsulamento)
`Veiculo`	Herda `Vaga`, adiciona tipo do veículo (herança + polimorfismo)
`Display`	Exibe informações no Serial Monitor
`Estacionamento`	Orquestra tudo (composição)
Pilares de POO
Encapsulamento — todos os atributos são `private`, acesso via métodos públicos
Herança — `Veiculo` herda `Vaga` (`Veiculo` É-UM `Vaga`)
Polimorfismo — método `status()` é `virtual` em `Vaga` e `override` em `Veiculo`
Composição — `Estacionamento` TEM vagas, sensores e display
---
🚀 Como Rodar no Wokwi
Acesse wokwi.com e crie um novo projeto Arduino
Cole o conteúdo de `main.ino` no editor
Adicione os componentes conforme o mapeamento de pinos acima
Clique em ▶ Start Simulation
Abra o Serial Monitor para ver o painel em tempo real
Coloque objetos na frente dos sensores para simular veículos
---
📁 Estrutura do Repositório
```
estacionamento-inteligente/
├── src/
│   └── main.ino               ← código principal com todas as classes
├── docs/
│   ├── descricao_classes.md   ← descrição detalhada de cada classe e método
│   ├── diagrama_classes.png   ← diagrama UML (draw.io)
│   └── esquema_circuito.png   ← print do circuito no Wokwi
├── slides/
│   └── apresentacao.pdf       ← slides da apresentação
└── README.md                  ← este arquivo
```
---
👥 Equipe
## 👥 Equipe

| Membro                      | Responsabilidade                                  |
|-----------------------------|---------------------------------------------------|
| Lucas Mariot                | Integração: Display, Estacionamento e Wokwi       |
| Pedro Vitor Ouriques Albino | Código: classes Sensor, Vaga e Veiculo            |
| Alice da Cruz Justi         | Testes, simulação e esquema do circuito           |
| Cauã de Souza Borges        | Documentação: README, UML, descrição de classes   |
| Jean Carlo Braz da Silveira | Hardware, mapeamento de pinos e validação         |
📊 Saída Esperada (Serial Monitor)
```
========================================
   ESTACIONAMENTO INTELIGENTE - SATC   
========================================
Vaga 1: LIVRE
Vaga 2: OCUPADA por Carro
Vaga 3: LIVRE
Vaga 4: OCUPADA por Carro
----------------------------------------
Vagas livres: 2 / 4
========================================
```
