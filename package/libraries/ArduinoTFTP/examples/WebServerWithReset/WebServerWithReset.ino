/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>

#include <EEPROM.h>

#include <avr/wdt.h>


// Enter a MAC address for your controller below--it must match the one the bootloader uses.
byte mac[] = {0x12,0x34,0x45,0x78,0x9A,0xBC};

// The IP address will be read from EEPROM or be set to a value default in `configureNetwork()`.
IPAddress ip;

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

// Connections to this port will cause a reset to the bootloader
// so a sketch can be uploaded.
EthernetServer reset_server(81);


void configureNetwork() {
  // Reads IP address from EEPROM as stored by `WriteNetworkSettings` sketch.
  
#define EEPROM_SIG_1_VALUE 0x55
#define EEPROM_SIG_2_VALUE 0xAA

#define EEPROM_SIG_1_OFFSET 0
#define EEPROM_SIG_2_OFFSET 1

#define EEPROM_GATEWAY_OFFSET 3
#define EEPROM_MASK_OFFSET 7
#define EEPROM_MAC_OFFSET 11
#define EEPROM_IP_OFFSET 17
  
  if ((EEPROM.read(EEPROM_SIG_1_OFFSET) == EEPROM_SIG_1_VALUE)
       && (EEPROM.read(EEPROM_SIG_2_OFFSET) == EEPROM_SIG_2_VALUE)) {
         ip = IPAddress(EEPROM.read(EEPROM_IP_OFFSET),
                        EEPROM.read(EEPROM_IP_OFFSET+1),
                        EEPROM.read(EEPROM_IP_OFFSET+2),
                        EEPROM.read(EEPROM_IP_OFFSET+3));  
       } else {
         ip = IPAddress(192,168,1,1);
       };
       
   // TODO: Handle MAC, mask & gateway also.    
}

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  configureNetwork();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  
  // Check for a connection that indicates we should reset to the bootloader
  // so another sketch can be uploaded.
  // Note: This approach means that if the sketch hangs later in the loop then any connection
  //       attempt will not be detected so reset will not occur. So, don't hang. :)
  EthernetClient reset_client = reset_server.available(); // Unfortunately requires a byte sent.
  
  if (reset_client) {
    reset_client.stop();
    
    wdt_disable();  
    wdt_enable(WDTO_2S);
    while (1);
  }
  
  
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connnection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
          client.println("<meta http-equiv=\"refresh\" content=\"5\">");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

