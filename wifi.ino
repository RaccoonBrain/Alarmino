#include <ESP8266WiFi.h>


//wifi
const char* ssid = "FERIA-EIT";
const char* password = "EIT.2022";

const char* host = "192.168.1.110"; // cmd -> ipconfig
const uint16_t port = 80;



// RX msg
bool received = false;
String receivedMsg;

// variables to control
String s_son;
String s_humo;
String s_mov;
const byte numVars = 3;

//                          0      1        3
String controlledVars[] = {s_son, s_humo, s_mov};

// variables float
float son = 0.0;
float humo = 0.0;
float mov = 0.0;


void setup(){
  
  Serial.begin(9600);

// We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
} // EOF setup

void loop()
{  
  checkSerialCom();
  if (received){
    ProcessMsg();
    receivedMsg = "";
    received = false;
    ConvertVariables();
    delay(50);
    // attempt to send data to host
    SendDataToHost(); 
  }

  //delay(300000); // execute once every 5 minutes
  delay(3000);
} // EOF loop


/** Helper Functions ****/
void checkSerialCom()
{
  char data;
  if (Serial.available() > 0)
  { // Checks whether data is comming from the serial port
    while (Serial.available() > 0)
    {
      data = (char)Serial.read(); // Reads the data from the serial port
      receivedMsg += data;        // Para la segunda parte del vídeo
    }
    received = true;
  }
} // EOF checkSerialCom


void ProcessMsg(){
  receivedMsg.trim();
  Serial.println("Recibido: ");
  Serial.println(receivedMsg);
  int index;
  for (size_t i = 0; i < numVars; i++)
  {
    index = receivedMsg.indexOf('_');
    controlledVars[i] = receivedMsg.substring(0, index);
    receivedMsg = receivedMsg.substring(index + 1);
    // Serial.print("=> ");
    // Serial.println(controlledVars[i]);
  }
  s_son = controlledVars[0];
  s_humo = controlledVars[1];
  s_mov = controlledVars[2];
} // EOF ProcessMsg


void ConvertVariables(){
  son = s_son.toFloat();
  humo = s_humo.toFloat();
  mov = s_mov.toFloat();
  // if conversion not possible 0 is returned
}// EOF ConvertVariables


void SendDataToHost(){
  Serial.print(">>>>>>> connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  String query = "esp8266/save.php?son=";//OJO CARPETAS DENTRO DE XAMPP
  query += son;
  query += "&humo=";
  query += humo;
  query += "&mov=";
  query += mov;
  
  Serial.println(query);

  Serial.println("[Sending a request]");
  client.print(String("GET /") + query + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n" +
               "\r\n"
              );

  // wait for data to be available
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      delay(3000);
      return;
    }
  }

  // Read all the lines of the reply from server
  Serial.println("receiving from remote server");
  String msg;
  while (client.available()) {
    char ch = static_cast<char>(client.read());
    Serial.print(ch); // print the reply
    msg += ch;
  }
  
  Serial.println();
  if (msg.indexOf("Guardado correcto!") != -1){
    Serial.println("Data Saved");
  }
  else{
    Serial.println("Could not save data");
  }

  // Close the connection
  Serial.println();
  Serial.println(">>>>>>>>>> closing connection");
  Serial.println();
  client.stop();
} // EOF SendDataToHOST
