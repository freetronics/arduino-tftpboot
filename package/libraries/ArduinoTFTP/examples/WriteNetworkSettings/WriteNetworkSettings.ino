/*

   WriteNetworkSettings

   This sketch writes a set of network configuration settings to EEPROM
   in such a way that both the `arduino-tftpboot` bootloader & a sketch
   can read them back later and share the same settings.

   As commented below these are the settings available for modification:

     * Gateway IP address
     * Subnet mask
     * MAC address
     * Arduino device IP address

   Once the settings have been written to EEPROM, an LED on pin 9
   (if connected) will flash rapdily.
   
   
   The default settings used by the bootloader:
   
      192,168,1,254,                       // Gateway IP address
      255,255,255,0,                       // Subnet mask
      0x12,0x34,0x45,0x78,0x9A,0xBC,       // MAC address
      192,168,1,1,                         // Arduino device IP address

   
   Settings suitable for link-local use:
   
      169,254,1,1,                         // Gateway IP address
      255,255,0,0,                         // Subnet mask
      0x12,0x34,0x45,0x78,0x9A,0xBC,       // MAC address
      169,254,254,169,                     // Arduino device IP address
   
 */

#include <EEPROM.h>

// TODO: Allow settings to be specified as individual variables.
// TODO: Wrap this all in a library.

uint8_t NetworkSettings[] = {
  0x55, 0xAA, // Signature
  0xFF, // Image Status (bad)
  
  // EEPROM block starts here
  10,1,1,1,                       // Gateway IP address
  255,0,0,0,                      // Subnet mask
  0x12,0x34,0x45,0x78,0x9A,0xBC,  // MAC address
  10,1,1,20,                      // Arduino device IP address
  // EEPROM block ends here
};


void setup() {
  for (int address = 0; address < sizeof(NetworkSettings)/sizeof(uint8_t); address++) {
    EEPROM.write(address, NetworkSettings[address]);
  }
  
  pinMode(9, OUTPUT);     
}


void loop() {
  // Blink an LED on pin 9 rapidly if present
  // to indicate the settings have been stored in EEPROM.
  digitalWrite(9, HIGH);
  delay(100);
  digitalWrite(9, LOW);
  delay(100);
}
