#include <Ethernet.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define millisEspera 1000

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);
EthernetServer servidor(80);

//VARIABLES A ANALIZAR Y MOSTRAR EN LA WEB
String json = "{\"temperatura\":0.0, \"humedad\":0, \"color\":\"Ninguno\", \"velocidad\":0}";
float temperatura = 0;
int humedad = 0;
const char* color = "";
double velocidad = 0;

//PARA TIMEOUTS EN BUCLES
long millisActual = 0;
long millisUlt = 0;

//CÓDIGO DE LA PÁGINA (NO ENTRA EN UNA SOLA VARIABLE)
//PROGMEM -> ALMACENA TODO EN LA MEMORIA FLASH PORQUE SINO AL OCUPAR TANTO ESPACIO LA RAM SE SATURA
const char htmlPagina1[] PROGMEM = "<html><head><title>Interfaz Web - Microprocesadores</title><style>body { background-color: lime; font-family: 'Trebuchet MS'; height: 100%; margin: 0; display: flex; justify-content: center; align-items: center; } .bloque { background-color: white; color: black; font-size: 20px; border-radius: 5px; width: 50%; height: 50%; top: 50%; left: 50%; text-align: center; padding: 20px; }</style></head><body><div class='bloque'><h1>Datos del sensor</h1><p>Color:";
const char htmlPagina2[] PROGMEM = " <span id='color'>Esperando datos...</span></p><p>Temperatura: <span id='temperatura'>Esperando datos...</span> °C</p><p>Humedad: <span id='humedad'>Esperando datos...</span> %</p><p>Velocidad: <span id='velocidad'>Esperando datos...</span> km/h</p></div><script>function actualizarDatos() { fetch('/datos').then(response => response.json()).then(datos => { document.getElementById('temperatura').innerText = datos.temperatura;";
const char htmlPagina3[] PROGMEM = " document.getElementById('humedad').innerText = datos.humedad; document.getElementById('color').innerText = datos.color; document.getElementById('velocidad').innerText = datos.velocidad; }).catch(error => console.error('Error al obtener los datos:', error)); } setInterval(actualizarDatos, 1000);</script></body></html>";

void setup() {
    Serial.begin(9600);
    Serial.println("Iniciando Servidor Web Arduino.");
    Serial.println("Conectando...");
    Ethernet.begin(mac, ip);
    delay(1000);

    //REVISA SI EL ETHERNET SHIELD ESTÁ CONECTADO
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet Shield no encontrado. Conecte el shield y reinicie el servidor.");
        while (true); //DETENER EL PROGRAMA
    }

    Serial.println("Conexión Ethernet establecida.");
    servidor.begin();
    Serial.print("Servidor iniciado. IP: ");
    Serial.println(Ethernet.localIP());

}

void loop() {
    //SI HAY UN CLIENTE SOLICITANDO UNA CONEXIÓN, LO TOMA
    EthernetClient cliente = servidor.available();
    bool SERVIDOR_OCUPADO = 0;

    //SI UN CLIENTE SE CONECTA AL SERVIDOR Y EL SERVIDOR NO ESTÁ OCUPADO
    if(cliente && !SERVIDOR_OCUPADO) {
      SERVIDOR_OCUPADO = 1;
      Serial.println("Nuevo cliente conectado.");
      String request = "";
      bool SOLICITUD_COMPLETA = 0;
      bool GET_SOLICITADO = 0;
      bool GET_DATOS_SOLICITADO = 0;
      bool POST_DATOS_SOLICITADO = 0;

      //LECTURA BYTE POR BYTE DE LA SOLICITUD QUE SE RECIBIÓ
      //SI TARDA DEMASIADO, DEJA DE LEER
      while(cliente.connected() && !SOLICITUD_COMPLETA) {
        millisActual = millis();
        if(cliente.available()) {
          char c = cliente.read();
          request += c;
          millisUlt = millisActual;
        }
        else if(millisActual - millisUlt > millisEspera) {
          Serial.println(request);
          SOLICITUD_COMPLETA = 1;
        }
        //ANALIZA EL TIPO DE SOLICITUD QUE SE RECIBIÓ
        if(request == "GET / ") GET_SOLICITADO = 1;
        else if(request == "GET /datos") GET_DATOS_SOLICITADO = 1;
        else if(request == "POST /datos") POST_DATOS_SOLICITADO = 1;
      }

      //RESPUESTA AL CLIENTE

      //SI ALGUIEN ACCEDE A LA IP DESDE EL PUERTO 80 Y HACE UN GET / (POR EJ, ENTRANDO DESDE UN NAVEGADOR)
      //SE ENVÍA TODO EL CÓDIGO DE LA PÁGINA WEB
      if(GET_SOLICITADO) {
        //ENVÍA AL CLIENTE LA PÁGINA BYTE POR BYTE DESDE LA MEMORIA FLASH
        //TARDA UN POCO MÁS PERO EVITA QUE SE SATURE LA RAM
        for(int i = 0; pgm_read_byte(htmlPagina1 + i); i++) {
          cliente.write(pgm_read_byte(htmlPagina1 + i));
        }
        for(int i = 0; pgm_read_byte(htmlPagina2 + i); i++) {
          cliente.write(pgm_read_byte(htmlPagina2 + i));
        }
        for(int i = 0; pgm_read_byte(htmlPagina3 + i); i++) {
          cliente.write(pgm_read_byte(htmlPagina3 + i));
        }
      }

      //SI LA SOLICITUD FUE GET /datos
      else if(GET_DATOS_SOLICITADO) {
        //ENVÍA AL CLIENTE EL JSON CON LOS DATOS QUE ALMACENA EL SERVIDOR
        cliente.println(json);
      }

      //SI LA SOLICITUD FUE POST /datos
      else if(POST_DATOS_SOLICITADO) {
        //RECIBE Y SEPARA EL JSON CON LOS DATOS ACTUALIZADOS
        json = "";
        bool JSON_SEPARADO = 0;
        bool JSON_LEER = 0;
        for(int i = 0; !JSON_SEPARADO; i++) {
          if(request[i] == '{') JSON_LEER = 1;
          if(JSON_LEER) {
            json += request[i];
          }
          if(request[i] == '}') {
            JSON_LEER = 0;
            JSON_SEPARADO = 1;
          }
        }
        //INTERPRETA EL JSON Y ACTUALIZA LAS VARIABLES SEGÚN CORRESPONDA
        JsonDocument doc;
        deserializeJson(doc, json.c_str());
        temperatura = doc["temperatura"];
        humedad = doc["humedad"];
        color = doc["color"];
        velocidad = doc["velocidad"];
      }
      
      //UNA VEZ LA SOLICITUD FUE RECIBIDA, PROCESADA Y RESPONDIDA COMO CORRESPONDE
      //CIERRA LA CONEXIÓN Y LIBERA EL SERVIDOR PARA QUE PUEDA CONECTARSE OTRO CLIENTE
      cliente.stop();
      Serial.println("Cliente desconectado");
      SERVIDOR_OCUPADO = 0;
      delay(10);
    }
}