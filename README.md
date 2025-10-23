# Projekt Alarmanlage

Dieses GitHub Repository dient als Code Austausch Plattform

Folgende Unterordner sind geplant:
1. ArduinoCode
2. Web-App (html - css)
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
2. **[Markdown Syntax](https://docs.github.com/de/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax)**

## Biblotheken Arduino IDE
Alles wichtige ist hier zu beziehen:

> [!IMPORTANT]
> Mit dependency installieren!
- AdafruitGFX Library &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für O-LED*
- Adafruit SSD1306    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für O-LED*
- Grove Ultrasonic Ranger by Seeed Studio    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;*Für Ultraschallsensor*
- *evtl noch etwas für Lautsprecher*

