//Basado en https://github.com/Makuna/Rtc/blob/master/examples/DS1302_Simple/DS1302_Simple.ino
#include <RtcDS1302.h>
ThreeWire myWire(6, 5, 4);  // DAT (I/O), CLK (SCLK), RST (CE)
RtcDS1302<ThreeWire> Rtc(myWire);
#include <Servo.h>
Servo panel;

unsigned long time;
float tiempo = 5000.50;  // CADA 5.5 SEGUNDOS AVANZA UN GRADO, HASTA LLEGAR A LOS 150°
unsigned long t = 0;

int pos_default = 35;
int pos_default_add = 35;
boolean act_1, act_2 = false;
char DiaSemana[][4] = { "Dom", "Lun", "Mar", "Mie", "Jue", "Vie", "Sab" };

// La linea fija la fecha, hora y dia de la semana, se debe suprimir la linea en la segunda carga
// Ejemplo 2017 diciembre 09, 08:00:00  dia 1-Lunes (0=Dom, 1=Lun, 2=Mar, 3=Mie, 4=Jue, 5=Vie, 6=Sab)
// DateTime dt(2017, 12, 09, 08, 00, 0, 6);

void setup() {
  Serial.begin(9600);

  Serial.print("compilado: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();
  // La linea fija la fecha, hora y dia de la semana, se debe suprimir la linea en la segunda carga
  //rtc.setDateTime(dt);
  RtcDateTime fechaCompilado = RtcDateTime(__DATE__, __TIME__);
  imprimirFecha(fechaCompilado);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("Fecha invalida RTC");
    Rtc.SetDateTime(fechaCompilado);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC protegido contra escritura, habilitando...");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC no esta corriendo, iniciando ahora");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime fechaActual = Rtc.GetDateTime();

  if (fechaActual < fechaCompilado) {
    Serial.println("Fecha RTC es menor que fecha de compilado!  (Actualizando Fecha)");
    Rtc.SetDateTime(fechaCompilado);
  }

  panel.attach(9);
  panel.write(35);
}

void loop() {


  int LDR = analogRead(A0);

  Serial.print("SENSOR_LDR ");
  Serial.println(LDR);

  RtcDateTime now = Rtc.GetDateTime();
  Serial.print(now.Year(), DEC);
  Serial.print('/');
  Serial.print(now.Month(), DEC);
  Serial.print('/');
  Serial.print(now.Day(), DEC);
  Serial.print(' ');
  Serial.print(now.Hour(), DEC);
  Serial.print(':');
  Serial.print(now.Minute(), DEC);
  Serial.print(':');
  Serial.print(now.Second(), DEC);
  Serial.print(' ');
  Serial.print(DiaSemana[now.DayOfWeek()]);
  Serial.println();
  delay(1000);  // Se actualiza cada segundo

  if (LDR <= 600) {
    panel.write(pos_default);
  }

  //MODIFICADO
  //if (now.Minute() >= 2 && now.Minute() < 12) {
  if (now.Minute() >= 2 && now.Minute() < 58) {
    act_1 = true;
  }


  if (act_1 == true) {

    time = millis();
    if (time - t > tiempo) {
      t = time;
      pos_default_add++;
    }

    //MODIFICADO
    //if (LDR > 800) {
    if (LDR > 220) {
      panel.write(pos_default_add);
      Serial.print("GRADOS SERVO");
      Serial.println(pos_default_add);
    }
  }

  if (pos_default_add >= 150) {
    act_1 = false;
  }
  if (pos_default_add >= 150 && now.Minute() >= 12) {
    pos_default_add = 35;
    panel.write(pos_default);
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void imprimirFecha(const RtcDateTime& dt) {
  char cadenaFecha[26];

  snprintf_P(cadenaFecha,
             countof(cadenaFecha),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(cadenaFecha);
}
