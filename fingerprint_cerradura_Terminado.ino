#include <Keypad.h> 
#include <Adafruit_Fingerprint.h> 
#include <LiquidCrystal_I2C.h> // Incluir la libreria Keypad
#include <EEPROM.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(51, 50);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
int bypass = 0 ;               //contador huela digital , la cantidad de errores 
int endloop = 0 ;              //detiene el scaner de la huela en caso que no se le ingresara una 
const int buzzer = 10 ;        //buzzer / sonido apuertura / error 
int reedswitch = 11 ;          //alarma sensor 
bool scanning  =  false;      // escaneo del sensor de huellas 
int rojo = 16 ;               // led rojo 
int amarillo = 15 ;            // led amarilo
int verde = 14 ;                // led  verde 
int val = 1 ;  // 1=cerrado 0=abierto   // sensor de alarma
int estado=0;   // 0=cerrado 1=abierto  // variable de control 
int boton = 0 ; // 0=cerrado 1=abierto   // boton 1 posterior
int botonfing = 0 ; // 0=cerrado 1=abierto // boton 2 frontal
int botonfingenrrol = 0 ;      // boton 1 frontal asignar una huella digital
int buttonPushCounter = 0;   // contador del boton 
int buttonState = 0;         // estado actual del boton 
int control = 0 ;           // control de loop evitar repeticion 
int relay = 12 ;            // rele / relay
float contadorintentos = 0 ;
char data [5];              // almacenamiento clave memoria ram 
char eprmsave[5]  ;         // almacenimiento en memoria eeprom
int codigoerror = 0;        // contador de errores de ingreso de clave , si se supera hay una demora de x segundos 
int alertaerror = 0 ;       // se dispara la alerta 
const byte FILAS = 4;          // define numero de filas
const byte COLUMNAS = 4;       // define numero de columnas
char keys[FILAS][COLUMNAS] = {    // define la distribucion de teclas
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinesFilas[FILAS] = {9,8,7,6};         // pines correspondientes a las filas
byte pinesColumnas[COLUMNAS] = {5,4,3,2};  // pines correspondientes a las columnas
Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);  // crea objeto teclado
char TECLA;                        // almacena la tecla presionada
char CLAVE[5];       // almacenamiento clave memoria ram 
char CHECK[5]; // almacena en un array 4 digitos ingresados
char CLAVE_MAESTRA[5] = "0000";    // almacena en un array la contraseña inicial
byte INDICE = 0;                   // indice del array
LiquidCrystal_I2C lcd(0x27,16,2);  
void setup()
{
Serial.begin(9600); 
finger.begin(57600);
  lcd.init();                          
  lcd.backlight(); 
  pinMode(28, OUTPUT) ;
  pinMode(29, OUTPUT) ; 
  pinMode(13, INPUT) ;  
  pinMode(17, INPUT) ;    
  pinMode(18, INPUT) ;   
  pinMode(14, OUTPUT) ;   
  pinMode(15, OUTPUT) ; 
  pinMode(16, OUTPUT) ;
  pinMode(44, INPUT) ;
  pinMode(relay, OUTPUT) ;   
 limpia();
 pinMode(reedswitch, INPUT) ;
}
uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
void loop(){
boton = digitalRead(13);
botonfing = digitalRead(17);
botonfingenrrol = digitalRead(18);
buttonState = digitalRead(18);
  val = digitalRead(reedswitch);
 digitalWrite(verde,LOW);
 digitalWrite(amarillo,LOW);
 digitalWrite(rojo,HIGH);
 digitalWrite(relay,HIGH);
for(int i = 0; i < 4 ;i++){
    data[i] = EEPROM.read(i);
}
  TECLA = teclado.getKey();
  char waitForKey ;
           // obtiene tecla presionada y asigna a variable
  if (TECLA)                  // comprueba que se haya presionado una tecla
  {
    CLAVE[INDICE] = TECLA;    // almacena en array la tecla presionada
    INDICE++;   // incrementa indice en uno
    lcd.print(TECLA);        
    Serial.print(TECLA);         // envia al LCD la tecla presionada
  }
  if(INDICE == 4 )             // si ya se almacenaron los 4 digitos
  {
    if(!strcmp(CLAVE  , data  ))   // compara clave ingresada con data
        abierto();              
    else 
        error();
    INDICE = 0;
  }
  if((digitalRead(13)==1))    // si se pulsa el  boton de Nueva Clave
         CambioDeClave()  ;
  if((digitalRead(17)==1))  {   
 NuevaHuella ()  ;
 }
 if (buttonState == HIGH) {
  do {
 getFingerprintID();
 }
 while (buttonState  != LOW );
control = 0; 
  }

if((digitalRead(44)==1))  {   
 abierto()  ;
 }

if (val == 0 ) {
do {
contadorintentos = contadorintentos + 1 ;
Serial.print (contadorintentos) ; 
digitalWrite(29,HIGH) ;
}
while (contadorintentos < 2000 )  ;
digitalWrite(29,LOW) ;
contadorintentos = 0 ;  
}




}
///////////////////// Error //////////////////////////////
void error(){   
    lcd.setCursor(0,1);
    lcd.print("ERROR DE CLAVE    ");
    Serial.println("ERROR DE CLAVE    "); 
    delay(1000);   
codigoerror = codigoerror +1 ;
if (codigoerror == 5 ) {
lcd.setCursor(0,0);
lcd.print("DEMACIADOS         ");
lcd.setCursor(0,1);
lcd.print("INTENTOS           ");
do {
contadorintentos = contadorintentos + 1 ;
digitalWrite(28,HIGH) ;
codigoerror = 0 ;
Serial.print (contadorintentos) ; 
}
while (contadorintentos < 10000 )  ;
digitalWrite(28,LOW) ;
contadorintentos = 0 ; 
}
      tone(buzzer,100);
        delay(250);
        noTone(buzzer); 
         
 digitalWrite(rojo,HIGH);
 delay(100);
 digitalWrite(rojo,LOW);
  delay(100);
 digitalWrite(rojo,HIGH);
 delay(100);
 digitalWrite(rojo,LOW);
 delay(100);
 digitalWrite(rojo,HIGH);
      limpia();
}
/////////////////////abierto o cerrado //////////////////////////////
void abierto(){  
  
  codigoerror = 0 ; 
   if(estado==0 && val == 1 ){                                            //estado=0;   // 0=cerrado 1=abierto
      estado=1;
      lcd.setCursor(0,0);
      lcd.print("Clave Correcta     ");
      lcd.setCursor(0,1);
      lcd.print("Abierto            ");  // imprime en el LCD que esta abierta
      Serial.println("      ");
      Serial.println("Abierto            ");  // imprime en el LCD que esta abierta
      digitalWrite(relay,LOW);
digitalWrite(rojo,LOW);
digitalWrite(verde,HIGH);
 delay(100);
 digitalWrite(verde,LOW);
 delay(100);
 digitalWrite(verde,HIGH);
 delay(100);
 digitalWrite(verde,LOW);
 delay(100);
 digitalWrite(verde,HIGH);
 delay(100);
      tone(buzzer,256);
      delay(2000);
      noTone(buzzer);
       delay(6000);
digitalWrite(relay,HIGH);       
 digitalWrite(verde,LOW);
 delay(100) ;    
digitalWrite(rojo,HIGH);
} 
if (digitalRead(11) == 0 && estado == 1  ){
do {
digitalRead(11); 
     lcd.setCursor(0,0);
     lcd.print("Estado Actual :");  
     lcd.setCursor(0,1);
     lcd.print("Abierto"            );  
Serial.println("Abiertoo            ") ;
 digitalWrite(verde,HIGH);
 delay(100) ;    
digitalWrite(rojo,LOW);
}
while (digitalRead(11) == 0 && estado == 1  ) ;
}
Serial.println("Termino            ") ;
if (val == 1 ) {
 estado = 0 ;
 lcd.setCursor(0,0);
 lcd.print("Cerrado            "); 
 lcd.setCursor(0,1);
 lcd.print("                   "); 
 Serial.println("cerrado            ") ;
digitalWrite(verde,LOW);
digitalWrite(rojo,HIGH);
delay (2000) ;
}
limpia();
}
/////////////////////Nueva_Clave //////////////////////////////
void nueva_clave(){  
  codigoerror = 0 ; 
   lcd.setCursor(0,1);
 lcd.print("                     ");
 lcd.setCursor(0,0);
  lcd.print("NUEVA CLAVE:        ");
  lcd.setCursor(12,0);
  digitalWrite(verde,HIGH);
 digitalWrite(amarillo,HIGH);
 digitalWrite(rojo,LOW);
  Serial.println("NUEVA CLAVE:        ");
 tone(buzzer,256);
 delay(200) ;
 noTone(buzzer);
 digitalWrite(amarillo,HIGH);
 
 digitalWrite(verde,HIGH);
 
 digitalWrite(rojo,LOW);
  INDICE=0;
  while (INDICE<=3) {
   TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable TECLA
   if (TECLA)                 // comprueba que se haya presionado una tecla
    {
      CLAVE_MAESTRA[INDICE] = TECLA;    // almacena en array la tecla presionada
      CLAVE[INDICE] = TECLA;
      EEPROM.write(INDICE,CLAVE_MAESTRA[INDICE]);
      INDICE++; // incrementa indice en uno
       Serial.print(TECLA); 
        lcd.print(TECLA);  // envia a monitor serial la tecla presionada
    }   
  }
 estado=0;
tone(buzzer,250);
 delay(200) ;
 noTone(buzzer);
 delay(200) ;
  Serial.println("CLAVE CAMBIADA");
    lcd.setCursor(0,1);
  lcd.print("CLAVE CAMBIADA");
  finger.emptyDatabase();
  delay (2000) ; 
  lcd.setCursor(0,1);
  lcd.print("HUELLAS ELIMINADAS");
 digitalWrite(verde,HIGH);
 delay(200) ; 
 digitalWrite(verde,LOW);
  delay(200) ; 
 digitalWrite(verde,HIGH);
  delay(200) ; 
 digitalWrite(verde,LOW);
  delay(200) ; 
 digitalWrite(verde,HIGH);
  delay(200) ; 
 digitalWrite(rojo,LOW);
 digitalWrite(verde,LOW);
 digitalWrite(rojo,HIGH);
  delay(2000);
  limpia();
}
///////////////////// limpia //////////////////////////////
void limpia()
{  
 lcd.setCursor(0,0);
 lcd.print("CLAVE :          ");
 lcd.setCursor(0,1);
 lcd.print("                 ");
 lcd.setCursor(9,0);
 Serial.println("CLAVE :          ");
}


