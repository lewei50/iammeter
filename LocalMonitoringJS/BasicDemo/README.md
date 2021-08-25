[toc]

Firstly , you should download this folder. 

Then, you need modify the config.js, replace the configuration with yours. This is the main work for you to use this demo. 

# Modify the "config.js"

![image-20210825125537177](https://leweidoc.oss-cn-hangzhou.aliyuncs.com/lewei50/img/iammeter/tmpliu/tmpimage-20210825125537177.png)

## Replace the SN with yours



## Type definition

3: 3phase meter (WEM3080T)

1: 1phase meter (WEM3080)

## items definition

Select the parameter you want to show in the chart.

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



# open the Basic.html









