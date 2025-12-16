import cv2
import mediapipe as mp
import serial
import time

# ---------------- SERIAL ----------------
try:
    arduino = serial.Serial('COM4', 9600, timeout=1)  # Cambia 'COM4' si tu puerto es diferente
    time.sleep(2)  # Espera para establecer la conexión
    print("Conectado a Arduino")
except:
    print("No se pudo conectar con Arduino")
    exit()

# ---------------- MEDIAPIPE ----------------
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    max_num_hands=1,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.6
)
mp_draw = mp.solutions.drawing_utils

# ---------------- CAMARA ----------------
cap = cv2.VideoCapture(0)

estado_anterior = ""
cooldown = 0
MARGIN = 0.05  # margen para detección estable de dedos

GESTO_BUFFER = []
BUFFER_SIZE = 5  # Número de frames para confirmar un gesto

# ---------- Función para verificar si un dedo está extendido ----------
def dedo_extendido(pip, tip):
    return tip.y < pip.y - MARGIN

# ---------- Función para detectar gestos ----------
def detectar_gesto(dedos):
    # dedos = [meñique, índice, anular, medio, pulgar]

    # PAZ: índice y medio extendidos, otros doblados
    if dedos[1] == 1 and dedos[3] == 1 and dedos[0] == 0 and dedos[2] == 0 and dedos[4] == 0:
        return "PAZ"
    # CUATRO_DEDOS: meñique, índice, anular, medio extendidos, pulgar doblado
    elif dedos[0] == 1 and dedos[1] == 1 and dedos[2] == 1 and dedos[3] == 1 and dedos[4] == 0:
        return "CUATRO_DEDOS"
    # TRES_DEDOS: índice, medio, anular extendidos, pulgar y meñique doblados
    elif dedos[1] == 1 and dedos[2] == 1 and dedos[3] == 1 and dedos[0] == 0 and dedos[4] == 0:
        return "TRES_DEDOS"
    # Gestos de un solo dedo
    elif dedos[1] == 1 and sum(dedos) == 1:
        return "INDICE"
    elif dedos[3] == 1 and sum(dedos) == 1:
        return "MEDIO"
    elif dedos[2] == 1 and sum(dedos) == 1:
        return "ANULAR"
    elif dedos[0] == 1 and sum(dedos) == 1:
        return "MEÑIQUE"
    elif dedos[4] == 1 and sum(dedos) == 1:
        return "PULGAR"
    # CERRAR (ningún dedo extendido)
    elif sum(dedos) == 0:
        return "CERRAR"
    # ABRIR (todos los dedos extendidos)
    elif sum(dedos) == 5:
        return "ABRIR"
    else:
        return None  # No enviar comando si gesto no está claro

# ---------- LOOP PRINCIPAL ----------
while True:
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)  # Voltea la imagen horizontalmente
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    gesto = None

    if result.multi_hand_landmarks:
        hand = result.multi_hand_landmarks[0]
        mp_draw.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)

        lm = hand.landmark
        handedness = result.multi_handedness[0].classification[0].label

        # Dedos: [meñique, índice, anular, medio, pulgar]
        dedos = [0, 0, 0, 0, 0]

        # PULGAR (Determinar si el pulgar está extendido dependiendo de la mano)
        if handedness == "Right":
            dedos[4] = 1 if lm[4].x < lm[3].x else 0  # Pulgar derecho extendido
        else:
            dedos[4] = 1 if lm[4].x > lm[3].x else 0  # Pulgar izquierdo extendido

        # OTROS DEDOS
        dedos[1] = 1 if dedo_extendido(lm[6], lm[8]) else 0  # Índice
        dedos[3] = 1 if dedo_extendido(lm[10], lm[12]) else 0  # Medio
        dedos[2] = 1 if dedo_extendido(lm[14], lm[16]) else 0  # Anular
        dedos[0] = 1 if dedo_extendido(lm[18], lm[20]) else 0  # Meñique

        # Detectar gesto
        gesto_detectado = detectar_gesto(dedos)
        if gesto_detectado:
            # Añadir al buffer de estabilidad
            GESTO_BUFFER.append(gesto_detectado)
            if len(GESTO_BUFFER) > BUFFER_SIZE:
                GESTO_BUFFER.pop(0)

            # Solo aceptar gesto si se mantiene estable en todos los frames del buffer
            if len(GESTO_BUFFER) == BUFFER_SIZE and all(g == GESTO_BUFFER[0] for g in GESTO_BUFFER):
                gesto = GESTO_BUFFER[0]

    # Enviar comando solo si hay gesto válido y cooldown
    if gesto and gesto != estado_anterior and cooldown == 0:
        arduino.write((gesto + "\n").encode())
        print("Enviado:", gesto, "Dedos:", dedos)
        estado_anterior = gesto
        cooldown = 3  # evita envíos demasiado rápidos

    if cooldown > 0:
        cooldown -= 1

    # Mostrar cámara
    cv2.imshow("Control Mano Robot", frame)
    if cv2.waitKey(1) & 0xFF == 27:  # Salir con la tecla 'Esc'
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()
    