//// agregar huellas digitales Nuevas 

uint8_t getFingerprintEnroll() {
        tone(buzzer,256);
digitalWrite(verde,HIGH);
digitalWrite(amarillo,LOW);
digitalWrite(rojo,LOW);
      delay(1500);
      noTone(buzzer);
digitalWrite(verde,LOW);
digitalWrite(amarillo,LOW);
digitalWrite(rojo,HIGH);
  int p = -1;
lcd.setCursor(0,0);
lcd.print("COLOQUE EL DEDO  ");
lcd.setCursor(0,1);
lcd.print("EN EL ESCANER    ");
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
lcd.setCursor(0,1);
lcd.print("IMAGEN CAPTURADA  ");
lcd.setCursor(0,1);
lcd.print("                ");
      Serial.println("Image taken  ");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
lcd.setCursor(0,0);
lcd.print("COLOQUE EL DEDO  ");
lcd.setCursor(0,1);
lcd.print("EN EL ESCANER    ");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE       ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION   ");
delay(1000);
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
lcd.setCursor(0,0);
lcd.print("ERROR           ");
lcd.setCursor(0,1);
lcd.print("CAPTURANDO IMAGEN");
delay(1000);
      break;
    default:
      Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO   ");
lcd.setCursor(0,1);
lcd.print("                 ");
delay(1000);
      break;
    }
  }
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
lcd.setCursor(0,0);
lcd.print("IMAGEN RECIBIDA     ");
lcd.setCursor(0,1);
lcd.print("                     ");
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
lcd.setCursor(0,0);
lcd.print("IMAGEN BORROSA   ");
lcd.setCursor(0,1);
lcd.print("                 ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE         ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION      ");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
lcd.setCursor(0,0);
lcd.print("ERROR ENCONTRANDO ");
lcd.setCursor(0,1);
lcd.print("LA HUELLA         ");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
lcd.setCursor(0,0);
lcd.print("ERROR ENCONTRANDO ");
lcd.setCursor(0,1);
lcd.print("LA HUELLA         ");
      return p;
    default:
      Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO ");
