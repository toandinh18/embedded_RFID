#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
  
const char *ssid = "GATSBY";  //ENTER YOUR WIFI SETTINGS
const char *password = "12356789";
const char *host = "rfidembedded.000webhostapp.com";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
const char fingerprint[] PROGMEM = "42e9f5f9302114cd75a141ef3933e8d4c797b97c";

String getUIDs;
String getTimes;

volatile int i=0;
int k=0;

void setup() {
  delay(1000);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password); //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  
}

void HTTPS_Send_Data(void);
void UART_Receive(void);

//=======================================================================
//                    Main Program Loop
//=======================================================================

void loop() {
  UART_Receive();
  if(k){
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print(getUIDs);
    Serial.print("\n");
    Serial.print(getTimes);
    Serial.print("\n");
    delay(500);
    HTTPS_Send_Data(getUIDs,getTimes);
    k=0;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
  }                    
}

///////////////////////////////////////////////
void UART_Receive(void) {
  while (Serial.available()) {
    digitalWrite(LED_BUILTIN, LOW);
    if(i==0) {  
      getUIDs=String(Serial.read());
      i++;
    }
    else {
      if(i<5){
        getUIDs+=":"+String(Serial.read());
        i++;
      }
      else {
        if (i==5) {
          getTimes= String (Serial.read());
          i++;
        }
        else {
          if (i<10) {
            getTimes+=":" + String(Serial.read());
            i++;
          }
          else {
            getTimes+=":" + String(Serial.read());
            i=0;
            k=1;
          } 
        } 
      } 
    }
  }
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
}

/////////////////////////////////////////////
void HTTPS_Send_Data(String getUID, String getTime) {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 10)){
      delay(50);
      Serial.print(".");
      r++;
  }
  if(r==10) {
    Serial.println("Connection failed");
    int k;
    for (k=0;k<10;k++) {
      digitalWrite(LED_BUILTIN, LOW);
      delay(30);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(30);
    }
  }
  else {
    Serial.println("Connected to web");
  }
  
  String  Link;
  Link = "/api/getdata?UID=" +getUID +"&Time=" +getTime;

  Serial.print("requesting URL: ");
  Serial.println(host+Link);

  httpsClient.print(String("GET ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
    
  delay(1000);  
}
