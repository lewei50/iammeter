import logging
import time
import requests


__author__ = "Mark Ruys"
__copyright__ = "Copyright 2017, Mark Ruys"
__license__ = "MIT"
__email__ = "mark@paracas.nl"
__modifier="yangbo"

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
class PVOutputApi:
    def __init__(self, system_id, api_key):
        self.m_system_id = system_id
        self.m_api_key = api_key

    def add_status(self, energy_generation, power_generation, energy_consumption,power_consumption,temperature, voltage,cumulative_flag,net_flag):
        t = time.localtime()
        payload = {
            'd' : "{:04}{:02}{:02}".format(t.tm_year, t.tm_mon, t.tm_mday),
            't' : "{:02}:{:02}".format(t.tm_hour, t.tm_min),
            'v1' : round(energy_generation),
            'v2' : round(power_generation)
        }
        if energy_consumption is not None:
            payload['v3'] = energy_consumption
        if power_consumption is not None:
            payload['v4'] = power_consumption
        if temperature is not None:
            payload['v5'] = temperature
        if voltage is not None:
            payload['v6'] = voltage
        if cumulative_flag is not None:
            payload['c1'] = cumulative_flag
        if net_flag is not None:
            payload['n'] = net_flag
        data = ""
        for key, value in payload.items():
            data = data + ("&%s=%s" % (key, value))
        print(data[1:])
        self.call("https://pvoutput.org/service/r2/addstatus.jsp", data[1:])
'''
    def add_day(self, data, temperatures):
        # Send day data in batches of 30.
        for chunk in [ data[i:i + 30] for i in range(0, len(data), 30) ]:
            readings = []
            for reading in chunk:
                dt = reading['dt']
                fields = [
                    dt.strftime('%Y%m%d'),
                    dt.strftime('%H:%M'),
                    str(round(reading['eday_kwh'] * 1000)),
                    str(reading['pgrid_w'])
                ]

                if temperatures is not None:
                    fields.append('')
                    fields.append('')
                    temperature = list(filter(lambda x: dt.timestamp() > x['time'], temperatures))[-1]
                    fields.append(str(temperature['temperature']))

                readings.append(",".join(fields))

            payload = {
                'data' : ";".join(readings)
            }

            self.call("https://pvoutput.org/service/r2/addbatchstatus.jsp", payload)
'''
    def call(self, url, payload):
        logging.debug(payload)
        
        headers = {
            'X-Pvoutput-Apikey' : self.m_api_key,
            'X-Pvoutput-SystemId' : self.m_system_id,
            'X-Rate-Limit': '1',
            'Content-Type': 'application/x-www-form-urlencoded'
        }

        for i in range(1, 2):
            try:
                r = requests.post(url, headers=headers, data=payload, timeout=10)
                if 'X-Rate-Limit-Reset' in r.headers:
                    reset = round(float(r.headers['X-Rate-Limit-Reset']) - time.time())
                else:
                    reset = 0
                if 'X-Rate-Limit-Remaining' in r.headers:
                    if int(r.headers['X-Rate-Limit-Remaining']) < 10:
                        logging.warning("Only {} requests left, reset after {} seconds".format(
                            r.headers['X-Rate-Limit-Remaining'],
                            reset))
                if r.status_code == 403:
                    logging.warning("Forbidden: " + r.reason)
                    time.sleep(reset + 1)
                else:
                    r.raise_for_status()
                    break
            except requests.exceptions.RequestException as arg:
                logging.warning(arg)
            time.sleep(i ** 3)
        else:
            logging.error("Failed to call PVOutput API")
