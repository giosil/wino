/*
 Wino - Arduino sample.
*/

#include <Time.h>

// TinyGPS.h e' l'header della libreria che consente la lettura delle coordinate GPS
// La libreria SoftwareSerial consente la comunicazione seriale con il ricevitore.
// La lettura avviene tramite porta seriale da inizializzare nella funzione setup.
#include <SoftwareSerial.h>
#include <TinyGPS.h>

// Pin utilizzati dai sensori di distanza
#define TRIG_DIST 1
#define ECHO_DIST 2

// Porta analogica utilizzata dall'altimetro
#define ALTIMETER_PORT A1

TinyGPS gps; // TinyGPS object 
SoftwareSerial gpsSerial(3, 4); // rx, tx

// Pin utilizzato dal pulsante digitale
#define DIGITAL_BTN 5

float lat = 0.0; // Latitudine
float lng = 0.0; // Longitudine

long d = 0; // Distanza in cm misurata dal sensore
long a = 0; // Altezza rilevata dall'altimetro analogico

int state = 0; // 0 = ready, 1 = view distance, 2 = altitude, 3 = gps

time_t lastExec = 0;

void setup() {
  // Inizializzazione per il controllo del sensore di distanza
  pinMode(TRIG_DIST, OUTPUT);
  pinMode(ECHO_DIST, INPUT);
  
  // Inizializzazione interrupt pulsante digitale
  attachInterrupt(digitalPinToInterrupt(DIGITAL_BTN), onButtonRising, RISING);

  // Inizializzazione ricevitore GPS (SoftwareSerial)
  gpsSerial.begin(9600);
  
  // Inizializzazione porta seriale per la visualizzazione
  Serial.begin(9600);
}

void loop() {
  // Returns the current time as seconds since Jan 1 1970
  
  switch(state) {
  case 0:
    lastExec = now();
    
    int h = hour(lastExec);
    int m = minute(lastExec);
    int s = second(lastExec);
    
    String sh = String(h);    
    String sm = String(m);
    String ss = String(s);
    
    Serial.println("READY at " + sh + ":" + sm + ":" + ss);
      
    break;
  case 1:
    readDistance();
      
    String sdis = String(d);
      
    Serial.println("Distance: " + sdis); 
      
    break;
  case 2:
    readAltitude();
      
    String salt = String(a);
    
    Serial.println("Altitude: " + salt); 
        
    break;
  case 3:
    readGPS();
      
    String slat = String(lat, 6);
    String slng = String(lng, 6);
      
    Serial.println("GPS: " + slat + "," + slng); 
      
    break;
  }
    
  delay(1000);
}

void readDistance() {
  // Trigger per l'invio dell'onda
  digitalWrite(TRIG_DIST, LOW);
  digitalWrite(TRIG_DIST, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_DIST, LOW);
  // Echo per il calcolo del ritorno
  long elasped = pulseIn(ECHO_DIST, HIGH);
  // Calcola la distanza in centimetri dall'oggetto
  d = elasped / 58.2;
}

void readAltitude() {
  // Lettura porta analogica a cui e' collegato l'altimetro.
  int ar = analogRead(ALTIMETER_PORT);
  // Calcola l'altezza in metri dal suolo dal valore di ar.
  // analogRead restituisce un intero da 0 a 1023.
  // Poiche' la portata dell'altimetro e' 0-80 m si
  // applica la seguente formula per ottenere l'altezza.
  a = (ar * 80) / 1023;
}

void readGPS() {
  // Controllo disponibilita' dei dati GPS
  if (gpsSerial.available()) {
    // Codifica dati GPS
    if (gps.encode(gpsSerial.read())) {
      // Si leggono le coordinate GPS
      gps.f_get_position(&lat, &lng);
    }
  }
}

void onButtonRising() {
  state++;
  if(state > 3) state = 0;
}
