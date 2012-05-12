// This demo does web requests to a fixed IP address, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,3,203 }; // nanode ip
// gateway ip address
static byte gwip[] = { 192,168,3,1 }; // gateway (router) ip
// remote website ip address and port
static byte hisip[] = { 192,168,3,1 }; // ip of the site you request
// remote website name
char website[] PROGMEM = "192.168.3.1"; // the hostname of the url of the site you request

// ~guidocalvano/
static BufferFiller bfill;  // used as cursor while filling the buffer
byte Ethernet::buffer[1000];   // a very small tcp/ip buffer is enough here
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[getStaticIP]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  ether.staticSetup(myip, gwip);

  ether.copyIp(ether.hisip, hisip);
  ether.printIp("Server: ", ether.hisip);

  while (ether.clientWaitingGw())
    ether.packetLoop(ether.packetReceive());
    
  Serial.println("Gateway found");
  
  timer = - REQUEST_RATE; // start timing out right away
}

char page[] PROGMEM = "HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "No worries"
  "</title></head>"
  "<body>"
    "<h3>This service is currently fine</h3>"
    "<p><em>"
      "The main server is currently on-line.<br />"
    "</em></p>"
  "</body>"
"</html>"
;

char* nextLine( char* bufferOffset )
  {
   while( *bufferOffset != '\n' )
     bufferOffset++ ;
   
   bufferOffset++ ;
   
   return bufferOffset ; 
  } ;


void loop () {
  word len = ether.packetReceive() ;
  word pos = ether.packetLoop(len) ;

if( pos )
  {
    Serial.println( "start of packet\n" );
    bfill = ether.tcpOffset();
    char* dataOffset = (char*) Ethernet::buffer + pos ;
    
    Serial.println( dataOffset ) ;
    
    char* next = nextLine( nextLine( nextLine( nextLine( nextLine( nextLine( dataOffset ) ) ) ) ) ) ;
    Serial.println( "payload" ) ;
    Serial.println( next ) ;
    
    Serial.print( "x" ) ; Serial.println( (int) next[ 0 ] ) ;
    Serial.print( "y" ) ; Serial.println( (int) next[ 1 ] ) ;
    Serial.print( "r" ) ; Serial.println( (int) next[ 2 ] ) ;
    Serial.print( "g" ) ; Serial.println( (int) next[ 3 ] ) ;
    Serial.print( "b" ) ; Serial.println( (int) next[ 4 ] ) ;
    
    Serial.println( "end of packet\n" );
    
    memcpy_P(ether.tcpOffset(), page, sizeof page);
    ether.httpServerReply(sizeof page - 1);
  }
  /*
  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    // Serial.println(PSTR("/~guidocalvano/"));
    ether.browseUrl(PSTR("/~guidocalvano/"), "", website, my_result_cb);
   
  }*/
}
