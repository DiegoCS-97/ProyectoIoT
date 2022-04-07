#include "HX711.h"
#include "ThingSpeak.h"
#include "NewPing.h"
#include "WiFi.h"

//Pines para sensores
const int LOADCELL_DOUT_PIN = 26;
const int LOADCELL_SCK_PIN = 25;

const int trigE = 13; // trigger pin
const int echoE = 12; // echo pin

const int trigS = 13; // trigger pin
const int echoS = 12; // echo pin

// Conexion a internet
const char *ssid = "GalaxyNote10"; // Nombre de la red
const char *password = "12345678"; // Contraseña de la red

// Información para poder subir datos a ThingSpeak
unsigned long channelID = 1697467;            // ID del canal
const char *WriteAPIKey = "PZIFTNYNUAH3G83E"; // Write API Key del canal
WiFiClient cliente;                           // Objeto Cliente del WiFi

//Instancias de los sensores
HX711 balanza;
NewPing sonarEntrada(trigE, echoE, 2000);
NewPing sonarSalida(trigS, trigS, 2000);

//Cantidades a monitorear y reportar
int personas;
int asientos;

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

  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(250);

  balanza.set_scale(397);
  balanza.tare(10); // Hacer 10 lecturas, el promedio es la tara
}

void loop()
{

  leerAsiento();
  leerEntrada();
  leerSalida();

  ThingSpeak.writeFields(channelID, WriteAPIKey);
  Serial.println("Datos enviados a ThingSpeak!");

  delay(1000);
} 

void leerEntrada() {
  int distance;
  long duration;

  // Esperar 1 segundo entre mediciones
  delay(1000);
  // Obtener medicion de tiempo de viaje del sonido y guardar en variable uS
  int uS = sonarEntrada.ping_median();
  // Imprimir la distancia medida a la consola serial
  Serial.print("Distancia: ");
  // Calcular la distancia con base en una constante
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");

  //Si la distancia es menor que el ancho de la entrada, significa que una persona entró
  if(uS / US_ROUNDTRIP_CM < 90) {
    personas++;
  } 
  
  ThingSpeak.setField(1, personas); // En el campo 1 se le asigna la cantidad de personas en el autobus
}

void leerSalida() {
  int distance;
  long duration;

  // Esperar 1 segundo entre mediciones
  delay(1000);
  // Obtener medicion de tiempo de viaje del sonido y guardar en variable uS
  int uS = sonarEntrada.ping_median();
  // Imprimir la distancia medida a la consola serial
  Serial.print("Distancia: ");
  // Calcular la distancia con base en una constante
  Serial.print(uS / US_ROUNDTRIP_CM);
  Serial.println("cm");

  //Si la distancia es menor que el ancho de la salida, significa que una persona salió
  if(uS / US_ROUNDTRIP_CM < 90) {
    personas--;
  } 
  
  ThingSpeak.setField(1, personas); // En el campo 1 se le asigna la cantidad de personas en el autobus
}

void leerAsiento() {
  float reading = 0;
  if (balanza.is_ready())
  {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    float reading = balanza.get_units(10);
    Serial.print("Peso: ");
    Serial.print(reading);
    Serial.println();
    ThingSpeak.setField(2, reading); // En el campo 2 se le asigna la cantidad de personas en asientos preferenciales
  }
  else
    Serial.println("HX711 not found.");

  //TODO detectar cuándo se sienta y se para alguien en funcion del peso captado
  //TODO sumar y restar numero de asientos preferenciales ocupados
}
