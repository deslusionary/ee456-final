#include <RadioLib.h>
#include "PiHal.h"
#include <chrono> // For timestamping

// Create a new instance of the HAL class
PiHal* hal = new PiHal(0); // 0 for SPI 0 , set to 1 if using SPI 1(this will change NSS pinout)

// Create the radio module instance/////////////////////////
// Pinout *****MBED SHIELD****************PI HAT************
// NSS pin:  WPI# 10 (GPIO 8)  WPI # 29 (GPIO 21) for Pi hat
// DIO1 pin: WPI# 2  (GPIO 27) WPI # 27 (GPIO 16) for Pi hat
// NRST pin: WPI# 21 (GPIO 5)  WPI # 1  (GPIO 18) for Pi hat
// BUSY pin: WPI# 0  (GPIO 17) WPI # 28 (GPIO 20) for Pi hat
////////////////////////////////////////////////////////////

// Radio initialization based on Pi Hat wiring
// change for MBED Shield use
// According to SX1262 radio = new Module(hal, NSS,DI01,NRST,BUSY)
SX1262 radio = new Module(hal, 29, 27, 1, 28);


int main() {
    // Initialize the radio module
    printf("[SX1262] Initializing ... ");
    int state = radio.begin(915.0, 62.5, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0.0, false);
    if (state != RADIOLIB_ERR_NONE) {
        printf("Initialization failed, code %d\n", state);
        return 1;
    }
    printf("Initialization success!\n");

    int count = 0;

    while (true) {
        // Get the current time 
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        // Print timestamp and packet number
        printf("[SX1262] Transmitting packet #%d at time %lld ms ... ", count, timestamp);

        // Create a packet with the timestamp and message
        char str[64];
        sprintf(str, "Timestamp: %lld, Hello World! #%d", timestamp, count++);

        // Send the packet
        state = radio.transmit(str);
        if (state == RADIOLIB_ERR_NONE) {
            printf("success!\n");

            // Get and print the effective data rate for the transmitted packet
            float effectiveDataRate = radio.getDataRate();
            printf("Effective Data Rate: %.2f bps\n", effectiveDataRate);
        } else {
            printf("failed, code %d\n", state);
        }

        hal->delay(1000); // delay 1 second
    }

    return 0;
}