/*
==============================================================================
Slave.ino
Created by: Rakesh G Raman
Date: 2020-04-12
Function: Connects to a master AP and communicates data
==============================================================================
*/

//----------------------------------------------------------------------------
// Libraries included
#include <ESP8266WiFi.h>
#include <Wire.h>

// Defining I/O pins
#define		LED		2		// WiFi module LED onboard
#define		LED_D0	D0 		// Indicating connection with master

// Authentication variables
char*			TKDssid;						// Master WiFi SSID
char*			TKDpwd;							// Master WiFi password

// Master details
int				TKDServerPort	= 9001;		// Master server port
IPAddress		TKDServer(192, 168, 4, 1);	// Master server IP Address
WiFiClient		TKDClient;					// Setting up client

// LED variables
int 			LED_State	= LOW;
const String	slave_Name	= "Roverbot";
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void setup()
{
	// Setting baud rate for communication
	Serial.begin(9600);

	// Setting the pin modes
	pinMode(LED, OUTPUT);		// WiFi module LED onboard
	pinMode(LED_D0, OUTPUT);	// Master connection indicator LED
	digitalWrite(LED, HIGH);	// Turned LED on to indicate no connection

	// Print message of I/O setting
	Serial.println("\nI/O pin modes set - DONE");

	// Disconnect from any other previous WiFi masters
	if(WiFi.status() == WL_CONNECTED)
	{
		WiFi.disconnect();
		WiFi.mode(WIFI_OFF);
		delay(50);
	}

	// Setting up of new WiFi credentials to connect
	WiFi.mode(WIFI_STA);
	WiFi.begin("MasterESP", "RoVeRbOtEsP");

	// Printing connection updates
	Serial.println("Connecting to: " + WiFi.SSID());

	// Checking for WiFi connection
	CheckWiFiConnectivity();

	// Turn off conectivity LED indicator and display connection output
	digitalWrite(LED, LOW);
	Serial.println("Client Device connected");

	// Connecting to the master as a slave (client)
	TKDRequest();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void loop()
{
	ProgrammeRun();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void ProgrammeRun()
{
	// If connectivity indicator LED on, turn it off and vice versa
	LED_State = !digitalRead(LED_D0);
	digitalWrite(LED_D0, LED_State);
	delay(100);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CheckWiFiConnectivity()
{
	// If no connectivity blink LED onboard
	while(WiFi.status() != WL_CONNECTED)
	{
		digitalWrite(LED, LOW);
		delay(100);
		digitalWrite(LED, HIGH);
		delay(400);
		Serial.print(".");
	}
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void I2C_Begin()
{
	/*
	Setting the I2C pins SDA, SCL
	SDA = D4 <GPIO2>
	SCL = D5 <GPIO14>
	For ESP8266 Dev Kit Node MCU v3
	*/
	// Frequency of the I2C
	#define		TWI_FREQ	 400000L	// I2C frequency set to 400KHz

	// Setting up the I2C using Wire
	Wire.begin();
	Wire.setClock(TWI_FREQ);			// Setting frequency for MPU9250
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void TKDRequest()
{
	// Client must be disconnected
	TKDClient.stop();

	// Once connected, send authentication
	if(TKDClient.connect(TKDServer, TKDServerPort))
	{
		Serial.println(slave_Name + " - CONNECTED to Master");
		TKDClient.println(slave_Name + " - CONNECTED to Master");
	}
}
//----------------------------------------------------------------------------