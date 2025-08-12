#include "DFMobile.h"
#include "HUSKYLENS.h"
#include "Wire.h"
DFMobile car(4, 5, 7, 6);
HUSKYLENS huskylens;
const int vitesseBase = 80; // Vitesse de base (loin)
const int vitesseMin = 50; // Vitesse minimale (proche)
const int maxCorrection = 60; // Correction max pour tourner
const int faceID = 1; // ID du visage appris

void setup() {
Serial.begin(9600);
Wire.begin();

car.Direction(false, false); // ← bon sens moteur confirmé

while (!huskylens.begin(Wire)) {
Serial.println("HuskyLens non détecté !");
delay(100);
}
huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);
Serial.println("Mode reconnaissance faciale actif !");
}
void loop() {
if (huskylens.request() && huskylens.countBlocks(faceID) > 0) {
HUSKYLENSResult face = huskylens.getBlock(faceID, 0);
int xCenter = face.xCenter;
int erreur = xCenter - 160; // Écart par rapport au centre de la caméra

int correction = map(abs(erreur), 0, 160, 0, maxCorrection);
correction = constrain(correction, 0, maxCorrection);
int vitesse = map(abs(erreur), 0, 160, vitesseBase, vitesseMin);
vitesse = constrain(vitesse, vitesseMin, vitesseBase);
Serial.print("xCenter: ");
Serial.print(xCenter);
Serial.print(" | Correction: ");
Serial.print(correction);
Serial.print(" | Vitesse: ");
Serial.println(vitesse);
if (erreur < -15) {
// Visage à gauche
car.Speed(-(vitesse + correction), -(vitesse - correction));
}
else if (erreur > 15) {
// Visage à droite
car.Speed(-(vitesse - correction), -(vitesse + correction));
}
else {
// Visage centré
car.Speed(-vitesse, -vitesse);
}
}
else {
Serial.println("Visage non détecté - arrêt !");
car.Speed(0, 0);
}
}
