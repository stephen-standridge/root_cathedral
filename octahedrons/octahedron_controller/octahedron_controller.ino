#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_DRV2605.h>

#include <OSCBundle.h>
#include <OSCMessage.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>

#define BNO055_SAMPLERATE_DELAY_MS (200)
Adafruit_BNO055 bno = Adafruit_BNO055(55);
Adafruit_DRV2605 drv;

const IPAddress outIp(10, 10, 1, 1);
const IPAddress inIp(10, 10, 1, 2);
const unsigned int outPort = 8888;
const unsigned int inPort = 9999;        // local port to listen for OSC packets (actually not used for sending)

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;

const char * initializedChannel = "/spatial/10_10_1_2/initialized";
const char * rebootingChannel = "/spatial/10_10_1_2/rebooting";
const char * bnoErrorChannel = "/spatial/10_10_1_2/error_bno";
const char * drvErrorChannel = "/spatial/10_10_1_2/error_drv";

const char * axChannel = "/spatial/10_10_1_2/ax";
const char * ayChannel = "/spatial/10_10_1_2/ay";
const char * azChannel = "/spatial/10_10_1_2/az";
//
const char * qxChannel = "/spatial/10_10_1_2/qx";
const char * qyChannel = "/spatial/10_10_1_2/qy";
const char * qzChannel = "/spatial/10_10_1_2/qz";
const char * qwChannel = "/spatial/10_10_1_2/qw";


bool bnoWorking = false;
bool drvWorking = false;
int prevMessage;
int currentVibration= 999;
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

OSCMessage ax_msg(axChannel);
OSCMessage ay_msg(ayChannel);
OSCMessage az_msg(azChannel);

OSCMessage qx_msg(qxChannel);
OSCMessage qy_msg(qyChannel);
OSCMessage qz_msg(qzChannel);
OSCMessage qw_msg(qwChannel);  

float acceleration = 0.0;

float oldQX = 0.0;
float oldQY = 0.0;
float oldQZ = 0.0;
float oldQW = 0.0;
float newQX = 0.0;
float newQY = 0.0;
float newQZ = 0.0;
float newQW = 0.0;

void publishOrientationAcceleration() {
  /* ACCELERATION */
    imu::Vector<3> acceleration = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    float ax = float(acceleration.x());
    float ay = float(acceleration.y());
    float az = float(acceleration.y());
    ax_msg.add(ax);
    ay_msg.add(ay);
    az_msg.add(az);
    float accel = abs(ax) + abs(ay) + abs(az); 
    accel = min((accel/20.), 1.0);
    
//  /* QUATERNION */
    imu::Quaternion quat = bno.getQuat();  
    newQX = float(quat.x());
    newQY = float(quat.y());
    newQZ = float(quat.z());
    newQW = float(quat.w());
    qx_msg.add(newQX);
    qy_msg.add(newQY);
    qz_msg.add(newQZ);
    qw_msg.add(newQW);
    float rotationalAccel = abs(oldQX - newQX) + abs(oldQY - newQY) + abs(oldQZ - newQZ);
    rotationalAccel = min(rotationalAccel, 1.0);
    
    Udp.beginPacket(outIp, outPort);
    ax_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    ax_msg.empty(); // free space occupied by message
    
    Udp.beginPacket(outIp, outPort);
    ay_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    ay_msg.empty(); // free space occupied by message
    
    Udp.beginPacket(outIp, outPort);
    az_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    az_msg.empty(); // free space occupied by message  

    Udp.beginPacket(outIp, outPort);
    qx_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    qx_msg.empty(); // free space occupied by message  

    Udp.beginPacket(outIp, outPort);
    qy_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    qy_msg.empty(); // free space occupied by message  

    Udp.beginPacket(outIp, outPort);
    qz_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    qz_msg.empty(); // free space occupied by message       

    Udp.beginPacket(outIp, outPort);
    qw_msg.send(Udp); // send the bytes to the SLIP stream
    Udp.endPacket(); // mark the end of the OSC Packet
    qw_msg.empty(); // free space occupied by message                    

    int vibration = round(((accel * .0) + (rotationalAccel * 1)) * 127);
    drv.setRealtimeValue(vibration);
    oldQX = newQX;
    oldQY = newQY;
    oldQZ = newQZ;
    oldQW = newQW;    
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
  Serial.print("Ethernet connected ");
  Serial.println(inIp);
  Serial.print("Starting UDP ");
  Serial.println(inPort);
  Udp.begin(inPort);

  /* Initialise the sensor */
  bnoWorking = bno.begin();
  if(!bnoWorking)
  {
    OSCMessage error_bno_msg(bnoErrorChannel);
//OSCMessage error_drv_msg(drvErrorChannel);
    error_bno_msg.add(1);
    Udp.beginPacket(outIp, outPort);
    error_bno_msg.send(Udp);
    Udp.endPacket();
    error_bno_msg.empty();
    delay(200);  
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.println("BNO055 not found");
  } else {
    Serial.println("BNO055 detected"); 
    Serial.println("");  
    /* Display some basic information on this sensor */
    displaySensorDetails();
  
    bno.setExtCrystalUse(true); 
  } 


  drv.begin();
  Serial.println("DRV detected");    
  Serial.println("");      
  drv.setMode(DRV2605_MODE_REALTIME);

  onClientInitialized();  
}

void reboot(OSCMessage &msg, int addrOffset) {
  if (int(msg.getFloat(0)) == 0) return;
  Serial.println("received");
  OSCBundle bndl;
  bndl.add(rebootingChannel).add(1);
  bndl.add(initializedChannel).add(0);
  bndl.add(bnoErrorChannel).add(0);
  
  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();   
  ESP.restart();
}

void loop() {

   OSCBundle bundleIN;
   int size = Udp.parsePacket();
   if( size>0)
   {
     while(size--)
       bundleIN.fill(Udp.read());
      if(!bundleIN.hasError()) {
        bundleIN.route("/spatial/10_10_1_2/reboot", reboot);
        bundleIN.route("/reboot", reboot); 
      }
      
      Udp.flush();
   }  

  if (bnoWorking) {
    publishOrientationAcceleration();        
  }
}
