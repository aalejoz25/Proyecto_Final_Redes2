
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
//#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321

String XML, xmlTemperatura, xmlHumedad;
unsigned long previousmillis = 0 ;
uint8_t DHTPin = D2;
DHT dht(DHTPin, DHTTYPE);
const char* ssid = "Hugorin 3.0";      // Aqui va ti identificador
const char* password = "c0ntr@sen@.nuev@";  //Aqui va tu contraseña
ESP8266WebServer server(80);
String sitioWeb = "<!DOCTYPE html>"
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
  //Configuracion de IP estatica
  IPAddress staticIP(192, 168, 0, 213); //ESP static ip
  IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
  IPAddress subnet(255, 255, 255, 0);  //Subnet mask
  IPAddress dns(8, 8, 8, 8);  //DNS
  WiFi.disconnect();  //Prevent connecting to wifi based on previous configuration
  WiFi.config(staticIP, subnet, gateway, dns);
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  while (WiFi.status() != WL_CONNECTED) { //check wi-fi is connected to wi-fi network
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado..!");
  Serial.print("IP Obtenida: "); Serial.println(WiFi.localIP());

  server.on("/", handleWebsite);
  server.on("/datos.xml", handleXML);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");

}
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousmillis >= 1000) {
    previousmillis = currentMillis;
    float temp = dht.readTemperature(); 
    float hum = dht.readHumidity();  
    xmlTemperatura = String(temp, 1);
    xmlHumedad = String(hum,0);
    
  }

  server.handleClient();

}

void handleWebsite() {
  server.send(200, "text/html", sitioWeb);
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void construirXML() {
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

void handleXML() {
  construirXML();
  server.send(200, "text/xml", XML);
}