lcd.setCursor(0,1);
lcd.print("                  ");
      return p;
  }
  lcd.setCursor(0,0);
lcd.print("RETIRA EL DEDO ");
lcd.setCursor(0,1);
lcd.print("                  ");
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
   lcd.setCursor(0,0);
lcd.print("COLOCA EL MISMO  ");
lcd.setCursor(0,1);
lcd.print("DEDO AL ESCANER  ");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
   lcd.setCursor(0,0);
lcd.print("IMAGEN CAPTURADA  ");
lcd.setCursor(0,1);
lcd.print("                  ");
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
lcd.setCursor(0,0);
lcd.print("COLOQUE EL DEDO  ");
lcd.setCursor(0,1);
lcd.print("EN EL ESCANER    ");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE        ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION    ");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
lcd.setCursor(0,0);
lcd.print("ERROR DE      ");
lcd.setCursor(0,1);
lcd.print("IMAGEN        ");
      break;
    default:
      Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO      ");
lcd.setCursor(0,1);
lcd.print("                       ");
      break;
    }
  }
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
lcd.setCursor(0,0);
lcd.print("IMAGEN CONVERTIDA      ");
lcd.setCursor(0,1);
lcd.print("                       ");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
lcd.setCursor(0,0);
lcd.print("IMAGEN BORROSA         ");
lcd.setCursor(0,1);
lcd.print("                       ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE               ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION            ");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
lcd.setCursor(0,0);
lcd.print("NO SE ENCONTRARON               ");
lcd.setCursor(0,1);
lcd.print("COINCIDENCIAS                    ");
delay(1000);
lcd.setCursor(0,0);
lcd.print("INTENTE NUEVAMENTE               ");
lcd.setCursor(0,1);
lcd.print("                                 ");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
lcd.setCursor(0,0);
lcd.print("NO SE ENCONTRARON               ");
lcd.setCursor(0,1);
lcd.print("COINCIDENCIAS                   ");
delay(1000);
lcd.setCursor(0,0);
lcd.print("INTENTE NUEVAMENTE               ");
lcd.setCursor(0,1);
lcd.print("                                 ");
      return p;
    default:
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO              ");
lcd.setCursor(0,1);
lcd.print("                               ");
      Serial.println("Unknown error");
