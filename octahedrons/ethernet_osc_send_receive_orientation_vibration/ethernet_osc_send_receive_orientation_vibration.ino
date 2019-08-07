#include <Wire.h>
#include <Adafruit_DRV2605.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <OSCBundle.h>
#include <OSCMessage.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>

Adafruit_DRV2605 drv;

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

const IPAddress outIp(10, 10, 1, 1);
const IPAddress inIp(10, 10, 1, 2);
const unsigned int outPort = 8888;
const unsigned int inPort = 9999;        // local port to listen for OSC packets (actually not used for sending)
int delayval = 10; // delay for half a second


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void publishOrientationAcceleration() {
  /* Get a new sensor event */
  imu::Vector<3> acceleration = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  OSCMessage msg("/feeds/acceleration_quaternion");
  msg.add(float(acceleration.x()));
  msg.add(float(acceleration.y()));
  msg.add(float(acceleration.z()));
  if (false) {
      Serial.print("x: ");
      Serial.print(float(acceleration.x()));
      Serial.print("\ty: ");
      Serial.print(float(acceleration.y()));
      Serial.print("\tz: ");
      Serial.print(float(acceleration.z()));
      Serial.println("");  
  }  
  
  imu::Quaternion quat = bno.getQuat();  
  msg.add(float(quat.x()));
  msg.add(float(quat.y()));
  msg.add(float(quat.z()));
  msg.add(float(quat.w()));    
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();  
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  Ethernet.begin(mac, inIp);
  delay(10);

  Serial.println();
  Serial.println("Ethernet connected");
  Serial.println("Starting UDP");
  Udp.begin(inPort);

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  } else {
    Serial.println("BNO055 detected"); Serial.println("");    
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  bno.setExtCrystalUse(true);   
//  Serial.println("DRV test");
//  drv.begin();
//  
//  drv.selectLibrary(1);
//  
//  // I2C trigger by sending 'go' command 
//  // default, internal trigger when sending GO command
//  drv.setMode(DRV2605_MODE_INTTRIG); 
//  drv.setWaveform(83, 0);  // play effect 
//  drv.setWaveform(70, 1);  // play effect 
//  drv.setWaveform(0, 2);       // end waveform

}

uint8_t effect = 1;

void changeVibration(OSCMessage &msg, int addrOffset ){
  for (int i=0; i<msg.size(); i++) {
    if (msg.isInt(i)) {
      Serial.println(msg.getInt(i));
      if (msg.getInt(i) == 1) {
        drv.setWaveform(0, 83);  // play effect 
        drv.setWaveform(1, 70);  // play effect 
        drv.setWaveform(2, 0);       // end waveform
      }
      if (msg.getInt(i) == 0) {
        drv.setWaveform(0, 0);       // end waveform
        // drv.setWaveform(0, 0);       // end waveform
      }    
      drv.go();  
    }
  }

}

void loop() {


//  Serial.print("Effect #"); Serial.println(effect);
//
//  // set the effect to play
//  drv.setWaveform(0, effect);  // play effect 
//  drv.setWaveform(1, 0);       // end waveform
//
//  // play the effect!
//  drv.go();
//
//  // wait a bit
//  delay(500);
//
//  effect++;
//  if (effect > 117) effect = 1;
   OSCBundle bundleIN;
   int size;
 
   if( (size = Udp.parsePacket())>0)
   {
     while(size--)
       bundleIN.fill(Udp.read());

      if(!bundleIN.hasError())
        bundleIN.route("/vibrate", changeVibration);
   }  

   publishOrientationAcceleration();
   delay(BNO055_SAMPLERATE_DELAY_MS);
}
