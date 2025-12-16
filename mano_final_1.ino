#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// -------- DECLARACIÓN DE FUNCIONES --------
// Definiciones necesarias para el compilador
void moverServoSuave(int ch, int objetivo);
void moverTodos(bool cerrar);
void moverUnDedo(int d);
void moverDosDedos(int d1, int d2);
void moverTresDedos(int d1, int d2, int d3);
void moverCuatroDedos(int d1, int d2, int d3, int d4);


// -------- CONFIGURACIÓN DE SERVOS (Tus valores calibrados) --------

int servoOpen[5] = {
  400, // 0: MENIQUE
  400, // 1: PULGAR
  400, // 2: ANULAR
  400, // 3: MEDIO
  360  // 4: INDICE
};

int servoClose[5] = {
  485, // 0: MENIQUE
  230, // 1: PULGAR
  230, // 2: ANULAR
  240, // 3: MEDIO
  490  // 4: INDICE
};

// Mapeo REAL de canales
#define MENIQUE 0
#define PULGAR  1
#define ANULAR  2
#define MEDIO   3
#define INDICE  4

// Posición actual de cada servo
int servoPos[5] = {
  servoOpen[0], // MENIQUE
  servoOpen[1], // PULGAR
  servoOpen[2], // ANULAR
  servoOpen[3], // MEDIO
  servoOpen[4]  // INDICE
};

void setup() {
  Serial.begin(9600);
  Serial.println("Inicializando mano robotica...");
  pwm.begin();
  pwm.setPWMFreq(50);   // Frecuencia estándar para servos (50Hz)

  // Inicializa todos los servos en la posición abierta
  for (int ch = 0; ch < 5; ch++) {
    pwm.setPWM(ch, 0, servoOpen[ch]);
    servoPos[ch] = servoOpen[ch];
  }
}

// -------- MOVIMIENTO SUAVE (Ajustado para mayor robustez) --------
void moverServoSuave(int ch, int objetivo) {
  if (servoPos[ch] == objetivo) return;

  // Paso ajustado a 5 para asegurar que el servo supere la fricción
  int paso = (objetivo > servoPos[ch]) ? 5 : -5;
  int inicio = servoPos[ch];
  
  // Condición de bucle para moverse gradualmente
  while (abs(objetivo - inicio) > abs(paso)) {
      inicio += paso;
      pwm.setPWM(ch, 0, inicio);
      delay(5); // Pequeño delay para movimiento suave y fluido
  }

  // Asegurar que llegue al objetivo final
  pwm.setPWM(ch, 0, objetivo);
  servoPos[ch] = objetivo;
}


// -------- COMANDOS --------
void loop() {
  if (Serial.available()) {
    // Lectura del comando serial más tolerante
    String comando = Serial.readString();
    comando.trim();   // Eliminar saltos de línea y espacios extra

    Serial.print("Comando recibido: [");
    Serial.print(comando);
    Serial.println("]");

    // Gestos de los dedos
    if       (comando == "CERRAR")        moverTodos(true);
    else if (comando == "ABRIR")         moverTodos(false);
    // Para gestos de un solo dedo, el dedo se cierra y el resto se abre.
    else if (comando == "PULGAR")        moverUnDedo(PULGAR);
    else if (comando == "INDICE")        moverUnDedo(INDICE);
    else if (comando == "MEDIO")         moverUnDedo(MEDIO);
    else if (comando == "ANULAR")        moverUnDedo(ANULAR);
    else if (comando == "MEÑIQUE")       moverUnDedo(MENIQUE);
    // Para gestos de dedos múltiples (PAZ, TRES_DEDOS, CUATRO_DEDOS), 
    // la lógica está invertida para que los dedos especificados se abran.
    else if (comando == "PAZ")           moverDosDedos(INDICE, MEDIO);
    else if (comando == "TRES_DEDOS")    moverTresDedos(INDICE, MEDIO, ANULAR);
    else if (comando == "CUATRO_DEDOS")  moverCuatroDedos(MENIQUE, INDICE, MEDIO, ANULAR);
    else 
      Serial.println("Comando desconocido. (Verifique mayúsculas/minúsculas)");
  }
}

// -------- FUNCIONES DE MOVIMIENTO --------

void moverTodos(bool cerrar) {
  for (int ch = 0; ch < 5; ch++) {
    moverServoSuave(ch, cerrar ? servoClose[ch] : servoOpen[ch]);
  }
  Serial.println(cerrar ? "Mano cerrada." : "Mano abierta.");
}

// Función: Cierra el dedo 'd', abre el resto. (Correcto)
void moverUnDedo(int d) {
  for (int ch = 0; ch < 5; ch++) {
    moverServoSuave(ch, (ch == d) ? servoClose[ch] : servoOpen[ch]);
  }
  Serial.print("Gesto: Un dedo ("); Serial.print(d); Serial.println(")");
}

// Función: Abre los dedos d1 y d2, cierra el resto. (Lógica invertida corregida)
// Se usa para PAZ (Índice y Medio Abiertos)
void moverDosDedos(int d1, int d2) {
  for (int ch = 0; ch < 5; ch++) {
    // Condición: si el canal es d1 O d2, ¡abre! Si no, ¡cierra!
    moverServoSuave(ch, (ch == d1 || ch == d2) ? servoOpen[ch] : servoClose[ch]); 
  }
  Serial.println("Gesto: Dos dedos (PAZ)");
}

// Función: Abre los dedos d1, d2 y d3, cierra el resto. (Lógica invertida corregida)
// Se usa para TRES_DEDOS (Índice, Medio, Anular Abiertos)
void moverTresDedos(int d1, int d2, int d3) {
  for (int ch = 0; ch < 5; ch++) {
    // Condición: si el canal coincide, ¡abre! Si no, ¡cierra!
    moverServoSuave(ch, (ch == d1 || ch == d2 || ch == d3) ? servoOpen[ch] : servoClose[ch]);
  }
  Serial.println("Gesto: Tres dedos");
}

// Función: Abre los dedos d1, d2, d3 y d4, cierra el resto. (Lógica invertida corregida)
// Se usa para CUATRO_DEDOS (todos menos Pulgar Abiertos)
void moverCuatroDedos(int d1, int d2, int d3, int d4) {
  for (int ch = 0; ch < 5; ch++) {
    // Condición: si el canal coincide, ¡abre! Si no, ¡cierra!
    moverServoSuave(ch, (ch == d1 || ch == d2 || ch == d3 || ch == d4) ? servoOpen[ch] : servoClose[ch]);
  }
  Serial.println("Gesto: Cuatro dedos");
}