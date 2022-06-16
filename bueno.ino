#include <LiquidCrystal.h>

#define analogPin A3
#define dirPin 2
#define stepPin 3
#define stepsPerRevolution 20

// LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7;

const int pasosDefault = 600;
int incremento = 50;

//Parametros presiones
int pSis = 120;
int pDias = 80;

int opcionPresion = 1;
int bandera = 0;

const int pin_BL = 10; 

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

void setup() {
  Serial.begin(9600);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  
  /* ========== Display ========== */
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("P Sis: ");
  lcd.setCursor(0,1);
  lcd.print("P Dias:");
  lcd.setCursor(12,0);
  lcd.print("mmHg");
  lcd.setCursor(12,1);
  lcd.print("mmHg");

  Serial.println("Comienza esta madre...");

  pinMode(analogPin, INPUT);
}

void loop() {
  /* Imprimir presiones en la pantalla */
  lcd.setCursor(8,0);
  lcd.print(pSis);
  lcd.setCursor(8,1);
  lcd.print(pDias);

  /* Controlador de botones */
  int x;
  x = analogRead (0);
  lcd.setCursor(10,1);

  /* ========== RIGHT ========== */
  if (x < 60) {
    opcionPresion++;
    if(opcionPresion > 2){
      opcionPresion = 1;
    }
    delay(300);
  }
  /* ========== UP ========== */
  else if (x < 200) {
    //lcd.print ("Up    ");
    if (opcionPresion == 1){
      pSis += 10;
    } else{
      pDias += 10;
    }
    delay(300);
  }
  /* ========== DOWN ========== */
  else if (x < 400){
    if (opcionPresion == 1){
      pSis -= 10;
    } else{
      pDias -= 10;
    }
    delay(300);

    /* Ajuste Display */
    if (pSis < 100){
      lcd.setCursor(10,0);
      lcd.print(" ");
    }else if(pDias < 100){
      lcd.setCursor(10,1);
      lcd.print(" ");
    }
  }
  /* ========== LEFT ========== */
  else if (x < 600){
    opcionPresion--;
    if(opcionPresion < 1){
      opcionPresion = 2;
    }
    delay(300);
  }
  /* ========== SELECT ========== */
  else if (x < 800){
    Raul(pSis, pDias, PAM(pSis, pDias));
  }
}

float PAM(int pSis, int pDias){
  return (pSis + 2*pDias)/3;
}

void Raul(int pSis, int pDias, float pam){
  int contador = 0;
  int incremento = 30;
  
  while(1){
     /* ========== Conversion de entrada ADC a mmHg ========== */
    int adc = analogRead(analogPin);
    float voltaje = adc * (5.0/1023.0);
    float adcRead = (396.21 * (voltaje)) - 84.892;

    if(adcRead >= pSis + 5){
      bandera++;
    }
    
    Serial.print("Lectura adc: ");
    Serial.print(adc);
    Serial.print(", ");
    Serial.print("Voltaje: ");
    Serial.print(voltaje);
    Serial.print(", ");
    Serial.print("mmHg: ");
    Serial.println(adcRead);

    if (bandera >= 2){
      
      /* ========== De presion SIS a PAM ========== */
      while(adcRead <= pSis && adcRead > pam){
        
        digitalWrite(dirPin, HIGH);
        for (int i = 0; i <  2*stepsPerRevolution + incremento; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1500 - incremento);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1500 - incremento);
        } 
        
        digitalWrite(dirPin, LOW);
        for (int i = 0; i <  2*stepsPerRevolution + incremento; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1500 - incremento);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1500 - incremento);
        }
        incremento += 5;
        contador++;
      }

      /* ========== De presion PAM a DIAS ========== */
      while(adcRead <= pam && adcRead > pDias){
        incremento -= 5;
        digitalWrite(dirPin, HIGH);
        for (int i = 0; i <  2*stepsPerRevolution + incremento; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1000 - incremento);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1000 - incremento);
        }
        
        digitalWrite(dirPin, LOW);
        for (int i = 0; i <  2*stepsPerRevolution + incremento; i++) {
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1000 - incremento);
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1000 - incremento);
        }
      }
    }
  }
}
