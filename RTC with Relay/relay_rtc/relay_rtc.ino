#include <DS3231.h>

int Relay = 4;

DS3231  rtc(A4, A5);
Time t;

const int OnHour = 12;
const int OnMin = 24;
const int OffHour = 12;
const int OffMin = 25;

void setup() {
  Serial.begin(115200);
  rtc.begin();
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay, LOW);
}

void loop() {
  t = rtc.getTime();
  Serial.print(t.hour);
  Serial.print(" hour(s), ");
  Serial.print(t.min);
  Serial.print(" minute(s)");
  Serial.println(" ");
  delay (1000);
  
  if(t.hour == OnHour && t.min == OnMin){
    digitalWrite(Relay,HIGH);
    Serial.println("LIGHT ON");
    }
    
    else if(t.hour == OffHour && t.min == OffMin){
      digitalWrite(Relay,LOW);
      Serial.println("LIGHT OFF");
    }
}
