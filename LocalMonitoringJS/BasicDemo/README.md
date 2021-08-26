* [Modify the "config\.js"](#modify-the-configjs)
  
  * [Replace the SN with yours](#replace-the-sn-with-yours)
  * [Type definition](#type-definition)
  * [items definition](#items-definition)
  * [username&amp;password](#usernamepassword)
  
* [open the Basic\.html](#open-the-basichtml)

  

[video tutorial](https://www.youtube.com/watch?v=WcAbBRx75H4)

Three steps to use this demo

1. Firstly , you should download this folder. 

2. [modify the "config.js"](#modify-the-configjs)

3. [open the Basic\.html](#open-the-basichtml)

   

# Modify the "config.js"

Then, you need modify the config.js, replace the configuration with yours. This is the main work for you to use this demo. 

![image-20210825125537177](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/tmpliu/tmpimage-20210825125537177.png)

## Replace the SN with yours

Replace the sn in "config.js" with your own. 

## Type definition

 "type" in "config.js"

3: 3phase meter (WEM3080T)

1: 1phase meter (WEM3080)

## items definition

Set the array of items, select the parameter you want to show in the chart.

```
This is Data format 
{
	"method": "uploadsn",
	"mac": "B0F8933C4FC5",
	"version": "2.75.66",
	"server": "em",
	"SN": "29C9F55D",
	"Datas": [ // voltage, current,active power, forward energy,reverse energy,frequency,pf
		[0.0, 0.1, 0.2, 21.063, 20.667, 49.99, 1.00], //phase A
		[1.0, 1.1, 1.2, 21.063, 20.667, 49.99, 0.50], //phase B
		[2.0, 2.1, 2.2, 21.063, 20.667, 49.99, 0.55], //phase C
		[3.0, 3.1, 3.2, 26.329, 25.834, 49.99, 0.55]  //net metering mode
	]
}
/*  
  item value mean the data position in Datas array(2 dimension array).
  For example
  [0,0] mean the voltage in phaseA
  [0,1] mean the current in phaseA
  [0,2] mean the active power in phaseA
  [0,3] mean the forward energy in phaseA
  [0,4] mean the reverse energy in phaseA
  [1,0] mean the voltage in phaseB
  [1,1] mean the current in phaseB
  [2,2] mean the active power in phaseB
  [2,3] mean the forward energy in phaseB
  [2,4] mean the reverse energy in phaseB
  .....
  [3,4] mean the reverse energy in phaseC
*/


var sns =[
    {"sn":"C662DE1F","name":"C662DE1F","type":"3","items":[[0,2],[1,2]]},
    {"sn":"29C9F55D","name":"29C9F55D","type":"3","items":[[1,2],[2,2]]}
];
Explain the sns setting meaning
SN:C662DE1F ,3phase meter, show phaseA active power and phase B active power in the chart.
SN:29C9F55D ,3phase meter, show phaseB active power and phase C active power in the chart.

```



## username&password

Input your own MQTT username and password here.

![image-20210825180802295](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/tmpliu/tmpimage-20210825180802295.png)



# open the Basic.html

![image-20210825180922014](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/tmpliu/tmpimage-20210825180922014.png)



You can also replace the logo pic in the html.

![image-20210825184812938](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/tmpliu/tmpimage-20210825184812938.png)





