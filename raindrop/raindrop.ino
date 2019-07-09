#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//#include "index.h" //Our HTML webpage contents with javascripts

const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>

<head>
  <title>Line Chart | RAINFALL MEASUREMENT</title>
  
  <script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>  
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }

  /* Data Table Styling */
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
  }

  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }

  #dataTable tr:nth-child(even){background-color: #f2f2f2;}

  #dataTable tr:hover {background-color: #ddd;}

  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: left;
    background-color: #4CAF50;
    color: white;
  }
  </style>
</head>

<body>
    <div style="text-align:center;"></div>
    <div class="chart-container" position: relative; height:350px; width:100%">
        <canvas id="Chart" width="400" height="200"></canvas>
    </div>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>RAINFALL_PWM_VALUES</th></tr>
  </table>
</div>
<br>
<br>  

<script>

var values = [];
var timeStamp = [];
function showGraph()
{
    for (i = 0; i < arguments.length; i++) {
      values.push(arguments[i]);    
    }

    var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [{
                label: "RAINFALL 100 FOR HIGH/0 FOR LOW",
                fill: false,  //Try with true
                backgroundColor: 'rgba( 243, 156, 18 , 1)', //Dot marker color
                borderColor: 'rgba( 243, 156, 18 , 1)', //Graph Line Color
                data: values,
            }],
        },
        options: {
            title: {
                    display: true,
                    text: "RAINDROP SENSOR"
                },
            maintainAspectRatio: false,
            elements: {
            line: {
                    tension: 0.5 //Smoothening (Curved) of data lines
                }
            },
            scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero:true
                        }
                    }]
            }
        }
    });

}

//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString());
  showGraph(5,10,4,58);
};



setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
}, 5000); //5000mSeconds update rate
 
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString();
  var val_rain = this.responseText; 
      values.push(val_rain);
      timeStamp.push(time);
      showGraph();  //Update Graphs
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1); //Add after headings
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    cell1.innerHTML = time;
    cell2.innerHTML = val_rain;
    }
  };
  xhttp.open("GET", "readRAIN", true); //Handle readDHT server on ESP8266
  xhttp.send();
}
    
</script>
</body>

</html>

)=====";

 
//#define LED 2  //On board LED
#define RAINPIN 2 
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum

/*IPAddress staticIP(192, 168, 43, 194); //ESP static ip
IPAddress gateway(192, 168, 43, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);  //DNS */
//SSID and Password of your WiFi router
const char* ssid = "Senti";
const char* password = "senti143";


 
ESP8266WebServer server(80); //Server on port 80
 

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleRAIN() {
   String val_rain;
 int sensorReading = analogRead(RAINPIN);
 if(sensorReading==1023)
 {
  sensorReading=0;
 val_rain = String(sensorReading);
 }
 else
 {
 sensorReading=100;
 val_rain = String(sensorReading);
 }
 Serial.println(sensorReading);
server.send(200, "text/plane", val_rain);
 
}

void setup(void){
  Serial.begin(115200);

  Serial.println();
  pinMode(RAINPIN,INPUT);
  Serial.println();

  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  //Onboard LED port Direction output
  //pinMode(LED,OUTPUT); 
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readRAIN", handleRAIN); //This page is called by java Script AJAX
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();          //Handle client requests
}
