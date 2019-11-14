# HomeAssistant
home assistant custom components
The iMeter integration connects home-assistant to iMeter power meters. iMeters may be connected to a home Wi-Fi network and expose a REST API.

Configuration

To use the sensors in your installation, add the following to your configuration.yaml file:

# Example configuration.yaml entry
sensor:
  - platform: iammeter
    ip_address: IP_ADDRESS
    name: your device name

CONFIGURATION VARIABLES

host
(string)(Required)
The IP address of your iMeter system.

name
(string)(Required)
