//Arduino 1.8.13
//esp32 https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// 1.0.6
#include <EEPROM.h>
#include "RTClib.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define EEPROM_SIZE 30


BluetoothSerial SerialBT;
RTC_DS1307 rtc;


// Function Declaration

void  with_BT();
void  without_BT();
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
#define R6 25
#define R7 2
#define R8 15

// Pins of Relay (Fan Speed Control)
#define Speed1 18
#define Speed2 19
#define Speed4 23



// Necessary Variables
bool BT_S = 0;
bool FoS = 0;
bool btp = 0;
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

  btStart();
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
  EEPROM.begin(EEPROM_SIZE);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  Serial.println("BT Device is started");
  Data_backup();
}

void loop()
{
  DateTime now = rtc.now();
  BT_S = (SerialBT.connected());
  if (BT_S) {
    if (!btp) {
     feedback();
      btp = 1;
    }

    if (SerialBT.available()) {
      BT_Valve = SerialBT.readString();
      Serial.println(BT_Valve);
      Data_from_BT();
    }
    with_BT();
  } else {
    without_BT();
    btp =0;
  }

  if (Auto_mode == 1) {
    if (now.hour() == OnHour && now.minute() == OnMin) {
      On_fun();
    } else if (now.hour() == OffHour && now.minute() == OffMin) {
      Off_fun();
    }
  }
  delay(1000);
}
void feedback() {
  if ( EEPROM.read(0)) {
    if (BT_S) {
      SerialBT.print("l1");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("fl1");
      delay(2000);
    }
  }
  if ( EEPROM.read(1)) {
    if (BT_S) {
      SerialBT.print("l2");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("fl2");
      delay(2000);
    }
  }
  if ( EEPROM.read(2)) {
    if (BT_S) {
      SerialBT.print("l3");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("fl3");
      delay(2000);
    }
  }
  if ( EEPROM.read(3)) {
    if (BT_S) {
      SerialBT.print("l4");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("fl4");
      delay(2000);
    }
  }
  if ( EEPROM.read(4)) {
    if (BT_S) {
      SerialBT.print("fo");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("ff");
      delay(2000);
    }
  }
  if ( EEPROM.read(6)) {
    if (BT_S) {
      SerialBT.print("A");
      delay(2000);
    }
  }else{
    if (BT_S) {
      SerialBT.print("a");
      delay(2000);
    }
  }
  if (0== EEPROM.read(5)) {
    if (BT_S) {
      SerialBT.print("sp0");
      delay(2000);
    }
  }else if (1== EEPROM.read(5)) {
    if (BT_S) {
      SerialBT.print("sp1");
      delay(2000);
    }
  }else if (2== EEPROM.read(5)) {
    if (BT_S) {
      SerialBT.print("sp2");
      delay(2000);
    }
  }
  else if (3== EEPROM.read(5)) {
    if (BT_S) {
      SerialBT.print("sp3");
      delay(2000);
    }
  }
  else if (4== EEPROM.read(5)) {
    if (BT_S) {
      SerialBT.print("sp4");
      delay(2000);
    }
  }
}
void On_fun() {
  if (L1_flag == "1")
    L1_ON();
    delay(2000);
  if (L2_flag == "1")
    L2_ON();
    delay(2000);
  if (L3_flag == "1")
    L3_ON();
    delay(2000);
  if (L4_flag == "1")
    L4_ON();
    delay(2000);
  AutoS = "1";
  if (BT_S) {
    SerialBT.print("AMS");
    delay(2000);
  }
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
  if (BT_S) {
    SerialBT.print("AMP");
    delay(2000);
  }
}


