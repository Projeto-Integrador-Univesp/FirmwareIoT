#include <SPI.h>
#include <Ethernet.h>

// this must be unique
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// change to your network settings
IPAddress ip(192,168,0,7);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

// change to your server
//IPAddress server(46,4,105,116);// webhook.site //74,125,227,16); // Google
//Change to your domain name for virtual servers
char serverName[] = "webhook.site"; //"www.meuservidor.com.br";

// If no domain name, use the ip address above
// char serverName[] = "74.125.227.16";

// change to your server's port
int serverPort = 80;

EthernetClient client;
int totalCount = 0;
char pageAdd[64];

// set this to the number of milliseconds delay
// this is 30 seconds
#define delayMillis 30000UL

unsigned long thisMillis = 0;
unsigned long lastMillis = 0;

void setup() {
  Serial.begin(9600);

  // disable SD SPI
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);

  // Start ethernet
  Serial.println(F("Inicializando a conexão Ethernet..."));
  Ethernet.begin(mac, ip, gateway, gateway, subnet);

  // If using dhcp, comment out the line above 
  // and uncomment the next 2 lines plus the Ethernet.maintain call in loop

  // if(!Ethernet.begin(mac)) Serial.println(F("failed"));
  // else Serial.println(F("ok"));

  Serial.println(Ethernet.localIP());

  delay(2000);
  Serial.println(F("Pronto"));
}

void loop()
{
  // If using dhcp to get an IP, uncomment the next line
  // Ethernet.maintain();

  thisMillis = millis();

  if(thisMillis - lastMillis > delayMillis)
  {
    lastMillis = thisMillis;

    // Modify next line to load different page
    // or pass values to server
    sprintf(pageAdd,"/",totalCount);

    // sprintf(pageAdd,"/arduino.php?test=%u",totalCount);

    if(!getPage(server,serverPort,pageAdd)) Serial.print(F("Fail "));
    else Serial.print(F("Pass "));
    totalCount++;
    Serial.println(totalCount,DEC);
  }    
}

byte getPage(IPAddress ipBuf,int thisPort, char *page)
{
  int inChar;
  char outBuf[128];

  Serial.print(F("connecting..."));

  /*if(client.connect(ipBuf,thisPort) == 1)
  {
    Serial.println(F("connected"));

    sprintf(outBuf,"GET %s HTTP/1.1",page);
    client.println(outBuf);
    sprintf(outBuf,"Host: %s",serverName);
    client.println(outBuf);
    client.println(F("Connection: close\r\n"));
  } 
  else
  {
    Serial.println(F("failed"));
    return 0;
  }*/
  if (client.connect(server, 80)) {
      Serial.println("connected");
      //Faz uma requisição HTTP
      client.println("GET /#!/dd623f05-f5f7-4bde-b3dc-544667152714/Sensor?aviso=5555 HTTP/1.1");
      client.println("Host: 46.4.105.116"); //192.168.0.1");
      client.println("Connection: close");
      client.println();
  } else {
      //Caso não seja possível obter uma conexao
      Serial.println("connection failed");
  }
  // connectLoop controls the hardware fail timeout
  int connectLoop = 0;

  while(client.connected())
  {
    while(client.available())
    {
      inChar = client.read();
      Serial.write(inChar);
      // set connectLoop to zero if a packet arrives
      connectLoop = 0;
    }

    connectLoop++;

    // if more than 10000 milliseconds since the last packet
    if(connectLoop > 10000)
    {
      // then close the connection from this end.
      Serial.println();
      Serial.println(F("Timeout"));
      client.stop();
    }
    // this is a delay for the connectLoop timing
    delay(1);
  }

  Serial.println();

  Serial.println(F("disconnecting."));
  // close client end
  client.stop();

  return 1;
}
