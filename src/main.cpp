#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FS.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define ccw 32
#define cw 35
#define swr 34
#define sw1 26

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Funktionen
int rotary(int state, int laststate); // Rotary Encoder lesen, Menüposition bekommen
void menuscroll(int counter, String menu[], int menuSize);
void timer(int t);
void menucopy(String source[], String dest[], int size);
void submenu(String choice);

// Variablen
int timers[] = {2, 5, 10, 15, 20, 25, 30, 35, 40, 45};
String timermenu[] = {"Time 2s", "Time 5s", "Time 10s", "Time 15s", "Time 20s", "Time 25s", "Time 30s", "Time 35s", "Time 40s", "Zurueck"};
String mainmenu[] = {"Timer", "Eigener Timer", "2 Timer", "Gerichte", "Einstellungen", "Aus"};
String Gerichtemenu[] = {"weiches Ei", "mittleres Ei", "hartes Ei", "Spaghetti", "Penne", "Zurueck"};
int Gerichtetimer[] = {5,7,10,8,9};
String Settingsmenu[] = {"Lautstärke", "Helligkeit", "StandbyTime","Zurueck"};

bool laststate;
int counter = 0, position = 0, sec = 0, t = 0;
long currentmillis = 0, lastmillis = 0;
String currentMenu[10];
int currentMenuSize = 0;
int menunumber = 99;

void setup() {
  Serial.begin(115200);
  pinMode(cw, INPUT);
  pinMode(ccw, INPUT);
  pinMode(swr, INPUT);
  pinMode(sw1, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresse 0x3C für 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  // Hauptmenü initialisieren
  currentMenuSize = sizeof(mainmenu) / sizeof(mainmenu[0]);
  menucopy(mainmenu, currentMenu, currentMenuSize);

  for (int i = 0; i < currentMenuSize; i++) {
    if (i == counter) {
      display.print("-> ");
    } else {
      display.print("   ");
    }
    display.println(mainmenu[i]);
  }
  display.display();

  laststate = digitalRead(ccw);
}

void loop() {
  bool state = digitalRead(ccw);
  counter = rotary(state, laststate);

  if (state != laststate) {
    menuscroll(counter, currentMenu, currentMenuSize);
  }
  laststate = state;

  if (digitalRead(swr) == LOW) {
    String choice = currentMenu[counter];
    if (choice == "Timer") {
      currentMenuSize = sizeof(timermenu) / sizeof(timermenu[0]);
      menucopy(timermenu, currentMenu, currentMenuSize);
    }else if (choice == "Zurueck"){
      currentMenuSize = sizeof(mainmenu) / sizeof(mainmenu[0]);
      menucopy(mainmenu, currentMenu, currentMenuSize);
    } else if (choice == "Eigener Timer"){
      //Eigene Timer Funtkion
    } else if (choice == "2 Timer"){
      //Zwei Timer Funktion
    }else if (choice == "Gerichte"){
      currentMenuSize = sizeof(Gerichtemenu) / sizeof(Gerichtemenu[0]);
      menucopy(Gerichtemenu, currentMenu, currentMenuSize);
      menunumber = 1;
    } else if (choice == "Einstellungen") {
      currentMenuSize = sizeof(Settingsmenu) / sizeof(Settingsmenu[0]);
      menucopy(Settingsmenu, currentMenu, currentMenuSize);
      menunumber = 2;
    } else if (choice == "Aus") {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("System aus...");
      display.display();
      delay(2000);
      // Hier können Sie eine Funktion zum Ausschalten oder in den Schlafmodus wechseln hinzufügen
    } else if (choice.startsWith("Time")) {
      int t = timers[counter];
      timer(t);
      currentMenuSize = sizeof(timermenu) / sizeof(timermenu[0]);
      menucopy(timermenu, currentMenu, currentMenuSize);
    } else{
      submenu(choice);
    }
    counter = 0; // Reset counter to start at the beginning of new menu
    menuscroll(counter, currentMenu, currentMenuSize);
    delay(500);
  }
}

int rotary(int state, int laststate) { // Rotary Encoder lesen, Menüposition bekommen
  if (state != laststate) {
    if (digitalRead(cw) != state) {
      counter = (counter + 1) % currentMenuSize;
    } else {
      counter = (counter - 1 + currentMenuSize) % currentMenuSize;
    }
  }
  return counter;
}

void menuscroll(int counter, String menu[], int menuSize) {
  display.clearDisplay();
  display.setCursor(0, 0);
  if (counter > 7){
    for (int i = 7; i < menuSize; i++) {
    if (i == counter) {
      display.print("-> ");
    } else {
      display.print("   ");
    }
    display.println(menu[i]);
  }
  display.display();
  }else{
    for (int i= 0; i < menuSize; i++) {
    if (i == counter) {
      display.print("-> ");
    } else {
      display.print("   ");
    }
    display.println(menu[i]);
  }
  display.display();
  }
  display.display();
  Serial.print("ms:");
  Serial.println(counter);
}

void timer(int t) {
  Serial.printf("%d timer", t);
  sec = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Starting Timer %ds", t);
  display.display();
  delay(1000);
  lastmillis = millis();
  while (sec < t) {
    if (digitalRead(swr) == LOW) {
      break;
    }
    currentmillis = millis();
    if (currentmillis - lastmillis >= 1000) {
      lastmillis = currentmillis;
      sec++;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.printf("Timer %ds", t);
      display.setCursor(0, 10);
      display.printf("%ds", t - sec);
      display.display();
    }
  }
  // Timer completed
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Timer Completed");
  display.display();
  delay(2000);
}

void menucopy(String source[], String dest[], int size) {
  for (int i = 0; i < size; i++) {
    dest[i] = source[i];
  }
}

void submenu(String choice){
  switch (menunumber)
  {
  case 1:
    if (choice == "weiches Ei") {
      timer(Gerichtetimer[0]);
    } else if (choice == "mittleres Ei") {
      timer(Gerichtetimer[1]);
    } else if (choice == "hartes Ei") {
      timer(Gerichtetimer[2]);
    } else if (choice == "Spaghetti"){
      timer(Gerichtetimer[3]);
    } else if (choice == "Penne"){
      timer(Gerichtetimer[4]);
    } 
    break;
  case 2:
    if (choice == "Lautstärke"){
      Serial.print("Lautstärke");
      //Funktion für Lautstärke
    } else if (choice == "Helligkeit"){
      Serial.print("Helligkeit");
      //Funktion für Helligkeit
    }else if (choice == "Standby Time"){
      Serial.print("Standby Time");
      //Funktion für Standby Time
    }
  default:
    break;
  }
  
}