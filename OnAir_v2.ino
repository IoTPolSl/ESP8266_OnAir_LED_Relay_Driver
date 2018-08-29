/*
 * ESP8266(WeMos D1 R2) WiFi Webserver Relay Control_Rev_2
 *
 * IoT_OnAir_Lab_Silesian University of Technology
 *
 * 29 Aug 2018
 * By Rafal Potempa
 *
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define espLED D4
#define RELAY D5

bool relayStatus;
bool internalLED;
int blink = 0;

const char* ssid = "INTER81";
const char* password = "welpixure321";

ESP8266WebServer server(80);

//Check if header is present and correct
bool is_authentified() {
	if (server.hasHeader("Cookie")) {
		Serial.print("Found cookie: ");
		String cookie = server.header("Cookie");
		Serial.println(cookie);
		if (cookie.indexOf("ESPSessionID=1") != -1) {
			Serial.println("Authentification Successful");
			return true;
		}
	}
	Serial.println("Authentification Failed");
	return false;
}

//login page, also called for disconnect
void handleLogin() {
	String msg;
	if (server.hasHeader("Cookie")) {
		Serial.print("Found cookie: ");
		String cookie = server.header("Cookie");
		Serial.println(cookie);
	}
	if (server.hasArg("disconnect")) {
		Serial.println("Disconnection");
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.sendHeader("Set-Cookie", "ESPSessionID=0");
		server.send(301);
		return;
	}
	if (server.hasArg("username") && server.hasArg("password")) {
		if (server.arg("username") == "admin" &&  server.arg("password") == "admin") {
			server.sendHeader("Location", "/");
			server.sendHeader("Cache-Control", "no-cache");
			server.sendHeader("Set-Cookie", "ESPSessionID=1");
			server.send(301);
			Serial.println("Log in Successful.");
			return;
		}
		msg = "Wrong username/password! \n Try again.";
		Serial.println("Log in Failed");
	}
	String content = "";
	content += "<html>";
	content += "	<head>";
	content += "		<meta name = \"viewport\" content = \"width=device-width,height=device-height,initial-scale=1.0\"/>";
	content += "	</head>";
	content += "	<body style=\'font-family:\"consolas\"\'>";
	content += "		<form action='/login' method='POST'>";
	content += "			<input type ='text' name='username' placeholder ='username'><br>";
	content += "			<input type='password' name='password' placeholder='password'><br>";
	content += "			<input type='submit' name='SUBMIT' value='Submit' style=\"font-family:consolas\">";
	content += "		</form>";
	content += "		<p style=\"color:red;\">" + msg + "</p><br>";
	content += "	</body>";
	content += "</html>";
	server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
	String header;
	if (!is_authentified()) {
		server.sendHeader("Location", "/login");
		server.sendHeader("Cache-Control", "no-cache");
		server.send(301);
		return;
	}
	if (server.hasArg("relay"))
	{
		if (server.arg("relay") == "On")
		{
			digitalWrite(RELAY, HIGH);
			relayStatus = HIGH;
		}
		else if (server.arg("relay") == "Off")
		{
			digitalWrite(RELAY, LOW);
			relayStatus = LOW;
		}
	}

	String content = "";
	content += "<html>";
	content += "	<head>";
	content += "		<meta name = \"viewport\" content = \"width=device-width,height=device-height,initial-scale=1.0\"/>";
	content += "	</head>";
	content += "	<body style=\"font-family:consolas\">";
	content += "		Led is: ";
	if (relayStatus == HIGH)
			content += "ON";
	if (relayStatus == LOW)
			content += "OFF";
	content += "		<br>";
	content += "		<br>";
	content += "		<form method='POST'>";
	content += "			<input type='submit' name='relay' value='On'>";
	content += "			<input type='submit' name='relay' value='Off'><br>";
	content += "		</form>";
	content += "		<a href=\"/login?disconnect=yes\">Log out</a>";
	content += "	</body>";
	content += "</html>";
	server.send(200, "text/html", content);
}

//no need authentification
void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

void blinkLEDForWorking() {
	if (blink == 96000)
	{
		digitalWrite(espLED, LOW);

		internalLED = LOW;
		blink += 1;
	}
	else if (blink == 100000)
	{
		blink = 1;
		if (internalLED == LOW)
		{
			digitalWrite(espLED, HIGH);
			internalLED = HIGH;
		}
	}
	else
	{
		blink += 1;
	}

}

void setup(void) {
	Serial.begin(115200);
	pinMode(RELAY, OUTPUT);
	pinMode(espLED, OUTPUT);
	digitalWrite(espLED, HIGH);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	Serial.println("");

	// Wait for connection
	Serial.print("Connecting to ");
	Serial.println(ssid);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());


	server.on("/", handleRoot);
	server.on("/login", handleLogin);
	server.on("/inline", []() {
		server.send(200, "text/plain", "this works without need of authentification");
	});

	server.onNotFound(handleNotFound);
	//here the list of headers to be recorded
	const char * headerkeys[] = { "User-Agent", "Cookie" };
	size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
	//ask server to track these headers
	server.collectHeaders(headerkeys, headerkeyssize);
	server.begin();
	Serial.println("HTTP server started");
}

void loop(void) {
	server.handleClient();
	blinkLEDForWorking();
}