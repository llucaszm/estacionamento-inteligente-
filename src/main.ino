// ============================================================
// ESTACIONAMENTO INTELIGENTE — Programação Aplicada (POO)
// UNISATC — Engenharia de Computação
// Profa. Kajiana Nuernberg Sartor Vidotto
//
// Conceitos POO aplicados:
//   - Encapsulamento: atributos private + getters/setters
//   - Herança: Veiculo herda Vaga
//   - Polimorfismo: método virtual status() sobrescrito
//   - Composição: Estacionamento TEM vetor de Vagas, Sensores e Display
//
// Hardware:
//   - Arduino Mega 2560
//   - 4x HC-SR04 (sensores ultrassônicos)
//   - 4x LED verde + 4x LED vermelho
//   - 8x Resistor 220Ω
//   - 1x LCD 20x4 com módulo I2C (endereço 0x27)
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================
// CLASSE Sensor
// Responsável por medir distância via HC-SR04
// e determinar se uma vaga está ocupada.
// ============================================================
class Sensor {
private:
  int   trigPIN;
  int   echoPIN;
  float limiteOcupado;

public:
  Sensor(int trig, int echo, float limite = 10.0) {
    trigPIN       = trig;
    echoPIN       = echo;
    limiteOcupado = limite;
    pinMode(trigPIN, OUTPUT);
    pinMode(echoPIN, INPUT);
    digitalWrite(trigPIN, LOW);
  }

  // Dispara pulso e retorna distância em cm
  float medir() {
    digitalWrite(trigPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPIN, LOW);
    long duracao = pulseIn(echoPIN, HIGH, 30000);
    return duracao * 0.034 / 2.0;
  }

  // Retorna true se há objeto dentro do limite
  bool estaOcupado() {
    float dist = medir();
    if (dist == 0) return false;
    return (dist < limiteOcupado);
  }
};

// ============================================================
// CLASSE Vaga (classe base)
// Representa uma vaga com LEDs de sinalização.
// ============================================================
class Vaga {
private:
  int  id;
  int  pinoVerde;
  int  pinoVermelho;
  bool ocupada;

public:
  Vaga(int idVaga, int pVerde, int pVermelho) {
    id           = idVaga;
    pinoVerde    = pVerde;
    pinoVermelho = pVermelho;
    ocupada      = false;
    pinMode(pinoVerde,    OUTPUT);
    pinMode(pinoVermelho, OUTPUT);
    atualizarLEDs();
  }

  void setOcupada(bool estado) {
    ocupada = estado;
    atualizarLEDs();
  }

  bool isOcupada() const { return ocupada; }
  int  getId()     const { return id; }

  // Método virtual — polimorfismo
  virtual String status() const {
    String msg = "Vaga ";
    msg += id;
    msg += ": ";
    msg += (ocupada ? "OCUPADA" : "LIVRE");
    return msg;
  }

  virtual ~Vaga() {}

private:
  void atualizarLEDs() {
    digitalWrite(pinoVerde,    ocupada ? LOW  : HIGH);
    digitalWrite(pinoVermelho, ocupada ? HIGH : LOW);
  }
};

// ============================================================
// CLASSE Veiculo (herda Vaga)
// Adiciona tipo do veículo. Demonstra herança e polimorfismo.
// ============================================================
class Veiculo : public Vaga {
private:
  String tipoVeiculo;

public:
  Veiculo(int idVaga, int pVerde, int pVermelho, String tipo = "Carro")
    : Vaga(idVaga, pVerde, pVermelho) {
    tipoVeiculo = tipo;
  }

  void   setTipo(String tipo) { tipoVeiculo = tipo; }
  String getTipo() const      { return tipoVeiculo; }

  // Sobrescreve status() — polimorfismo
  String status() const override {
    String msg = "Vaga ";
    msg += getId();
    msg += ": ";
    msg += isOcupada() ? "OCUPADA por " + tipoVeiculo : "LIVRE";
    return msg;
  }
};

// ============================================================
// CLASSE Display
// Gerencia Serial Monitor e LCD I2C 20x4.
// Demonstra composição — Estacionamento TEM um Display.
// ============================================================
class Display {
private:
  LiquidCrystal_I2C lcd; // LCD 20 colunas x 4 linhas

public:
  // Construtor: 20 colunas, 4 linhas
  Display() : lcd(0x27, 20, 4) {}

