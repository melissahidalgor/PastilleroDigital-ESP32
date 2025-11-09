//---------------------------------------- Including Libraries
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "time.h"
#include "miWebServer.h"

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui.h"

#include "HX711.h"

//---------------------------------------- LCD TFT
/* IN USER SETUP

// ESP32 S3 HSPI
VCC
GND
#define TFT_CS   10  // Chip select control pin
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
#define TFT_DC   14   // Data Command control pin
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_BL   16           // LED back-light control pin
#define TFT_MISO 13
*/

// ------------------------------- Galgas 
#define DT1 1
#define SCK1 2

#define DT2 36
#define SCK2 37

// ******************************************************************************* Globales
WebServer server(80);
TFT_eSPI tft = TFT_eSPI();

uint8_t* draw_buf;
uint32_t lastTick = 0;

doseInfo nextDose;
bool nuevaTomaDisponible = false;

HX711 scale1;
HX711 scale2;

// Parámetros de configuración
const int num_lecturas = 5;              // Muestras para promedio
const long umbral_diferencia = 330000;     // Cambios mayores a esto son considerados eventos
const unsigned long tiempo_confirmacion = 5000;  // Tiempo mínimo sostenido en ms 

// Variables de estado para las galgas
long promedio_anterior1 = 0;
long promedio_actual1 = 0;
unsigned long tiempo_inicio_diferencia1 = 0;
bool cambio_en_progreso1 = false;

long promedio_anterior2 = 0;
long promedio_actual2 = 0;
unsigned long tiempo_inicio_diferencia2 = 0;
bool cambio_en_progreso2 = false;

long promedio_anterior3 = 0;
long promedio_actual3 = 0;
unsigned long tiempo_inicio_diferencia3 = 0;
bool cambio_en_progreso3 = false;

// ******************************************************************************** Constantes
// Credenciales de Wi-Fi
const char* ssid = "S23 FE de Jorge"; //"Melissa Galaxy A30"; //"dlink_extension";  
const char* password = "Pons2003"; //"isue7235"; //"3YYHxxcCGm";   

// Configuración de hora
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600;  // Time zone for Aguascalientes (UTC-6)
const int daylightOffset_sec = 0;

// LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes.
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

// ****************************************************************************** Funciones Externas
extern void handleRoot();
extern void handleCSS();
extern void handleJS();
extern void handleRecibirProximaToma();

//**********************************************************************************************************************************
//________________________________________________________________________________ log_print()
// If logging is enabled, it will inform the user about what is happening in the library.
void log_print(lv_log_level_t level, const char* buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

//________________________________________________________________________________ touchscreen_read() Dummy
// Get the Touchscreen data.
void touchscreen_read(lv_indev_t* indev, lv_indev_data_t* data) {
  // Serial.println("No hay touch");
}
//**********************************************************************************************************************************
//_____________________________________________________________________________________________________________ setup()
void setup() {
  Serial.begin(115200);
  delay(3000);
  // ___________________________________________________________ TFT begin
  // Start the tft display
  tft.init();
  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);
  Serial.println("Pantalla TFT configurada");

  // ___________________________________________________________ LVGL begin
  // Start LVGL.
  lv_init();

  // Register print function for debugging.
  lv_log_register_print_cb(log_print);

  // Create a display object.
  lv_display_t* disp;

  // Initialize the TFT display using the TFT_eSPI library.
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

  // Initialize an LVGL input device object (Touchscreen).
  lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  // Set the callback function to read Touchscreen input.
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Integrate EEZ Studio GUI.
  ui_init();

  // Register event handler.
  Serial.println("LVGL configurado");

  // ___________________________________________________________ SPIFFS begin
  if (!SPIFFS.begin()) {
    Serial.println("¡Error al montar SPIFFS!");
    return;
  }
  // ___________________________________________________________ WiFi begin
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lv_label_set_text(objects.label_wifi_status, "CONECTANDO");
  }
  lv_label_set_text(objects.label_wifi_status, "ON");
  Serial.println("");
  Serial.print("Conectado a la red Wi-Fi con IP: ");
  Serial.println(WiFi.localIP());
  lv_label_set_text(objects.label_ip, WiFi.localIP().toString().c_str());

  // ___________________________________________________________ Server begin
  server.on("/", handleRoot);
  server.on("/estilos.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/recibir_proxima_toma", HTTP_GET, handleRecibirProximaToma);
  server.on("/get_toma_notificacion", handleGetTomaNotificacion);
  server.begin();
  Serial.println("Servidor web iniciado");
  // ___________________________________________________________ Time begin
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Tiempo configurado");
  // ___________________________________________________________ Galga begin
  scale1.begin(DT1, SCK1);
  scale2.begin(DT2, SCK2);
  Serial.println("Galgas iniciadas");

  promedio_anterior1 = obtener_promedio_crudo(scale1, num_lecturas);
  promedio_anterior2 = obtener_promedio_crudo(scale2, num_lecturas);
}

