# EE456  
# LoRa 1262 Project  

**Class:** EE456  
**Project Name:** LoRa GPS Network
**Contributors**: Christopher Tinker, Armin Behbahani, Landon Foster 

This project utilizes the SX1262 chip and Raspberry Pi to transmit and receive GPS data using LoRa and FSK modulation. Received data is subsequently sent to a remote server through a TCP link. 

This project requires 3 Raspberry Pi’s, 2 of which are equipped with LoRa/GPS hats. The first RPi reads NMEA data from the GPS module and transmits it to the second RPi over a LoRa connection. The second RPi receives the GPS data over LoRa, and sends it over TCP/IP networks to the third RPi, which is running the TCP server.


********************VERY IMPORTANT*****************  
- Enable SPI before attempting to transmit or receive.  
Run the following commands:  
`sudo raspi-config`  
Navigate to `Interfacing Options` -> `SPI` and enable it.  
Reboot your Raspberry Pi.  

The Raspberry Pi must be connected to WiFi for this project.
**********************************************************  

## Installation Instructions 

--Run  `sudo apt update && sudo apt install -y cmake git`  

1. Clone the GitHub Repository for EE456:  
`git clone https://github.com/delusionary/ee456-final.git`  
Navigate into the 'ee456-final' directory.  

2. Navigate to 'WiringPi' directory:  
Run:  
`./build`  
  
3. Navigate to 'RadioLib/examples/NonArduino/Raspberry/'  
Run:  
`mkdir build`  
`cd build`  
`cmake ..`  
`make`    

4.  lora_rx.cpp user **ONLY**: Navigate to `EE456/src/’
Run: 
`cd`
`cd EE456/src/`
`nano lora_rx.cpp’

First navigate to the line beginning #define PORT…
Ensure that the port on which the server and client will be communicating on are the same. If not, use a port range between the dynamic ports allocation range (49152-65535).

Navigate to line beginning:  if (inet_pton… 
The server’s IP address will be specified in the quotations. Replace it with the IP address of the device that will run server.cpp

Exit nano and save the code.


5. Make and build the Project:  
From EE456 directory run:  
`mkdir build`  
Navigate to the `build` directory:    
`cd build`  
Run:  
`cmake ../src`  **(cmake must be ran after any changes to CMakeLists.txt such as adding a new executable!)**  
Then run:  
`make` **(make must be ran after all changes to .cpp files)**   

## Using the code

1. **Server Receive Rasp Pi**:
Run the `server.cpp` code to confirm server start up without any error using:
`sudo ./server.cpp

2. **LoRa Receive Rasp Pi**:  
Run the `lora_rx.cpp` code to confirm initialization without any errors using:  
`sudo ./lora_rx`  

3. **LoRa Transmit Rasp Pi**:  
Run the `lora_tx.cpp` code to confirm transmission without errors using:  
`sudo ./lora_rx`  


## Additional Resources  

- [RadioLib Documentation](https://jgromes.github.io/RadioLib/index.html)  
- [RadioLib GitHub Repository](https://github.com/jgromes/RadioLib)  
- [WiringPi Pinout](https://pinout.xyz/pinout/wiringpi)  
- [WiringPi GitHub Repository](https://github.com/WiringPi/WiringPi)  
- [Waveshare SX1262 LoRaWAN Hat](https://www.waveshare.com/sx1262-lorawan-hat.htm)  
- [Semtech SX1262 Product Page](https://www.semtech.com/products/wireless-rf/lora-connect/sx1262)  


