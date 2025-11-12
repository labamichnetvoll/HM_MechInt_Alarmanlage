# Projekt Alarmanlage 
> [!NOTE]
> [Aktuelle Doku HIER!](https://github.com/labamichnetvoll/HM_MechInt_Alarmanlage/tree/doku_github)


Dieses GitHub Repository dient als Code Austausch Plattform

Folgende Unterordner sind geplant:
1. ArduinoCode
2. Web-App (html - css) & Backend WebServer
3. Schaltplan (KiCAD 9.x)


## Branches:

| Branch                | Kurzbeschreibung                                              |
| ---                   | ---                                                           |
| `main`                | Hier werden alle Änderungen nach und nach eingebettet         |
| `DEV_Sensorik`        | Entwicklungsbranch für die Sensorik-Programmierung            |
| `Leoline99-patch-2`   | Programlogik                                                  |
| `dev_Schaltplan`      | Schaltplan Erstellung mit KiCAD v9.0.5 als Bauanleitung       |
| `dev_WebApp`          | WebUI Entwicklung & Arduino "Backend"                         |
| `doku_github`         | Dokumentation auf GitHub (z.B. Strukturerläuterung, usw..)    |

> [!TIP]
> Im `Main`-Branch können **nicht direkt** Änderungen vorgenommen werden.  
> Daher wird für jedes Feature ein vorhandener bzw. neuer Branch genutzt.  
> Bei relevanten "Meilensteinen" wird der Quelltext durch einen Pull Request aus den Branches dann in den `main`-branch kopiert.  
> Die Pull Requests könnt ihr jederzeit über die [GitHub Webseite](https://github.com/labamichnetvoll/HM_MechInt_Alarmanlage/) erstellen.
> Diese müssen von mind. einer weiteren Person bestätigt & kommentiert werden, bevor diese in den `main`-branch gemerged werden können.

## FAQ
1. Bei Fragen, Anmerkungen oder Verbesserungsvorschlägen an @labamichnetvoll (Florian) wenden
2. **[Markdown Syntax](https://docs.github.com/de/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)** für Doku auf GitHub

## Arduino IDE Biblotheken 
Alle relevanten Bibliotheken sind hier aufgeführt.  
> [!IMPORTANT]
> Alle Bibliotheken **mit** dependencys (Abhängigkeiten) installieren!
> Bitte haltet diese Liste aktuell, sonst kann der Code evtl. nicht von jedem kompiliert werden!
- Adafruit SSD1306    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für O-LED*
- (AdafruitGFX Library) &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für O-LED*
- (Adafruit BusIO)
- Grove Ultrasonic Ranger by Seeed Studio    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für Ultraschallsensor*
- LSM6DS3 by Arduino    [Doku](https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/)&nbsp;&nbsp;&nbsp;&nbsp; *Für IMU*
- WiFiNINA by Arduino   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für das Wlan-Modul des Nano 33 IoT*
- 

## Arduino IDE Board Bibliothek
- Arduino SAMD Boards (32-bits ARM Cortex-M0+)
> [!TIP]
> Bei Auswahl des Arduino Nano 33 IoT in der Board-Auswahl wird diese Board-Bibliothek automatisch zur Installation vorgeschlagen  

## nützliche Links für die Programmierung
- [erweiterter Zustandsautomat](https://forum.arduino.cc/t/zustandsautomaten-fur-fortgeschrittene/320984/2)
- [switch case Bsp Zustandsautomat](https://spacehal.github.io/docs/arduino/stateMachine)
- [Interessante WebServer Libary](https://github.com/khoih-prog/WiFiWebServer?tab=readme-ov-file#features)
- [Gute Erklärung Arduino WebServer](https://werner.rothschopf.net/202001_arduino_webserver_post.htm)
- [Optimierung Arduino WebServer](https://werner.rothschopf.net/microcontroller/202011_arduino_webserver_optimieren.htm)
- [Erklärung Arduino Speicher](https://docs.arduino.cc/learn/programming/memory-guide/)