delay(1000);
limpia;
      return p;
  }
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
 lcd.setCursor(0,0);
lcd.print("CREANDO ARCHIVO   ");
lcd.setCursor(0,1);
lcd.print("ID:"); 
lcd.setCursor(6,1);
lcd.print(id); 
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
 lcd.setCursor(0,0);
lcd.print("LAS IMAGENES       ");
lcd.setCursor(0,1);
lcd.print("CONCUERDAN          "); 
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
 lcd.setCursor(0,0);
lcd.print("ERROR DE     ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION ");
delay(1000);
lcd.setCursor(0,0);
lcd.print("INTENTE DE NUEVO               ");
lcd.setCursor(0,1);
lcd.print("                                 ");
delay(1000);
limpia();
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
 lcd.setCursor(0,0);
lcd.print("LAS IMAGENES");
lcd.setCursor(0,1);
lcd.print("NO CONCUERDAN  "); 
delay(1000);
lcd.setCursor(0,0);
lcd.print("INTENTE DE NUEVO               ");
lcd.setCursor(0,1);
lcd.print("                                 ");
delay(1000);
limpia();
    return p;
  } else {
Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO");
lcd.setCursor(0,1);
lcd.print("                  ");
delay(1000);
lcd.setCursor(0,0);
lcd.print("INTENTE DE NUEVO               ");
lcd.setCursor(0,1);
lcd.print("                                 ");
delay(1000);
limpia();
    return p;
  }   
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
lcd.setCursor(0,0);
lcd.print("HUELLA GUARDADA       ");
lcd.setCursor(0,1);
lcd.print("ID                    ");
lcd.setCursor(6,1);
lcd.print(id);
tone(buzzer,250);
 delay(200) ;
 noTone(buzzer);
 tone(buzzer,250);
 delay(200) ;
 noTone(buzzer);
