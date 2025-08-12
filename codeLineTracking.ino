#include "DFMobile.h"
#include "HUSKYLENS.h"
#include "Wire.h"
DFMobile car(4, 5, 7, 6); // EN/PWM gauche = 4/5, EN/PWM droite = 7/6
HUSKYLENS huskylens;
const int vitesseBase = 80;
const int vitesseMin = 50;
const int maxCorrection = 60;
const unsigned long delaiPerte = 800;
unsigned long dernierContact = 0;
int derniereDirection = 0;
void setup() {
Serial.begin(9600);
Wire.begin();

// Teste les 4 possibilités ici jusqu’à ce que tout fonctionne correctement :
car.Direction(false, false); // ← à modifier selon ton robot

while (!huskylens.begin(Wire)) {
Serial.println("HuskyLens non détecté !");
delay(100);
}
huskylens.writeAlgorithm(ALGORITHM_LINE_TRACKING);
Serial.println("Mode : suivi de ligne avec recherche");
}
void loop() {
bool ligneDetectee = huskylens.request() && huskylens.countArrows() > 0;
if (ligneDetectee) {
HUSKYLENSResult arrow = huskylens.getArrow(0);
int xTarget = arrow.xTarget;
int erreur = xTarget - 160;
dernierContact = millis();
derniereDirection = 0;
int correction = map(abs(erreur), 0, 160, 0, maxCorrection);
correction = constrain(correction, 0, maxCorrection);
int vitesse = map(abs(erreur), 0, 160, vitesseBase, vitesseMin);
vitesse = constrain(vitesse, vitesseMin, vitesseBase);

// Debug affichage
Serial.print("xTarget: ");
Serial.print(xTarget);
Serial.print(" | erreur: ");
Serial.print(erreur);
Serial.print(" | correction: ");
Serial.print(correction);
Serial.print(" | vitesse: ");
Serial.println(vitesse);
if (erreur < -15) {
derniereDirection = -1;
car.Speed(-(vitesse + correction), -(vitesse - correction));
}
else if (erreur > 15) {
derniereDirection = 1;
car.Speed(-(vitesse - correction), -(vitesse + correction));
}
else {
car.Speed(-vitesse, -vitesse);
}
}
else {
unsigned long tempsDepuisPerte = millis() - dernierContact;
if (tempsDepuisPerte < delaiPerte) {
car.Speed(-vitesseMin, -vitesseMin); // continuer doucement
}
else {
Serial.println("Recherche active de ligne...");
if (derniereDirection == -1) {
car.Speed(-100, 100); // tourner sur place vers la gauche
}
else if (derniereDirection == 1) {
car.Speed(100, -100); // tourner vers la droite
}
else {
car.Speed(100, -100); // par défaut
}
}
}
}
