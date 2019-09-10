import requests
import winsound
import datetime
import json
import time
class wem3162(): # one phase wifi energy meter
    def __init__(self,url):
        self.url=url
        self.headers={'Authorization':'Basic YWRtaW46YWRtaW4='}
        self.f=open('abc.txt','a')
        print (self.f)
    def monitorjson(self):
        apiurl=self.url+'/monitorjson'
        print (apiurl)
        r=requests.get(apiurl,headers=self.headers,timeout=3)
        print (r.text)
        self.monitorjsonR=r.text
        self.f.write(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')+'\r\n'+self.monitorjsonR)
        return(self.monitorjsonR)
    def json2parameter(self):
        x=self.monitorjsonR
        jsonR=json.loads(x)
        voltage=jsonR['data'][0]
        current=jsonR['data'][1]
        power=jsonR['data'][2]
        importEnergy=jsonR['data'][3]
        exportEnergy=jsonR['data'][4]
        return(voltage,current,power,importEnergy,exportEnergy)
    
    def close(self):
        self.f.close()

'''
{
    "method":"uploadsn",
    "mac":"B0F8933C4F7E",
    "version":"1.72.7",
    "server":"em",
    "SN":"E395281A",
    "Datas":[[236.0,4.4,-1044,0.92,0.04,49.99,1.00],[220.0,10.0,-1100,6971.88,1789550.92,49.99,0.50],[220.0,10.0,-1213,8.60,8.44,49.99,0.55]]
}
'''
class wem3080T(): # three phase wifi energy meter
    def __init__(self,url):
        self.url=url
        self.headers={'Authorization':'Basic YWRtaW46YWRtaW4='}
        self.f=open('abc.txt','a')
        print (self.f)
    def monitorjson(self):
        apiurl=self.url+'/monitorjson'
        print (apiurl)
        r=requests.get(apiurl,headers=self.headers,timeout=3)
        print (r.text)
        self.monitorjsonR=r.text
        self.f.write(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')+'\r\n'+self.monitorjsonR)
        return(self.monitorjsonR)
    def close(self):
        self.f.close()



if __name__ =='__main__':
    meter = wem3162('http://192.168.1.8') # ip of the WEM3162
    #meter = wem3080T('http://192.168.1.2') # ip of the WEM3080T
    counter=0
    retryCounter=0
    while(1):
        try:
            R=meter.monitorjson()
            print('test %d times,retryCounter:%d'%(counter,retryCounter))
            print(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
            counter=counter+1
            time.sleep(10)
            retryCounter=0
            
        except Exception as e:
            print (e)
            retryCounter=retryCounter+1
            print (retryCounter)
            if(retryCounter==3):
                break
            
    print ('bye')




