import wem
import PVOutput
import json
meter=wem.wem3162('http://192.168.1.8')
'''
one phase(wem3162 wem3080) meter response
{"status":"succeed","data":[235.00,1.214,189,1265.17,0.00]} 
'''

print (meter.monitorjson())
(vol,cur,power,importE,exportE)=meter.json2parameter()
print ("vol:%dV\r\ncur:%fA\r\npower:%dw\r\nimportEnergy:%fKWH\r\nexportEnergy:%fKWH\r\n"%(vol,cur,power,importE,exportE))
pv=PVOutput.PVOutputApi('siteNumber','api-key')

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

pv.add_status(importE,power,None,None,None,vol,1,None)
