/*
* ESP8266(WeMos D1 R2) WiFi Relay Control_Rev_4
*
* IoT_OnAir_Lab_Silesian University of Technology
* 22 Aug 2018
* By Ritankar Sahu & Rafa³ Potempa
*
*/

#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h> 

const char* ssid = "alcatel IDOL 4";
const char* password = "123123123";

ESP8266WebServer server(80);

char temp[400];

void ledOn() {
	server.send(200, "text/plain", "turn on the LED!!!");
	pinMode(D5, OUTPUT);
	digitalWrite(D5, HIGH);
}

void ledOff() {
	server.send(200, "text/plain", "Turn off the LED");
	pinMode(D5, OUTPUT);
	digitalWrite(D5, LOW);
}

void Flash() {

	server.send(200, "text/plain", "LED flashing");

	pinMode(D5, OUTPUT);
	for (int i = 0; i < 5; i++)
	{
		digitalWrite(D5, HIGH);
		delay(500);
		digitalWrite(D5, LOW);
		delay(500);
	}
}

void LEDdim()
{
	pinMode(5, OUTPUT);
	analogWrite(5, 200);
}

void handleRoot()
{
	snprintf(temp, 400,
	"<html>\
		<head>\
			<meta name = \"viewport\" content = \"width=device-width,height=device-height,initial-scale=1.0\"/>\
		</head>\
		<body>\
			<a href =\"/1\"> <button>ON</button> </a>\
			<a href =\"/0\"> <button>OFF</button> </a>\
			<a href =\"/2\"> <button>blink</button> </a>\
		</body>\
	</html>");
	server.send(200, "text/html", temp);
}

void setup(void) {

	Serial.begin(115200);
	Serial.print("Connecting to network");
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);   // Connect to WiFi network

	while (WiFi.status() != WL_CONNECTED) {    // Wait for connection
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	server.on("/", handleRoot);
	server.on("/0", ledOff);
	server.on("/1", ledOn);
	server.on("/2", Flash);
	server.on("/3", LEDdim);

	server.begin();
	Serial.println("HTTP server started");
}

void loop(void)
{
	server.handleClient();
}
