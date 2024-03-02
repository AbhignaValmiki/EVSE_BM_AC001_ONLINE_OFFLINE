#if 1
#include "CustomEthernet.h"
#include "Master.h"
#include "display.h"
#include <iostream>
#include <string>
#define SS_ETH 	2
bool flag_ethoffline = false;
extern bool webSocketConncted;
// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);

/* Added by Gopi*/
// initialize the library instance:
// EthernetClient client;
// char server[] = "www.arduino.cc";  // also change the Host line in httpRequest()
//IPAddress server(64,131,82,241);

// unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
// const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

#if 0
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// uint8_t mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };



// Set the static IP address to use if the DHCP fails to assign
//IPAddress ip(192, 168, 0, 177);

uint16_t ip_arr[4] = {192, 168, 1, 71};
uint16_t dns_arr[4] = {192, 168, 0, 1};
uint16_t gate_arr[4] = {192, 168, 0, 1};
uint16_t sub_arr[4] = {255, 255, 255, 0};

typedef enum opType {IPADD,DNS,GATE,SUBN,MAC};
/*
Ethernet.begin(mac);
Ethernet.begin(mac, ip);
Ethernet.begin(mac, ip, dns);
Ethernet.begin(mac, ip, dns, gateway);
Ethernet.begin(mac, ip, dns, gateway, subnet);
*/




//IPAddress ip_new(192, 168, 0, 71);

void ethernetSetup(){

	Serial.println(F("Enabling Ethernet"));
    Ethernet.init(SS_ETH);
    //if (Ethernet.begin(mac) == 0) {
      preferences.begin("credentials", false);    //creating namespace "credentials" in EEPROM, false --> R/W mode
      String IP_ADDRESS = preferences.getString("ip_addr","192.168.0.50");
      String DNS_ADDRESS = preferences.getString("dns","192.168.0.1");
      String GATEWAY_ADDRESS = preferences.getString("gateway","192.168.0.1");
      String SUBNET_ADDRESS = preferences.getString("subnet","255.255.255.0");
      String MAC_ADDRESS = preferences.getString("mac_addr","DE:AD:BE:EF:FE:ED");
      preferences.end();
      Serial.print(F("The fetched ip address is:"));
      Serial.println(IP_ADDRESS);
      parse_string(IP_ADDRESS,IPADD);
      parse_string(DNS_ADDRESS,DNS);
      parse_string(GATEWAY_ADDRESS,GATE);
      parse_string(SUBNET_ADDRESS,SUBN);
      parse_string(MAC_ADDRESS,MAC);
      for(int i=0;i<6;i++)
      {
        Serial.println(mac[i]);
      }
      IPAddress ip(ip_arr[0], ip_arr[1], ip_arr[2], ip_arr[3]);

      IPAddress dnServer(dns_arr[0], dns_arr[1], dns_arr[2], dns_arr[3]);
      // the router's gateway address:
      IPAddress gateway(gate_arr[0], gate_arr[1], gate_arr[2], gate_arr[3]);
      // the subnet:
      IPAddress subnet(sub_arr[0], sub_arr[1], sub_arr[2], sub_arr[3]);

      Ethernet.begin(mac, ip, dnServer, gateway, subnet);
      //Ethernet.begin(mac,ip);
      #if 0
    if (Ethernet.begin(mac,ip_arr) == 0) {

     	//Bue LED
    	Serial.println(F("Failed to configure Ethernet using DHCP"));
      	if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      		Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
   		 } else if(Ethernet.linkStatus() == LinkOFF) {
      		Serial.println(F("Ethernet cable is not connected."));

   		 }
     
      	// try to congifure using IP address instead of DHCP:
      	Ethernet.begin(mac, ip);
    }
    #endif
      // print your local IP address:

  

  Serial.print(F("My IP address: "));
 	Serial.println(Ethernet.localIP());

}



void ethernetLoop(){

  if(Ethernet.linkStatus() != LinkON)
  {
    Serial.println(F("[CustomEthernet] Trying to connect to Internet"));
    //cloudConnect_Disp(false);
    //checkForResponse_Disp();
    delay(100);
    flag_ethoffline = true;
    webSocketConncted = false;
  }
  else if((Ethernet.linkStatus() == LinkON) && (flag_ethoffline == true)){
   flag_ethoffline = false;
   
    Serial.println(F("Starting Ethernet Setup again"));
    ethernetSetup();
   // //cloudConnect_Disp(true);
   // //checkForResponse_Disp();
   // delay(500);
  }
	
	/*
  while(Ethernet.linkStatus() != LinkON){
		Serial.println("No Internet");
	//	requestLed(BLUE,START,1); // 1 means 1st connector
    //cloudConnect_Disp(false);
    //checkForResponse_Disp();
    delay(1000);
		Serial.println("LinkStatus == "+ String(Ethernet.linkStatus()));
		delay(500);
		
		if(Ethernet.linkStatus() == LinkON)
		ethernetSetup();
    Serial.println("Starting Ethernet Setup again");
    //cloudConnect_Disp(true);
    //checkForResponse_Disp();
    delay(1000);
	}
  */
}

