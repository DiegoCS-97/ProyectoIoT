#include "ThingSpeak.h"
#include "NewPing.h"
#include "WiFi.h"

//Pines para sensores
const int pinAsiento = 35;

const int trigE = 13; // trigger pin
const int echoE = 12; // echo pin

const int trigS = 27; // trigger pin
const int echoS = 26; // echo pin

// Conexion a internet
const char *ssid = "GalaxyNote10"; // Nombre de la red
const char *password = "12345678"; // Contraseña de la red

// Información para poder subir datos a ThingSpeak
unsigned long channelID = 1697467;            // ID del canal
const char *WriteAPIKey = "PZIFTNYNUAH3G83E"; // Write API Key del canal
WiFiClient cliente;                           // Objeto Cliente del WiFi

//Instancias de los sensores
NewPing sonarEntrada(trigE, echoE, 2000);
NewPing sonarSalida(trigS, echoS, 2000);

//Cantidades a monitorear y reportar
int personas = 0;
int asientos = 0;

int state = 0;
int lastState = 0;


void setup()
{

  Serial.begin(115200);
  // Conexión a internet
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");
  ThingSpeak.begin(cliente); // Inicializar el servidor de ThingSpeak
}

void loop()
{

  leerAsiento();
  leerEntrada();
  delay(200);
  leerSalida();

  ThingSpeak.writeFields(channelID, WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");

  delay(100);
} 

void leerEntrada() {
  int distance;
  long duration;

  // Obtener medicion de tiempo de viaje del sonido y guardar en variable uS
  int uS = sonarEntrada.ping_median();
  // Imprimir la distancia medida a la consola serial
  Serial.print("Distancia: ");
  // Calcular la distancia con base en una constante
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");

  //Si la distancia es menor que el ancho de la entrada, significa que una persona entró
  if(uS / US_ROUNDTRIP_CM < 20) {
    personas++;
  } 
  
  ThingSpeak.setField(1, personas); // En el campo 1 se le asigna la cantidad de personas en el autobus
}

void leerSalida() {
  int distance;
  long duration;

  // Obtener medicion de tiempo de viaje del sonido y guardar en variable uS
  int uS = sonarSalida.ping_median();
  // Imprimir la distancia medida a la consola serial
  Serial.print("Distancia: ");
  // Calcular la distancia con base en una constante
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");

  //Si la distancia es menor que el ancho de la salida, significa que una persona salió
  if(uS / US_ROUNDTRIP_CM < 20) {
    personas--;
  } 
  
  ThingSpeak.setField(1, personas); // En el campo 1 se le asigna la cantidad de personas en el autobus
}

void leerAsiento() {
  int ResRead = analogRead(pinAsiento);
  Serial.print("Lectura analogica = ");
  Serial.println(ResRead);

  state = ResRead;

  if(state != lastState) {
    if(state >= 3000) {
      asientos++;
    } else {
      asientos--;  
    }
  }

  lastState = state;

  ThingSpeak.setField(2, asientos); // En el campo 1 se le asigna la cantidad de personas en el autobus
}
