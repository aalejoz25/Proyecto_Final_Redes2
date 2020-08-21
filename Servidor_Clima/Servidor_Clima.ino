#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#define DHTTYPE DHT11 // DHT 11 //Definimos el tipo de sensor

String XML, xmlTemperatura, xmlHumedad; // Creacion de variables globaes para crear XML y almacenar Temp y Humedad
unsigned long previousmillis = 0 ; // Variable global para intervalo de tiempo de lectura
uint8_t DHTPin = D2; // Variable para definir pin del NodeMCU
DHT dht(DHTPin, DHTTYPE); // Se crea el objeto dht donde se pasa el pin y el tipo de sensor
/*_____________________________________________________________________________________________________________*/
const char* ssid = "Hugorin 3.0";      // Nombre de la red Wifi
const char* password = "c0ntr@sen@.nuev@";  //Contraseña de la red
ESP8266WebServer server(4321); //Creacion del objeto servidor, parametro: puerto
/*_____________________________________________________________________________________________________________*/
String sitioWeb = "<!DOCTYPE html>" //Almacenamiento de la pagina web en un string
"<html lang='es'>"

"<head>"
    "<title>Proyecto Final Redes 2</title>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<script src='https://code.jquery.com/jquery-2.1.1.min.js' integrity='sha256-h0cGsrExGgcZtSZ/fRz4AwV+Nn6Urh/3v3jFRQ0w9dQ=' crossorigin='anonymous'></script>"
    "<script>"
        "var temp;"
        "var hum;"
        "$(document).on('ready', function() {"
            "setInterval(function() {"
                "$.ajax({"
                    "url: 'datos.xml',"
                    "dataType: 'XML',"
                    "success: procesarXML"
                "});"

                "function procesarXML(xml) {"
                    "temp = $(xml).find('Temperatura').text();"
                    "hum = $(xml).find('Humedad').text();"
                    "$('#temperatura').text(temp.concat('ºC'));"
                    "$('#humedad').text(hum.concat('%'));"
                "}"
            "}, 3000);"
        "});"
    "</script>"

    "<style>"
        "html {"
            "font-family: Helvetica;"
            "display: inline-block;"
            "margin: 0px auto;"
            "text-align: center;"
        "}"
        
        "body {"
            "margin-top: 50px;"
        "}"
        
        "h1 {"
            "color: #444444;"
            "margin: 50px auto 30px;"
        "}"
        
        "p {"
            "font-size: 24px;"
            "color: #444444;"
            "margin-bottom: 10px;"
        "}"
    "</style>"
"</head>"
"<body>"
    "<div id='webpage'>"
        "<h1>Estacion de Clima</h1>"
        "<p>Temperatura: </p>"
        "<p id='temperatura'>...</p>"
        "<p>Humedad: </p>"
        "<p id='humedad'>...</p>"
    "</div>"
"</body>"
"</html>";


void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(DHTPin, INPUT);
  dht.begin();
  Serial.println("Conectandose a: ");
  Serial.print(ssid);
  /*_______________________________________________________________________________*/
  //Configuracion de IP estatica
  IPAddress IP(192, 168, 0, 213); 
  IPAddress gateway(192, 168, 0, 1);   
  IPAddress subnet(255, 255, 255, 0);  
  IPAddress dns(8, 8, 8, 8);  
  WiFi.disconnect();  
  WiFi.config(IP, subnet, gateway, dns);
  
  //Verificacion de conexion a la red Wifi
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado..!");
  Serial.print("IP Obtenida: "); Serial.println(WiFi.localIP());
  /*_______________________________________________________________________________*/
  //Manejo del servidor
  server.on("/", handleWebsite); //crea el fichero html
  server.on("/datos.xml", handleXML); //crea el fichero XML
  server.onNotFound(handle_NotFound); //en caso de una ruta no existente "error"
  server.begin(); //inicia el servidor
  Serial.println("Servidor HTTP iniciado");
  /*_______________________________________________________________________________*/
}
void loop() {

  unsigned long currentMillis = millis(); //tiempo en milisegundos desde que se encendió la tarjeta

//Lectura de  datos cada segundo
  if (currentMillis - previousmillis >= 1000) {
    previousmillis = currentMillis;
    float temp = dht.readTemperature(); 
    float hum = dht.readHumidity();  
    xmlTemperatura = String(temp, 1);
    xmlHumedad = String(hum,0);
    
  }
  server.handleClient();//Manejo de solicitudes entrantes de los clientes
}

void handleWebsite() {
  server.send(200, "text/html", sitioWeb);//envia el tipo de arvhivo al servidor
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");//envio del error de ruta
}

void construirXML() { //funcion que construye XML
  XML = "";
  XML += "<?xml version='1.0' encoding='UTF-8'?>";
  XML += "<DHT11>";
  XML += "<Datos>";
  XML += "<Temperatura>";
  XML += xmlTemperatura;
  XML += "</Temperatura>";
  XML += "<Humedad>";
  XML += xmlHumedad;
  XML += "</Humedad>";
  XML += "</Datos>";
  XML += "</DHT11>";
}

void handleXML() { //envia el XML construido al servidor
  construirXML();
  server.send(200, "text/xml", XML);
}
