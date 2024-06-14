#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FS.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define ccw 32
#define cw 35
#define swr 34
#define sw1 26
#define sw2 27
#define neoPixelPin 14
#define BUZZER_PIN 12
#define LED1 18
#define LED2 19
//Sleep
#define BUTTON_PIN_BITMASK 0x20080


// Funktionen

// Rotary Encoder lesen, Menüposition bekommen
int rotary(int state, int laststate); 
//Durch Menü scrollen
void menuscroll(int counter, String menu[], int menuSize); 
// Base Timer Function
void timer(int t); 
// Copy von ausgewählten Menü erstellen
void menucopy(String source[], String dest[], int size); 
// Auswahl in Submenüs
void submenu(String choice);
// Neopixel aus 
void allOff(); 
// Pixel nach Timer
void Pixels(int pixels,int pixels2, int timeZahl);
// LED Streifen Farbe auswählen 
void LEDcolor(); 
//Töne
void playTone(float frequency, int duration);
void goodTone();
void badTone();
void tonesetting();
//LED Augen
void eyes();
//Helligkeit
void brightness();
//Header
void header();
//Sbtimesettings
void sbtimeset();

// Variablen

// Basetimer zeiten
int timers[] = {2, 5, 10, 15, 20, 25, 30, 35, 40, 45}; 
// SUbmenü für Basetimer
String timermenu[] = {"Time 2s", "Time 5s", "Time 10s", "Time 15s", "Time 20s", "Time 25s", "Time 30s", "Time 35s", "Time 40s", "Zurueck"}; 
//Hauptmenü
String mainmenu[] = {"Timer", "Eigener Timer", "2 Timer", "Gerichte", "Einstellungen", "Aus"}; 
//Gerichtetimer Submenü
String Gerichtemenu[] = {"weiches Ei", "mittleres Ei", "hartes Ei", "Spaghetti", "Penne", "Zurueck"}; 
// Gerichte Ziten
int Gerichtetimer[] = {5,7,10,8,9}; 
// Einstellungen Submenü
String Settingsmenu[] = {"Ton", "Helligkeit", "StandbyTime","LED Farbe","Zurueck"}; 

// State for Rotary
bool laststate; 
// Variablen für Rotary, Timer
int counter = 0, sec = 0, t = 0; 
// Variablen für Millis funktion
long currentmillis = 0, lastmillis = 0; 
//String für Menüauswahl
String currentMenu[99]; 
//int für Menüauswahl
int currentMenuSize = 0; 
//Switch int für Menüauswahl
int menunumber = 99; 
//LED Streifen Pixelzahl
float numPixels = 8; 
//Farben LED Streifen
int green, blue = 0; 
int red = 255;
int pixelcol = 0;
float pixelpersec = 0;
int back = 0;
int colorchange = 0;
int color = 0;
//Brightness
bool dim = 0;
char dimchar = 'Nein';
//Time
int hh = 11;
int mm = 11;
int ss = 11;
//afterheader Y
int y = 11;
//Ton
bool tone = 1;
//Standbytime
int sbtime = 60000;
int sbmillis;

//Initialisiere Adafruit

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

//Base Setup /Mainmenu

void setup() {
  Serial.begin(115200);
  //pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(cw, INPUT);
  pinMode(ccw, INPUT);
  pinMode(swr, INPUT);
  pinMode(sw1, INPUT);
  pinMode(neoPixelPin, OUTPUT);
  //LEDStrip
  strip.begin();  // initialize the strip
  strip.show();   // make sure it is visible
  strip.clear();

  for(int i; i>numPixels; i++){
    strip.setPixelColor(i, 255,0,0);// Pixeltest
  }
  strip.show();
  //Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresse 0x3C für 128x64
    Serial.println(F("SSD1306 allocation failed"));// Display finden / Fehler
    for (;;);
  }
  //initialize display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  header();

  //Sleepmode
  sbmillis = millis();
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  // Hauptmenü initialisieren
  currentMenuSize = sizeof(mainmenu) / sizeof(mainmenu[0]);
  menucopy(mainmenu, currentMenu, currentMenuSize);

  for (int i = 0; i < currentMenuSize; i++) {
    if (i == counter) {
      display.print("-> ");// Auswahlpfeil auf Baseposition
    } else {
      display.print("   ");
    }
    display.println(mainmenu[i]);
  }
  display.display();

  laststate = digitalRead(ccw);// Erster Status für Rotary
}

