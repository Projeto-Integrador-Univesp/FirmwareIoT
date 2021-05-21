#include <SPI.h>
#include <Ethernet.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 2);
IPAddress myDns(192, 168, 0, 1);

// initialize the library instance:
EthernetClient client;

/*
int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "webhook.site";
String PATH_NAME   = "/c454626d-623e-406f-9d2a-1087e8c34595/";
String queryString = "?nr_sensor=226&valor=";
*/
int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "enchente.azurewebsites.net";
String PATH_NAME   = "/api/RecebeDados/Enchente/";
String queryString = "?enchente=";

char server[] = "webhook.site";  // also change the Host line in httpRequest()
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds
int cont = 2;


void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  

  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Inicializando Ethernet com DHCP:");
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Falha ao configurar a Ethernet usando DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield não identificada.  Verifique se a Shield foi bem encaixada.");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cabo não conectado.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    Serial.print("Meu endereço IP é: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP definiu o IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(10000);
}

void loop() {
  
  if(client.connect(HOST_NAME, HTTP_PORT)) {
        // if connected:
        Serial.println("Connected to server");
        // make a HTTP request:
        // send HTTP header
        if(cont<3){
          Serial.println("GET " + PATH_NAME + queryString + "True" + " HTTP/1.1 " + (String) cont);
          client.println("GET " + PATH_NAME + queryString + "True" + " HTTP/1.1");
        }
        else{
          Serial.println("GET " + PATH_NAME + queryString + "False" + " HTTP/1.1 "  + (String) cont);
          client.println("GET " + PATH_NAME + queryString + "False" + " HTTP/1.1");
        }
        client.println("Host: " + String(HOST_NAME));
        client.println("Connection: close");
        client.println(); // end HTTP header
    
        while(client.connected()) {
          if(client.available()){
            // read an incoming byte from the server and print it to serial monitor:
            char c = client.read();
            //Serial.print(c);
          }
        }
    
        // the server's disconnected, stop the client:
        client.stop();
        Serial.println();
        Serial.println("disconnected");
      }else {// if not connected:
        Serial.println("connection failed");
  }

  cont = random(0, 10);
  delay(60000);
  /*
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
/*
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
    client.println("GET /c454626d-623e-406f-9d2a-1087e8c34595/Sensor?valor=151 HTTP/1.1");
    //client.println("Host: www.arduino.cc");
    //client.println("User-Agent: arduino-ethernet");
    //client.println("Connection: close");
    //client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  /**/
}
