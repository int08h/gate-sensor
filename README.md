# Overview

Implementation of an ESP32 based sensor for detecting open/close condition of a Hall-effect sensor 
and communicating events via Wifi to Google Cloud IoT Core.

This is very much a "scratch your own itch" type project and is not intended for wide use. 

# Shout-outs and resources

* Big thank you to [@TvE](https://github.com/tve) for the amazing 
[Running Wifi Microcontrollers on Battery](https://blog.voneicken.com/projects/low-power-wifi-intro/)
blog series. 

# Tested ESP32 boards

Selection criteria

* Low deep-sleep power consumption on battery - needs to run for 45+ days on a 3,000 mAh 18650 battery.
* Development convenience - easy to use, integrated battery connector, no bugs/annoyances. 

## [Adafruit Feather HUZZAH32](https://www.adafruit.com/product/3405)

WROOM-32 based development board. Very high-quality Adafruit part with superb 
[documentation](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather).

Unfortunately has high quiescent power consumption (see https://blog.voneicken.com/2018/lp-wifi-esp32-boards/) 
making it a poor choice for powering via a battery. So Sad.

## [EzSBC ESP32-01](https://www.ezsbc.com/index.php/products/wifi01-33.htm)

Out of the box 17uA quiescent power consumption. WROOM-32 based board just like the Feather. Great price.

# Random notes

## Wifi AP tweaks for lower-power devices

* Ensure multicast/broadcast filtering is enabled to prevent ARP and other random packets from
  potentially waking device. 
* Increase DTIM period.
 
## TODO: Compare on-board Hall effect sensor to external US5881

TODO: ESP32's on-board Hall effect sensor does not seem that sensitive. Need to compare to external
[Melexis US5881](https://www.melexis.com/en/product/US5881/Unipolar-Hall-Effect-Switch-Low-Sensitivity) 
(and potentially medium- or high-sensitivity versions of same part).

# Copyright and license

Copyright (c) 2020 int08h LLC. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.