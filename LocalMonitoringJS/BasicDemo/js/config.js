/*
1 change the sn to yours
2 type=3 mean 3phase 
  type=1 meana 1phase
3 select the item you want to show in front panel
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
//setup option using your login name and userkey

var username = 'laoliu';
var password = '123456';

/*
Data format
{
	"method": "uploadsn",
	"mac": "B0F8933C4FC5",
	"version": "2.75.66",
	"server": "em",
	"SN": "29C9F55D",
	"Datas": [
		[0.0, 0.1, 0.2, 21.063, 20.667, 49.99, 1.00],
		[1.0, 1.1, 1.2, 21.063, 20.667, 49.99, 0.50],
		[2.0, 2.1, 2.2, 21.063, 20.667, 49.99, 0.55],
		[3.0, 3.1, 3.2, 26.329, 25.834, 49.99, 0.55]
	]
}
*/