//_____________________________________________________________________________________________________________ loop()
void loop() {
  server.handleClient();

  promedio_actual1 = obtener_promedio_crudo(scale1, num_lecturas);
  promedio_actual2 = obtener_promedio_crudo(scale2, num_lecturas);

  detectar_cambio_galga(1, promedio_actual1, promedio_anterior1, cambio_en_progreso1, tiempo_inicio_diferencia1);
  detectar_cambio_galga(2, promedio_actual2, promedio_anterior2, cambio_en_progreso2, tiempo_inicio_diferencia2);

  lv_tick_inc(millis() - lastTick);
  lastTick = millis();
  lv_timer_handler();
  //delay(5);

  // Actualizar cada segundo (1000 milisegundos)
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime >= 1000) {
    getDateTime();
    lastUpdateTime = millis();
  }
}

//__________________________________________________________________________________ getDateTime()
void getDateTime() {
  struct tm timeinfo;  // Fecha actual

  if (getLocalTime(&timeinfo)) {
    char fechaStr[11];  // YYYY-MM-DD
    strftime(fechaStr, sizeof(fechaStr), "%Y-%m-%d", &timeinfo);
    //Serial.println(fechaStr);
    lv_label_set_text(objects.label_date, fechaStr);

    char horaStrCompleta[9];  // HH:MM:SS
    strftime(horaStrCompleta, sizeof(horaStrCompleta), "%H:%M:%S", &timeinfo);
    //Serial.println(horaStrCompleta);
    lv_label_set_text(objects.label_time, horaStrCompleta);

    // ------- Alarma
    char horaStrCorta[7];  // HH:MM
    strftime(horaStrCorta, sizeof(horaStrCorta), "%H:%M", &timeinfo);
    String fechaActual = String(fechaStr);
    String horaActual = String(horaStrCorta);

    if (fechaActual.equals(nextDose.fecha)) {
      if (horaActual.equals(nextDose.hora) && !nuevaTomaDisponible) {
        Serial.println("Alarma: Tomar " + nextDose.nombre);

        lv_label_set_text(objects.label_alarma_time, horaStrCorta);
        lv_label_set_text(objects.label_alarma_dose, nextDose.nombre.c_str());
        //lv_label_set_text(objects.label_notas, nextDose.notas.c_str());

        lv_obj_add_flag(objects.main, LV_OBJ_FLAG_HIDDEN);      // Oculta la pantalla principal
        lv_obj_clear_flag(objects.alarma, LV_OBJ_FLAG_HIDDEN);  // Muestra la pantalla de la alrma
        lv_scr_load(objects.alarma);                            // Carga la pantalla de historial
      } else {
        lv_obj_add_flag(objects.alarma, LV_OBJ_FLAG_HIDDEN);  // Oculta la pantalla de alarma
        lv_obj_clear_flag(objects.main, LV_OBJ_FLAG_HIDDEN);  // Muestra la pantalla principal
        lv_scr_load(objects.main);                            // Carga la pantalla principal

        int horaActualInt = horaActual.substring(0, 2).toInt();   
        int minutoActualInt = horaActual.substring(3, 5).toInt();
        int horaNextDoseInt = nextDose.hora.substring(0, 2).toInt();
        int minutoNextDoseInt = nextDose.hora.substring(3, 5).toInt();

       if (horaActualInt > horaNextDoseInt || (horaActualInt == horaNextDoseInt && minutoActualInt > minutoNextDoseInt)) {
        Serial.println("Desfase de hora");
        nextDose.nombre = "";
        nextDose.hora = "";
        nextDose.fecha = "";
        }
      }
    } 
  } else {
    //Serial.println("Error al obtener la hora");
    lv_label_set_text(objects.label_date, "Conectando");
    lv_label_set_text(objects.label_time, "Conectando");
  }
}
//________________________________________________________________________________ obtener_promedio_crudo() 
long obtener_promedio_crudo(HX711 &scale, int n) {
  long suma = 0;
  for (int i = 0; i < n; i++) {
    suma += scale.read();
    delay(2); // Retardo más corto por lectura individual
  }
  return suma / n;
}

//________________________________________________________________________________ detectar_cambio() 
void detectar_cambio_galga(int galga_num, long promedio_actual, long &promedio_anterior, bool &cambio_en_progreso, unsigned long &tiempo_inicio_diferencia) {
  long diferencia = abs(promedio_actual - promedio_anterior);

  Serial.printf("Galga %d: Actual=%ld, Anterior=%ld, Diff=%ld, InProgress=%d, TimeStart=%ld\n",
                galga_num, promedio_actual, promedio_anterior, diferencia, cambio_en_progreso, tiempo_inicio_diferencia);

  if (diferencia > umbral_diferencia) {
    if (!cambio_en_progreso) {
      tiempo_inicio_diferencia = millis();
      cambio_en_progreso = true;
      Serial.printf("Galga %d: INICIO DE CAMBIO DETECTADO. InProgress = true\n", galga_num);
    } else if (millis() - tiempo_inicio_diferencia >= tiempo_confirmacion) {
      Serial.printf("⚠️ ¡Cambio confirmado en Galga %d! Diferencia: %ld\n", galga_num, diferencia);
      promedio_anterior = promedio_actual;
      cambio_en_progreso = false;
      nuevaTomaDisponible = true;
      delay(7000);
    }
  } else {
    cambio_en_progreso = false;
  }
}