delay (1000);
    id = id + 1 ;
limpia();
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE            ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION        ");
delay (1000);
limpia();
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
lcd.setCursor(0,0);
lcd.print("NO SE PUDO            ");
lcd.setCursor(0,1);
lcd.print("GUARDAR LA HUELA        ");
delay (1000);
limpia();
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
lcd.setCursor(0,0);
lcd.print("ERROR GUARDANDO          ");
lcd.setCursor(0,1);
lcd.print("EN LA MEMORIA            ");
delay (1000);
limpia();
    return p;
  } else {
    Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR                    ");
lcd.setCursor(0,1);
lcd.print("DESCONOCIDO              ");
delay (1000);
limpia();
    return p;
  }   
}


// Abrir la huella Digital


uint8_t getFingerprintID() {
scanning = true;
  uint8_t p = finger.getImage();
  lcd.setCursor(0,0);
  lcd.print("ESCANEANDO.......");
    lcd.setCursor(0,1);
  lcd.print("                 ");
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
       lcd.setCursor(0,0);
       lcd.print("IMAGEN CAPTURADA");
       delay(1000);
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      endloop = endloop + 1 ;
      if (endloop > 60 ) {
       buttonState = LOW;
       exit ;
      endloop = 0;
      }
      return p;    
    case FINGERPRINT_PACKETRECIEVEERR:
    lcd.setCursor(0,0);
lcd.print("ERROR DE       ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION   ");
delay(1000);
      Serial.println("Communication error");
        endloop = endloop + 1 ;
      if (endloop > 60 ) {
       buttonState = LOW;
       exit ;
      endloop = 0;
      }
      return p;
    case FINGERPRINT_IMAGEFAIL:
lcd.setCursor(0,0);
lcd.print("ERROR DE           ");
lcd.setCursor(0,1);
lcd.print("IMAGEN             ");
delay(1000);
      Serial.println("Imaging error");
        endloop = endloop + 1 ;
      if (endloop > 60 ) {
       buttonState = LOW;
       exit ;
      endloop = 0;
      }
      return p;
    default:
lcd.setCursor(0,0);
lcd.print("ERROR          ");
lcd.setCursor(0,1);
lcd.print("DESCONOCIDO    ");
delay(1000);
      Serial.println("Unknown error");
        endloop = endloop + 1 ;
      if (endloop > 60 ) {
       buttonState = LOW;
       exit ;
      endloop = 0;
      }
      return p;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
lcd.setCursor(0,0);
lcd.print("LA IMAGEN SE  HA   ");
lcd.setCursor(0,1);
lcd.print("CONVERTIDO        ");
delay(1000);
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
    lcd.setCursor(0,0);
lcd.print("IMAGEN MUY SUCIA");
lcd.setCursor(0,1);
lcd.print("                ");
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
lcd.setCursor(0,0);
lcd.print("ERROR DE ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION   ");
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
lcd.setCursor(0,0);
lcd.print("NO SE ENCONTRO ");
lcd.setCursor(0,1);
lcd.print("LA HUELLA      ");
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
lcd.setCursor(0,0);
lcd.print("NO SE ENCONTRO ");
lcd.setCursor(0,1);
lcd.print("LA HUELLA      ");
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
lcd.setCursor(0,0);
lcd.print("ERROR DESCONOCIDO");
lcd.setCursor(0,1);
lcd.print("                 ");
      return p;
  }
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
lcd.setCursor(0,0);
lcd.print("SE ENCONTRO");
lcd.setCursor(0,1);
lcd.print("LA HUELLA");
        abierto();
    buttonState = LOW;
    exit ;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
