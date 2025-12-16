**# Control de Mano Robótica con Servos (Cámara + Arduino + Python)

**Estado:** Terminado (Versión 2.0)

Este proyecto permite el control en tiempo real de una mano robótica de 5 dedos mediante visión artificial. El sistema procesa gestos de la mano humana captados por una webcam y los traduce en movimientos coordinados a través de un controlador PWM.

---

## Descripción General

El sistema funciona mediante una arquitectura de comunicación serial:
1.  **Visión (Python):** Utiliza MediaPipe para identificar los puntos clave de la mano y determinar qué dedos están extendidos.
2.  **Control (Arduino):** Recibe comandos de texto y gestiona una placa PCA9685 para mover los servos con interpolación suave.

---

## Conexiones de Hardware y Alimentación

> [!IMPORTANTE]
> **Nota Crítica de Alimentación:** Para evitar daños al Arduino y asegurar que los servos tengan suficiente fuerza, se requiere una fuente de poder externa de **5V**.
> * **Conexión de la fuente:** Conecta únicamente los dos cables libres de tu fuente externa: el positivo al pin **V+** y el negativo al pin **GND** de la regleta de alimentación de la PCA9685.
> * **Tierra común:** El GND de la fuente debe estar compartido con el GND del Arduino para cerrar el circuito de señal.

| Componente | Conexión Arduino |
| :--- | :--- |
| **PCA9685 - SDA** | Pin A4 |
| **PCA9685 - SCL** | Pin A5 |
| **PCA9685 - VCC** | Pin 5V (Lógica del chip) |
| **PCA9685 - GND** | Pin GND |
| **Servos** | Canales 0 a 4 del PCA9685 |

---

## Funcionamiento del Software

### Detección de Gestos (Python)
El script detecta automáticamente si la mano es **Derecha o Izquierda** para corregir la lógica del pulgar. Implementa un **buffer de estabilidad** de 5 frames para evitar movimientos erráticos por detecciones falsas.

### Control de Movimiento (Arduino)
El código Arduino utiliza una función de `moverServoSuave` que desplaza los servos en pasos de 5 unidades con un delay de 5ms. Esto asegura un movimiento fluido y protege la estructura mecánica de la mano.

| Gesto Detectado | Acción en la Mano |
| :--- | :--- |
| **ABRIR / CERRAR** | Mueve todos los dedos a su posición límite. |
| **PULGAR / INDICE / etc.** | Cierra el dedo indicado y mantiene el resto abiertos. |
| **PAZ / TRES / CUATRO** | Abre los dedos indicados y cierra los demás. |

---

## Instalación y Orden de Ejecución

Para un funcionamiento correcto, es obligatorio seguir estrictamente este orden:

1.  **Cargar el Código Arduino:** Primero, sube el archivo `mano_final_1.ino` a tu placa desde el IDE de Arduino.
2.  **Preparar el entorno:** Una vez cargado el código, asegúrate de que el Monitor Serie del IDE esté **cerrado** para que el puerto COM quede libre.
3.  **Ejecutar Python:** * Con el Arduino ejecutándose en segundo plano, abre una terminal y asegúrate de tener instaladas las librerías (`opencv-python`, `mediapipe`, `pyserial`).
    * Ejecuta el script: `python "mano camara final.py"`.
    * **Resultado:** Python tomará el control del puerto serie y enviará los comandos de los gestos detectados por la cámara.

---

## Notas de Configuración

* **Puerto Serial:** En el archivo de Python, verifica que la línea `serial.Serial('COM4', 9600)` coincida con el puerto asignado a tu Arduino en el Administrador de Dispositivos.
* **Calibración:** Los valores de los servos (abierto/cerrado) están ajustados específicamente en los arrays `servoOpen` y `servoClose` del código Arduino para cada dedo (Meñique, Pulgar, Anular, Medio, Índice).

---

**Desarrollado por:** Maria-Ignacia Rojas, FabLab UV
**Versión:** v2.0 – Terminado
