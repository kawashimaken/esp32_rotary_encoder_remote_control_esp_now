# esp32_rotary_encoder_remote_control_esp_now


# what is this?

Take a look at description YouTube video click the picture below or click the link here [https://www.youtube.com/watch?v=8IGkFBghzww]

[![](https://img.youtube.com/vi/8IGkFBghzww/0.jpg)](https://www.youtube.com/watch?v=8IGkFBghzww)

# folder description

* esp_now_sender : sender platformio project
* esp_now_receiver : receiver platformio project
* find_mac_address : platformio project to find the MAC address for your (receiver ESP32)


# you need to do

## This is using VSCode + platformio, you need to be familiar with platformio

## libraries you need

find them in eache platformio.ini file

## run the program find_mac_address first to find your ESP32(receiver)'s MAC address.

## IMPORTANT:you need to make sure the serial port configuration is updated to yours!
change accordingly in your platformio.ini file

* sender: https://github.com/kawashimaken/esp32_rotary_encoder_remote_control_esp_now/blob/8f424417976c2a0e3f3eb11c2d825ebfcde463da/esp_now_sender/platformio.ini#L18

* receiver : https://github.com/kawashimaken/esp32_rotary_encoder_remote_control_esp_now/blob/8f424417976c2a0e3f3eb11c2d825ebfcde463da/esp_now_receiver/platformio.ini#L18

* find_mac_address : https://github.com/kawashimaken/esp32_rotary_encoder_remote_control_esp_now/blob/02b02999540bb97b2a663cfd300dd3b9f7744a08/find_mac_address/platformio.ini#L15



