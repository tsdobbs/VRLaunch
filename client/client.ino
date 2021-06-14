#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <ESP8266WiFiMulti.h>

#include <WakeOnLan.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);

//ESP8266WiFiMulti WiFiMulti;

const char* ssid     = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

const char* host = "IP_OF_COMPUTER_TO_LAUNCH_STEAM";
const uint16_t port = 3000;

const int greenPin =  14;
const int yellowPin = 13;
const int redPin = 12;
const int bluePin = 4;

void wakeMyPC() {
    const char *MACAddress = "MAC_ADDRESS_OF_COMPUTER_TO_LAUNCH_STEAM";
  
    WOL.sendMagicPacket(MACAddress); // Send Wake On Lan packet with the above MAC address. Default to port 9.
    // WOL.sendMagicPacket(MACAddress, 7); // Change the port number
}


String api_request(WiFiClient client, String endpoint) {

    Serial.println("Hitting API Endpoint '" + endpoint +"'");
    client.print(String("GET ") + endpoint + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
    write_state(4);
    delay(12000);

    Serial.println("Response from endpoint:");
    String response_line = client.readStringUntil('\r');
    Serial.println(response_line);

    String status = response_line.substring(9,12);
    Serial.print("Parsed Status: ");
    Serial.println(status);

    return status;
} 

void write_state(int state) {
  switch (state) {
    case 1: //1 = "STARTED"
      digitalWrite(bluePin, HIGH);
      break;
    case 2: // 2= "WIFI_CONNECTED"
      digitalWrite(bluePin, HIGH);
      digitalWrite(greenPin, HIGH);
      break;
    case 3: //3 = "WAKE-ON-LAN"
      digitalWrite(bluePin, HIGH);
      digitalWrite(greenPin, HIGH);
      digitalWrite(yellowPin, HIGH);
      break;
    case 4: //4 = "WAITING"
      digitalWrite(bluePin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);
      break;
    case 5: //"FAIL: Timeout connecting to host"
      digitalWrite(bluePin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, HIGH);
      break;
    case 6:  //"FAIL: Server not responding"
      digitalWrite(bluePin, LOW);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(redPin, HIGH);
      break;
    case 7: //"FAIL: Error on SteamVR launch"
      digitalWrite(bluePin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, HIGH);
      digitalWrite(redPin, HIGH);
      break;
    case 8: //"Success!"
      digitalWrite(bluePin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      break;
    default:  //"SLEEP"
      digitalWrite(greenPin, LOW);
      digitalWrite(yellowPin, LOW);
      digitalWrite(redPin, LOW);
      digitalWrite(bluePin, LOW);
      break;
  }
}

void setup()
{
    
    Serial.begin(115200);

    Serial.print("Setting Pins");
    // initialize the LED pin as an output:
    pinMode(greenPin, OUTPUT);
    pinMode(yellowPin, OUTPUT);
    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
     
    //1 = "STARTED"
    write_state(1);
    
    WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    // 2= "WIFI_CONNECTED"
    write_state(2);

    Serial.println("Setup complete");

    WiFiClient client;
    Serial.println("Connecting to host");
    float gap_limit = 7;
    float gap_time = 30;
    float rate = 2;
    while (!client.connect(host, port) & gap_time > gap_limit) {
        Serial.println("No connection. Sending WOL");
        //3 = "WAKE-ON-LAN"
        write_state(3);
        wakeMyPC();
        Serial.println("Waiting " + String(gap_time) + " seconds");

        //hold before blinking
        float hold_time = 1;
        delay(hold_time*2*1000);
        float remaining_gap = gap_time - hold_time;
        
        //set up blinking
        while (remaining_gap > 0) {
          //4 = "WAITING"
          write_state(4);
          delay(hold_time/rate*1000);
          write_state(3);
          delay(hold_time/rate*1000);
          remaining_gap = remaining_gap - (2/rate*hold_time);
        }
        //initial waits 2 min for startup, then half as long each gap until gap is too small.
        gap_time = gap_time/2;
        rate = rate + 1;
    }
  

    if (gap_time <= gap_limit) {
        Serial.println("FAIL: Timeout connecting to host");
        write_state(5);
    } else {
      Serial.println("Successful connection to host");
      String launchVR = api_request(client, "/launchVR");
      if (launchVR="200") {
          Serial.println("Success!");
          write_state(8);
      } else {
          write_state(1);
          String awake = api_request(client, "/awake");
          if (awake=="200"){
              Serial.println("FAIL: Error on SteamVR launch");
              write_state(7);
          } else {
              Serial.println("FAIL: Server not responding");
              write_state(6);
          }
      }
      client.stop();
    }

    delay(3*1000);
    write_state(9);
    Serial.println("...Sleeping...");
    ESP.deepSleep(0);
    Serial.print("If you're seeing this, system did not sleep");
}



void loop()
{
    Serial.print("If you're seeing this, system did not sleep");
       
 
}
