import cv2
import mediapipe as mp
import serial
import time

arduino = serial.Serial('COM7', 9600)
time.sleep(2)

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(
    max_num_hands=1,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.5
)
mp_drawing = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)
estado_anterior = ""

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    frame = cv2.flip(frame, 1)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    if result.multi_hand_landmarks:
        hand = result.multi_hand_landmarks[0]
        mp_drawing.draw_landmarks(frame, hand, mp_hands.HAND_CONNECTIONS)

        lm = hand.landmark
        # [pulgar, índice, medio, anular, meñique]
        dedos_arriba = [0, 0, 0, 0, 0]

        # Pulgar (usa eje X)
        if lm[4].x < lm[3].x:
            dedos_arriba[0] = 1

        # Otros dedos (usa eje Y)
        for i, tip in enumerate([8, 12, 16, 20]):
            if lm[tip].y < lm[tip - 2].y:
                dedos_arriba[i + 1] = 1

        # Identificar gestos
        gesto = ""
        if dedos_arriba == [0, 1, 0, 0, 0]:
            gesto = "INDICE"
        elif dedos_arriba == [0, 1, 1, 0, 0]:
            gesto = "PAZ"
        elif dedos_arriba == [0, 1, 1, 1, 0]:
            gesto = "TRES_DEDOS"
        elif dedos_arriba == [0, 1, 1, 1, 1]:
            gesto = "CUATRO_DEDOS"
        elif dedos_arriba == [1, 0, 0, 0, 0]:
            gesto = "PULGAR"
        elif dedos_arriba == [0, 0, 1, 0, 0]:
            gesto = "MEDIO"
        elif sum(dedos_arriba) == 0:
            gesto = "CERRAR"
        elif sum(dedos_arriba) == 5:
            gesto = "ABRIR"

        if gesto and gesto != estado_anterior:
            estado_anterior = gesto
            print("Gesto detectado:", gesto)
            arduino.write((gesto + '\n').encode())

    cv2.imshow("Control de mano", frame)
    if cv2.waitKey(5) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()
