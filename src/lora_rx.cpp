#include <iostream>
#include <fstream>
#include <RadioLib.h>
#include "PiHal.h"
#include <cstdio>
#include <chrono>

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// NSS pin: WiringPi 10 (GPIO 8) WPI # 29 GPIO 21 for Pi hat
// DIO1 pin: WiringPi 2 (GPIO 27) WPI # 27 GPIO 16 for Pi hat
// NRST pin: WiringPi 21 (GPIO 5) WPI #1 GPIO 18
// BUSY pin: WiringPi 0 (GPIO 17) WPI #28 GPIO 20

// Create the radio module instance
Module* module = new Module(hal, 29, 27, 1, 28);
SX1262 radio(module);

// Flag to indicate a packet was received
volatile bool receivedFlag = false;
int packetNumber = 0; // Packet counter

// ISR function to set the received flag
void setFlag(void) {
  receivedFlag = true;
}

int main(int argc, char** argv) {
  hal->init();
  // freopen("debug.txt", "w", stdout);

  // Open files for writing
  std::ofstream payloadFile("payload_received.txt", std::ios::out | std::ios::binary);
  std::ofstream dataFile("data.txt", std::ios::out);

  if (!payloadFile.is_open() || !dataFile.is_open()) {
    printf("Error opening files!\n");
    return 1;
  }

  int16_t resetState = radio.reset();
  if (resetState != RADIOLIB_ERR_NONE) {
    printf("Reset failed, code %d\n", resetState);
    return 1;
  }

  radio.XTAL = true;

  // Initialize the radio module
  printf("[SX1262] Initializing ... ");
  int state = radio.begin(915.0, 125.0, 11, 8, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 22, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // Set the ISR for packet received
  radio.setPacketReceivedAction(setFlag);

  // Start receiving packets
  printf("[SX1262] Starting to listen ... ");
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // Main loop with timeout
  uint8_t buff[256] = { 0 };
  auto lastReceivedTime = std::chrono::steady_clock::now();

  for(;;) {
    if(receivedFlag) {
      // Reset flag
      receivedFlag = false;

      // Increment packet counter
      packetNumber++;

      // Get the length of the received packet
      size_t len = radio.getPacketLength();

      // Read the received packet
      state = radio.readData(buff, len);
      if (state != RADIOLIB_ERR_NONE) {
        printf("Read failed, code %d\n", state);
      } else {
        // Write payload to file
        payloadFile.write(reinterpret_cast<char*>(buff), len);

        // Get RSSI
        float rssi = radio.getRSSI();
        printf("RSSI: %.2f dBm\n", rssi);

        // Get SNR 
        float snr = radio.getSNR();
        printf("SNR: %.2f dB\n", snr);

        // Write packet info to data file
        dataFile << "Packet #" << packetNumber << ": RSSI = " << rssi << " dBm, SNR = " << snr << " dB\n";
      }

      // Update last received time
      lastReceivedTime = std::chrono::steady_clock::now();

      // Restart the receiver
      state = radio.startReceive();
      if (state != RADIOLIB_ERR_NONE) {
        printf("Restart receive failed, code %d\n", state);
        return 1;
      }
    }

    // Check for timeout (20 seconds)
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastReceivedTime);
    if(duration.count() >= 20) {
      printf("No packet received for 20 seconds. Exiting...\n");
      break;
    }
  }

  // Clean up and close files
  payloadFile.close();
  dataFile.close();

  printf("Program closed properly.\n");
  return(0);
}