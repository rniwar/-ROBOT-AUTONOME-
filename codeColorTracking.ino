#include "DFMobile.h"
#include "HUSKYLENS.h"
#include "Wire.h"

DFMobile car(4, 5, 7, 6); // EN/PWM gauche = 4/5, droite = 7/6
HUSKYLENS huskylens;

const int objetID = 1; // ID de la couleur apprise
const int vitesseBase = 80;
const int correction = 50;

void setup() {
Serial.begin(9600);
Wire.begin();

car.Direction(true, true); // ← confirmé bon réglage pour ton robot

while (!huskylens.begin(Wire)) {
Serial.println("HuskyLens non détecté !");
delay(100);
}

huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION); // Mode suivi de couleur
Serial.println("Mode : Color Tracking actif");
}

void loop() {
if (huskylens.request() && huskylens.countBlocks(objetID) > 0) {
HUSKYLENSResult couleur = huskylens.getBlock(objetID, 0);
int xCenter = couleur.xCenter;

Serial.print("xCenter: ");
Serial.println(xCenter);

if (xCenter < 120) {
// Couleur à gauche
car.Speed(vitesseBase - correction, vitesseBase + correction);
}
else if (xCenter > 200) {
// Couleur à droite
car.Speed(vitesseBase + correction, vitesseBase - correction);
}
else {
// Couleur centrée
car.Speed(vitesseBase, vitesseBase);
}
}
else {
// Aucune couleur détectée
Serial.println("Couleur perdue...");
car.Speed(0, 0);
}

delay(100);
}
