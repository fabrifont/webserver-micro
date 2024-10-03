#include <Ethernet.h>
#include <SPI.h>

String request = "";  // Variable para almacenar la solicitud
struct mapita{
    String nombre;
    int* valor;
  };


bool leer_datos = 0;
bool datos_legibles = 0;
bool almacenar = 0;
bool enviar_datos = 0;
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
  Serial.print("Dirección IP: ");
  Serial.println(Ethernet.localIP());
  Serial.println("");
}

void loop() {
  EthernetClient cliente = servidor.available();  // Captura el cliente conectado
  
  if (cliente) {  // Si hay un cliente conectado
    Serial.println("\nNuevo cliente conectado\n");
    request = "";
    
    // Lee la solicitud del cliente
    while (cliente.connected()) {
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
      if (cliente.available()) {
        char c = cliente.read();
        request += c;  // Agrega el carácter a la solicitud
        /*if(c == '}') leer_datos = 0; datos_legibles = 1; //comienza a tomar datos cuando detecta un {, deja de leer cuando detecta un }
        if(leer_datos){
            datos += c;
          }
        if(c == '{') leer_datos = 1; datos_legibles = 0;*/
        // Si hemos llegado al final de la solicitud (indicado por un doble salto de línea)
        if (c == '\n' && request.endsWith("\r\n\r\n")) {
          Serial.println(request);  // Imprime la solicitud completa
          
          // Verifica si es una solicitud a la página principal "/"
          if (request.indexOf("GET / ") >= 0) {
            enviarPaginaHTML(cliente);
          }
          // Verifica si es una solicitud de datos "/datos"
          else if (request.indexOf("GET /datos") >= 0) {
            enviarDatosSensor(cliente);
          }

          break;  // Sale del ciclo una vez que se procesa la solicitud
        }
      }
    }
    
    cliente.stop();  // Cierra la conexión con el cliente
    Serial.println("Cliente desconectado");
  }
}

// Función para enviar la página HTML
void enviarPaginaHTML(EthernetClient& cliente) {
  cliente.print("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n");
  cliente.print("<!DOCTYPE HTML>\n<html>\n<h1>Datos del sensor</h1>\n");
  cliente.print("<p>El valor del sensor es: <span id='data'>Esperando datos...</span></p>\n");
  cliente.print("<script>\nfunction actualizarDatos() {\n");
  cliente.print("  fetch('/datos')\n");
  cliente.print("    .then(response => response.text())\n");
  cliente.print("    .then(data => document.getElementById('data').innerText = data);\n");
  cliente.print("}\nsetInterval(actualizarDatos, 5000);\n</script>\n</html>\n");
}

// Función para enviar los datos del sensor
void enviarDatosSensor(EthernetClient& cliente) {
  cliente.print("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\n");
  cliente.println(velocidad);  // Envía el valor de la variable "velocidad"
  Serial.println("Datos enviados");
}

