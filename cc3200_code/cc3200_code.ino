
#ifndef __CC3200R1M1RGC__
// Do not include SPI for CC3200 LaunchPad
#include <SPI.h>
#endif
#include <WiFi.h>

long count = 0;
boolean active = false;
int active_brightness = 0;
// your network name also called SSID
char ssid[] = "hacktheplanet";
// your network password
char password[] = "hackrpi2014";
// your network key Index number (needed only for WEP)
int keyIndex = 0;

unsigned int localPort = 2390;      // local port to listen for UDP packets

IPAddress timeServer(216,229,0,179); // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;



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
  
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
}

void loop() {
  int i = 0;
  WiFiClient client = server.available();   // listen for incoming clients
  
  delay(10);
  count++;
  if(count >= 30000 && active){//30000
    Serial.println("Updating time");
    boolean updated = false;
    do
    {
    sendNTPpacket(timeServer); // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
    if ( Udp.parsePacket() ) {
      updated = true;
    Serial.println("packet received");
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:

    // print the hour, minute and second:
    Serial.print("The EST time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print(((epoch  % 86400L)-5*3600) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    float fluxtime = ((epoch  % 86400L)-5*3600) / 3600 + (epoch  % 3600) / 3600.0;
    Serial.println(fluxtime);
    Serial1.print("lighting ");
    Serial1.print(time_to_temp(fluxtime));
    Serial1.print(" ");
    Serial1.println(active_brightness);
    }
    else
    {
      Serial.println("Failed to update time");
    }
    } while (!updated);
    count = 0;
  }
  
  if (client) {     // if you get a client,
    boolean sent_command = false;
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
            client.println("<p>Lighting Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Temperature: <input type=\"text\" name=\"temp\"> (input value between 1000-40000)<br>Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>RGB Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Red: <input type=\"text\" name=\"r\"> (input value between 0-255)<br>Green: <input type=\"text\" name=\"g\"> (input value between 0-255)<br>blue: <input type=\"text\" name=\"b\"> (input value between 0-255) <br>Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>Random Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Brightness: <input type=\"text\" name=\"brig\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>\n<p>Plasma Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Brightness: <input type=\"text\" name=\"plasma\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
            client.println("<p>Active Mode</p><form action=\"\" method=\"get\" target=\"_blank\">Brightness: <input type=\"text\" name=\"active\"> (input value between 0-100) <br><input type=\"submit\" value=\"Submit\"></form>");
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
        if (endsWith(buffer, "HTTP/1.1") && sent_command == false) {
          sent_command = true;
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
              active = false;
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
              active = false;
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
            active = false;
            brightness = temp.indexOf('=')+1;
            stopstuff = temp.indexOf(' ', 6);
            Serial1.print("random ");
            Serial1.println(temp.substring(brightness, stopstuff));
          }
          if(temp[6] == 'p'){
            //random brightness mode
            active = false;
            brightness = temp.indexOf('=')+1;
            stopstuff = temp.indexOf(' ', 6);
            Serial1.print("plasma ");
            Serial1.println(temp.substring(brightness, stopstuff));
          }
          if(temp[6] == 'a'){
            //Active mode
            active = true;
            count = 30001; //CHANGE
            brightness = temp.indexOf('=')+1;
            stopstuff = temp.indexOf(' ', 6);
            active_brightness = temp.substring(brightness, stopstuff).toInt();
            //Serial.println(active_brightness);
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

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
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

unsigned int time_to_temp(float t){
return (unsigned int) 3200*pow(((1+cos((3.14159*(t-12)/12)))/2),1.5)+3000;
}
