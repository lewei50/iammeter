import wem
import PVOutput
meter=wem.wem3162('http://192.168.1.8')
meter.monitorjson()
pv=PVOutput.PVOutputApi('67669','4c5b6575896610b64b125517395b93579281e6e4')
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
pv.add_status(1100,2000,None,None,None,220,1,None)