  // Inicialização separada — chamada após Wire.begin() no setup()
  void inicializar() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    // Tela de boas-vindas
    lcd.setCursor(2, 1);
    lcd.print("Estacionamento");
    lcd.setCursor(4, 2);
    lcd.print("Inteligente");
    delay(1500);
    lcd.clear();
  }

  // Cabeçalho no Serial Monitor
  void exibirCabecalho() {
    Serial.println("========================================");
    Serial.println("   ESTACIONAMENTO INTELIGENTE - SATC   ");
    Serial.println("========================================");
  }

  // Exibe status de uma vaga no Serial (polimorfismo via Vaga&)
  void exibirVaga(const Vaga& vaga) {
    Serial.println(vaga.status());
  }

  // Atualiza Serial Monitor e LCD 20x4
  void exibirResumo(int livres, int total, const Vaga* vagas[], int numVagas) {

    // ── Serial Monitor ──────────────────────────────
    Serial.println("----------------------------------------");
    Serial.print("Vagas livres: ");
    Serial.print(livres);
    Serial.print(" / ");
    Serial.println(total);
    if (livres == 0) Serial.println(">>> ESTACIONAMENTO LOTADO! <<<");
    Serial.println("========================================");
    Serial.println();

    // ── LCD 20x4 ────────────────────────────────────
    lcd.clear();

    // Linha 0: título (20 chars)
    // "  Estacionamento    "
    lcd.setCursor(0, 0);
    lcd.print("  Estacionamento  ");

    // Linha 1: resumo de vagas
    // "Livres: 3  Ocup: 1  "
    lcd.setCursor(0, 1);
    lcd.print("Livres:");
    lcd.print(livres);
    lcd.print("  Ocup:");
    lcd.print(total - livres);
    if (livres == 0) {
      lcd.print(" LOTADO!");
    } else {
      lcd.print("        ");
    }

    // Linha 2: Vaga 1 e Vaga 2
    // "V1: LIVRE   V2:OCUP "
    lcd.setCursor(0, 2);
    lcd.print("V1:");
    lcd.print(vagas[0]->isOcupada() ? "OCUP  " : "LIVRE ");
    lcd.print("V2:");
    lcd.print(vagas[1]->isOcupada() ? "OCUP  " : "LIVRE ");

    // Linha 3: Vaga 3 e Vaga 4
    // "V3: LIVRE   V4:OCUP "
    lcd.setCursor(0, 3);
    lcd.print("V3:");
    lcd.print(vagas[2]->isOcupada() ? "OCUP  " : "LIVRE ");
    lcd.print("V4:");
    lcd.print(vagas[3]->isOcupada() ? "OCUP  " : "LIVRE ");
  }
};

// ============================================================
// CLASSE Estacionamento (composição)
// Orquestra vagas, sensores e display.
// ============================================================
class Estacionamento {
private:
  static const int TOTAL_VAGAS = 4;
  Veiculo* vagas[TOTAL_VAGAS];
  Sensor*  sensores[TOTAL_VAGAS];
  Display  display;
  int      vagasLivres;

public:
  Estacionamento() {
    vagasLivres = TOTAL_VAGAS;

    // Veiculo(id, pinoVerde, pinoVermelho)
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

  ~Estacionamento() {
    for (int i = 0; i < TOTAL_VAGAS; i++) {
      delete vagas[i];
      delete sensores[i];
    }
  }

  // Inicializa LCD — chamado após Wire.begin() no setup()
  void inicializar() {
    display.inicializar();
  }

  // Lê sensores e atualiza estado das vagas
  void atualizar() {
    vagasLivres = 0;
    for (int i = 0; i < TOTAL_VAGAS; i++) {
      bool ocupada = sensores[i]->estaOcupado();
      vagas[i]->setOcupada(ocupada);
      if (!ocupada) vagasLivres++;
    }
  }

  // Exibe painel no Serial + LCD
  void exibirPainel() {
    display.exibirCabecalho();
    for (int i = 0; i < TOTAL_VAGAS; i++) {
      display.exibirVaga(*vagas[i]); // polimorfismo
    }
    const Vaga* vagasBase[TOTAL_VAGAS];
    for (int i = 0; i < TOTAL_VAGAS; i++) vagasBase[i] = vagas[i];
    display.exibirResumo(vagasLivres, TOTAL_VAGAS, vagasBase, TOTAL_VAGAS);
  }

  int getVagasLivres() const { return vagasLivres; }
  int getTotalVagas()  const { return TOTAL_VAGAS; }
};

// ============================================================
// INSTÂNCIA GLOBAL
// ============================================================
Estacionamento estacionamento;

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(500);
  estacionamento.inicializar();
  Serial.println("Sistema inicializado!");
}

// ============================================================
// LOOP — atualiza a cada 2 segundos
// ============================================================
void loop() {
  estacionamento.atualizar();
  estacionamento.exibirPainel();
  delay(2000);
}

