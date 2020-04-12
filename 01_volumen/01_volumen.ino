
/*Llenador de recipientes setenado el la cantidad de litros deseados 10/4/20*/  

/*Inicio carga de Librerias*/
//#include <OneWire.h>
#include <DallasTemperature.h> //Lee sensor DS18B20 (temperatura)
//#include <Wire.h>//Comunicacion I2C
#include <LiquidCrystal_I2C.h>
/*Fin carga de Librerias*/

//Crear el objeto lcd  dirección  "0x3F" y configuro las caracteristicas del lcd (20 columnas x 4 filas)
LiquidCrystal_I2C lcd(0x3F, 20, 4);

//asignacion de salidas
const int pinD2 = 2;   //Sondas Temperatura
const int pinD3 = 3;   //Caudalimetro

const int K1    = A0; //Solenoide Agua    
const int pinD4 = A4;   //LCD
const int pinD5 = A5;   //LCD

   

// Instancia a las clases OneWire y DallasTemperature
OneWire oneWireObj(pinD2);
DallasTemperature sensorDS18B20(&oneWireObj);

//Defino en variables las direcciones de las sondas
DeviceAddress address1 = {0x28, 0xAA, 0x7A, 0xB6, 0x13, 0x13, 0x2, 0x6E};//dirección del sensor 1
DeviceAddress address2 = {0x28, 0xAA, 0x6, 0x2A, 0x13, 0x13, 0x2, 0xD1};//dirección del sensor 2
DeviceAddress address3 = {0x28, 0xAA, 0x2E, 0x31, 0x13, 0x13, 0x2, 0xBB};//dirección del sensor 3

volatile int NumPulsos; //variable para la cantidad de pulsos recibidos

float factor_conversion=7.11; //para convertir de frecuencia a caudal
float volumen=0;
long dt=0; //variación de tiempo por cada bucle
long t0=0; //millis() del bucle anterior
float litros = 0;

//---Función que se ejecuta en interrupción---------------
void ContarPulsos ()  
{ 
  NumPulsos++;  //incrementamos la variable de pulsos
} 

//---Función para obtener frecuencia de los pulsos--------
int ObtenerFrecuecia() 
{
  int frecuencia;
  NumPulsos = 0;   //Ponemos a 0 el número de pulsos
  interrupts();    //Habilitamos las interrupciones
  delay(1000);   //muestra de 1 segundo
  noInterrupts(); //Deshabilitamos  las interrupciones
  frecuencia=NumPulsos; //Hz(pulsos por segundo)
  return frecuencia;
}

void setup() 
{ 
// Inicio la comunicación serie
  Serial.begin(9600);

  sensorDS18B20.begin();
  lcd.init(); //Inicio el LCD
  lcd.backlight(); //enciendo el Backlight del LCD
  
  pinMode(pinD2, INPUT); 
  attachInterrupt(0,ContarPulsos,RISING);//(Interrupción 0(Pin2),función,Flanco de subida)
  Serial.println ("Envie 'r' para restablecer el volumen a 0 Litros"); 
  t0=millis();
  digitalWrite((K1), HIGH); // Todos los relays apagados al INICO
} 

void loop ()    
{

String solenoide;
  if (Serial.available()) {
    if(Serial.read()=='r')volumen=0;//restablecemos el volumen si recibimos 'r'
  }
  float frecuencia=ObtenerFrecuecia(); //obtenemos la frecuencia de los pulsos en Hz
  float caudal_L_m=frecuencia/factor_conversion; //calculamos el caudal en L/m
  dt=millis()-t0; //calculamos la variación de tiempo
  t0=millis();
  volumen=volumen+(caudal_L_m/60)*(dt/1000); // volumen(L)=caudal(L/s)*tiempo(s)

   //-----Enviamos por el puerto serie---------------
  Serial.print ("Caudal: "); 
  Serial.print (caudal_L_m,3); 
  Serial.print ("L/min\tVolumen: "); 
  Serial.print (volumen,3); 
  Serial.println (" L");
  if  (volumen <= litros){
      digitalWrite(K1, LOW);
      solenoide = "Llenado";
      Serial.println (solenoide);
      }else{
        digitalWrite(K1, HIGH);
        solenoide = "Agua cerrada";
        Serial.println (solenoide);
   }

/*Salida LCD  */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Estado: " + solenoide);
  
  lcd.setCursor(0, 1);
  lcd.print("");

  lcd.setCursor(0, 2);
  lcd.print("");
  
  lcd.setCursor(0, 3);
  lcd.print("");

}
