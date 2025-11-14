#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Orden físico:
// [0] Pulgar
// [1] Índice
// [2] Anular 
// [3] Medio  
// [4] Meñique

int servoOpen[5]  = {330, 330, 290, 450, 290};
int servoClose[5] = {450, 450, 450, 250, 450};
String comando = "";

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);
  for (int ch = 0; ch < 5; ch++) pwm.setPWM(ch, 0, servoOpen[ch]);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      comando.trim();
      ejecutarGesto(comando);
      comando = "";
    } else {
      comando += c;
    }
  }
}

void ejecutarGesto(String g) {
  if (g == "CERRAR") moverTodos(true);
  else if (g == "ABRIR") moverTodos(false);
  else if (g == "PULGAR") moverUnDedo(0);
  else if (g == "INDICE") moverUnDedo(1);
  else if (g == "MEDIO") moverUnDedo(3);   // medio
  else if (g == "PAZ") moverDosDedos(1, 3); // índice + medio
  else if (g == "TRES_DEDOS") moverTresDedos(1, 2, 3);  // índice + anular + medio
  else if (g == "CUATRO_DEDOS") moverCuatroDedos(1, 2, 3, 4); // índice + anular + medio + meñique
}

void moverTodos(bool cerrar) {
  for (int ch = 0; ch < 5; ch++) {
    int valor = cerrar ? servoClose[ch] : servoOpen[ch];
    pwm.setPWM(ch, 0, valor);
  }
}

void moverUnDedo(int d) {
  for (int ch = 0; ch < 5; ch++) {
    int valor = (ch == d) ? servoClose[ch] : servoOpen[ch];
    pwm.setPWM(ch, 0, valor);
  }
}

void moverDosDedos(int d1, int d2) {
  for (int ch = 0; ch < 5; ch++) {
    int valor = (ch == d1 || ch == d2) ? servoClose[ch] : servoOpen[ch];
    pwm.setPWM(ch, 0, valor);
  }
}

void moverTresDedos(int d1, int d2, int d3) {
  for (int ch = 0; ch < 5; ch++) {
    int valor = (ch == d1 || ch == d2 || ch == d3) ? servoClose[ch] : servoOpen[ch];
    pwm.setPWM(ch, 0, valor);
  }
}

void moverCuatroDedos(int d1, int d2, int d3, int d4) {
  for (int ch = 0; ch < 5; ch++) {
    int valor = (ch == d1 || ch == d2 || ch == d3 || ch == d4) ? servoClose[ch] : servoOpen[ch];
    pwm.setPWM(ch, 0, valor);
  }
}
