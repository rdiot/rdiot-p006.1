/* UVC Camera Servo Motor Control [P006.1] : http://rdiot.tistory.com/318 [RDIoT Demo] */

#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
//https://www.arduino.cc/en/Tutorial/Bridge
#include <Servo.h> 
 
//http://192.168.0.5:8080/?action=stream
//http://192.168.0.5/arduino/control/up/5
//http://192.168.0.5/arduino/control/down/5
//http://192.168.0.5/arduino/control/left/5
//http://192.168.0.5/arduino/control/right/5

YunServer server;

Servo myservo1;
Servo myservo2;

int servoPin_leftright = 9; //좌-우 모터 (0~90)
int servoPin_updown = 10; //상-하 모터 (150~100)

int pos_updown_min = 100; 
int pos_updown_max = 150;

int pos_leftright_min = 0;
int pos_leftright_max = 90;

// 현재 위치
int pos_updown;
int pos_leftright;

// 증분 감소 값
int pos_cnt = 5;
 
void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  
  digitalWrite(13, LOW);
  Bridge.begin();
  digitalWrite(13, HIGH);
  
  server.listenOnLocalhost();
  server.begin();

  myservo1.attach(servoPin_leftright); 
  myservo2.attach(servoPin_updown); 

  //자동운행 후 시작포인트 
  for(int pos = 0; pos < 65; pos += 1) 
  { 
    myservo1.write(pos);
    pos_leftright = pos;

    delay(100); //delay값을 조정하여 모터의 속도를 컨터롤가능
  }

  for(int pos = 150; pos>=100; pos-=1)
  { 
    myservo2.write(pos); 
    pos_updown = pos;
    delay(100); 
  } 

}
 
void loop() {
  YunClient client = server.accept();
 
  if (client) {
    process(client);
    client.stop();
  }
 
  delay(50); 
}
 
void process(YunClient client) {
  String command = client.readStringUntil('/');
 
  if (command == "digital") {
    digitalCommand(client);
  }
  if (command == "analog") {
    analogCommand(client);
  }
  if (command == "mode") {
    modeCommand(client);
  }
  if (command == "control") {
    controlCommand(client);
  }
}
 
void digitalCommand(YunClient client) {
  int pin, value;
pin = client.parseInt();
 
  if (client.read() == '/') {
    value = client.parseInt();
    digitalWrite(pin, value);
  } 
  else {
    value = digitalRead(pin);
  }
  client.print(F("Pin D"));
  client.print(pin);
  client.print(F(" set to "));
  client.println(value);
 
  String key = "D";
  key += pin;
  Bridge.put(key, String(value));

}
 
void analogCommand(YunClient client) {
  int pin, value;
 
  pin = client.parseInt();
 
  if (client.read() == '/') {
    value = client.parseInt();
    analogWrite(pin, value);
 
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" set to analog "));
    client.println(value);
 
    String key = "D";
    key += pin;
    Bridge.put(key, String(value));
  }
  else {
    value = analogRead(pin);
 
    client.print(F("Pin A"));
    client.print(pin);
    client.print(F(" reads analog "));
    client.println(value);
 
    String key = "A";
    key += pin;
    Bridge.put(key, String(value));
  }
}
 
void modeCommand(YunClient client) {
  int pin;
  pin = client.parseInt();
if (client.read() != '/') {
    client.println(F("error"));
    return;
  }
 
String mode = client.readStringUntil('\r');
 
  if (mode == "input") {
    pinMode(pin, INPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as INPUT!"));
    return;
  }
 
  if (mode == "output") {
    pinMode(pin, OUTPUT);
    // Send feedback to client
    client.print(F("Pin D"));
    client.print(pin);
    client.print(F(" configured as OUTPUT!"));
    return;
  }
 
  client.print(F("error: invalid mode "));
  client.print(mode);
}


void controlCommand(YunClient client) {
  String mode = client.readStringUntil('/');
  
  if(mode == "up")
  {
    int temp = pos_updown + pos_cnt;
    if(temp < pos_updown_max)
    {
      myservo2.write(temp); 
      pos_updown = temp;
    }
  }


  if(mode == "down")
  {
   int temp = pos_updown - pos_cnt;
    if(temp > pos_updown_min)
    {
      myservo2.write(temp); 
      pos_updown = temp;
    } 
  }

  if(mode == "right")
  {    
   int temp = pos_leftright - pos_cnt;
    if(temp > pos_leftright_min)
    {
      myservo1.write(temp); 
      pos_leftright = temp;
    }  
  }

  if(mode == "left")
  {
   int temp = pos_leftright + pos_cnt;
    if(temp < pos_leftright_max)
    {
      myservo1.write(temp); 
      pos_leftright = temp;
    }

  }

client.print(F("Camera "));
  client.print(mode);
  client.print(F(" Current Point X = "));
  client.print(pos_leftright);
  client.print(F(", Current Point Y = "));
  client.print(pos_updown);

}
