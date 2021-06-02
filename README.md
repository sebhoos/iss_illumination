# ISS in Sight Illumination

This little project illuminates a LEGO ISS model according to the current position of the ISS. The idea was to start an running light at the LEGO ISS as soon as the ISS is at a location where it could be visible from the current position of the LEGO ISS model. 

In detail, four LEDs are illuminated orange when the ISS is not in sight. A running light (white which runs over the orange base color) occurs when the real ISS is close enough to the parametrized (e.g. own) position. All LEDs are turning pink if there is an issue with the network.


# How to Build it (Brief Description)

Some quick steps to get the lights on!

## Hardware
1. Connect APA102 LED strip (SPI) with [D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) (2x 2 LEDs)
2. Print LEGO compatible [LED Strip Holder](https://www.thingiverse.com/thing:4407300)

## Software
1. Install [VSCode](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/install/ide?install=vscode) 
3. Clone this repository with its submodules e.g. using `git clone --recurse-submodules https://github.com/sebhoos/iss_illumination.git`
4. Open this Project in VSCode / PlatformIO
5. Set custom parameters (see _Software Parametrization_ below)
6. Build
7. Upload build to D1 Mini
8. Done


# How to Build it (Detailed Description)

The following steps describe how to equip your own ISS with the illumination presented in the image above.


## Custom LEGO Compatible Bricks
For this project some custom LEGO compatible bricks have been designed and 3D printed to hold two small LED strips. Additionally a LEGO compatible brick to hold some cables has been designed. You can download the `.stl` files to print them by yourself from my Thingiverse account:
- [LEGO Compatible Cable Holder](https://www.thingiverse.com/thing:4434793)
- [LEGO Compatible LED Strip Holder](https://www.thingiverse.com/thing:4407300)


## Electronics and Wiring
As a control unit a [D1 Mini](https://www.wemos.cc/en/latest/d1/d1_mini.html) which contains an ESP8266 chip has been used. Other chips can be used as well but this description will focus on the D1 Mini.

As LED Strip, an SPI based APA102 LED strip has been used (60 LEDs/m). The LED Strip has been cut in a way, such that two LEDs are holded by one [LED Strip Holder](https://www.thingiverse.com/thing:4407300). The LED strips are wired in line - meaning that the output of the first LED strip is connected to the input of the second LED strip (5V to 5V, CL to CL etc.). The 5V and GND of the D1 Mini PCB are connected directly to the LED strip. Additionally, the `D5` and `D7` port are connected to the clock `CL` and the data input `DI` input of the first LED strip.

## Software
The software has been created using [VSCode](https://code.visualstudio.com/) and [PlatformIO](https://platformio.org/install/ide?install=vscode).

To easily deploy the code in this repository simply install [VSCode](https://code.visualstudio.com/) and within [VSCode](https://code.visualstudio.com/) install the [PlatformIO](https://platformio.org/install/ide?install=vscode) plugin. You might need to restart VSCode after installing the PlatformIO plugin. After that clone this repository e.g. using

``git clone --recurse-submodules https://github.com/sebhoos/iss_illumination.git``

and open the cloned repository using the `Open Project` button of the PlatformIO Plugin. Finally, click on the PlatformIO icon (alien head symbol) and click on `Build`. After building, click on `Upload` to flash the software on the `D1 Mini`.  

## Software Parametrization

Inside the main.cpp some parameters can be set as described in the table below.

| Parameter | Description |
|--- |--- |
| MAX_LED_BRIGHTNES | Max brightnes level of the LEDs [0,100]
| loops_until_next_poll | number of loops that are passing until the current iss location is requested from the server (one loop is 3 sec) |
| time_until_wifi_issue_is_illuminated | time [s] until the LEDs turn pink to signal a connection issue |
| max_sight_dist_to_iss | distance at which the LEDs are illuminated with a running light [m]|
| ssid | SSID of the used Wifi |
| password | Password of the used Wifi |
| own_longitude | Own GPS longitude (positive numbers mean N/E negative S/W) | 
own_latitude | Own GPS latitude (positive numbers mean N/E negative S/W) |



# Used Software

For this project the [FastLED](https://github.com/FastLED/FastLED) library and the [cJSON](https://github.com/DaveGamble/cJSON) library has been used and is cloned as a submodule when recursivly cloning this repository.

Additionally, the [Open Notify](http://open-notify.org/) API is used to evaluate the current position of the ISS.