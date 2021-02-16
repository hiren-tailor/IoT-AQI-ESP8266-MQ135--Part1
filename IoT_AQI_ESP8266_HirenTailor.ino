#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>  // include the library code:

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = D5, en = D4, d4 = D3, d5 = D2, d6 = D1, d7 = D0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int aqsensor = A0;  //output of mq135 connected to A0 pin of NodeMCU

int gled = D6;  //green led connected to pin 6
int rled = D7;  //red led connected to pin 7
int buz = D8;   //buzzer is connected to pin 8

const char *ssid = "xxxxxx"; // Enter your WiFi Name
const char *pass = "xxxxxxxx"; // Enter your WiFi Password

WiFiServer server(80);

void setup() {
  pinMode (gled,OUTPUT);      // gled is connected as output from ESP8266
  pinMode (aqsensor,INPUT);   // MQ135 is connected as INPUT to ESP8266
  pinMode (rled,OUTPUT);      // rled is connected as output from ESP8266
  pinMode (buz,OUTPUT);       // buzz is connected as output from ESP8266
  
  Serial.begin (115200);      //begin serial communication with baud rate of 115200

  lcd.clear();              // clear lcd
  lcd.begin (16,2);         // consider 16,2 lcd

  Serial.println("Connecting to ");
  lcd.print("Connecting.... ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  delay(5000);
  Serial.println(WiFi.localIP());
  delay(5000);
  server.begin();
}

void loop() {
  int ppm = analogRead(aqsensor); //read MQ135 analog outputs at A0 and store it in ppm

  Serial.print("Air Quality: ");  //print message in serail monitor
  Serial.println(ppm);            //print value of ppm in serial monitor

  lcd.setCursor(0,0);             // set cursor of lcd to 1st row and 1st column
  lcd.print("Air Quality: ");      // print message on lcd
  lcd.print(ppm);                 // print value of MQ135
  delay(1000);

  WiFiClient client = server.available();
  if ( client.connected()) { // loop while the client's connected
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println("Refresh: 3"); // update the page after 4 sec  
  client.println();

  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");

  // Web Page Heading
  client.println("<body><h1 style=\"color:black;\"> Air Quality Monitoring </h1>");
  client.println("<body><p style=\"color:blue;\"> Pollution Content(in PPM) = " + String(ppm) +" ppm"+ " </p>");
  
  if(ppm <= 130)
  {
    client.println("<body><p style=\"color:green;\"> Normal </p>");
    digitalWrite(gled,LOW);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   //Turn off Buzzer
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Normal");
    Serial.println("AQ Level Normal");
  }
  else if (ppm > 130 && ppm < 250)
  {
    client.println("<body><p style=\"color:purple;\"> Medium </p>");
    digitalWrite(gled,HIGH);   //jump here if ppm is not greater than threshold and turn off gled
    digitalWrite(rled,LOW);
    digitalWrite(buz,LOW);   //Turn off Buzzer
    lcd.setCursor(1,1);
    lcd.print ("AQ Level Medium");
    Serial.println("AQ Level Medium");
  }
  else
  {
    client.println("<body><p style=\"font-size:200%; color:red\"> Danger!!! </p>");
    lcd.setCursor(1,1);         //jump here if ppm is greater than threshold
    lcd.print("AQ Level Danger!");
    Serial.println("AQ Level Danger!");     
    digitalWrite(gled,LOW);
    digitalWrite(rled,HIGH);
    digitalWrite(buz,HIGH);     //Turn ON Buzzer
  }

  client.println("</body></html>");
  delay(500);

}
}
