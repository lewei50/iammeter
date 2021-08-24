[toc]

# Wi-Fi Energy Meter binding
This Wi-Fi Energy Meter binding is used to get the real data from  Wi-Fi energy meter from [Iammeter](https://www.iammeter.com).



| Device                                                       | Feature                              | pic                       |
| :----------------------------------------------------------- | ------------------------------------ | ------------------------- |
| [WEM3080](https://www.iammeter.com/doc/iammeter/wem3080-quickstart.html) | 1phase,din-rail,bi-directional Wi-Fi | ![](https://cdn.iammeter.com/content/em2020/images/b1.png) |
| [WEM3080T](https://www.iammeter.com/doc/iammeter/wem3080t-quickstart.html) | 3phase,din-rail,bi-directional Wi-Fi | ![](https://cdn.iammeter.com/content/em2020/images/b2.png)               |










## Prerequisites

open PAPER UI(HABmin),install "HTTP Binding" and "JsonPath Transformation"。

![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps1.jpg) 

![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps2.jpg)		 

## Binding Configuration

This binding can be configured in the file `services/openenergymonitor.cfg`.

```javascript
iammeter.url=http://192.168.12.33/monitorjson{Authorization=Basic YWRtaW46YWRtaW4=}
iammeter.updateInterval=4000
```

​	![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps3.jpg)			 

​	![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps4.jpg)	 

## Item Configuration and example

open  openhab/conf/items/default.items (Create it if not),and add:

```javascript
Group     gIammeter				"Meters"				 <energy>        (Home)                    ["GroundFloor"]

Number  Iammeter_Voltage       "Voltage [%0f V]"    	<energy>     	 (gIammeter)				{ http="<[iammeter:4000:JSONPATH($.Data[0])]" }
Number  Iammeter_Current       "Current [%0f A]"   		<energy>    	 (gIammeter)        		{ http="<[iammeter:30000:JSONPATH($.Data[1])]" }
Number  Iammeter_Power         "Power [%0f W]"  		<energy>    	 (gIammeter)            	{ http="<[iammeter:4000:JSONPATH($.Data[2])]" }
Number  Iammeter_Importenergy  "Importenergy [%0f kWh]" <energy>    	 (gIammeter)           		{ http="<[iammeter:30000:JSONPATH($.Data[3])]" }
Number  Iammeter_Exportgrid    "Exportgrid [%0f kWh]"  	<energy>     	 (gIammeter)            	{ http="<[iammeter:30000:JSONPATH($.Data[4])]" }

```

​	![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps5.jpg)	 

 open  openhab/conf/sitemaps/default.sitemap (Create it if not),and add:

```javascript
    Frame {
        Text item=Iammeter_Voltage label="Voltage [%.1f V]" icon="energy"
        Text item=Iammeter_Current label="Current [%.1f A]" icon="energy"
        Text item=Iammeter_Power label="Power [%.1f W]" icon="energy"
        Text item=Iammeter_Importenergy label="Importenergy [%.1f kWh]" icon="energy"
    }
```

 

​	![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps6.jpg)	 

 

## Finally 

1. Open the Basic UI and you will see the data of the electricity meter.

   ![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps7.jpg) 

2. Also you can add it to habpanel.

   ![img](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/openhabwps8.jpg) 

# 

# reference

1. [API reference](https://www.iammeter.com/doc/iammeter/integrate-with-PVOutput.html)
2. [integrated in third-party server](https://www.iammeter.com/doc/iammeter/integrate-with-thirdparty-server.html)
3. [FAQ](https://www.iammeter.com/doc/iammeter/FAQ.html)












