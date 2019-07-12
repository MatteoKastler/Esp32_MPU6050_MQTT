
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string.h>

const int MAX = 2000;
//wifi zeug
WiFiClient espClient;

//mqtt 
const char* mqttServer = "192.168.1.188";
const int mqttPort = 1883;
PubSubClient client(espClient);


MPU6050 mpu6050(Wire);

char AccX[1000], AccY[1000], AccZ[1000];
long timer = 0;

void wlan_connect(){
  for(int i = 0; i < 30; i++){
     Serial.println(); 
  }
  WiFi.begin("CompiCamp", "CompiCamp2019");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting");
    for (int i = 0; i < 3; i++) {
      delay(400);
      Serial.print(".");
    }
    Serial.println();
  }
  Serial.println();
  Serial.println("Success! connection to WLAN established"); 

}



void setup() {
  Serial.begin(9600);

  //wlan
  wlan_connect();

  //Mqtt
  client.setServer(mqttServer, mqttPort);
 
  //MPU
  Wire.begin(4,5);
  Wire.setClock(10000);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
}

void loop() {
  char Json[MAX];
   mpu6050.update();

  if (!client.connected()) {
    reconnect();
  }
    client.loop();
    
  mpu6050.update();
  if(millis() - timer > 10){
    gcvt(mpu6050.getAccX(),3,AccX);
    gcvt(mpu6050.getAccY(),3,AccY);
    gcvt(mpu6050.getAccZ(),3,AccZ);

    char * start = "{id:1, X: " ;char * Y = ", Y: "; char * Z = ", Z: "; char * ende = "}";
    
    strcat(Json,start);
    strcat(Json,AccX);
    strcat(Json,Y);
    strcat(Json,AccY);
    strcat(Json,Z);
    strcat(Json,AccZ);
    strcat(Json,ende);
    Serial.println(Json);
    Serial.println();
    client.publish("Accelerometer", Json);
    }
    memset(&Json[0], 0, sizeof(Json));
    /*Serial.print("gyroX : ");Serial.print(mpu6050.getGyroX());
    Serial.print("\tgyroY : ");Serial.print(mpu6050.getGyroY());
    Serial.print("\tgyroZ : ");Serial.println(mpu6050.getGyroZ());
  
    Serial.print("accAngleX : ");Serial.print(mpu6050.getAccAngleX());
    Serial.print("\taccAngleY : ");Serial.println(mpu6050.getAccAngleY());
  
    Serial.print("gyroAngleX : ");Serial.print(mpu6050.getGyroAngleX());
    Serial.print("\tgyroAngleY : ");Serial.print(mpu6050.getGyroAngleY());
    Serial.print("\tgyroAngleZ : ");Serial.println(mpu6050.getGyroAngleZ());
    
    Serial.print("angleX : ");Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : ");Serial.print(mpu6050.getAngleY());
    Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ());
    Serial.println("=======================================================\n");
    */timer = millis();
    
  }

void reconnect(){
  while(!client.connected()){
    Serial.println("reconnecting");
    
    if(client.connect("ESP")) {
 
      Serial.println("Success! MQTT client connected to server");
      break;
 
    }else{
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(1000);
 
    }
  }  
}
