/*
==============================================================================
Master.ino
Created by: Rakesh G Raman
Date: 2020-04-12
Function: Establishes WiFi AP for other modules (slaves) to connect
==============================================================================
*/

//----------------------------------------------------------------------------
// Libraries included
#include <ESP8266WiFi.h>

// Defining I/O pins
#define		LED			2		// WiFi module LED onboard
#define		LED_SLAVE	21 		// Indicating connection with slave

// Authentication variables
char*			TKDssid;						// Master WiFi SSID
char*			TKDpwd;							// Master WiFi password
String			MACAddr;						// MAC address of AP
// const String	nameWiFi	= "MasterESP";		// Set WiFi SSID
// const String	namePwd		= "RoVeRbOtEsP";	// Set WiFi password

// Master details
#define		MAXC		6		// Maximum number of clients (6 given for safety)
WiFiServer	TKDServer(9001);	// Master server and port number
WiFiClient	TKDClient[MAXC];	// Master server clients

// LED variables
int 	LED_State 	= LOW;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void setup()
{
	// Setting the serial port
	Serial.begin(9600);			// Baud rate for communication

	// Setting the mode for pins
	pinMode(LED, OUTPUT);		// WiFi module LED onboard
	pinMode(LED_SLAVE, OUTPUT);	// Slave connection indicator LED

	// Print setting progress
	Serial.println("\nI/O pin modes set - DONE");

	// Setting up WiFi AP
	SetWifi("MasterESP", "RoVeRbOtEsP");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void loop()
{
	// Checking for available clients
	AvailableClients();
	// Checking for available client(s) messages
	AvailableMessages();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void SetWifi(char* the_SSID, char* the_PWD)
{
	// Disconnect any previous WiFI
	WiFi.disconnect();

	// Setting WiFi mode: Access Point Station
	WiFi.mode(WIFI_AP_STA);
	Serial.println("WiFi Mode: Access Point Station");

	// Setting WiFi AP credentials
	TKDssid	= the_SSID;
	TKDpwd	= the_PWD;

	// Starting the AP
	WiFi.softAP(TKDssid, TKDpwd);
	Serial.println("WiFI: " + String(TKDssid) + ", is set");
	delay(1000);

	// Extracting and printing IP address
	IPAddress IPAddr = WiFi.softAPIP();
	Serial.print("IP Address: ");
	Serial.println(IPAddr);

	// Extractinf and printing MAC address
	MACAddr = WiFi.softAPmacAddress();
	Serial.println("MAC Address: "+ String(MACAddr));

	// Starting the Master server
	TKDServer.begin();
	TKDServer.setNoDelay(true);
	Serial.println("Master server started");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void AvailableClients()
{
	// Clients available
	if(TKDServer.hasClient())
	{
		// Switch onboard LED off if on
		if(digitalRead(LED))
		{
			digitalWrite(LED, LOW);
		}

		// If connectivity indicator LED on, turn it off and vice versa
		LED_State = !digitalRead(LED_SLAVE);
		digitalWrite(LED_SLAVE, LED_State);
		delay(100);

		// Count client token number
		for(uint8_t token = 0; token < MAXC; token++)
		{
			// Find free/disconnected spot to connect - accept
			if(!TKDClient[token] || !TKDClient[token].connected())
			{
				// Check if client is already taken
				if(TKDClient[token])
				{
					TKDClient[token].stop();
				}

				// Check if the clients are connected to the master server
				if(TKDClient[token] = TKDServer.available())
				{
					Serial.println("New client: " + String(token));
				}
			}

			// No free/disconnected spot to connect - reject
			else
			{
				WiFiClient TKDClient = TKDServer.available();
				TKDClient.stop();
			}
		}
	}

	// No clients available
	else
	{
		digitalWrite(LED, HIGH);
		delay(400);
		digitalWrite(LED, LOW);
		delay(100);
	}
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void AvailableMessages()
{
	// Check clients for messages
	for(uint8_t token = 0; token < MAXC; token++)
	{
		// Checking availability of the clients
		if(TKDClient[token] && TKDClient[token].connected() && TKDClient[token].available())
		{
			while(TKDClient[token].available())
			{
				// Extract message received and display
				String message = TKDClient[token].readStringUntil('\r');
				TKDClient[token].flush();
				Serial.println("Client No." + String(token) + " - " + message);
			}
		}
	}
}
//----------------------------------------------------------------------------