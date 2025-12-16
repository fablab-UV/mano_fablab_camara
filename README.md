# Control de mano con servos mediante cámara, Arduino y Python

**Estado:** Terminado

Actualmente el sistema controla 5 dedos y gestos predefinidos, pero se planea ampliar la detección, añadir más grados de libertad y mejorar la precisión del movimiento.

---

## Descripción general

Este proyecto permite controlar una mano robótica mediante gestos de la mano humana detectados con una cámara web.
El sistema utiliza **Python** (con OpenCV y MediaPipe) para el reconocimiento visual y **Arduino** para el control de los servomotores.

Cuando se detecta un gesto (por ejemplo, “paz”, “índice”, “pulgar arriba”, etc.), el programa en Python envía un comando al Arduino a través del puerto serie.
El Arduino interpreta este comando y mueve los servos de la mano robótica para imitar el gesto.

---

## Tecnologías utilizadas

* **Python 3**
* **OpenCV**: Captura y procesamiento de video.
* **MediaPipe Hands**: Detección de puntos clave de la mano.
* **Arduino UNO / Nano**
* **Adafruit PCA9685**: Controlador PWM para hasta 16 servos.
* **Librerías de Arduino:**

  * `Wire.h`
  * `Adafruit_PWMServoDriver.h`

---

## Estructura del proyecto

### Código en Python

El programa:

1. Captura el video desde la cámara.
2. Detecta la posición de los dedos mediante MediaPipe.
3. Determina qué dedos están levantados o cerrados.
4. Asocia combinaciones de dedos a gestos predefinidos.
5. Envía el nombre del gesto al Arduino por puerto serie.

### Código en Arduino

El programa:

* Recibe los comandos enviados desde Python.
* Interpreta el gesto recibido.
* Mueve los servos a las posiciones definidas para cada dedo.
* Permite configurar las posiciones abiertas y cerradas de los servos según el montaje físico.

---

## Gestos reconocidos actualmente

| Gesto detectado | Acción en la mano robótica            |
| --------------- | ------------------------------------- |
| CERRAR          | Cierra todos los dedos                |
| ABRIR           | Abre todos los dedos                  |
| PULGAR          | Solo mueve el pulgar                  |
| INDICE          | Solo mueve el índice                  |
| MEDIO           | Solo mueve el dedo medio              |
| PAZ             | Mueve índice y medio                  |
| TRES_DEDOS      | Mueve índice, medio y anular          |
| CUATRO_DEDOS    | Mueve índice, medio, anular y meñique |

---

## Conexiones de hardware

| Componente     | Conexión Arduino                  |
| -------------- | --------------------------------- |
| PCA9685 - SDA  | A4                                |
| PCA9685 - SCL  | A5                                |
| PCA9685 - VCC  | 5V                                |
| PCA9685 - GND  | GND                               |
| Servos         | Canales 0–4 del PCA9685           |
| Fuente externa | 5V (V+) y GND (compartido con Arduino) |

---

## Instalación y uso

### En Arduino IDE

1. Instalar las librerías necesarias:

   * Adafruit PWM Servo Driver Library
   * Wire
2. Cargar el código del Arduino proporcionado.
3. Conectar el Arduino al PC y anotar el puerto COM correspondiente.

### En Python

1. Instalar dependencias:

   ```bash
   pip install opencv-python mediapipe pyserial
   ```
2. Editar el puerto serial en el código:

   ```python
   arduino = serial.Serial('COM7', 9600)
   ```
3. Ejecutar el programa:

   ```bash
   python control_mano.py
   ```
4. Apuntar la cámara hacia la mano y realizar los gestos.

---

## Próximas mejoras (en desarrollo)

* Reconocimiento de nuevos gestos como “ok”, “rock” o “puño”.
* Filtrado y suavizado de detección para reducir errores.
* Interfaz gráfica que muestre el estado de la mano robótica.
* Modo de calibración de posiciones desde software.

---

## Ejemplo de uso

```
[Python] Gesto detectado: PAZ
[Arduino] Ejecutando gesto: índice + medio
```

La mano robótica replica el gesto ✌️ frente a la cámara.

---

Créditos

Desarrollado por [Maria-Ignacia Rojas, FabLab UV]
Basado en tecnologías de *OpenCV*, *MediaPipe* y *Adafruit*.
Versión actual: *v1 – En desarrollo*
