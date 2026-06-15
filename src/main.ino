// ============================================================
// ESTACIONAMENTO INTELIGENTE — Programação Aplicada (POO)
// UNISATC — Engenharia de Computação
// Profa. Dra. Kajiana Nuernberg Sartor Vidotto
// Entrega: 22/06/2026
//
// Conceitos POO aplicados:
//   - Encapsulamento: atributos private + getters/setters
//   - Herança: Veiculo herda Vaga
//   - Polimorfismo: método virtual status() sobrescrito
//   - Composição: Estacionamento TEM vetor de Vagas e objeto Display
// ============================================================

#include <Arduino.h>

// ============================================================
// CLASSE Sensor
// Responsável por medir distância via ultrassônico HC-SR04
// e determinar se uma vaga está ocupada.
// ============================================================
class Sensor {
private:
  int trigPIN;   // pino que envia o pulso
  int echoPIN;   // pino que recebe o eco
  float limiteOcupado; // distância (cm) abaixo da qual a vaga é ocupada

public:
  // Construtor: recebe os pinos e o limiar de ocupação
  Sensor(int trig, int echo, float limite = 10.0) {
    trigPIN       = trig;
    echoPIN       = echo;
    limiteOcupado = limite;

    pinMode(trigPIN, OUTPUT);
    pinMode(echoPIN, INPUT);   // ECHO é entrada
    digitalWrite(trigPIN, LOW);
  }

  // Dispara o sensor e retorna a distância medida em cm
  float medir() {
    // Garante nível baixo antes do pulso
    digitalWrite(trigPIN, LOW);
    delayMicroseconds(2);

    // Pulso de 10 µs para iniciar medição
    digitalWrite(trigPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPIN, LOW);

    // Tempo de retorno do eco em microssegundos
    long duracao = pulseIn(echoPIN, HIGH, 30000); // timeout 30 ms

    // Converte para cm (velocidade do som: 0,034 cm/µs, dividido por 2 = ida)
    float distancia = duracao * 0.034 / 2.0;

    return distancia;
  }

  // Retorna true se a distância medida indica vaga ocupada
  bool estaOcupado() {
    float dist = medir();
    // Distância 0 = sem leitura (timeout) — considera livre
    if (dist == 0) return false;
    return (dist < limiteOcupado);
  }

  // Getter do limite configurado
  float getLimite() { return limiteOcupado; }
};

// ============================================================
// CLASSE Vaga (classe base)
// Representa uma vaga de estacionamento com LED de sinalização.
// ============================================================
class Vaga {
private:
  int    id;            // número identificador da vaga
  int    pinoVerde;     // LED verde = livre
  int    pinoVermelho;  // LED vermelho = ocupado
  bool   ocupada;       // estado atual da vaga

public:
  // Construtor: inicializa pinos e estado
  Vaga(int idVaga, int pVerde, int pVermelho) {
    id           = idVaga;
    pinoVerde    = pVerde;
    pinoVermelho = pVermelho;
    ocupada      = false;

    pinMode(pinoVerde,    OUTPUT);
    pinMode(pinoVermelho, OUTPUT);

    // Estado inicial: vaga livre (verde aceso)
    atualizarLEDs();
  }

  // Atualiza o estado da vaga e os LEDs correspondentes
  void setOcupada(bool estado) {
    ocupada = estado;
    atualizarLEDs();
  }

  // Retorna se a vaga está ocupada
  bool isOcupada() const { return ocupada; }

  // Retorna o ID da vaga
  int getId() const { return id; }

  // Método virtual: exibe o status da vaga no Serial
  // Sobrescrito em Veiculo (polimorfismo)
  virtual String status() const {
    String msg = "Vaga ";
    msg += id;
    msg += ": ";
    msg += (ocupada ? "OCUPADA" : "LIVRE");
    return msg;
  }

  // Destrutor virtual (boa prática com herança)
  virtual ~Vaga() {}

private:
  // Controla os LEDs conforme o estado da vaga
  void atualizarLEDs() {
    digitalWrite(pinoVerde,    ocupada ? LOW  : HIGH);
    digitalWrite(pinoVermelho, ocupada ? HIGH : LOW);
  }
};

// ============================================================
// CLASSE Veiculo (herda Vaga)
// Adiciona o tipo do veículo à vaga ocupada.
// Demonstra herança e polimorfismo.
// ============================================================
class Veiculo : public Vaga {
private:
  String tipoVeiculo; // ex: "Carro", "Moto", "Caminhão"

public:
  // Construtor: repassa pinos à classe base e define o tipo
  Veiculo(int idVaga, int pVerde, int pVermelho, String tipo = "Carro")
    : Vaga(idVaga, pVerde, pVermelho) {
    tipoVeiculo = tipo;
  }

