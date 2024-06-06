#include "SoftwareSerial.h"
#include <Servo.h>

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards
int pos = 0; // variable to store the servo position
// PINES
int const pinBoton = 3;
int const pinBotonReserva = 2;
int const pinServo = 4;
int const pinPIR = 5;
int const ledLleno = 6;
int const ledLibre = 7;

volatile int numcoches = 0;

// TIEMPO
unsigned long lastButtonPressTime = 0;
unsigned long lastButtonPressTime2 = 0;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long debounceDelay = 50;

// WiFi config
String ssid       = "Livebox6-CF99";
String password   = "D2RPZ6CFXz9b";

float tempC=0.0;
float reading;

// Server, file, and port
const String hostname = "api.thingspeak.com";
const String uri_write = "/update?api_key=ATCU0EON0K5MW6VP&field1=";
const String uri_read = "/channels/2510239/fields/1.json?api_key=I53ATH9YS4OA3BXN&results=1";

const int port = 80;

volatile bool esperandoParaSalir = false; // Para saber si la barrera se ha abierto para que salga un coche
volatile bool puertaEntradaAbierta = false;
volatile bool puedeSalirAlguien = true; // Para que cuando el PIR detecte durante un rato, solo cuente la primera vez

// Software serial object
// Cable verde 52 y amarillo 51
SoftwareSerial soft(51, 52); // RX, TX

const int groundpin = A2;
const int powerpin = A0;
int tempPin = A1;
void setup() {

  attachInterrupt(digitalPinToInterrupt(pinBoton), contReboteEntrar, RISING);

  myservo.attach(pinServo); // attaches the servo on pin 2 to the servo object
  pinMode(pinBoton,INPUT);
  pinMode(pinPIR, INPUT);  
  pinMode(ledLleno, OUTPUT);
  pinMode(ledLibre, OUTPUT);
  digitalWrite(ledLibre,1);
    
  myservo.write(0);
  
  soft.begin(115200);
  Serial.begin(9600);

  Serial.println("Init ESP8266...");

  soft.setTimeout(2000);

  soft.println("AT+UART_DEF=115200,8,1,0,0");

  soft.println("AT+RST");
  soft.println("AT");
  if(soft.find("OK"))
    Serial.println("Respuesta AT correcto");
  else
    Serial.println("Error en ESP8266");

  //-----Configuración de red-------//Podemos comentar si el ESP ya está configurado
    
    //ESP8266 en modo estación (nos conectaremos a una red existente)
    soft.println("AT+CWMODE=1");
    if(soft.find("OK"))
      soft.println("ESP8266 en modo Estacion");
      
    soft.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");

    Serial.println("Conectandose a la red ...");
    delay(4000); //Aumentar si demora la conexion
    while (!soft.find("OK")) {
      soft.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
      Serial.println("Error al conectarse en la red. siguiente intento");
      soft.setTimeout(2000);
    }
    Serial.println("Conectado");
      
    delay(5000);
    //Desabilitamos las conexiones multiples
    soft.println("AT+CIPMUX=0");
    if(soft.find("OK"))
      Serial.println("Multiconexiones deshabilitadas");
    
  //------fin de configuracion-------------------
  
}

int comprobarBarrera(){
  // Verificar si la barrera está abierta y ha pasado suficiente tiempo
  unsigned long currentMillis = millis();
  if (puertaEntradaAbierta && (currentMillis - lastButtonPressTime >= 5000)) { // Comprobar si han pasado 5 segundos
    Serial.println("CERRANDO BARRERA");
    myservo.write(0);
    // Realizar otras tareas que necesites hacer después de cerrar la barrera
    
    // Reiniciar el tiempo de la última pulsación para evitar que la barrera se cierre repetidamente
    lastButtonPressTime = currentMillis;
    puertaEntradaAbierta = false;

    return 0;
  }else{
    return -1;
  }
}

int enviarNumCoches(){
  // Abrimos conexion TCP
  soft.println("AT+CIPSTART=\"TCP\",\"" + hostname + "\"," + port);
  delay(5000);
  
  if ( soft.find("OK") == 0 ) {
    Serial.println("Error start");
  }

  //Construyo la peticion GET
  String req = "GET " + uri_write + String(numcoches) +" HTTP/1.1\r\n" +
            "Host: " + hostname + "\r\n" +
              "Connection: close\r\n" +
              "\r\n";

  int len = req.length();
  Serial.print(req);  
  
  // 1. Enviamos la longitud
  String longi="AT+CIPSEND=";
  longi+=(String) len;
  Serial.print(longi);

  soft.println(longi);

  delay(5000);
  //Compurebo que todo ok
  if ( !soft.find(">") ) {
    Serial.println("Error send longitud mensaje");
  }

  // Enviar la solicitud HTTP GET
  soft.print(req);

  // Esperar respuesta del servidor
  //delay(5000); // Esperar un tiempo suficiente para recibir la respuesta

  String response = "";

  while (soft.available()) {
    char c = soft.read();
    response += c;
    Serial.print(c);
  }

  // Imprimir la respuesta recibida
  Serial.println("RESPUESTA:");
  Serial.println(response);
  Serial.println("Respuesta acabada");

  // Cerrar conexión TCP
  soft.println("AT+CIPCLOSE=0");

  return 0;
}

int salidaCoches(){
  if(numcoches == 0) return 0;

  int estado = digitalRead(pinPIR);

  if(estado == 1 && puedeSalirAlguien){
    puedeSalirAlguien = false;
    contReboteSalir();
  }else if(estado == 0){
    puedeSalirAlguien = true;
  }

  return estado;
}

void loop() {
  int r = -1;
  int es = -1;
  // BARRERA ------------------------------------------------------------------------------------------------------
  r = comprobarBarrera();
  es = salidaCoches();
 
  // WIFI ---------------------------------------------------------------------------------------------------------
  if(r == 0){ // Si se ha cerrado la barrera de entrada, habrá que informar de que ha entrado un nuevo coche
    Serial.println("Ha entrado un nuevo coche. Hay que informar al servidor");
    if(esperandoParaSalir){
      numcoches--;
    }
    else numcoches++;
    if(numcoches < 0) numcoches = 0;
    if(numcoches >= 3){
      digitalWrite(ledLibre,0);
      digitalWrite(ledLleno,1);
    }else{
      digitalWrite(ledLibre,1);
      digitalWrite(ledLleno,0);
    }
    Serial.print("Num coches: ");
    Serial.println(numcoches);
    if(enviarNumCoches() == 0){
      Serial.println("numcoches actualizado con éxito");
    }else{
      Serial.println("ERROR: no se puedo actualizar numcoches");
    }
  } 
}

void contReboteEntrar() {
  unsigned long currentMillis = millis();
  if (millis() - lastDebounceTime1 > debounceDelay) {
    if(numcoches < 3){
      Serial.println("ABRIENDO BARRERA (Entrada)");
      myservo.write(90);
      esperandoParaSalir = false;
      puertaEntradaAbierta = true;
      puedeSalirAlguien = false;
      lastDebounceTime1 = millis();
      lastButtonPressTime = millis();
    }else{
      Serial.println("PARKING LLENO");
    }
  }
}

void contReboteSalir() {
  unsigned long currentMillis = millis();
  if (millis() - lastDebounceTime1 > debounceDelay) {
    Serial.println("ABRIENDO BARRERA (Salida)");
    myservo.write(90);
    esperandoParaSalir = true;
    puertaEntradaAbierta = true;
    lastDebounceTime1 = millis();
    lastButtonPressTime = millis();
  }
}

