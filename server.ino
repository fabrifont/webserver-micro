#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
//#include <Arduino.h>
//#include <map>

//std::map<String, int*> lista_variables;

struct mapita{
    String nombre;
    int* valor;
  };


bool leer_datos = 0;
bool datos_legibles = 0;
bool almacenar = 0;
bool enviar_datos = 0;
bool pagina_enviada = 0;
String datos = "";
String buffer_nombre = "";
String buffer_valor = "";
int valor = 0;

int velocidad = 0;
int aceleracion = 0;
int temperatura = 0;
mapita lista_variables[] = {
  {"velocidad", &velocidad},
  {"aceleracion", &aceleracion},
  {"temperatura", &temperatura}
  };

EthernetServer servidor(80);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

void setup() {
  Serial.begin(9600);
  Serial.println("Conectando...");
  Ethernet.begin(mac, ip);
  Serial.println("Conexión a Ethernet realizada exitosamente.");
  servidor.begin();
  Serial.println("Servidor conectado.");
  Serial.print("Direccion IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("");
}

void loop() {
  EthernetClient cliente = servidor.available();  // Captura el cliente conectado
  
  if (cliente) {  // Si hay un cliente conectado
    Serial.println("\nNuevo cliente conectado\n");
    
    String request = "";  // Variable para almacenar la solicitud
    pagina_enviada = 0;
    while (cliente.connected()) {  // Mientras el cliente esté conectado
      
      /*if(datos_legibles){ //algoritmo de lectura e interpretacion de datos desde la solicitud HTTP POST del ESP32
        for(int i = 0; datos[i] != 0; i++) {
            if(datos[i] == '"'){
              i++;
              while(datos[i] != '"'){
                buffer_nombre += datos[i];
                i++;
              }
            }
            if(datos[i] == ':'){
              i+=2;
              while(datos[i] != ',' && datos[i] != 0){
                buffer_valor += datos[i];
                i++;  
              }  
              valor = atoi(buffer_valor.c_str());
              almacenar = 1;
            }
            if(almacenar) {
              for(int j = 0; j < (sizeof(lista_variables)/sizeof(mapita)); j++){
                  if(lista_variables[j].nombre == buffer_nombre) *lista_variables[j].valor = valor;
              }
              almacenar = 0;
            }
          }
        datos = "";
        datos_legibles = 0;
        buffer_nombre = "";
        buffer_valor = "";
        valor = 0;
        almacenar = 0;
      }*/
      
      if (cliente.available()) {  // Si hay datos disponibles del cliente
        char c = cliente.read();  // Lee un byte de datos
        request += c;  // Añade el carácter a la solicitud
        //Serial.print(c);
        /*if(c == "}") leer_datos = 0; datos_legibles = 1; //comienza a tomar datos cuando detecta un {, deja de leer cuando detecta un }
        if(leer_datos){
            datos += c;
          }
        if(c == "{") leer_datos = 1; datos_legibles = 0;*/

        
        
        if (c == '\n') {  // Si se llega al final de la línea (nueva línea)
          Serial.print(request);  // Muestra la línea en el monitor serie
          if(request == "GET /datos") {
            Serial.println("solicitud de datos recibida");
            cliente.print("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\n");
            cliente.println(velocidad);
            enviar_datos = 0;
            
            Serial.println("DATOS ENVIADOS!!!!!!!!!!!!!");
          }
          else if(!pagina_enviada){
            cliente.print("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n<!DOCTYPE HTML>\n<html>\n<h1>Datos del sensor</h1>\n<p>El valor del sensor es: <span id='data'>Esperando datos...</span></p>\n<script>\nfunction actualizarDatos() {\n  fetch('http://190.225.29.144/datos')\n    .then(response => response.text())\n    .then(data => document.getElementById('data').innerText = data);\n}\nsetInterval(actualizarDatos, 5000);\n</script>\n</html>\n");
            cliente.stop();
            pagina_enviada = 1;
          }
          request = "";  // Limpia la variable para la siguiente línea
        }
      }
    }
    
    //cliente.stop();  // Cierra la conexión con el client
    //Serial.println("Cliente desconectado");
  }
}