lcd.setCursor(0,0);
lcd.print("ERROR DE ");
lcd.setCursor(0,1);
lcd.print("COMUNICACION   ");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
lcd.setCursor(0,0);
lcd.print("NO SE ENCONTRO");
lcd.setCursor(0,1);
lcd.print("LA HUELLA");
    Serial.println("Did not find a match");
    bypass = bypass + 1 ;
    if ( bypass == 3 ) {
    error();
    buttonState = LOW;
    bypass = 0 ;
      exit ; 
    }else{
    return p;
    }
  } else {
lcd.setCursor(0,0);
lcd.print("ERROR");
lcd.setCursor(0,1);
lcd.print("DESCONOCIDO");
    Serial.println("Unknown error");
    return p;
    error();
    buttonState = LOW;
    exit ;
  }   
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  delay (2000) ;
limpia(); 
  return finger.fingerID;
    }
// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
lcd.setCursor(0,0);
lcd.print(" ID ENCONTRADO :");
lcd.setCursor(0,1);
lcd.print(finger.fingerID);
delay(1000);
lcd.setCursor(0,0);
lcd.print("coincidencia del:");
lcd.setCursor(0,1);
lcd.print(finger.confidence);
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
  control  =  control  + 1 ;
}  

// Agregar Huella Nueva


void NuevaHuella (){ 
 digitalWrite(verde,LOW);
 digitalWrite(amarillo,HIGH);
 digitalWrite(rojo,LOW);
Serial.println("INGRESE CLAVE ACTUAL: "); 
  Serial.println(data);
lcd.setCursor(0,0);
lcd.print("CLAVE ACTUAL:") ;
lcd.setCursor(6,1);
INDICE=0;
  while (INDICE<=3) {
   TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable TECLA
   if (TECLA)                 // comprueba que se haya presionado una tecla
    {
      CHECK[INDICE] = TECLA;    // almacena en array la tecla presionada
      INDICE++;                 // incrementa indice en uno
       Serial.print(TECLA); 
       lcd.print(TECLA);
       // envia a monitor serial la tecla presionada
    }   
  }
    if(INDICE == 4 )             // si ya se almacenaron los 4 digitos
  {
    if(!strcmp(CHECK, data))    // compara clave ingresada con clave maestra
    
        getFingerprintEnroll();               
    else 
        error();
        digitalWrite(amarillo,LOW);
    INDICE = 0;
  }
}


// Cambiar La Clave y borrar Huellas 
void CambioDeClave (){ 
 digitalWrite(verde,LOW);
 digitalWrite(amarillo,HIGH);
 digitalWrite(rojo,LOW);
Serial.println("INGRESE CLAVE ACTUAL: "); 
  Serial.println(data);
lcd.setCursor(0,0);
lcd.print("CLAVE DE FABRICA:") ;
lcd.setCursor(6,1);
INDICE=0;
  while (INDICE<=3) {
   TECLA = teclado.getKey();   // obtiene tecla presionada y asigna a variable TECLA
   if (TECLA)                 // comprueba que se haya presionado una tecla
    {
      CHECK[INDICE] = TECLA;    // almacena en array la tecla presionada
      INDICE++;                 // incrementa indice en uno
       Serial.print(TECLA); 
       lcd.print(TECLA);
       // envia a monitor serial la tecla presionada
    }   
  }
    if(INDICE == 4 )             // si ya se almacenaron los 4 digitos
  {
    if(!strcmp(CHECK, CLAVE_MAESTRA))    // compara clave ingresada con clave maestra 
        nueva_clave();               
    else 
        error();
        digitalWrite(amarillo,LOW);
    INDICE = 0;
  }
}
