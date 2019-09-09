import requests
import winsound
import datetime
import json
import time
class wem3162():
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
        return(self.monitorjson)
    def close(self):
        self.f.close()


if __name__ =='__main__':
    meter = wem3162('http://192.168.1.8') # ip of the WEM3162
    counter=0
    retryCounter=0
    while(1):
        try:
            meter.monitorjson()
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




