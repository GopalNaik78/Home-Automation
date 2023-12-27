#include <WiFi.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>
#include <EEPROM.h>
#include "RTClib.h"
#include "BluetoothSerial.h"
#define EEPROM_SIZE 30
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run
`make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
RTC_DS1307 rtc;

// Firebase Credentials
#define FIREBASE_HOST "test3-35ff2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "M2hI2GQrEqntM5G7CNG3OC4gEZItUUaYWRTPRhRS"

// WiFi Credentials
#define WIFI_SSID "dontTry"
#define WIFI_PASSWORD "12341234"

// Function Declaration
void Data_from_firebase();
void  with_BT_Wifi();
void  without_BT_Wifi();
void Data_from_BT();
void Data_backup();
void speed0();
void speed1();
void speed2();
void speed3();
void speed4();
void speedM0();
void speedM1();
void speedM2();
void speedM3();
void speedM4();
void speedS0();
void speedS1();
void speedS2();
void speedS3();
void speedS4();
void L1_ON();
void L1_OFF();
void L2_ON();
void L2_OFF();
void L3_ON();
void L3_OFF();
void L4_ON();
void L4_OFF();
void On_fun();
void Off_fun();
void FanOn();
void FanOff();

// Pins of Fan Regulator Knob
#define s1  27
#define s2  13
#define s3  14
#define s4  26

// Pins of Switches
#define S5 39
#define S6 34
#define S7 35
#define S8 32
#define S9 33

// Pins of Relay (Appliances Control)
#define R5 4
#define R6 16
#define R7 2
#define R8 15

// Pins of Relay (Fan Speed Control)
#define Speed1 18
#define Speed2 19
#define Speed4 23

//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;

// Necessary Variables
bool BT_S = 0;
bool FoS = 0;

bool speed1_flag = 1;
bool speed2_flag = 1;
bool speed3_flag = 1;
bool speed4_flag = 1;
bool speed0_flag = 1;
bool speed1_BT_Fire_flag = 1;
bool speed2_BT_Fire_flag = 1;
bool speed3_BT_Fire_flag = 1;
bool speed4_BT_Fire_flag = 1;
bool speed0_BT_Fire_flag = 1;
bool BT_Mode = 0;
bool bto = 0;
bool wfo = 0;


int switch_ON_Flag1_previous_I = 0;
int switch_ON_Flag2_previous_I = 0;
int switch_ON_Flag3_previous_I = 0;
int switch_ON_Flag4_previous_I = 0;
int switch_ON_Flag5_previous_I = 0;

String Speed_Value ;
//String previous_Speed;

int OnHour;
int OnMin;
int OffHour;
int OffMin;
int Auto_mode;
int start_flag;
int i, LF1, LF2, LF3, LF4, LF5, SPD;
int x = 0 , p = 0;

String L1_flag;
String L2_flag;
String L3_flag;
String L4_flag;
String AutoS;
String BT_Valve;
String msg;

void setup()
{
  // put your setup code here, to run once:
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(s3, INPUT_PULLUP);
  pinMode(s4, INPUT_PULLUP);
  pinMode(S5, INPUT_PULLUP);
  pinMode(S6, INPUT_PULLUP);
  pinMode(S7, INPUT_PULLUP);
  pinMode(S8, INPUT_PULLUP);
  pinMode(S9, INPUT_PULLUP);

  pinMode(R5, OUTPUT);
  pinMode(R6, OUTPUT);
  pinMode(R7, OUTPUT);
  pinMode(R8, OUTPUT);
  pinMode(Speed1, OUTPUT);
  pinMode(Speed2, OUTPUT);
  pinMode(Speed4, OUTPUT);

  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  //WiFi.mode(WIFI_AP_STA);
  btStart();

#ifndef ESP32
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  SerialBT.begin("ESP_AjjuCreation12");
  Serial.println("BT Device is started");
  Data_backup();
}

void loop()
{
  delay(500);
  DateTime now = rtc.now();
  Serial.println("I am alive Bro");
  Serial.println(WiFi.status());
  BT_S = ((BT_Mode == 1) && (SerialBT.connected()));
  if ((WiFi.status() == WL_CONNECTED) && (p != 1)) {
    p = 1;
  }
  if ((p == 1) && ((WiFi.status() == WL_DISCONNECTED) || (WiFi.status() == WL_CONNECTION_LOST))) {
    Serial.println("esp restart");
    delay(500);
    ESP.restart();
  }
  if (SerialBT.connected()) {
    Serial.println("bt connected");
    if (BT_S == 1) {
      if (SerialBT.available()) {
        BT_Valve = SerialBT.readString();
        Serial.println(BT_Valve);
        if (BT_Valve == "Z") {
          BT_Mode = 0;
          SerialBT.print(BT_Valve);
        }
        else
          Data_from_BT();
      }
      with_BT_Wifi();
    } else {
      if (SerialBT.available()) {
        BT_Valve = SerialBT.readString();
        Serial.println(BT_Valve);
        if (BT_Valve == "Y") {
          BT_Mode = 1;
          SerialBT.print(BT_Valve);
        }
      }
    }
  } else if (WiFi.status() != WL_CONNECTED)
  {
    without_BT_Wifi();
  }
  else
  {
    Data_from_firebase();
    with_BT_Wifi();
  }

  if (Auto_mode == 1) {
    if (now.hour() == OnHour && now.minute() == OnMin) {
      On_fun();
    } else if (now.hour() == OffHour && now.minute() == OffMin) {
      Off_fun();
    }
  }
  Serial.println(WiFi.status());
  //  x++;
  //  if (x >= 61)
  //    x = 0;

}

void On_fun() {
  if (L1_flag == "1")
    L1_ON();
  if (L2_flag == "1")
    L2_ON();
  if (L3_flag == "1")
    L3_ON();
  if (L4_flag == "1")
    L4_ON();
  AutoS = "1";
  json.set("/AF", AutoS);
  Firebase.updateNode(firebaseData, "/Appliances", json);
  if (BT_S) SerialBT.print("AMS");
}


void Off_fun() {
  if (L1_flag == "1")
    L1_OFF();
  if (L2_flag == "1")
    L2_OFF();
  if (L3_flag == "1")
    L3_OFF();
  if (L4_flag == "1")
    L4_OFF();
  AutoS = "0";
  json.set("/AF", AutoS);
  Firebase.updateNode(firebaseData, "/Appliances", json);
  if (BT_S) SerialBT.print("AMP");
}


void Data_from_BT() {
  if (BT_Valve == "A") {
    Auto_mode = 1;
    msg = "A";
    SerialBT.print(msg);
    if (1 != EEPROM.read(6)) {
      EEPROM.write(6, 1);
      EEPROM.commit();
    }
  }
  else if (BT_Valve == "a") {
    Auto_mode = 0;
    msg = "a";
    SerialBT.print(msg);
    if (0 != EEPROM.read(6)) {
      EEPROM.write(6, 0);
      EEPROM.commit();
    }
  }
  else  if (BT_Valve.startsWith("H1")) {
    String msg1 = BT_Valve.substring(2, 4);
    OnHour = (((msg1[1] - 48) * 10) + (msg1[2] - 48));
    SerialBT.print(BT_Valve);
    if (OnHour != EEPROM.read(11)) {
      EEPROM.write(11, OnHour);
      EEPROM.commit();
    }
  }
  else  if (BT_Valve.startsWith("M1")) {
    String msg2 = BT_Valve.substring(2, 4);
    OnMin = (((msg2[1] - 48) * 10) + (msg2[2] - 48));
    SerialBT.print(BT_Valve);
    if (OnMin != EEPROM.read(12)) {
      EEPROM.write(12, OnMin);
      EEPROM.commit();
    }
  }
  else  if (BT_Valve.startsWith("H2")) {
    String msg3 = BT_Valve.substring(2, 4);
    OffHour = (((msg3[1] - 48) * 10) + (msg3[2] - 48));
    SerialBT.print(BT_Valve);
    if (OffHour != EEPROM.read(13)) {
      EEPROM.write(13, OffHour);
      EEPROM.commit();
    }
  }
  else  if (BT_Valve.startsWith("M2")) {
    String msg4 = BT_Valve.substring(2, 4);
    OffMin = (((msg4[1] - 48) * 10) + (msg4[2] - 48));
    SerialBT.print(BT_Valve);
    if (OffMin != EEPROM.read(14)) {
      EEPROM.write(14, OffMin);
      EEPROM.commit();
    }
  }

  else if (BT_Valve == "L1F1") {
    L1_flag = "1";
    SerialBT.print("L1F1");
  }

  else if (BT_Valve == "L1F0") {
    L1_flag = "0";
    SerialBT.print("L1F0");
  }

  else if (BT_Valve == "L2F1") {
    L2_flag = "1";
    SerialBT.print("L2F1");
  }

  else if (BT_Valve == "L2F0") {
    L2_flag = "0";
    SerialBT.print("L2F0");
  }

  else if (BT_Valve == "L3F1") {
    L3_flag = "1";
    SerialBT.print("L3F1");
  }

  else if (BT_Valve == "L3F0") {
    L3_flag = "0";
    SerialBT.print("L3F0");
  }

  else if (BT_Valve == "L4F1") {
    L4_flag = "1";
    SerialBT.print("L4F1");
  }

  else if (BT_Valve == "L4F0") {
    L4_flag = "0";
    SerialBT.print("L4F0");
  }

  else if (BT_Valve == "SP0") {
    speedM0();
    SPD = 0;
  }
  else if (BT_Valve == "SP1") {
    speedM1();
    SPD = 1;
  }
  else if (BT_Valve == "SP2") {
    speedM2();
    SPD = 2;
  }
  else if (BT_Valve == "SP3") {
    speedM3();
    SPD = 3;
  }
  else if (BT_Valve == "SP4") {
    speedM4();
    SPD = 4;
  }
  else if (BT_Valve == "FO") {
    FanOn();
    LF5 = 1;
  }
  else if (BT_Valve == "FOF") {
    FanOff();
    LF5 = 0;
  }
  else if (BT_Valve == "L1") {
    L1_ON();
    LF1 = 1;
  }
  else if (BT_Valve == "FL1") {
    L1_OFF();
    LF1 = 0;
  }
  else if (BT_Valve == "L2") {
    L2_ON();
    LF2 = 1;
  }
  else if (BT_Valve == "FL2") {
    L2_OFF();
    LF2 = 0;
  }
  else if (BT_Valve == "L3") {
    L3_ON();
    LF3 = 1;
  }
  else if (BT_Valve == "FL3") {
    L3_OFF();
    LF3 = 0;
  }
  else if (BT_Valve == "L4") {
    L4_ON();
    LF4 = 1;
  }
  else if (BT_Valve == "FL4") {
    L4_OFF();
    LF4 = 0;
  }
}



void Data_from_firebase()
{
  Serial.println("data of firebase");
  if (Firebase.getString(firebaseData, "/Appliances/A1"))
  { Serial.println("data of firebase a1");

    if (firebaseData.stringData() == "1")
    {
      Serial.println("data of firebase d");

      Auto_mode = 1;
      String AutoM = "1";
      json.set("/A", AutoM);
      Firebase.updateNode(firebaseData, "/Appliances", json);
      if (1 != EEPROM.read(6)) {
      EEPROM.write(6, 1);
      EEPROM.commit();
    }
    }
    else if (firebaseData.stringData() == "0")
    {
      Serial.println("data of firebase 0");

      Auto_mode = 0;
      String AutoM = "0";
      json.set("/A", AutoM);
      Firebase.updateNode(firebaseData, "/Appliances", json);
      if (0 != EEPROM.read(6)) {
        EEPROM.write(6, 0);
        EEPROM.commit();
      }
      if (Firebase.getString(firebaseData, "/Appliances/T"))
      {
        if (firebaseData.stringData() == "1")
        {
          i++;
          if (Firebase.getString(firebaseData, "/Appliances/TL1")) {
            L1_flag = firebaseData.stringData();
          }
          if (Firebase.getString(firebaseData, "/Appliances/TL2")) {
            L2_flag = firebaseData.stringData();
          }
          if (Firebase.getString(firebaseData, "/Appliances/TL3")) {
            L3_flag = firebaseData.stringData();
          }
          if (Firebase.getString(firebaseData, "/Appliances/TL4")) {
            L4_flag = firebaseData.stringData();
          }
          if (Firebase.getString(firebaseData, "/Appliances/H3")) {
            String msg1 = firebaseData.stringData();
            OnHour = (((msg1[1] - 48) * 10) + (msg1[2] - 48));
            json.set("/H1", msg1);
            Firebase.updateNode(firebaseData, "/Appliances", json);
            if (OnHour != EEPROM.read(11)) {
              EEPROM.write(11, OnHour);
              EEPROM.commit();
            }
          }
          if (Firebase.getString(firebaseData, "/Appliances/M3")) {
            String msg2 = firebaseData.stringData();
            OnMin = (((msg2[1] - 48) * 10) + (msg2[2] - 48));
            json.set("/M1", msg2);
            Firebase.updateNode(firebaseData, "/Appliances", json);
            if (OnMin != EEPROM.read(12)) {
              EEPROM.write(12, OnMin);
              EEPROM.commit();
            }
          }
          if (Firebase.getString(firebaseData, "/Appliances/H4")) {
            String msg3 = firebaseData.stringData();
            OffHour = (((msg3[1] - 48) * 10) + (msg3[2] - 48));
            json.set("/H2", msg3);
            Firebase.updateNode(firebaseData, "/Appliances", json);
            if (OffHour != EEPROM.read(13)) {
              EEPROM.write(13, OffHour);
              EEPROM.commit();
            }
          }
          if (Firebase.getString(firebaseData, "/Appliances/M4")) {
            String msg4 = firebaseData.stringData();
            OffMin = (((msg4[1] - 48) * 10) + (msg4[2] - 48));
            json.set("/M2", msg4);
            Firebase.updateNode(firebaseData, "/Appliances", json);
            if (OffMin != EEPROM.read(14)) {
              EEPROM.write(14, OffMin);
              EEPROM.commit();
            }
          }
        }
        if (i >= 10) {
          i = 0;
          String Ts = "0";
          json.set("/T", Ts);
          Firebase.updateNode(firebaseData, "/Appliances", json);
        }
      }
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/O")) {
    Serial.println("data of firebase O");

    if (firebaseData.stringData() == "0")
    {
      String Online = "1";
      json.set("/O", Online);
      Firebase.updateNode(firebaseData, "/Appliances", json);
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/fan"))
  {
    if (firebaseData.stringData() == "0")
      speedM0();
    else if (firebaseData.stringData() == "1")
      speedM1();
    else if (firebaseData.stringData() == "2")
      speedM2();
    else if (firebaseData.stringData() == "3")
      speedM3();
    else if (firebaseData.stringData() == "4")
      speedM4();
    else
    {
      // Do nothing
    }
  }
  if (Firebase.getString(firebaseData, "/Appliances/FO")) {
    if (firebaseData.stringData() == "1") {
      FanOn();
      LF5 = 1;
    }
    else {
      FanOff();
      LF5 = 0;
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/L1"))
  {
    Serial.println("data of firebase L1");

    if (firebaseData.stringData() == "1") {
      L1_ON();
      LF1 = 1;
    }
    else {
      L1_OFF();
      LF1 = 0;
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/L2")) {
    if (firebaseData.stringData() == "1") {
      L2_ON();
      LF2 = 1;
    }
    else {
      L2_OFF();
      LF2 = 0;
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/L3"))
  {
    if (firebaseData.stringData() == "1") {
      L3_ON();
      LF3 = 1;
    }
    else {
      L3_OFF();
      LF3 = 0;
    }
  }

  if (Firebase.getString(firebaseData, "/Appliances/L4"))
  {
    if (firebaseData.stringData() == "1") {
      L4_ON();
      LF4 = 1;
    }
    else {
      L4_OFF();
      LF4 = 0;
    }
  }
  Serial.println("end fire");
}

void with_BT_Wifi()
{
  Serial.println("with_BT_Wifi");
  if (digitalRead(s1) == LOW && speed1_flag == 1)
  {

    speedS1();
    json.set("/fan", Speed_Value);
    Firebase.updateNode(firebaseData, "/Appliances", json);
  }

  if (digitalRead(s2) == LOW && digitalRead(s3) == HIGH && speed2_flag == 1)
  {
    speedS2();
    json.set("/fan", Speed_Value);
    Firebase.updateNode(firebaseData, "/Appliances", json);
  }
  if (digitalRead(s2) == HIGH && digitalRead(s3) == LOW && speed3_flag == 1)
  {
    speedS3();
    json.set("/fan", Speed_Value);
    Firebase.updateNode(firebaseData, "/Appliances", json);
  }
  if (digitalRead(s4) == LOW  && speed4_flag == 1)
  {
    speedS4();
    json.set("/fan", Speed_Value);
    Firebase.updateNode(firebaseData, "/Appliances", json);
  }
  if (digitalRead(s1) == HIGH && digitalRead(s2) == HIGH && digitalRead(s3) == HIGH && digitalRead(s4) == HIGH  && speed0_flag == 1)
  {
    speedS0();
    json.set("/fan", Speed_Value);
    Firebase.updateNode(firebaseData, "/Appliances", json);
  }

  // FOR SWITCH
  if (digitalRead(S5) == LOW)
  {
    if (switch_ON_Flag1_previous_I == 0 )
    {
      L1_ON();
      switch_ON_Flag1_previous_I = 1;
    }
  }
  if (digitalRead(S5) == HIGH )
  {
    if (switch_ON_Flag1_previous_I == 1)
    {
      L1_OFF();
      switch_ON_Flag1_previous_I = 0;
    }
  }

  if (digitalRead(S6) == LOW)
  {
    Serial.println("L6");
    if (switch_ON_Flag2_previous_I == 0 )
    {
      L2_ON();
      switch_ON_Flag2_previous_I = 1;
    }
  }
  if (digitalRead(S6) == HIGH )
  {
    if (switch_ON_Flag2_previous_I == 1)
    {
      L2_OFF();
      switch_ON_Flag2_previous_I = 0;
    }
  }

  if (digitalRead(S7) == LOW)
  {
    if (switch_ON_Flag3_previous_I == 0 )
    {
      L3_ON();
      switch_ON_Flag3_previous_I = 1;
    }
  }
  if (digitalRead(S7) == HIGH )
  {
    if (switch_ON_Flag3_previous_I == 1)
    {
      L3_OFF();
      switch_ON_Flag3_previous_I = 0;
    }
  }

  if (digitalRead(S8) == LOW)
  {
    if (switch_ON_Flag4_previous_I == 0 )
    {
      L4_ON();
      switch_ON_Flag4_previous_I = 1;
    }
  }
  if (digitalRead(S8) == HIGH )
  {
    if (switch_ON_Flag4_previous_I == 1)
    {
      L4_OFF();
      switch_ON_Flag4_previous_I = 0;
    }
  }

  if (digitalRead(S9) == LOW)
  {
    if (switch_ON_Flag5_previous_I == 0 )
    {
      FanOn();
      switch_ON_Flag5_previous_I = 1;
    }
  }
  if (digitalRead(S9) == HIGH )
  {
    if (switch_ON_Flag5_previous_I == 1)
    {
      FanOff();
      switch_ON_Flag5_previous_I = 0;
    }
  }
}


void without_BT_Wifi()
{

  // FOR FAN
  if ((!digitalRead(s1) == LOW && speed1_flag == 1) && ((SPD == 1) || (SPD == 6))) {
    speedS1();
    if (SPD == 1)
      SPD = 6;
      if (1 != EEPROM.read(5)) {
      EEPROM.write(5, 1);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(s2) == LOW && digitalRead(s3) == HIGH && speed2_flag == 1) && ((SPD == 2) || (SPD == 6))) {
    speedS2();
    if (SPD == 2)
      SPD = 6;
      if (2 != EEPROM.read(5)) {
      EEPROM.write(5, 2);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(s2) == HIGH && digitalRead(s3) == LOW && speed3_flag == 1) && ((SPD == 3) || (SPD == 6))) {
    speedS3();
    if (SPD == 3)
      SPD = 6;
      if (3 != EEPROM.read(5)) {
      EEPROM.write(5, 3);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(s4) == LOW  && speed4_flag == 1) && ((SPD == 1) || (SPD == 6))) {
    speedS4();
    if (SPD == 4)
      SPD = 6;
    if (4 != EEPROM.read(5)) {
      EEPROM.write(5, 4);
      EEPROM.commit();
    }
  }
  if ((digitalRead(s1) == HIGH && digitalRead(s2) == HIGH && digitalRead(s3) == HIGH && digitalRead(s4) == HIGH  && speed0_flag == 1) && ((SPD == 0) || (SPD == 6))) {
    speedS0();
    if (SPD == 0)
      SPD = 6;
    if (0 != EEPROM.read(5)) {
      EEPROM.write(5, 0);
      EEPROM.commit();
    }
  }

  // FOR SWITCH
  if ((!digitalRead(S5)) && (!(LF1 == 0))) {
    LF1 = 2;
    digitalWrite(R5, HIGH);
    if (1 != EEPROM.read(0)) {
      EEPROM.write(0, 1);
      EEPROM.commit();
    }
  }
  if ((digitalRead(S5)) && (!(LF1 == 1))) {
    LF1 = 2;
    digitalWrite(R5, LOW);
    if (0 != EEPROM.read(0)) {
      EEPROM.write(0, 0);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(S6)) && (!(LF2 == 0))) {
    LF2 = 2;
    digitalWrite(R6, HIGH);
    if (1 != EEPROM.read(1)) {
      EEPROM.write(1, 1);
      EEPROM.commit();
    }
  }
  if ((digitalRead(S6)) && (!(LF2 == 1))) {
    LF2 = 2;
    digitalWrite(R6, LOW);
    if (0 != EEPROM.read(1)) {
      EEPROM.write(1, 0);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(S7)) && (!(LF3 == 0))) {
    LF3 = 2;
    digitalWrite(R7, HIGH);
    if (1 != EEPROM.read(2)) {
      EEPROM.write(2, 1);
      EEPROM.commit();
    }
  }
  if ((digitalRead(S7)) && (!(LF3 == 1))) {
    LF3 = 2;
    digitalWrite(R7, LOW);
    if (0 != EEPROM.read(2)) {
      EEPROM.write(2, 0);
      EEPROM.commit();
    }
  }
  if ((!digitalRead(S8)) && (!(LF4 == 0))) {
    LF4 = 2;
    digitalWrite(R8, HIGH);
    if (1 != EEPROM.read(3)) {
      EEPROM.write(3, 1);
      EEPROM.commit();
    }
  }
  if ((digitalRead(S8)) && (!(LF4 == 1))) {
    LF4 = 2;
    digitalWrite(R8, LOW);
    if (0 != EEPROM.read(3)) {
      EEPROM.write(3, 0);
      EEPROM.commit();
    }
  }

  if ((!digitalRead(S9)) && (!(LF5 == 0))) {
    LF5 = 2;
    FanOn();
    if (1 != EEPROM.read(4)) {
      EEPROM.write(4, 1);
      EEPROM.commit();
    }
  }

  if ((digitalRead(S9)) && (!(LF5 == 1))) {
    LF5 = 2;
    speed0();
    speed0_BT_Fire_flag = 0;
    speed0_flag = 0;
    if (0 != EEPROM.read(4)) {
      EEPROM.write(4, 0);
      EEPROM.commit();
    }
  }
}

// Fan Speed Control

void speedM0() {
  if (speed0_BT_Fire_flag == 1)
  {
    speed0();
    speed0_BT_Fire_flag = 0;
    speed1_BT_Fire_flag = 1;
    speed2_BT_Fire_flag = 1;
    speed3_BT_Fire_flag = 1;
    speed4_BT_Fire_flag = 1;
  }
}
void speedM1() {
  if (speed1_BT_Fire_flag == 1)
  {
    speed1();
    speed0_BT_Fire_flag = 1;
    speed1_BT_Fire_flag = 0;
    speed2_BT_Fire_flag = 1;
    speed3_BT_Fire_flag = 1;
    speed4_BT_Fire_flag = 1;
  }
}
void speedM2() {
  if (speed2_BT_Fire_flag == 1)
  {
    speed2();
    speed0_BT_Fire_flag = 1;
    speed1_BT_Fire_flag = 1;
    speed2_BT_Fire_flag = 0;
    speed3_BT_Fire_flag = 1;
    speed4_BT_Fire_flag = 1;
  }
}
void speedM3() {
  if (speed3_BT_Fire_flag == 1)
  {
    speed3();
    speed0_BT_Fire_flag = 1;
    speed1_BT_Fire_flag = 1;
    speed2_BT_Fire_flag = 1;
    speed3_BT_Fire_flag = 0;
    speed4_BT_Fire_flag = 1;
  }
}
void speedM4() {
  if (speed4_BT_Fire_flag == 1)
  {
    speed4();
    speed0_BT_Fire_flag = 1;
    speed1_BT_Fire_flag = 1;
    speed2_BT_Fire_flag = 1;
    speed3_BT_Fire_flag = 1;
    speed4_BT_Fire_flag = 0;
  }
}

void speedS0() {
  if (speed0_flag == 1)
  {
    speed0();
    speed0_flag = 0;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) SerialBT.print("sp0");
}
void speedS1() {
  if (speed1_flag == 1)
  {
    speed1();
    speed0_flag = 1;
    speed1_flag = 0;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) SerialBT.print("sp1");
}
void speedS2() {
  if (speed2_flag == 1)
  {
    speed2();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 0;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) SerialBT.print("sp2");
}
void speedS3() {
  if (speed3_flag == 1)
  {
    speed3();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 0;
    speed4_flag = 1;
  }
  if (BT_S) SerialBT.print("sp3");
}
void speedS4() {
  if (speed4_flag == 1)
  {
    speed4();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 0;
  }
  if (BT_S) SerialBT.print("sp4");
}

void speed0()
{
  Speed_Value = "0";
  //All Relays Off - Fan at speed 0
  digitalWrite(Speed1, LOW);
  digitalWrite(Speed2, LOW);
  digitalWrite(Speed4, LOW);
  if (0 != EEPROM.read(5)) {
    EEPROM.write(5, 0);
    EEPROM.commit();
  }
}
void speed1()
{
  Speed_Value = "1";
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(1000);
    digitalWrite(Speed1, HIGH);
    if (1 != EEPROM.read(5)) {
      EEPROM.write(5, 1);
      EEPROM.commit();
    }
  }
}

void speed2()
{
  Speed_Value = "2";
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(1000);
    digitalWrite(Speed2, HIGH);
    if (2 != EEPROM.read(5)) {
      EEPROM.write(5, 2);
      EEPROM.commit();
    }
  }
}
void speed3()
{
  Speed_Value = "3";
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(1000);
    digitalWrite(Speed1, HIGH);
    digitalWrite(Speed2, HIGH);
    if (3 != EEPROM.read(5)) {
      EEPROM.write(5, 3);
      EEPROM.commit();
    }
  }
}
void speed4()
{
  Speed_Value = "4";
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(1000);
    digitalWrite(Speed4, HIGH);
    if (4 != EEPROM.read(5)) {
      EEPROM.write(5, 4);
      EEPROM.commit();
    }
  }
}
////

void L1_ON() {
  digitalWrite(R5, HIGH);
  if (1 != EEPROM.read(0)) {
    EEPROM.write(0, 1);
    EEPROM.commit();
  }
  if (BT_S) SerialBT.print("l1");
  String Value1 = "1";
  json.set("/L5", Value1);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}
void L1_OFF() {
  digitalWrite(R5, LOW);
  if (0 != EEPROM.read(0)) {
    EEPROM.write(0, 0);
    EEPROM.commit();
  }
  if (BT_S)SerialBT.print("fl1");
  String Value1 = "0";
  json.set("/L5", Value1);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}

void L2_ON() {
  digitalWrite(R6, HIGH);
  if (1 != EEPROM.read(1)) {
    EEPROM.write(1, 1);
    EEPROM.commit();
  }
  if (BT_S)SerialBT.print("l2");
  String Value2 = "1";
  json.set("/L6", Value2);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}
void L2_OFF() {
  digitalWrite(R6, LOW);
  if (0 != EEPROM.read(1)) {
    EEPROM.write(1, 0);
    EEPROM.commit();
  }
  if (BT_S)SerialBT.print("fl2");
  String Value2 = "0";
  json.set("/L6", Value2);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}

void L3_ON() {
  digitalWrite(R7, HIGH);
  if (1 != EEPROM.read(2)) {
    EEPROM.write(2, 1);
    EEPROM.commit();
  }
  if (BT_S) SerialBT.print("l3");
  String Value3 = "1";
  json.set("/L7", Value3);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}
void L3_OFF() {
  digitalWrite(R7, LOW);
  if (0 != EEPROM.read(2)) {
    EEPROM.write(2, 0);
    EEPROM.commit();
  }
  if (BT_S) SerialBT.print("fl3");
  String Value3 = "0";
  json.set("/L7", Value3);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}

void L4_ON() {
  digitalWrite(R8, HIGH);
  if (1 != EEPROM.read(3)) {
    EEPROM.write(3, 1);
    EEPROM.commit();
  }
  if (BT_S) SerialBT.print("l4");
  String Value4 = "1";
  json.set("/L8", Value4);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}
void L4_OFF() {
  digitalWrite(R8, LOW);
  if (0 != EEPROM.read(3)) {
    EEPROM.write(3, 0);
    EEPROM.commit();
  }
  if (BT_S) SerialBT.print("fl4");
  String Value4 = "0";
  json.set("/L8", Value4);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}

void FanOn() {
  FoS = 1;
  if (1 != EEPROM.read(4)) {
    EEPROM.write(4, 1);
    EEPROM.commit();
  }
  if (Speed_Value == "1")
    speed1();
  else if (Speed_Value == "2")
    speed2();
  else if (Speed_Value == "3")
    speed3();
  else if (Speed_Value == "4")
    speed4();
  else if (Speed_Value == "0")
    speed0();

  if (BT_S) SerialBT.print("fo");
  String FS1 = "1";
  json.set("/FS", FS1);
  Firebase.updateNode(firebaseData, "/Appliances", json);
}

void FanOff()
{
  FoS = 0;
  //EEPROM.update(4, 0);
  if (0 != EEPROM.read(4)) {
    EEPROM.write(4, 0);
    EEPROM.commit();
  }
  //EEPROM.commit();
  speed0();
  String FS1 = "0";
  json.set("/FS", FS1);
  Firebase.updateNode(firebaseData, "/Appliances", json);
  if (BT_S) SerialBT.print("ff");
}

void Data_backup() {
  if (EEPROM.read(0)) {
    L1_ON();
    LF1 = 1;
    switch_ON_Flag1_previous_I = 1;
  } else {
    L1_OFF();
    LF1 = 0;
    switch_ON_Flag1_previous_I = 0;
  }
  if (EEPROM.read(1)) {
    L2_ON();
    LF2 = 1;
    switch_ON_Flag2_previous_I = 1;
  } else {
    L2_OFF();
    LF2 = 0;
    switch_ON_Flag2_previous_I = 0;
  }
  if (EEPROM.read(2)) {
    L3_ON();
    LF3 = 1;
    switch_ON_Flag3_previous_I = 1;
  } else {
    L3_OFF();
    LF3 = 0;
    switch_ON_Flag3_previous_I = 0;
  }
  if (EEPROM.read(3)) {
    L4_ON();
    LF4 = 1;
    switch_ON_Flag4_previous_I = 1;
  } else {
    L4_OFF();
    LF4 = 0;
    switch_ON_Flag4_previous_I = 0;
  }
  if (EEPROM.read(4)) {
    FanOn();
    LF5 = 1;
    switch_ON_Flag5_previous_I = 1;
  } else {
    FanOff();
    LF5 = 0;
    switch_ON_Flag5_previous_I = 0;
  }
  int SPD = EEPROM.read(5);
  switch (SPD) {
    case 0: speedS0();
      break;
    case 1: speedS1();
      break;
    case 2: speedS2();
      break;
    case 3: speedS3();
      break;
    case 4: speedS4();
      break;
  }
}
