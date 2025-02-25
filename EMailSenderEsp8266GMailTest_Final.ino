#include "Arduino.h"
#include <EMailSender.h>
#include <ESP8266WiFi.h>

const char* ssid = "santiwifi";
const char* password = "santi1234";
int activador = 0 ;
int antiloop = 0 ;
int activador2 = 0 ;
int antiloop2 = 0 ;
uint8_t connection_state = 0;
uint16_t reconnect_interval = 10000;

EMailSender emailSend("digitalhuelladigital@gmail.com", "edtgbhakmuvamfhp");

uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Conectandose a :  ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);
        Serial.println(nSSID);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
      digitalWrite( 2, HIGH);
        delay(200);
      digitalWrite( 2, LOW);
        Serial.print(".");
        
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
            digitalWrite( 2, LOW);
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    digitalWrite( 2, HIGH);
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}

void setup()
{

  pinMode( 4, INPUT);
  pinMode( 2, OUTPUT);
    Serial.begin(115200);
   
}

void loop()
{

    activador = digitalRead(4) ;




if (digitalRead(4) == HIGH  ) {
activador = 1 ; 
}else{ 
if (digitalRead(4) == LOW ){
activador = 0 ;
antiloop = 0 ;
}
}



        
if (activador == 1 &&  antiloop == 0  ) {
 Serial.println(" Enviando El Mensaje de Alerta ") ;
 Serial.println(" ALARM 01 ON") ;
   EMailSender::EMailMessage message;
    message.subject = "ALERTA 01";
    message.message = "SE HAN REGISTRADO 5 INTENTOS FALLIDOS EN EL INGRESO DE LA CONTRASEÑA ";
    EMailSender::Response resp = emailSend.send("santiagoarteta@gmail.com", message);
    Serial.println("Sending status: ");
    Serial.println(resp.status);
    Serial.println(resp.code);
    Serial.println(resp.desc);
    antiloop = 1 ;
}else{
Serial.println("Alarm 01 OFF " ) ;

}






 

 
  

}
