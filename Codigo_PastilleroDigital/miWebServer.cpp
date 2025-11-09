#include "miWebServer.h" 

extern WebServer server;
extern doseInfo nextDose;
extern bool nuevaTomaDisponible;
//________________________________________________________________________________ handleRoot()
void handleRoot() {
  File file = SPIFFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "Archivo no encontrado");
  }
}
//________________________________________________________________________________ handleCSS()
void handleCSS() {
  File file = SPIFFS.open("/estilos.css", "r");
  if (file) {
    server.streamFile(file, "text/css");
    file.close();
  } else {
    server.send(404, "text/plain", "Archivo CSS no encontrado");
  }
}
//________________________________________________________________________________ handleJS()
void handleJS() {
  File file = SPIFFS.open("/script.js", "r");
  if (file) {
    server.streamFile(file, "application/javascript");
    file.close();
  } else {
    server.send(404, "text/plain", "Archivo JS no encontrado");
  }
}
//________________________________________________________________________________ handleRecibirProximaToma()
void handleRecibirProximaToma() {
  if (server.hasArg("nombre") || server.hasArg("hora") || server.hasArg("fecha")) {
    if(server.arg("nombre") != nextDose.nombre || server.arg("hora") != nextDose.hora){
      nextDose.nombre = server.arg("nombre");
      nextDose.hora = server.arg("hora");
      nextDose.fecha = server.arg("fecha");
    
      String proximaTomaRecibida =  nextDose.nombre + " " + nextDose.hora;
      lv_label_set_text(objects.label_next_dose, proximaTomaRecibida.c_str());
      Serial.println("Próxima toma recibida: " + proximaTomaRecibida);
      server.send(200, "text/plain", "Próxima toma recibida y almacenada en el ESP32.");
    } else { 
      Serial.println("Sin cambios en la siguiente toma");
      server.send(200, "text/plain", "Sin cambios en la siguiente toma");
    }
  } else {
    server.send(400, "text/plain", "Parámetros incorrectos para la próxima toma.");
  }
}
//________________________________________________________________________________ handleGetTomaNotificación()
void handleGetTomaNotificacion() {
    if (nuevaTomaDisponible) {
        server.send(200, "text/plain", "toma_realizada"); // Enviar un simple indicador
        Serial.println("Se envio toma_realizada");
        nuevaTomaDisponible = false;
    } else {
        server.send(200, "text/plain", "");
    }
}
