/*
 * ESP8266(WeMos D1 R2) WiFi Relay Control_Rev_1
 * 
 * IoT_OnAir_Lab_Silesian University of Technology
 * 10 Aug 2018
 * By Ritankar Sahu & Arkadiusz Czerwiński
 * 
 */



#include <ESP8266WiFi.h>

WiFiServer server(80); //Initialize the server on Port 80

void setup() 
{
  // put your setup code here, to run once:
  
  WiFi.mode(WIFI_AP);                    //Our ESP8266-12E is an AccessPoint 
  WiFi.softAP("IoT_OnAir", "12345678");  // Provide the (SSID, password); . 
  server.begin();                        // Start the HTTP Server

  Serial.begin(115200);                      //Start communication between the ESP8266-12E and the monitor window
  IPAddress HTTPS_ServerIP= WiFi.softAPIP(); // Obtain the IP of the Server 
  Serial.print("Server IP is: ");            // Print the IP to the monitor window 
  Serial.println(HTTPS_ServerIP);

  pinMode(D5, OUTPUT);                  //GPIO5 is an OUTPUT pin;
  digitalWrite(D5, LOW);                //Initial state is ON  

}

void loop() 
{
  // put your main code here, to run repeatedly:

  WiFiClient client = server.available();
  if (!client) 
  { 
    return; 
  } 
  
  Serial.println("Somebody has connected :)");   //Looking at serial under the hood 

  //Read what the browser has sent into a String class and print the request to the monitor
  String request = client.readStringUntil('\r'); 
  
  Serial.println(request);    //Looking at serial under the hood

  // Handle the Request

  if (request.indexOf("/ON") != -1)
  { 
    digitalWrite(D5, HIGH); 
  }
    else if (request.indexOf("/OFF") != -1)
    { 
      digitalWrite(D5, LOW); 
    }
  
  //Prepare The HTML Document to respond and add buttons
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "";                                  // Must be feature. I don't know why. Need to consult with Dr. Czekalski
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  s += "<br><input type=\"button\" name=\"b1\" value=\"Turn Relay On\" onclick=\"location.href='/ON'\">";
  s += "<br><br><br>";
  s += "<input type=\"button\" name=\"b1\" value=\"Turn Relay Off\" onclick=\"location.href='/OFF'\">";
  s += "</html>\n";

  
  //Serve the HTML document to the browser.
  client.flush();                       //clear previous info in the stream 
  client.print(s);                      // Send the response to the client 
  delay(1); 
  Serial.println("Client disonnected"); //Looking at serial under the hood
}
