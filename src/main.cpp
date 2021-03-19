#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

TinyGPSPlus gps;

SoftwareSerial ss(4, 5); 

WiFiServer server(80);
const char* ssid = "WiFi_A5CB";
const char* password = "NEL1QR985QY";

int year , month , date, hour , minute , second;
String date_str , time_str , lat_str , lng_str, alt_str;
int pm;

// REFERENCIA DE POSICAO
int posLat = 2;
int posLng = 2;
// ITERAÇÃO DE POSIÇÃO
int cPL = 0;
int pPL = 99;
int cPN = 0;
int pPN = 99;
// ITERAÇÃO DE GRAUS
int cDL = 0;
int pDL = 99;
int cDN = 0;
int pDN = 99;
// ITERAÇÃO DE MINUTOS
int cML = 0;
int pML = 99;
int cMN = 0;
int pMN = 99;

StaticJsonDocument<3500> doc;
JsonObject latArray = doc.createNestedObject("lat");
JsonArray positionLatitude = latArray.createNestedArray("PL");
JsonArray degreesLatitude = latArray.createNestedArray("DL");
JsonArray minutesLatitude = latArray.createNestedArray("ML");
JsonArray secondsLatitude = latArray.createNestedArray("SL");
JsonObject lngArray = doc.createNestedObject("lng");
JsonArray positionLongitude = lngArray.createNestedArray("PN");
JsonArray degreesLongitude = lngArray.createNestedArray("DN");
JsonArray minutesLongitude = lngArray.createNestedArray("MN");
JsonArray secondsLongitude = lngArray.createNestedArray("SN");

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void testeJson()
{
  ESP.wdtDisable();
  int teste = 0;

  while (teste < 30)
  {
    Serial.println(teste);
    //Serial.println(gps.location.rawLat().deg);
    Serial.print("LAT=");
    lat_str = String(gps.location.lat(), 6); 
    Serial.println(lat_str);
    Serial.print("LONG=");
    lng_str = String(gps.location.lng(), 6);
    Serial.println(lng_str);
    Serial.print("ALT=");
    alt_str = String(gps.altitude.meters());
    Serial.println();
    teste++;
    delay(1000);
  }
  serializeJson(doc, Serial);
  Serial.println();
  return;
}

