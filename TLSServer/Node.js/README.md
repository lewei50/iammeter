# TLS Server for iMeter
This is a TLS server demo for WEM3080 serial wifi energy meter


## Start server:

    node server.js

## Set the meter:
    
    Open iMeter settings
    Set runmode to 'tcp'
    Set address to 'tls://{your IP address}:8000'