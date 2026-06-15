Descrição das Classes — Estacionamento Inteligente
Visão Geral
O sistema é composto por 5 classes que juntas aplicam todos os pilares de POO:
encapsulamento, herança, polimorfismo e composição.
---
Classe `Sensor`
Arquivo: `main.ino`  
Responsabilidade: Medir distância via sensor ultrassônico HC-SR04 e determinar se uma vaga está ocupada.
Atributos (private)
Atributo	Tipo	Descrição
`trigPIN`	`int`	Pino TRIG do HC-SR04 (saída — envia pulso)
`echoPIN`	`int`	Pino ECHO do HC-SR04 (entrada — recebe eco)
`limiteOcupado`	`float`	Distância em cm abaixo da qual a vaga é considerada ocupada
Métodos (public)
Método	Retorno	Descrição
`Sensor(trig, echo, limite)`	—	Construtor: configura pinos e limiar
`medir()`	`float`	Dispara pulso e retorna distância em cm
`estaOcupado()`	`bool`	Retorna `true` se distância < limiteOcupado
`getLimite()`	`float`	Retorna o limiar configurado
Como funciona
```
TRIG envia pulso de 10 µs
→ HC-SR04 emite ultrassom
→ ECHO fica HIGH pelo tempo de retorno
→ pulseIn() mede esse tempo em µs
→ distância (cm) = tempo × 0,034 / 2
```
---
Classe `Vaga` (classe base)
Arquivo: `main.ino`  
Responsabilidade: Representar uma vaga física com LEDs de sinalização.
Atributos (private)
Atributo	Tipo	Descrição
`id`	`int`	Número identificador da vaga (1 a 4)
`pinoVerde`	`int`	Pino do LED verde (vaga livre)
`pinoVermelho`	`int`	Pino do LED vermelho (vaga ocupada)
`ocupada`	`bool`	Estado atual da vaga
Métodos (public)
Método	Retorno	Descrição
`Vaga(id, pVerde, pVermelho)`	—	Construtor: configura pinos e estado inicial
`setOcupada(bool)`	`void`	Atualiza estado e aciona LEDs
`isOcupada()`	`bool`	Retorna estado atual
`getId()`	`int`	Retorna o ID da vaga
`status()`	`String`	Método virtual — retorna string com status da vaga
Método privado
Método	Retorno	Descrição
`atualizarLEDs()`	`void`	Acende LED verde (livre) ou vermelho (ocupado)
---
Classe `Veiculo` (herda `Vaga`)
Arquivo: `main.ino`  
Responsabilidade: Estender `Vaga` adicionando o tipo do veículo estacionado.  
Relação: `Veiculo` É-UM `Vaga` (herança)
Atributo adicional (private)
Atributo	Tipo	Descrição
`tipoVeiculo`	`String`	Tipo do veículo: "Carro", "Moto", "Caminhão"
Métodos (public)
Método	Retorno	Descrição
`Veiculo(id, pVerde, pVermelho, tipo)`	—	Construtor: chama `Vaga()` e define tipo
`setTipo(String)`	`void`	Atualiza o tipo do veículo
`getTipo()`	`String`	Retorna o tipo do veículo
`status()`	`String`	Sobrescreve `Vaga::status()` — inclui tipo do veículo
Polimorfismo
```cpp
// Ponteiro para a classe base — chama o método da classe derivada
Vaga* v = new Veiculo(1, 2, 3, "Moto");
Serial.println(v->status());
// Saída: "Vaga 1: OCUPADA por Moto"  ← método de Veiculo, não de Vaga
```
---
Classe `Display`
Arquivo: `main.ino`  
Responsabilidade: Centralizar toda a saída de informações no Serial Monitor.
Métodos (public)
Método	Retorno	Descrição
`exibirCabecalho()`	`void`	Exibe o cabeçalho do painel
`exibirVaga(Vaga&)`	`void`	Exibe o status de uma vaga (recebe classe base — polimorfismo)
`exibirResumo(livres, total)`	`void`	Exibe contagem de vagas e alerta de lotação
`exibirSeparador()`	`void`	Exibe linha separadora
---
Classe `Estacionamento` (composição)
Arquivo: `main.ino`  
Responsabilidade: Orquestrar todo o sistema — agrega vagas, sensores e display.  
Relação: `Estacionamento` TEM vagas (`Veiculo[]`) e TEM display (`Display`) — composição
Atributos (private)
Atributo	Tipo	Descrição
`TOTAL_VAGAS`	`const int`	Número total de vagas (4)
`vagas[]`	`Veiculo*[4]`	Vetor de ponteiros para as vagas
`sensores[]`	`Sensor*[4]`	Vetor de ponteiros para os sensores
`display`	`Display`	Objeto display (composição)
`vagasLivres`	`int`	Contador de vagas disponíveis
Métodos (public)
Método	Retorno	Descrição
`Estacionamento()`	—	Construtor: instancia todas as vagas e sensores
`~Estacionamento()`	—	Destrutor: libera memória (`delete`)
`atualizar()`	`void`	Lê sensores e atualiza estado de cada vaga
`exibirPainel()`	`void`	Exibe painel completo via Display
`getVagasLivres()`	`int`	Retorna número de vagas livres
`getTotalVagas()`	`int`	Retorna total de vagas
---
Diagrama de Dependências
```
Estacionamento
├── TEM → Veiculo[4]   (composição / agregação)
│           └── HERDA → Vaga   (herança)
├── TEM → Sensor[4]    (composição)
└── TEM → Display      (composição)
```
---
Pilares de POO no Projeto
Pilar	Onde aparece
Encapsulamento	Todos os atributos são `private`; acesso via métodos públicos
Herança	`Veiculo` herda `Vaga` — reutiliza pinos, LEDs e estado
Polimorfismo	`status()` é `virtual` em `Vaga` e `override` em `Veiculo`
Composição	`Estacionamento` contém `Veiculo[]`, `Sensor[]` e `Display`
