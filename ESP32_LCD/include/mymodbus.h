#include <ModbusIP_ESP8266.h>

typedef struct _MeterDatas
{
    float Voltage_a ; //Voltage  0x0000  /100 V
    float Current_a;  //Current  0x0001  /100 A
    float Power_a;  //Power  0x0002  W
    double ImportEnergy_a;  //ImportEnergy  0x0004 /800 kWh
    double ExportGrid_a;  //ExportGrid  0x0006 /800 kWh
    float power_factor_a; //power_factor  0x0008 /1000 
    //NC  0x0009

    float Voltage_b ; //Voltage  0x000A  /100 V
    float Current_b;  //Current  0x000B  /100 A
    float Power_b;  //Power  0x000C  W
    double ImportEnergy_b;  //ImportEnergy  0x000E /800 kWh
    double ExportGrid_b;  //ExportGrid  0x0010 /800 kWh
    float power_factor_b; //power_factor  0x0012 /1000 
    //NC  0x0013

    float Voltage_c ; //Voltage  0x0014  /100 V
    float Current_c;  //Current  0x0015  /100 A
    float Power_c;  //Power  0x0016  W
    double ImportEnergy_c;  //ImportEnergy  0x0018 /800 kWh
    double ExportGrid_c;  //ExportGrid  0x001A /800 kWh
    float power_factor_c; //power_factor  0x001C /1000 
    //NC  0x001D

    float Frequency;  //Frequency  0x001E /100 Hz 
    //NC  0x001F
    float Power_s;  //Power sum  0x0020  W
    double ImportEnergy_s;  //ImportEnergy sum  0x0022 /800 kWh
    double ExportGrid_s;  //ExportGrid sum  0x0024 /800 kWh

    float apparent_power_a; //apparent_power
    float reactive_power_a; //reactive_power

    uint16_t address = 0x0048;  //starting at register
    uint16_t quantitysingle = 10;  //single phase meter
    uint16_t quantity = 38;     //IAMMETER Three phase meter
    bool data_ok = false;

    String jsondata;
} MeterDatas;

MeterDatas wem3080t;
uint16_t read_datas[38];

const int REG = 0;               // Modbus Hreg Offset
IPAddress remote(192, 168, 1, 6);  // Address of Modbus Slave device
const int LOOP_COUNT = 10;
long refreshTime = 1000;

ModbusIP mb;  //ModbusIP object