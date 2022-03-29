#include "HX711.h"
#include "NewPing.h"
const int LOADCELL_DOUT_PIN = 27;
const int LOADCELL_SCK_PIN = 14;

const int trigPin = 13;      // trigger pin
const int echoPin = 14;      // echo pin

HX711 balanza;
NewPing sonar(trigPin, echoPin);

void setup()
{  Serial.begin(115200);
   balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
   delay(250);

   balanza.set_scale(397);
   balanza.tare(10);  // Hacer 10 lecturas, el promedio es la tara
}

void loop()
{  if (balanza.is_ready())
     { float reading = balanza.get_units(10);
       Serial.print("Peso: ");
       Serial.print(reading);
       Serial.println();
     }
   else
       Serial.println("HX711 not found.");
   delay(5000);

  float distance = sonar.ping_median(5);
  Serial.print("Distance: ");
  Serial.print(distance, 1); Serial.println(" cm");
}
