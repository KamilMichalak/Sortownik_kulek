#define s0 8
#define s1 9
#define s2 10
#define s3 11
#define out 12

#include <Servo.h>

Servo kierunek;
Servo klapa;
Servo hak;

// Definicje pinów dla LED i przycisku
#define LED_CZERWONY 4
#define LED_ZIELONY 3
#define LED_NIEBIESKI 13
#define PRZYCISK_PAUZA 2

int data = 0;
float redMult = 1.0;
float greenMult = 1.0;
float blueMult = 1.0;

// Zmienna do obsługi pauzy
volatile bool pauza = false;
// Zmienna do przechowywania ostatniego koloru
int ostatniKolor = 0; // 1-czerwony, 2-zielony, 3-niebieski

void setup() {
  kierunek.attach(5);
  klapa.attach(6);
  hak.attach(7);

  // Konfiguracja pinów LED
  pinMode(LED_CZERWONY, OUTPUT);
  pinMode(LED_ZIELONY, OUTPUT);
  pinMode(LED_NIEBIESKI, OUTPUT);
  
  // Konfiguracja pinu przycisku
  pinMode(PRZYCISK_PAUZA, INPUT_PULLUP);
  
  // Dodanie przerwania dla przycisku
  attachInterrupt(digitalPinToInterrupt(PRZYCISK_PAUZA), togglePauza, FALLING);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);

  Serial.begin(9600);

  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);

  //ustawienie serw w pozycji neutralnej
  kierunek.write(90);
  klapa.write(20);
  hak.write(20);

  delay(500);
  calibrateWhite();
  delay(1000);
  
  // Wyłącz wszystkie LED na start
  wyczyscLED();
}

void loop() {
  // Sprawdź czy program jest zapauzowany
  if (pauza) {
    // Miganie wszystkimi LED w pauzie
    migajWszystkimiLED();
    return; // Wyjdź z loop() - zatrzymaj dalsze wykonywanie
  }
  
  hakuj();

  long R_sum = 0, G_sum = 0, B_sum = 0;
  //pobranie 100 pomiarów każdego koloru
  for (int i = 0; i < 100; i++) {
    R_sum += readColor('R');
    G_sum += readColor('G');
    B_sum += readColor('B');
    delay(10); // krótki odstęp między pomiarami
  }
  //uśrednienie pomiarów
  int R = R_sum / 100;
  int B = B_sum / 100;
  int G = G_sum / 100;

  Serial.print("Średnia R: ");
  Serial.print(R);
  Serial.print("  G: ");
  Serial.print(G);
  Serial.print("  B: ");
  Serial.println(B);

  // Wyłącz wszystkie LED przed zapaleniem nowego koloru
  wyczyscLED();
  delay(10);

  //zapalenie odpowieniego led-a i ustawienie kierunku w zależności od wykrytego koloru
  if (R < G && R < B) {
    kierunek.write(50);
    delay(10);
    zapalLED(LED_CZERWONY);
    ostatniKolor = 1;
    Serial.print("Czerwony! \n\n");
  } else if (B < G && B < R) {
    kierunek.write(130);
    delay(10);
    zapalLED(LED_NIEBIESKI);
    ostatniKolor = 3;
    Serial.print("Niebieski! \n\n");
  } else {
    kierunek.write(94);
    delay(10);
    zapalLED(LED_ZIELONY);
    ostatniKolor = 2;
    Serial.print("Zielony! \n\n");
  }

  klapuj();
}

int GetData() {
  data = pulseIn(out, LOW, 25000);
  if (data == 0) data = 25000;
  return data;
}

int readColor(char color) {
  switch (color) {
    case 'R':
      digitalWrite(s2, LOW);
      digitalWrite(s3, LOW);
      return GetData() * redMult;
    case 'G':
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      return GetData() * greenMult;
    case 'B':
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      return GetData() * blueMult;
    default:
      return 0;
  }
}
//kalibracja
void calibrateWhite() {
  Serial.println("Kalibracja - ustaw bialy kolor");
  delay(500);

  long R_sum = 0, G_sum = 0, B_sum = 0;

  for (int i = 0; i < 50; i++) {    //pobranie 50 pomiarów białego koloru
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);
    R_sum += GetData();

    digitalWrite(s2, HIGH);
    digitalWrite(s3, HIGH);
    G_sum += GetData();

    digitalWrite(s2, LOW);
    digitalWrite(s3, HIGH);
    B_sum += GetData();

    delay(10);
  }
  //obliczenie średniej z pomiarów
  float R_white = R_sum / 50.0;
  float G_white = G_sum / 50.0;
  float B_white = B_sum / 50.0;
  //wyznaczenie mnożników
  redMult   = G_white / R_white * 1.15;
  greenMult = 1.0;
  blueMult  = G_white / B_white * 0.93;

  Serial.println("Kalibracja zakonczona:");
  Serial.print("redMult = "); Serial.println(redMult);
  Serial.print("greenMult = "); Serial.println(greenMult);
  Serial.print("blueMult = "); Serial.println(blueMult);
}

//otwarcie i zamknięcie haka
void hakuj() {
  hak.write(90);
  delay(1100);
  hak.write(20);
  delay(500);
}

//otwarcie i zamknięcie klapy
void klapuj() {
  delay(1000);
  klapa.write(90);
  delay(1000);
  klapa.write(20);
  delay(2000);
}

// Funkcja do przełączania pauzy - wywoływana przez przerwanie
void togglePauza() {
  static unsigned long ostatnieKlikniecie = 0;
  unsigned long aktualnyCzas = millis();
  
  // Zabezpieczenie przed drganiem styków
  if (aktualnyCzas - ostatnieKlikniecie > 300) {
    pauza = !pauza;
    Serial.print("Pauza: ");
    Serial.println(pauza ? "WŁĄCZONA" : "WYŁĄCZONA");
    
    if (!pauza) {
      // Po wznowieniu zapal LED ostatniego koloru
      wyczyscLED();
      switch(ostatniKolor) {
        case 1: zapalLED(LED_CZERWONY); break;
        case 2: zapalLED(LED_ZIELONY); break;
        case 3: zapalLED(LED_NIEBIESKI); break;
      }
    }
  }
  ostatnieKlikniecie = aktualnyCzas;
}

// Funkcje do obsługi LED
void zapalLED(int pinLED) {
  digitalWrite(pinLED, HIGH);
}

void wyczyscLED() {
  digitalWrite(LED_CZERWONY, LOW);
  digitalWrite(LED_ZIELONY, LOW);
  digitalWrite(LED_NIEBIESKI, LOW);
}

void migajWszystkimiLED() {
  static unsigned long ostatniaZmiana = 0;
  static bool stan = false;
  
  if (millis() - ostatniaZmiana > 500) {
    stan = !stan;
    digitalWrite(LED_CZERWONY, stan);
    digitalWrite(LED_ZIELONY, stan);
    digitalWrite(LED_NIEBIESKI, stan);
    ostatniaZmiana = millis();
  }
}
