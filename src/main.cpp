#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define ccw 32
#define cw 35
#define swr 34
#define sw1 26

Adafruit_SSD1306 display=(128, 64, &Wire, -1);

//functions
int rotary(int state, int laststate);//read rotary, get menu position
void menuscroll(int counter, String menu);
void timer(int t);
String menucopy(String menu);

//var
int timers[] = {2,5,10,15,20,25,30,35,40,45};
String timermenu[] = {"Time 2s", "Time 5s", "Time 10s", "Time 15s", "Time 20s", "Time 25s", "Time 30s", "Time 35s", "Time 40s", "Time 45s"};
String mainmenu[] = {"Timer", "Eigener Timer", "2 Timer", "Gerichte", "Einstellungen", "Aus"};
String Gerichtemenu[] = {"weiches Ei", "mittleres Ei", "hartes Ei", "Spaghetti", "Penne"};
String Settingsmenu[] = {"Lautstärke", "Helligkeit", "StandbyTime"};
bool laststate = digitalRead(ccw);
int counter, position, sec, t, menuSize = 0;
long currentmillis, lastmillis;
String menu[], choice[];


void setup(){
  Serial.begin(115200);
  pinMode(cw, INPUT);
  pinMode(ccw, INPUT);
  pinMode(swr, INPUT);
  pinMode(sw1, INPUT);
  display.begin();
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  for(int i=0; i<menuSize; i++){
    if(i==1){
      display.print("->");
    }
    display.println(mainmenu[i]);
  }
  display.display();
  String menu = menucopy(mainmenu[6]);
  int menuSize = sizeof(mainmenu) / sizeof(mainmenu[0]);
}

void loop(){
  if(digitalRead(sw1)==HIGH){
    for(int i=0; i<menuSize; i++){
      Serial.print("Zahl: ");
      Serial.println(menu[i]);
    }
  }
  bool state = digitalRead(ccw);
  counter = rotary(state, laststate);
  if(state != laststate){
    menuscroll(counter, menu[menuSize]);
  }
  laststate = state;
  if(digitalRead(swr)==LOW){
    String choice = menu[(counter+1)%menuSize];
    int menuSize = sizeof(choice)/ sizeof(choice[0]);
    String menu = menucopy(choice[menuSize]);
  }
}

int rotary(int state, int laststate){//read rotary, get menu position
  if(state != laststate){
    if(digitalRead(cw) != state){
      counter = (counter + 1)%menuSize;
      //Serial.print("cw:");
      //Serial.println(counter);
    }else{
      counter = (counter - 1 + menuSize)%menuSize;
      //Serial.print("ccw:");
      //Serial.println(counter);
    }
  }
  return counter;
}

void menuscroll(int counter, String menu){
  int menuSize = sizeof(menu) / sizeof(menu[0]);
  display.clearDisplay();
  display.setCursor(0,0);
  for(int i=0; i<menuSize; i++){
    if(i==1){
      display.print("->");
    }
    
    display.println(menu[(counter+i)%menuSize]);
  }
  display.display();
  Serial.print("ms:");
  Serial.println(counter);
}

void timer(int t){
  Serial.printf("%d timer", t);
  sec = 0;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Starting Timer %ds", t);
  display.display();
  delay(1000);
  lastmillis = millis();
  while (sec < t) {
    if(digitalRead(sw1) == HIGH){
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

String menucopy(String menu){
  int menuSize = sizeof(menu) / sizeof(menu[0]);
  String currentmenu[menuSize];
  for(int i=0; i>=menuSize; i++){
    currentmenu[i] = menu[i];
  }
  return currentmenu[menuSize];
}