void jsonWriter()
{
  ESP.wdtDisable();
  int numberOfSamples = 0;

  while (numberOfSamples < 70)
  {

    while (ss.available() > 0)
    {

        //if (gps.encode(ss.read()) == 1){

        if (gps.location.isValid()){

        //if (gps.location.isUpdated()) {

        numberOfSamples++;
        // ANÁLISE DE POSIÇÃO
        if ((gps.location.rawLat().negative ? "-" : "+") == "+")
        {
          posLat = 0;
        }
        else if ((gps.location.rawLat().negative ? "-" : "+") == "-")
        {
          posLat = 1;
        }
        if ((gps.location.rawLng().negative ? "-" : "+") == "+")
        {
          posLng = 1;
        }
        else if ((gps.location.rawLng().negative ? "-" : "+") == "-")
        {
          posLng = 0;
        }
        // VERIFICAÇÃO PARA NÚMERO DE REPETIÇÕES
        if (posLat != pPL)
        {
          pPL = posLat;
          cPL++;
        }
        if (posLng != pPN)
        {
          pPN = posLng;
          cPN++;
        }
        // CRIAÇÃO DE VETOR
        //positionLatitude.add(posLat);
        //positionLongitude.add(posLng);

        // ANÁLISE DOS GRAUS
        int degreeLat = gps.location.rawLat().deg;
        int degreeLng = gps.location.rawLng().deg;
        //degreesLatitude.add(degreeLat);
        //degreesLongitude.add(degreeLng);
        if (degreeLat != pDL)
        {
          pDL = degreeLat;
          cDL++;
        }
        if (degreeLng != pDN)
        {
          pDN = degreeLng;
          cDN++;
        }
        Serial.println("TESTE DE GRAUS");
        Serial.println(degreeLat);

        // ANÁLISE DOS MINUTOS E SEGUNDOS
        float bill1 = gps.location.rawLat().billionths;
        float bill2 = gps.location.rawLng().billionths;
        float dmsML = bill1 * 60 / 1000000000;
        float dmsMN = bill2 * 60 / 1000000000;
        ;
        // MINUTOS
        int minuteLat = (int)dmsML;
        int minuteLng = (int)dmsMN;
        //minutesLatitude.add(minuteLat);
        //minutesLongitude.add(minuteLng);
        if (minuteLat != pML)
        {
          pML = minuteLat;
          cML++;
        }
        if (minuteLng != pMN)
        {
          pMN = minuteLng;
          cMN++;
        }

        // SEGUNDOS
        int secondLat = 10000 * (dmsML - minuteLat);
        int secondLng = 10000 * (dmsMN - minuteLng);
        //secondsLatitude.add(secondLat);
        //secondsLongitude.add(secondLng);

        delay(1000);
        //}                                         // LOCATION IS UPDATED
        }                                           // LOCATION IS VALID
        //}                                         // ENCODE SS READ
      }                                             // SS AVAILABLE

      // if (cPL == 1 && cPN == 1)
      // {
      //   if (cDL == 1 && cDN == 1)
      //   {
      //     if (cML == 1 && cMN == 1)
      //     {
      //       doc["FX"] = 2;
      //       // EVITAR REPETIÇÕES
      //       positionLatitude.clear();
      //       positionLongitude.clear();
      //       degreesLatitude.clear();
      //       degreesLongitude.clear();
      //       minutesLatitude.clear();
      //       minutesLongitude.clear();
      //       // ADICIONAR ÚLTIMO VALOR
      //       positionLatitude.add(posLat);
      //       positionLongitude.add(posLng);
      //       degreesLatitude.add(degreeLat);
      //       degreesLongitude.add(degreeLng);
      //       minutesLatitude.add(minuteLat);
      //       minutesLongitude.add(minuteLng);
      //     }
      //     else
      //     {
      //       doc["FX"] = 1;
      //       // EVITAR REPETIÇÕES
      //       positionLatitude.clear();
      //       positionLongitude.clear();
      //       degreesLatitude.clear();
      //       degreesLongitude.clear();
      //       // ADICIONAR ÚLTIMO VALOR
      //       positionLatitude.add(posLat);
      //       positionLongitude.add(posLng);
      //       degreesLatitude.add(degreeLat);
      //       degreesLongitude.add(degreeLng);
      //     }
      //   }
      //   else
      //   {
      //     doc["FX"] = 0;
      //     // EVITAR REPETIÇÕES
      //     positionLatitude.clear();
      //     positionLongitude.clear();
      //     // ADICIONAR ÚLTIMO VALOR
      //     positionLatitude.add(posLat);
      //     positionLongitude.add(posLng);
      //   }
      // }
  }
  //serializeJson(doc, Serial);
  Serial.println();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td {border: 1px solid black;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> GPS Interfacing with NodeMCU</h1>";
  s += "<p ALIGN=CENTER style="
       "font-size:150%;"
       "";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  // s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  // s += date_str;
  // s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  // s += time_str;
  s += "</td>  </tr> </table> ";

  if (gps.location.isValid())
  {
    s += "<p align=center><a style="
         "color:RED;font-size:125%;"
         " href="
         "http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += lat_str;
    s += "+";
    s += lng_str;
    s += ""
         " target="
         "_top"
         ">Click here!</a> To check the location in Google maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);

  return;
}

void timeGPS()
{

  if (gps.date.isValid())
  {
    date_str = "";
    date = gps.date.day();
    month = gps.date.month();
    year = gps.date.year();

    if (date < 10)
      date_str = '0';
    date_str += String(date);
    date_str += " / ";
    if (month < 10)
      date_str += '0';
    date_str += String(month);
    date_str += " / ";
    if (year < 10)
      date_str += '0';
    date_str += String(year);
  }
  if (gps.time.isValid())
  {
    time_str = "";
    hour = gps.time.hour();
    minute = gps.time.minute();
    second = gps.time.second();
    minute = (minute + 30);
    if (minute > 59)
    {
      minute = minute - 60;
      hour = hour + 1;
    }
    hour = (hour + 5);
    if (hour > 23)
      hour = hour - 24;
    if (hour >= 12)
      pm = 1;
    else
      pm = 0;
    hour = hour % 12;
    if (hour < 10)
      time_str = '0';
    time_str += String(hour);
    time_str += " : ";
    if (minute < 10)
      time_str += '0';
    time_str += String(minute);
    time_str += " : ";
    if (second < 10)
      time_str += '0';
    time_str += String(second);
    if (pm == 1)
      time_str += " PM ";
    else
      time_str += " AM ";
  }

  return;
}

void loop()
  {
    testeJson();
    //jsonWriter();
    delay(2000);
  }
