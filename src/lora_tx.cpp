#include <RadioLib.h>
#include <fstream>
#include <vector>
#include <iostream>
#include "PiHal.h"
#include <termios.h>
#include <unistd.h>

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0);

// Create the radio module instance
// Pinout corresponds to your SX1262 setup
// NSS pin: WiringPi 10 (GPIO 8)
// DIO1 pin: WiringPi 2 (GPIO 27)
// NRST pin: WiringPi 21 (GPIO 5)
// BUSY pin: WiringPi 0 (GPIO 17)
Module* module = new Module(hal, 10, 2, 21, 0);
SX1262 radio(module);

int main() {
  // Set test parameters
  float BW = 125.0; // Bandwidth
  int SF = 7;       // Spreading Factor
  int CR = 5;       // Coding Rate

  int state;

  // Open the file in binary mode
  std::ifstream file("msg.txt", std::ios::binary);

  // Make sure file opening worked
  if (!file.is_open()) {
    printf("Failed to open file!\n");
    return 1;
  }

  // Read the entire file into a vector of bytes
  std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();

  // Initialize radio with the set parameters
  state = radio.begin(915.0, BW, SF, CR, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("Initialization failed, code %d\n", state);
    return 1;
  }

  // Packetize and transmit the file data
  size_t packetSize = 255;  // Maximum packet size
  size_t totalPackets = (fileData.size() + packetSize - 1) / packetSize;  // Calculate the number of packets required

  for (size_t i = 0; i < totalPackets; ++i) {
    size_t startIdx = i * packetSize;
    size_t endIdx = std::min(startIdx + packetSize, fileData.size());
    
    // Create a packet from the slice of the file data
    std::vector<uint8_t> packet(fileData.begin() + startIdx, fileData.begin() + endIdx);

    // Transmit the packet
    state = radio.transmit(packet.data(), packet.size());
    if (state == RADIOLIB_ERR_NONE) {
      printf("Packet %lu/%lu Transmission success!\n", i + 1, totalPackets);
    } else {
      printf("Packet %lu/%lu Transmission failed, code %d\n", i + 1, totalPackets, state);
    }

    hal->delay(1000); // 1 second delay before transmitting the next packet
  }

  return 0;
}
