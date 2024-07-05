#include <SoftwareSerial.h>
#include <ModbusRTU.h>
#include <DHT.h>

class HCSR04 {
  private:
    byte _trigPin;
    byte _echoPin;
    long _duration;
    int _distance;

  public:
    HCSR04(byte trigPin, byte echoPin) {
      _trigPin = trigPin;
      _echoPin = echoPin;
    }

    void begin() {
      pinMode(_trigPin, OUTPUT);
      pinMode(_echoPin, INPUT);
    }

    int readDistance() {
      digitalWrite(_trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(_trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(_trigPin, LOW);

      _duration = pulseIn(_echoPin, HIGH);
      _distance = _duration / 58;
      return _distance;
    }
};


// DHT22 config
#define DHTPIN 2
#define DHTTYPE DHT22

// software serial config
#define RX_pin 14
#define TX_pin 12
#define BAUD_RATE 9600
#define PARITY SWSERIAL_8N1

// modbus data address
#define SLAVE_ID 1
#define IReg_temp_address 1
#define IReg_hum_address 2
#define IReg_dist_address 3

//ultrasonic HCSR04 config
#define triggerPin 5
#define echoPin 4

HCSR04 ultrasonic(triggerPin, echoPin);
SoftwareSerial swSerial(RX_pin, TX_pin);
ModbusRTU modbus;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  ultrasonic.begin();
  
  swSerial.begin(BAUD_RATE, PARITY);
  modbus.begin(&swSerial);
  modbus.slave(SLAVE_ID);
  
  modbus.addIreg(IReg_temp_address);
  modbus.addIreg(IReg_hum_address);
  modbus.addIreg(IReg_dist_address);
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int distance = ultrasonic.readDistance();

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println(" C");

  Serial.print("Hum: ");
  Serial.print(hum);
  Serial.println(" %");

  Serial.print("Dist: ");
  Serial.print(distance);
  Serial.println(" Cm");
  Serial.println();

  modbus.Ireg(IReg_temp_address, temp * 10);
  modbus.Ireg(IReg_hum_address, hum * 10);
  modbus.Ireg(IReg_dist_address, distance);

  modbus.task();
  delay(1000);
}