void Data_from_BT() {
  if (BT_Valve == "A") {
    Auto_mode = 1;
    SerialBT.print("A");
    delay(2000);
    if (1 != EEPROM.read(6)) {
      EEPROM.write(6, 1);
      EEPROM.commit();
    }
  }
  else if (BT_Valve == "a") {
    Auto_mode = 0;
    SerialBT.print("a");
    delay(2000);
    if (0 != EEPROM.read(6)) {
      EEPROM.write(6, 0);
      EEPROM.commit();
    }
  } else  if (BT_Valve.startsWith("H1")) {
    String msg1 = BT_Valve.substring(2, 4);
    if ((msg1[1] - 48) == -48) {
      OnHour = (msg1[0] - 48);
    } else {
      OnHour = (((msg1[0] - 48) * 10) + (msg1[1] - 48));
    }
    SerialBT.print(BT_Valve);
    if (OnHour != EEPROM.read(11)) {
      EEPROM.write(11, OnHour);
      EEPROM.commit();
    }
  }
  else  if (BT_Valve.startsWith("M1")) {
    String msg2 = BT_Valve.substring(2, 4);
    if ((msg2[1] - 48) == -48) {
      OnMin = (msg2[0] - 48);
    } else {
      OnMin = (((msg2[0] - 48) * 10) + (msg2[1] - 48));
    }
    SerialBT.print(BT_Valve);
    if (OnMin != EEPROM.read(12)) {
      EEPROM.write(12, OnMin);
      EEPROM.commit();
    }
  } else  if (BT_Valve.startsWith("H2")) {
    String msg3 = BT_Valve.substring(2, 4);
    if ((msg3[1] - 48) == -48) {
      OffHour = (msg3[0] - 48);
    } else {
      OffHour = (((msg3[0] - 48) * 10) + (msg3[1] - 48));
    }
    SerialBT.print(BT_Valve);
    if (OffHour != EEPROM.read(13)) {
      EEPROM.write(13, OffHour);
      EEPROM.commit();
    }
  } else  if (BT_Valve.startsWith("M2")) {
    String msg4 = BT_Valve.substring(2, 4);
    if ((msg4[1] - 48) == -48) {
      OffMin = (msg4[0] - 48);
    } else {
      OffMin = (((msg4[0] - 48) * 10) + (msg4[1] - 48));
    }
    SerialBT.print(BT_Valve);
    if (OffMin != EEPROM.read(14)) {
      EEPROM.write(14, OffMin);
      EEPROM.commit();
    }
  } else if (BT_Valve == "L1F1") {
    L1_flag = "1";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L1F0") {
    L1_flag = "0";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L2F1") {
    L2_flag = "1";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L2F0") {
    L2_flag = "0";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L3F1") {
    L3_flag = "1";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L3F0") {
    L3_flag = "0";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L4F1") {
    L4_flag = "1";
    SerialBT.print(BT_Valve);
  }
  else if (BT_Valve == "L4F0") {
    L4_flag = "0";
    SerialBT.print(BT_Valve);
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


void with_BT()
{
  Serial.println("with_BT");
  if (digitalRead(s1) == LOW && speed1_flag == 1)
  {
    speedS1();
  }

  if (digitalRead(s2) == LOW && digitalRead(s3) == HIGH && speed2_flag == 1)
  {
    speedS2();
  }
  if (digitalRead(s2) == HIGH && digitalRead(s3) == LOW && speed3_flag == 1)
  {
    speedS3();
  }
  if (digitalRead(s4) == LOW  && speed4_flag == 1)
  {
    speedS4();
  }
  if (digitalRead(s1) == HIGH && digitalRead(s2) == HIGH && digitalRead(s3) == HIGH && digitalRead(s4) == HIGH  && speed0_flag == 1)
  {
    speedS0();
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


void without_BT()
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
  Speed_Value = "0";
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
  Speed_Value = "1";
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
  Speed_Value = "2";
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
  Speed_Value = "3";
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
  Speed_Value = "4";
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
  Speed_Value = "0";
  if (speed0_flag == 1)
  {
    speed0();
    speed0_flag = 0;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) {
    SerialBT.print("sp0");
    delay(2000);
  }
}
void speedS1() {
  Speed_Value = "1";
  if (speed1_flag == 1)
  {
    speed1();
    speed0_flag = 1;
    speed1_flag = 0;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) {
    SerialBT.print("sp1");
    delay(2000);
  }
}
void speedS2() {
  Speed_Value = "2";
  if (speed2_flag == 1)
  {
    speed2();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 0;
    speed3_flag = 1;
    speed4_flag = 1;
  }
  if (BT_S) {
    SerialBT.print("sp2");
    delay(2000);
  }

}
void speedS3() {
  Speed_Value = "3";
  if (speed3_flag == 1)
  {
    speed3();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 0;
    speed4_flag = 1;
  }
  if (BT_S) {
    SerialBT.print("sp3");
    delay(2000);
  }
}
void speedS4() {
  Speed_Value = "4";
  if (speed4_flag == 1)
  {
    speed4();
    speed0_flag = 1;
    speed1_flag = 1;
    speed2_flag = 1;
    speed3_flag = 1;
    speed4_flag = 0;
  }
  if (BT_S) {
    SerialBT.print("sp4");
    delay(2000);
  }
}

void speed0()
{
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
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(500);
    digitalWrite(Speed2, HIGH);
    if (1 != EEPROM.read(5)) {
      EEPROM.write(5, 1);
      EEPROM.commit();
    }
  }
}

void speed2()
{
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(500);
    digitalWrite(Speed1, HIGH);

    if (2 != EEPROM.read(5)) {
      EEPROM.write(5, 2);
      EEPROM.commit();
    }
  }
}
void speed3()
{
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(500);
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
  if (FoS) {
    digitalWrite(Speed1, LOW);
    digitalWrite(Speed2, LOW);
    digitalWrite(Speed4, LOW);
    delay(500);
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
  if (BT_S) {
    SerialBT.print("l1");
    delay(1000);
  }
}
void L1_OFF() {
  digitalWrite(R5, LOW);
  if (0 != EEPROM.read(0)) {
    EEPROM.write(0, 0);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("fl1");
    delay(1000);
  }
}
void L2_ON() {
  digitalWrite(R6, HIGH);
  if (1 != EEPROM.read(1)) {
    EEPROM.write(1, 1);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("l2");
    delay(2000);
  }

}
void L2_OFF() {
  digitalWrite(R6, LOW);
  if (0 != EEPROM.read(1)) {
    EEPROM.write(1, 0);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("fl2");
    delay(1000);
  }

}

void L3_ON() {
  digitalWrite(R7, HIGH);
  if (1 != EEPROM.read(2)) {
    EEPROM.write(2, 1);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("l3");
    delay(1000);
  }
}
void L3_OFF() {
  digitalWrite(R7, LOW);
  if (0 != EEPROM.read(2)) {
    EEPROM.write(2, 0);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("fl3");
    delay(1000);
  }
}

void L4_ON() {
  digitalWrite(R8, HIGH);
  if (1 != EEPROM.read(3)) {
    EEPROM.write(3, 1);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("l4");
    delay(1000);
  }
}
void L4_OFF() {
  digitalWrite(R8, LOW);
  if (0 != EEPROM.read(3)) {
    EEPROM.write(3, 0);
    EEPROM.commit();
  }
  if (BT_S) {
    SerialBT.print("fl4");
    delay(1000);
  }
}

void FanOn() {
  FoS = 1;
  if (1 != EEPROM.read(4)) {
    EEPROM.write(4, 1);
    EEPROM.commit();
  }
  if (Speed_Value == "1")
    speed1();
  if (Speed_Value == "2")
    speed2();
  if (Speed_Value == "3")
    speed3();
  if (Speed_Value == "4")
    speed4();
  if  (Speed_Value == "0")
    speed0();

  if (BT_S) {
    SerialBT.print("fo");
    delay(1000);
  }
}

void FanOff()
{
  FoS = 0;
  if (0 != EEPROM.read(4)) {
    EEPROM.write(4, 0);
    EEPROM.commit();
  }
  speed0();

  if (BT_S) {
    SerialBT.print("ff");
    delay(1000);
  }
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
  if (EEPROM.read(6)) {
    Auto_mode = 1;
  } else {
    Auto_mode = 0;
  }
  OnHour = EEPROM.read(11);
  Serial.println(EEPROM.read(11));
 Serial.println(OnHour);
   Serial.println(EEPROM.read(12));
  OnMin = EEPROM.read(12);
  OffHour = EEPROM.read(13);
  OffMin = EEPROM.read(14);

}
