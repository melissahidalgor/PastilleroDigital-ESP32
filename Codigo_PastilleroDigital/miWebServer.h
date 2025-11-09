#ifndef MIWEBSERVER_H
#define MIWEBSERVER_H

#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <Arduino.h>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui.h"

struct doseInfo {
  String nombre;
  String hora;
  String fecha;
  String notas;
};

void handleRoot();
void handleCSS();
void handleJS();
void handleRecibirProximaToma();
void handleGetTomaNotificacion();

#endif /*miWebServer.h*/