void loop() {
  back = 0;
  if(millis()- sbmillis <= sbtime){
    esp_deep_sleep_start();
  }
  bool state = digitalRead(ccw);// Rotary neuauslesen
  counter = rotary(state, laststate);

  if (state != laststate) {
    menuscroll(counter, currentMenu, currentMenuSize);
  }
  laststate = state;

  //Menüauswahl

  if (digitalRead(swr) == LOW) {
    String choice = currentMenu[counter];
    if (choice == "Timer") {
      currentMenuSize = sizeof(timermenu) / sizeof(timermenu[0]);
      menucopy(timermenu, currentMenu, currentMenuSize);
    }else if (choice == "Zurueck"){ //goto mainmenu
      currentMenuSize = sizeof(mainmenu) / sizeof(mainmenu[0]);
      badTone();
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
      header();
      display.println("System aus...");
      display.display();
      delay(2000);
      esp_deep_sleep_start();
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
    sbmillis = millis();
    if (digitalRead(cw) != state) {
      counter = (counter + 1) % currentMenuSize;
    } else {
      counter = (counter - 1 + currentMenuSize) % currentMenuSize;
    }
  }
  return counter;
}

void menuscroll(int counter, String menu[], int menuSize) {
  header();
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
  int pixelTime = 0;
  Serial.printf("%d timer", t);
  sec = 0;
  header();
  display.printf("Starting Timer %ds", t);
  display.display();
  delay(1000);
  lastmillis = millis();
  goodTone();
  while (sec < t) {
    if (digitalRead(swr) == LOW) {
      break;
    }
    currentmillis = millis();
    if (currentmillis - lastmillis >= 1000) {
      Serial.printf("%d numPixels, %d t, %d sec \n", numPixels, t, sec);
      Serial.println(numPixels/t);
      pixelpersec = (numPixels/t);
      Serial.println(pixelpersec);
      pixelTime = int(pixelpersec*sec);
      Serial.printf("%dPixeltime\n", pixelTime);
      Pixels(pixelTime, 0, 1);
      lastmillis = currentmillis;
      sec++;
      header();
      display.printf("Timer %ds", t);
      display.setCursor(0, y+10);
      int min = (t - sec)/60;
      display.printf("%d:%d",min , (t-sec)-min*60);
      display.display();
    }
  }
  // Timer completed
  header();
  display.println("Timer Completed");
  display.println("OK");
  display.display();
  eyes();
  goodTone();
  delay(2000);
  allOff();
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
    }else if(choice == "LED Farbe"){
      LEDcolor();
    }
  default:
    break;
  }
  
}

void allOff() {
  strip.clear();
  strip.show();
}

void Pixels(int pixels, int pixels2, int TimerZahl){
  if(TimerZahl == 2){
    for(int i; i>(pixels/2); i++){
      strip.setPixelColor(i, red ,green ,blue);
    }
    for(int i; i>(pixels2/2); i++){
      strip.setPixelColor(numPixels-i, 255-red ,255-green ,255-blue);
    }
  }else{
    //Serial.println("in function");
    //Serial.println(pixels);
    for(int i; i>pixels; i++){
      strip.setPixelColor(i, red ,green ,blue);
    }
  }
  //Serial.print("LED an");
  strip.show();
}

void LEDcolor(){
  delay(500);
  while(back==0){
    switch (colorchange)
    {
    case 0:
      header();
      display.printf("-> Rot:%d\n", red);
      display.printf("   Blau:%d\n", blue);
      display.printf("   Gruen:%d\n", green);
      display.display();
      red = counter;
      break;
    case 1:
      header();
      display.printf("   Rot:%d\n", red);
      display.printf("-> Blau:%d\n", blue);
      display.printf("   Gruen:%d\n", green);
      display.display();
      blue = counter;
      break;
    case 2:
      header();
      display.printf("   Rot:%d\n", red);
      display.printf("   Blau:%d\n", blue);
      display.printf("-> Gruen:%d\n", green);
      display.display();
      green = counter;
      break;
    default:
      break;
    }
  
  bool state = digitalRead(ccw);
  currentMenuSize = 255;
  counter = rotary(state, laststate);
  if (state != laststate) {
    laststate = state;
    Pixels(numPixels, 0, 0);
  }
  if(digitalRead(swr) == LOW){
    colorchange = (colorchange+1)%3;
    switch(colorchange){
      case 0:
      counter = red;
      case 1:
      counter = blue;
      case 2:
      counter = green;
    }
    delay(500);
  }

  }
}

void playTone(float frequency, int duration) {
  int period = 1000000 / frequency;
  int halfPeriod = period / 2;
  unsigned long start = millis();
  while (millis() - start < duration) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(halfPeriod);
  }
}

void goodTone(){
  if(tone == 1){
    playTone(523.25, 150); // C für 150ms
    delay(100); // 100ms Pause
    playTone(659.25, 300); // E für 150ms
  }
}

void badTone(){
  if(tone == 1){
    playTone(392.00, 150); // G für 150ms
    delay(100); // 100ms Pause
    playTone(349.23, 300); // F für 150ms
  }
  
}

void eyes(){
  if(digitalRead(sw1) == 0){
    digitalWrite(LED2, HIGH);
    digitalWrite(LED2, HIGH);
    delay(50);
    digitalWrite(LED2, LOW);
    digitalWrite(LED1, LOW);
    delay(50);
  }
}

void tonesetting(){
  header();
}

void header(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("%d:%d:%d", hh, mm, ss);
  display.setCursor(80,0);
  display.print("Duoringo");
  display.setCursor(0,9);
  display.drawLine(0, 9, SCREEN_WIDTH-1, 9,SSD1306_INVERSE);
  display.setCursor(0,y);
}

void brightness(){
  header();
  display.printf("Dimmen: %c", dimchar);
  if(back == 0){
    if(digitalRead(swr)==LOW){
      dim = (dim+1)%2;
      delay(100);
      if(dim == 1){
        display.dim(true);
        dimchar = 'Ja';
        header();
        display.printf("Dimmen: %c", dimchar);
      }else{
        display.dim(false);
        dimchar = 'Nein';
        header();
        display.printf("Dimmen: %c", dimchar);
      }
    }
    if(digitalRead(sw1) == 1){
      back = 1;
    }
  }else{
    back = 0;
  }
}

void sbtimeset(){
  header();
  display.println("StandbyTime");
  display.printf("%d min", sbtime/60000);
  currentMenuSize = 10;
  bool state = digitalRead(ccw);
  counter = rotary(state, laststate);
  if (back == 0)
  {
    if (state != laststate) {
      laststate = state;
      sbtime = counter*60000;
      header();
      display.println("StandbyTime");
      display.printf("%d min", sbtime/60000);
    }
    if(digitalRead(sw1) == 1){
      back = 1;
    }
  }
  
  
}