void parse_string(String s,uint8_t op_type)
{
  Serial.println(F("The received ip is:"));
  Serial.println(s);
  std::string stdStr(s.c_str(), s.length());
  std::string delimiter = ".";
  if(op_type==MAC)
  {
    Serial.println(F("[CustomEthernet] MAC address parsing"));
    delimiter = ":";
    //Serial.print(F("The delimiter is: "));
    //std::cout << delimiter << std::endl;
    //std::string std_del(delimiter.c_str(), delimiter.length());
    //Serial.println(std_del);
  }
  size_t pos = 0;
  std::string token;
   int i = 0;
   int temp=0;
   //typedef enum opType {IPADD,DNS,GATE,SUBN,MAC};
    while ((pos = stdStr.find(delimiter)) != std::string::npos)
        {
          token = stdStr.substr(0, pos);
          char *cstr = new char[token.length() + 1];
          switch (op_type)
          {
          case 0: 
                  ip_arr[i] = atoi(token.c_str());
          break;

          case 1:
                  dns_arr[i] = atoi(token.c_str());    
          break;

          case 2:
                  gate_arr[i] = atoi(token.c_str());  
          break;

          case 3:
                  sub_arr[i] = atoi(token.c_str());
          break;

          case 4: 
                  /*mac[i] = atoi(token.c_str());
                  Serial.println((token.c_str()));
                  Serial.println(mac[i]);*/
                  
strcpy(cstr, token.c_str());
// do stuff
mac[i] = x2i(cstr);
delete [] cstr;
          break;

          default:
          break;
          }
          i++;
          stdStr.erase(0, pos + delimiter.length());
        }
 char *cstr_ = new char[stdStr.length() + 1];
          switch (op_type)
          {
          case 0: 
                  ip_arr[i] =atoi(stdStr.c_str());
          break;

          case 1:
                  dns_arr[i] =atoi(stdStr.c_str());    
          break;

          case 2:
                  gate_arr[i] =atoi(stdStr.c_str());  
          break;

          case 3:
                  sub_arr[i] =atoi(stdStr.c_str());
          break;

          case 4: 
                  /*mac[i] = atoi(stdStr.c_str());
                  Serial.println((stdStr.c_str()));
                  Serial.println(mac[i]);*/
                  
strcpy(cstr_, stdStr.c_str());
// do stuff
mac[i] = x2i(cstr_);
delete [] cstr_;
          break;

          default:
          break;
          }
        
        
}

uint8_t x2i(char *s)
{
  uint8_t x = 0;
  for(;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10;
    }
    else if (c >= 'a' && c <= 'f') {
      x *= 16;
      x += (c - 'a') + 10;
}
    else break;
    s++;
  }
  return x;
}

#endif

void ethernetSetup(){
	Serial.println(F("Enabling Ethernet"));
  Ethernet.init(SS_ETH);   // MKR ETH shield
  // delay(200);

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void ethernetLoop(){

  if(Ethernet.linkStatus() != LinkON)
  {
    Serial.println(F("[CustomEthernet] Trying to connect to Internet"));
    //cloudConnect_Disp(false);
    //checkForResponse_Disp();
    delay(100);
    flag_ethoffline = true;
    webSocketConncted = false;
  }
  else if((Ethernet.linkStatus() == LinkON) && (flag_ethoffline == true)){
   flag_ethoffline = false;
   
    Serial.println(F("Starting Ethernet Setup again"));
    ethernetSetup();
   // //cloudConnect_Disp(true);
   // //checkForResponse_Disp();
   // delay(500);
  }
	
	/*
  while(Ethernet.linkStatus() != LinkON){
		Serial.println("No Internet");
	//	requestLed(BLUE,START,1); // 1 means 1st connector
    //cloudConnect_Disp(false);
    //checkForResponse_Disp();
    delay(1000);
		Serial.println("LinkStatus == "+ String(Ethernet.linkStatus()));
		delay(500);
		
		if(Ethernet.linkStatus() == LinkON)
		ethernetSetup();
    Serial.println("Starting Ethernet Setup again");
    //cloudConnect_Disp(true);
    //checkForResponse_Disp();
    delay(1000);
	}
  */
}
#if 0
void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}


// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /latest.txt HTTP/1.1");
    client.println("Host: www.arduino.cc");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
#endif
#endif