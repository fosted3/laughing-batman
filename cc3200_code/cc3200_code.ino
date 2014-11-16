
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

// your network name also called SSID
char ssid[] = "hacktheplanet";
// your network password
char password[] = "hackrpi2014";
// your network key Index number (needed only for WEP)
int keyIndex = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);  // initialize serial communication
  Serial1.begin(9600);
  pinMode(RED_LED, OUTPUT);      // set the LED pin mode

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  
  // you're connected now, so print out the status  
  printWifiStatus();

  Serial.println("Starting webserver on port 80");
  server.begin();                           // start the web server on port 80
  Serial.println("Webserver started!");
}

void loop() {
  int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    char buffer[150] = {0};                 // make a buffer to hold incoming data
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (strlen(buffer) == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<html><head><title>Smart Lights</title></head><body align=center>\n<h1 align=center><font color=\"red\">Welcome to Smart Lights</font></h1>");
            client.println("<p>Lighting Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Temperature: <input type=\"text\" name=\"temp\"> (input value between 1000-4000)<br>Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>RGB Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Red: <input type=\"text\" name=\"r\"> (input value between 0-255)<br>Green: <input type=\"text\" name=\"g\"> (input value between 0-255)<br>blue: <input type=\"text\" name=\"b\"> (input value between 0-255) <br>Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>Random Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>Plasma Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Brightness: <input type=\"text\" name=\"plasma\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            //client.print("RED LED <button onclick=\"location.href='/H'\">HIGH</button>");
            //client.println(" <button onclick=\"location.href='/L'\">LOW</button><br>");
            

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          buffer[i++] = c;      // add it to the end of the currentLine
        }
        
        
        // Check to see if the client request was "GET /H" or "GET /L":
        if (endsWith(buffer, "HTTP/1.1")) {
          String temp = buffer;
          int tempLight;
          int stopstuff;
          int brightness;
          int stopstuff2;
          int red;
          int green;
          int blue;
          int stopstuff3;
          int stopstuff4;
          if(temp[6] == 't'){
              //lighting mode
              tempLight = temp.indexOf('=')+1;
              stopstuff = temp.indexOf('&');
              brightness = temp.indexOf('=', stopstuff)+1;
              stopstuff2 = temp.indexOf(' ', stopstuff);
              Serial1.print("lighting ");
              Serial1.print(temp.substring(tempLight, stopstuff));
              Serial1.print(" ");
              Serial1.println(temp.substring(brightness, stopstuff2));
          }
          if(temp[6] == 'r'){
              //RGB mode
              red = temp.indexOf('=')+1;
              stopstuff = temp.indexOf('&');
              green = temp.indexOf('=', stopstuff)+1;
              stopstuff2 = temp.indexOf('&', stopstuff+1);
              blue = temp.indexOf('=', stopstuff2)+1;
              stopstuff3 = temp.indexOf('&', stopstuff2+1);
              brightness = temp.indexOf('=', stopstuff3)+1;
              stopstuff4 = temp.indexOf(' ', stopstuff3+1);
              Serial1.print("rgb ");
              Serial1.print(temp.substring(red, stopstuff));
              Serial1.print(" ");
              Serial1.print(temp.substring(green, stopstuff2));
              Serial1.print(" ");
              Serial1.print(temp.substring(blue, stopstuff3));
              Serial1.print(" ");
              Serial1.println(temp.substring(brightness, stopstuff4));
              
              
          }
          if(temp[6] == 'b'){
            //random brightness mode
            brightness = temp.indexOf('=')+1;
            stopstuff = temp.indexOf(' ', 6);
            Serial1.print("random ");
            Serial1.println(temp.substring(brightness, stopstuff));
          }
          if(temp[6] == 'p'){
            //random brightness mode
            brightness = temp.indexOf('=')+1;
            stopstuff = temp.indexOf(' ', 6);
            Serial1.print("plasma ");
            Serial1.println(temp.substring(brightness, stopstuff));
          }
          
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

//
//a way to check if one array ends with another array
//
boolean endsWith(char* inString, char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
