#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <OSCBundle.h>
#include <OSCMessage.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>


#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55);

const IPAddress outIp(10, 10, 1, 1);
const IPAddress inIp(10, 10, 1, 2);
const unsigned int outPort = 8888;
const unsigned int inPort = 9999;        // local port to listen for OSC packets (actually not used for sending)
const String channelIdentifier = "/spatial/10_10_1_2";

int delayval = 10; // delay for half a second


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";        // a string to send back

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

const char * initializedChannel = "/spatial/10_10_1_2/initialized";
const char * rebootingChannel = "/spatial/10_10_1_2/rebooting";
const char * pingChannel = "/spatial/10_10_1_2/ping";
const char * bnoErrorChannel = "/spatial/10_10_1_2/error_bno";

const char * axChannel = "/spatial/10_10_1_2/ax";
const char * ayChannel = "/spatial/10_10_1_2/ay";
const char * azChannel = "/spatial/10_10_1_2/az";

const char * qxChannel = "/spatial/10_10_1_2/qx";
const char * qyChannel = "/spatial/10_10_1_2/qy";
const char * qzChannel = "/spatial/10_10_1_2/qz";
const char * qwChannel = "/spatial/10_10_1_2/qw";

OSCMessage error_bno_msg(bnoErrorChannel);
OSCMessage ping_msg(pingChannel);

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
  OSCBundle bndl;
  /* ACCELERATION */
  imu::Vector<3> acceleration = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  bndl.add(axChannel).add(float(acceleration.x()));
  bndl.add(ayChannel).add(float(acceleration.y()));
  bndl.add(azChannel).add(float(acceleration.z()));

  /* QUATERNION */
  imu::Quaternion quat = bno.getQuat();  

  bndl.add(qxChannel).add(float(quat.x()));
  bndl.add(qyChannel).add(float(quat.y()));
  bndl.add(qzChannel).add(float(quat.z()));
  bndl.add(qwChannel).add(float(quat.w()));
  
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();
  
  delay(BNO055_SAMPLERATE_DELAY_MS);
}

void onClientInitialized() {
  /* Send the orchestrator initial values */
  OSCBundle bndl;
  
  bndl.add(axChannel).add(0);
  bndl.add(axChannel).add(0);
  bndl.add(azChannel).add(0);

  bndl.add(qxChannel).add(0);
  bndl.add(qyChannel).add(0);
  bndl.add(qzChannel).add(0);
  bndl.add(qwChannel).add(0);


  bndl.add(initializedChannel).add(1);
  bndl.add(rebootingChannel).add(0);
  bndl.add(pingChannel).add(0);
  
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();   
  delay(1000); 
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Ethernet.init(33);  
  Ethernet.begin(mac, inIp);

  Serial.println();
  Serial.println("Ethernet connected");
  Serial.println("Starting UDP");
  Udp.begin(inPort);

  /* Initialise the sensor */
  if(!bno.begin())
  {
    error_bno_msg.add(1);
    Udp.beginPacket(outIp, outPort);
    error_bno_msg.send(Udp);
    Udp.endPacket();
    error_bno_msg.empty();
    delay(200);  
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
  onClientInitialized();  
}

int prevMessage;

void pong(OSCMessage &msg, int addrOffset) {
  int newMessage = int(msg.getFloat(0));
  if (newMessage == prevMessage) return;
  
  Serial.print("received message ");
  Serial.println(newMessage);
  ping_msg.add(newMessage);
  Udp.beginPacket(outIp, outPort);
  ping_msg.send(Udp);
  Udp.endPacket();
  ping_msg.empty();
  prevMessage = newMessage;
  delay(200); 
}

void reboot(OSCMessage &msg, int addrOffset) {
  if (int(msg.getFloat(0)) == 0) return;
  OSCBundle bndl;
  bndl.add(rebootingChannel).add(1);
  bndl.add(initializedChannel).add(0);
  bndl.add(pingChannel).add(0);
  bndl.add(bnoErrorChannel).add(0);
  
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();   
  ESP.restart();
}

void loop() {
   OSCBundle bundleIN;
   int size;
 
   if( (size = Udp.parsePacket())>0)
   {
     while(size--)
       bundleIN.fill(Udp.read());
      Serial.println("receiving");
      if(!bundleIN.hasError()) {
        bundleIN.route("/reboot", reboot);
        bundleIN.route("/ping", pong);
      }
   }  

  publishOrientationAcceleration();    
  
}
