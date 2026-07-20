#include <Wire.h>  // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(Wire);
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

bool wasError(const char* errorTopic = "") {
  uint8_t error = Rtc.LastError();
  if (error != 0) {
    // we have a communications error
    // see https://www.arduino.cc/reference/en/language/functions/communication/wire/endtransmission/
    // for what the number means
    Serial.print("[");
    Serial.print(errorTopic);
    Serial.print("] WIRE communications error (");
    Serial.print(error);
    Serial.print(") : ");

    switch (error) {
      case Rtc_Wire_Error_None:
        Serial.println("(none?!)");
        break;
      case Rtc_Wire_Error_TxBufferOverflow:
        Serial.println("transmit buffer overflow");
        break;
      case Rtc_Wire_Error_NoAddressableDevice:
        Serial.println("no device responded");
        break;
      case Rtc_Wire_Error_UnsupportedRequest:
        Serial.println("device doesn't support request");
        break;
      case Rtc_Wire_Error_Unspecific:
        Serial.println("unspecified error");
        break;
      case Rtc_Wire_Error_CommunicationTimeout:
        Serial.println("communications timed out");
        break;
    }
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);

  Serial.print("compilado: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);
  Rtc.Begin();
#if defined(WIRE_HAS_TIMEOUT)
  Wire.setWireTimeout(3000 /* us */, true /* reset_on_timeout */);
#endif
  // La linea fija la fecha, hora y dia de la semana, se debe suprimir la linea en la segunda carga
  //rtc.setDateTime(dt);
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    if (!wasError("setup IsDateTimeValid")) {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
    }
  }

  if (!Rtc.GetIsRunning()) {
    if (!wasError("setup GetIsRunning")) {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
    }
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (!wasError("setup GetDateTime")) {
    if (now < compiled) {
      Serial.println("RTC is older than compile time, updating DateTime");
      Rtc.SetDateTime(compiled);
    } else if (now > compiled) {
      Serial.println("RTC is newer than compile time, this is expected");
    } else if (now == compiled) {
      Serial.println("RTC is the same as compile time, while not expected all is still fine");
    }
  }

  panel.attach(2);
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
  if (now.Minute() >= 2 && now.Minute() < 12) {
    act_1 = true;
  }


  if (act_1 == true) {

    time = millis();
    if (time - t > tiempo) {
      t = time;
      pos_default_add++;
    }
    if (LDR > 800) {
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

void printDateTime(const RtcDateTime& dt) {
  char datestring[26];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}
