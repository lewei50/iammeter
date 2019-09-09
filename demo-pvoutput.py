import wem
import PVOutput
import json
meter=wem.wem3162('http://192.168.1.8')
voltage,current,power,importEnergy,exportEnergy=meter.monitorjson()
#pv=PVOutput.PVOutputApi('siteNumber','API-key')

'''
#pvo = PVOutput.PVOutputApi("deviceId", "apiKey")
#pvo.add_status(
energy_generation,
power_generation,
energy_consumption,
power_consumption,
temperature,
voltage,
cumulative_flag,
net_flag)
#pvo.add_status(100,200,None,None,None,None,None,None)
'''
#pv.add_status(1500,2000,None,None,None,220,1,None)
