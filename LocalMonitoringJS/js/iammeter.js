const connectUrl = 'wss://mqtt.iammeter.com:8084/mqtt';
//change your sn list here
//data shown configed in "items"


//setup option using your login name and userkey
var options = {
        clean: true,
        connectTimeout: 10000,
        clientId: '',
        username: username,
        password: password,
}
options.clientId = username + Math.floor((Math.random()*10000000));

var seriesData = [];
var units = ['V','A','W','kWh','kWh'];
var itemName = ['Vol','Cur','Pow','KwhIN','kWhOUT'];
var itemPhase = ['A','B','C'];

const client = mqtt.connect(connectUrl, options);
client.on('connect', function () {
    console.log('connect');
    for (var i = 0; i < sns.length; i++) {
        sub('device/'+sns[i].sn+'/realtime');
    }
});

function cb(err,a) {
    console.log(err,a);
}

var sub = function (topic) {
    client.subscribe(topic, function (err) {
        if (err)
            console.log(err);
    })
};

client.on('reconnect', (error) => {
    console.log('reconnect:', error)
})

client.on('error', (error) => {
    console.log('error:', error)
})

client.on('message', (topic, message) => {
    // console.log('message：', topic, message.toString());
    if (topic.indexOf('realtime') > -1) {
        for (var i = 0; i < sns.length; i++) {
            var sn = sns[i].sn;
            if (topic.indexOf(sn) > -1) {
                // 处理
                setChartData(sns[i], JSON.parse(message));
            }
        }
    }
    showLines(seriesData);
});

var setChartData =  function (sensor,data)
{
    if(sensor.type =="1")
    {
        for(i=0;i<sensor.items.length;i++)
        {
            addSerialData(sensor.name  +"_"+ itemName[sensor.items[i]],data.Data[sensor.items[i]]);
        }
    }
    else if(sensor.type =="3")
    {
        for(i=0;i<sensor.items.length;i++)
        {
            addSerialData(sensor.name +"_phase"+ itemPhase[sensor.items[i][0]] +"_"+ itemName[sensor.items[i][1]],data.Datas[sensor.items[i][0]][sensor.items[i][1]]);
        }
    }
};

var addSerialData = function(name,value)
{
    for(var idx=0;idx<seriesData.length;idx++)
    {
        if(seriesData[idx].name==name)
        {
            seriesData[idx].data.push([new Date().getTime(),parseFloat(value)]);
            if(seriesData[idx].data.length>100)seriesData[idx].data.shift();
        }
    }
};

for (var i = 0; i < sns.length; i++) {
    for(var itemId = 0;itemId<sns[i].items.length;itemId++)
    {
        if(sns[i].type=="1")seriesData.push({name:sns[i].name +"_" + itemName[sns[i].items[itemId]],data:[]});
        else if(sns[i].type=="3")seriesData.push({name:sns[i].name+"_phase" + itemPhase[sns[i].items[itemId][0]] +"_"+itemName[sns[i].items[itemId][1]],data:[]});
    }
}
// console.log(seriesData);


var showLines = function (serials){
    chart = Highcharts.chart('chartContainer', {
        chart: {
            zoomType: 'x',
            type: 'spline'
        },
        xAxis: {
            type: 'datetime',
            minPadding: 0.05,
            maxPadding: 0.05
        },
        title: {
            text: ''
        },
        xAxis: {
            type: 'datetime'
        },
        yAxis: {
            title: '',
            labels: {
                formatter: function () {
                        return this.value;
                }
            }
        },
        plotOptions: {
            area: {
                stacking: 'normal',
                lineColor: '#666666',
                lineWidth: 1,
                fillOpacity: 0.3,
                marker: {
                    enabled: true,
                    radius:2,
                    states: {
                        hover: {
                            enabled: true
                        }
                    }
                }
            }
        },
        series: seriesData
    });
};