  // Define o tipo do veículo
  void setTipo(String tipo) { tipoVeiculo = tipo; }

  // Retorna o tipo do veículo
  String getTipo() const { return tipoVeiculo; }

  // Sobrescreve status() — polimorfismo
  // Exibe também o tipo do veículo quando a vaga está ocupada
  String status() const override {
    String msg = "Vaga ";
    msg += getId();
    msg += ": ";
    if (isOcupada()) {
      msg += "OCUPADA por " + tipoVeiculo;
    } else {
      msg += "LIVRE";
    }
    return msg;
  }
};

// ============================================================
// CLASSE Display
// Centraliza a exibição das informações no Serial Monitor.
// ============================================================
class Display {
public:
  // Exibe o cabeçalho do painel
  void exibirCabecalho() {
    Serial.println("========================================");
    Serial.println("   ESTACIONAMENTO INTELIGENTE - SATC   ");
    Serial.println("========================================");
  }

  // Exibe o status de uma vaga (recebe referência à classe base)
  void exibirVaga(const Vaga& vaga) {
    Serial.println(vaga.status());
  }

  // Exibe o resumo geral do estacionamento
  void exibirResumo(int livres, int total) {
    Serial.println("----------------------------------------");
    Serial.print("Vagas livres: ");
    Serial.print(livres);
    Serial.print(" / ");
    Serial.println(total);
    if (livres == 0) {
      Serial.println(">>> ESTACIONAMENTO LOTADO! <<<");
    }
    Serial.println("========================================");
    Serial.println();
  }

  // Exibe uma linha separadora
  void exibirSeparador() {
    Serial.println("----------------------------------------");
  }
};

// ============================================================
// CLASSE Estacionamento (composição)
// Agrega Vagas (com Sensores) e o Display.
// Orquestra toda a lógica do sistema.
// ============================================================
class Estacionamento {
private:
  static const int TOTAL_VAGAS = 4;

  Veiculo* vagas[TOTAL_VAGAS];   // vetor de ponteiros para Veiculo (herda Vaga)
  Sensor*  sensores[TOTAL_VAGAS]; // vetor de sensores, um por vaga
  Display  display;               // objeto Display (composição)
  int      vagasLivres;

public:
  // Construtor: inicializa todas as vagas e sensores
  // Pinos definidos conforme montagem no Wokwi
  Estacionamento() {
    vagasLivres = TOTAL_VAGAS;

    // Vaga(id, pinoVerde, pinoVermelho)
    vagas[0] = new Veiculo(1,  2,  3);
    vagas[1] = new Veiculo(2,  4,  5);
    vagas[2] = new Veiculo(3,  7,  8);
    vagas[3] = new Veiculo(4,  9, 10);

    // Sensor(trigPIN, echoPIN, limiteOcupado cm)
    sensores[0] = new Sensor(22, 23, 10.0);
    sensores[1] = new Sensor(24, 25, 10.0);
    sensores[2] = new Sensor(26, 27, 10.0);
    sensores[3] = new Sensor(28, 29, 10.0);
  }

  // Destrutor: libera memória alocada dinamicamente
  ~Estacionamento() {
    for (int i = 0; i < TOTAL_VAGAS; i++) {
      delete vagas[i];
      delete sensores[i];
    }
  }

  // Atualiza o estado de todas as vagas com base nos sensores
  void atualizar() {
    vagasLivres = 0;

    for (int i = 0; i < TOTAL_VAGAS; i++) {
      bool ocupada = sensores[i]->estaOcupado();
      vagas[i]->setOcupada(ocupada);
      if (!ocupada) vagasLivres++;
    }
  }

  // Exibe o painel completo no Serial Monitor
  void exibirPainel() {
    display.exibirCabecalho();

    for (int i = 0; i < TOTAL_VAGAS; i++) {
      display.exibirVaga(*vagas[i]); // polimorfismo: chama status() de Veiculo
    }

    display.exibirResumo(vagasLivres, TOTAL_VAGAS);
  }

  // Retorna quantas vagas estão livres
  int getVagasLivres() const { return vagasLivres; }

  // Retorna o total de vagas
  int getTotalVagas() const { return TOTAL_VAGAS; }
};

// ============================================================
// INSTÂNCIA GLOBAL DO ESTACIONAMENTO
// ============================================================
Estacionamento estacionamento;

// ============================================================
// SETUP — executado uma vez na inicialização
// ============================================================
void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Sistema inicializado!");
}

// ============================================================
// LOOP — executado continuamente
// ============================================================
void loop() {
  // Lê todos os sensores e atualiza o estado das vagas
  estacionamento.atualizar();

  // Exibe o painel no Serial Monitor
  estacionamento.exibirPainel();

  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}
