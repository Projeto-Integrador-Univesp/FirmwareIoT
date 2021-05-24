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
String nivelAgua = "&nivelAgua=";

char server[] = "webhook.site";  // also change the Host line in httpRequest()
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;           // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds

//Nivel de água
int cont = 8;
int cont2 = 2;

//Display de 7 segmentos decoder
char disp[10] ={0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

//Sensor Ultrassonico
int echo = A1;
int trigger = A0;

void SetSeg(int valor){
  int v = disp[valor%10];
  for (int c = 2; c <9; c++){
    digitalWrite(c, v % 2);   
    v=v>>1;    
  }  
}

void setup() {

  //Display de 7 segmentos
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  SetSeg(8);

  
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
  pinMode(echo,INPUT);
  pinMode(trigger,OUTPUT);
  digitalWrite(trigger,LOW);
  delayMicroseconds(10);
}

String Str;
float tempo; // para armazenar o tempo de ida e volta do sinal em microsegundos
float distancia_cm; // para armazenar a distância em centímetros

void loop() {
  //leitura da distancia
  // disparar pulso ultrassônico
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  // medir tempo de ida e volta do pulso ultrassônico
  tempo = pulseIn(echo, HIGH);

  // calcular as distâncias em centímetros e polegadas
  distancia_cm = tempo / 29.4 / 2;
  cont = (int) distancia_cm /10;
  SetSeg(cont); // Atualiza o valor no Display de 7 segmentos 
  //if (cont2>9) cont2 = 0;
  
  if(client.connect(HOST_NAME, HTTP_PORT)) {
        // if connected:
        Serial.println("Connected to server");
        // make a HTTP request:
        // send HTTP header
        if(cont<2){ // 2 metros é a altura do poste em relação ao solo
          Str = "True";
          //Serial.println("GET " + PATH_NAME + queryString + (String)(cont<2) +  nivelAgua +  (String) cont + " HTTP/1.1");
          //client.println("GET " + PATH_NAME + queryString + "True" +  nivelAgua +  (String) cont + " HTTP/1.1");
        }
        else{
          Str = "False";
          //Serial.println("GET " + PATH_NAME + queryString + (String)(cont<2) + nivelAgua + (String) distancia_cm + " HTTP/1.1");
          //client.println("GET " + PATH_NAME + queryString + "False" + nivelAgua + (String) cont + " HTTP/1.1");
        }
        Serial.println("display 7SEG " + (String) cont + " ");
        Serial.println("GET " + PATH_NAME + queryString + Str + nivelAgua + (String) distancia_cm + " HTTP/1.1");
        client.println("GET " + PATH_NAME + queryString + Str + nivelAgua + (String) distancia_cm + " HTTP/1.1");
        
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
        //Serial.println();
        //Serial.println("disconnected");
      }else {// if not connected:
        Serial.println("connection failed");
  }

  cont = random(1, 5);
  Serial.println("esperar 6 segundos");
  delay(6000);
  
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
