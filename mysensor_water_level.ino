#define MY_RADIO_RF24
#define MY_DEBUG

// The temp and humidity sensor
#define DHT_PIN 4
#define DHT_TYPE DHT22

// the two "messages" we send
#define CHILD_ID_HUM 0
#define CHILD_ID_TEMP 1
#define CHILD_ID_WATER 8

#define WATERLEVEL_PIN 3

// the rf comms library
#include <SPI.h>
#include <MySensors.h>

// Adafruit unified sensor library
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

DHT_Unified dht(DHT_PIN, DHT_TYPE);


int seconds_between_messages = 1;
uint32_t delayMS = seconds_between_messages*1000;

MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgWater(CHILD_ID_WATER, V_TRIPPED);

void presentation() {
  sendSketchInfo("waterlevel", "1.1");
  present(CHILD_ID_HUM, S_HUM);
  present(CHILD_ID_TEMP, S_TEMP);
  present(CHILD_ID_WATER, S_MOISTURE);
}

void before() {
  dht.begin();

  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");
  Serial.println("------------------------------------");
}

bool is_wet, prev_wet;
int loop_count;

void loop() {
  Serial.print("loop count"); Serial.println(loop_count);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    Serial.print("Temperature: "); Serial.print(event.temperature); Serial.println(" *C");
//    send(msgTemp.set(event.temperature, 1));
  }

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    Serial.print("Humidity: "); Serial.print(event.relative_humidity); Serial.println("%");
//    send(msgHum.set(event.relative_humidity, 1));
  }


  int liquid_level=digitalRead(WATERLEVEL_PIN);
  Serial.print("Liquid_level= ");Serial.println(liquid_level,DEC);

  is_wet = (liquid_level == 1);
  

  if(is_wet != prev_wet || loop_count == 0) {
    if (is_wet) {
      Serial.println("*** WET!");
    }    
    send(msgWater.set(is_wet));
    Serial.println("");
  }
  
  prev_wet = is_wet;
  loop_count++;
  if (loop_count > 30)
    loop_count = 0;
  
  delay(delayMS);
}
