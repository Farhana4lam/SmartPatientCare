#define USE_ARDUINO_INTERRUPTS true
#include <DS3231.h>
#include <LiquidCrystal.h>
#include <PulseSensorPlayground.h>
#include <Wire.h>

LiquidCrystal lcd(7,6,5,4,3,2);
DS3231 rtc(SDA, SCL);
Time t;
int buz = 13;
int hrs, mint, scnd;
int tempPin = A1;  //room temperature
int BtempPin = A0;  //body temperature 
int fan = 11;       
int temp, Btemp;
int tempMin = 22;   
int tempMax = 37;  
int fanSpeed, fanLCD;
int light = 10;


const int PulseWire = A3;
int Threshold = 550;

PulseSensorPlayground pulseSensor;

void setup() {
  Wire.begin();
  rtc.begin();
  
  pinMode(fan, OUTPUT);
  pinMode(light, OUTPUT);
  pinMode(tempPin, INPUT);
  pinMode(BtempPin, INPUT);
  pinMode(buz, OUTPUT);
  
  lcd.begin(20,4);  
  Serial.begin(9600);

  rtc.setTime(10,0,0);
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.setThreshold(Threshold);
  pulseSensor.begin();
  
}

void loop() 
{  
   t = rtc.getTime(); 
   hrs = t.hour;
   mint = t.min;
   scnd = t.sec;

   //room temperature
   temp = analogRead(tempPin);
   temp = temp * 0.48828125;
   if(temp < tempMin)     
   {   
       fanSpeed = 0;      
       analogWrite(fan, fanSpeed); 
       fanLCD=0;
       digitalWrite(fan, LOW);       
   } 
   if((temp >= tempMin) && (temp <= tempMax)) 
   {  
       fanSpeed = temp;
       fanSpeed=1.5*fanSpeed;
       fanLCD = map(temp, tempMin, tempMax, 0, 100);  
       analogWrite(fan, fanSpeed);  
   }

  //medicine alarm
   if((hrs == 10 && mint == 01 && scnd == 00) || (hrs == 15 && mint == 00 && scnd == 00)|| (hrs == 20 && mint == 00 && scnd == 00))
   {
    for (int i = 0; i<6; i++)
    {
      digitalWrite(buz,HIGH);
      delay(500);
      digitalWrite(buz,LOW);
      delay(500);
      lcd.setCursor(0,0);
      lcd.print("***Medicine Time***"); 
      delay(500);
      lcd.clear();  
    }
    
    //printing all the medicines to take in LCD
      lcd.setCursor(0,0);
      lcd.print("Medicine List: ");
      lcd.setCursor(0,1);
      lcd.print("Paracitamol, Calbo");
      lcd.setCursor(0,2);
      lcd.print("Calcitrol");
      delay(1000);
      lcd.clear();
    
   }


   //heartbeat

      int myBPM = pulseSensor.getBeatsPerMinute();
      if((myBPM>130 || myBPM<40) && myBPM!=0)
      {
        for (int i = 0; i<4; i++)
        {
          digitalWrite(buz,HIGH);
          delay(500);
          digitalWrite(buz,LOW);
          delay(500);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Abnormal Heart Rate!");
          lcd.setCursor(0,1);
          lcd.print("BPM: ");
          lcd.print(myBPM);
          delay(1000);
          lcd.clear();
        }
        
      }

   //body temperature   
     Btemp = analogRead(BtempPin);
     Btemp = (Btemp * 0.48828125*1.8) + 32.0;
     if(Btemp>99)
     {
      for (int i = 0; i<4; i++)
        {
          digitalWrite(buz,HIGH);
          delay(500);
          digitalWrite(buz,LOW);
          delay(500);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Abnormal Temperature");
          lcd.setCursor(0,1);
          lcd.print("TEMP: ");
          lcd.print(Btemp);
          delay(1000);
          lcd.clear();
        }
        
     }

     //light
     int l = analogRead(A2);
     int b = map(l,0,1023,0,255);
     if(b<220) //its dark
     {
      digitalWrite(light,HIGH);
     }
     if(b>=220)
     {
      digitalWrite(light,LOW);
     }
     if(hrs == 22 && mint == 00 && scnd == 05)
      {
        digitalWrite(light,LOW); //bedtime
      }
     
        
   lcd.setCursor(0,0);
   lcd.print("BPM: ");
   lcd.print(myBPM);
   lcd.setCursor(0,1);
   lcd.print("BODY TEMP: ");
   lcd.print(Btemp);      
   lcd.print("F ");   
   lcd.setCursor(0,2);
   lcd.print("TEMP: ");
   lcd.print(temp);      
   lcd.print("C ");
   lcd.print("FANS: ");
   lcd.print(fanLCD);   
   lcd.print("%");
   lcd.setCursor(0,3);
   lcd.print("TIME: ");
   lcd.print(rtc.getTimeStr());
   delay(500);
   lcd.clear();
}
