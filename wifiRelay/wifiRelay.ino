/* -----------------------------------------------------------------------------------------
 * wifiRelay.ino commande un relais à partir d'une interface graphique 
 * accessible dans un navigateur web : Firefox, Chrome, Midori, Edge
 * les valeurs ssid et password doivent être pré-écrites en EEPROM avec eepromPutEsp.ino  
 * La classe C_Eprom se charge de gérer ces valeurs si l'utilisateur les change
 * et les fournit à wifiRelay.ino pour connection.
 * 
 * Les pages wepPage.html et boxPage.html du serveur sont mémorisées en SPIFFS
 * 
 * eepromPutEsp doit fournir des valeurs permettant de se connecter en mode Access Point (WIFI_AP)
 * L'utilisateur peut choisir de changer ces valeurs, afin de se connecter en Mode station (WIFI_STA)
 * s'il dispose d'une box
 * Dans ce cas, l'utilisateur doit trouver lui-même, avec la commande 
 * nmap -sP 192.168.1.* (par exemple) quelle adresse sa box lui a attribué.
 * 
 * wifiRelay.ino, à l'initialisation, 
 * - tente de se connecter en mode WIFI_AP avec les coordonnées fournies par eepromPutEsp, via C_Eprom
 * - en cas d'échec, tente 20 fois maximum de se connecter en mode WIFI_STA avec les coordonnées 
 *    que l'utilisateur a validées dans la page boxPage.html
 * - en cas d'échec, quelle qu'en soit la raison, C_Eprom remet identifiant en mot de passe à    
 *    leurs valeurs d'origine, et se connecte en mode WIFI_AP
 * Cette méthode garantit une connection, et la plus attendue par l'utilisateur.
 * 
 * Les boutons On et Off de la page webPage.html permettent d'ouvrir ou fermer le relais.
 * 
 * Merci pour votre aide ,
 * https://circuits4you.com/
 * https://github.com/esp8266/Arduino
 * https://projetsdiy.fr/e66-serveur-ftp-echange-fichiers-spiffs/
 * https://www.screenfeed.fr/blog/personnalisation-chesp82ckbox-boutons-radio-sans-image-0570/
 * http://tiptopboards.free.fr/arduino_forum/viewtopic.php?f=2&t=54
 * ------------------------------------------------------------------------------------------
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include "c_eprom.h"
#define GPIO 0 // commande du relais GPIO0 

// identifiant et Mot de passe
C_Eprom c_eprom;
String boxSsid = "";
String boxPass = "";

ESP8266WebServer server(80);
String webPage;
String boxPage;

/* @@ssid@@ est la valeur originelle de la variable ssid, dans la section javascript de boxPage.html
 * cette valeur est changée en c_eprom.ssid() dans la fonction updateBoxPage() de wifiRelay.ino
 * elle s'affiche dans le champ ssid de la page boxPage.html
*/
String ssidHtml = "@@ssid@@"; 

void setup() {
  Serial.begin(9600);
  while (!Serial) {;}
  SPIFFS.begin();
  wait(500);

  pinMode(GPIO, OUTPUT);
  digitalWrite(GPIO, HIGH);

  // charge l'identifiant et le mot de passe depuis l' EEPROM
  boxSsid = c_eprom.ssid();
  boxPass = c_eprom.pass();
 
  // se connecte en Accès Point, sinon essaie en mode Station
  // si les paramètres de connection à la box sont invalides
  // (mauvaise saisie, box changée ou arrêtée, ..)
  // retour à la configuration d'origine, et nouvelle tentative en Point d'accés
  if (!toAp()) {
    if (!toBox()) {
      c_eprom.toOrigine();
      boxSsid = c_eprom.ssid();
      boxPass = c_eprom.pass();
      toAp();
    }
  }

  if (MDNS.begin("wifiRelay")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/switch", handleSwitch); // On, Off
  server.on("/box", handleBox);       // positionne sur boxPage
  server.on("/check", handleCheck);   // gère les changements utilisateur de ssid et de password

  server.begin();
  Serial.println("your server is working");

  webPage = createFromData("/webPage.html");
  boxPage = createFromData("/boxPage.html");
}

// Equivalent delay avec yield
void wait(const int x) {
  int timer = millis();
  while (millis() - timer < x) {
    yield();
  }
}

// tente de créer un Point d'Accés, si l'identificateur est
// toujours celui installé par eepromPutEsp
bool toAp() {
  if (boxSsid != c_eprom.orig())
    return false;

  IPAddress IP = c_eprom.setIP();
  IPAddress subnet(255, 255, 255, 0);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP, IP, subnet);
  WiFi.softAP(boxSsid.c_str(), boxPass.c_str()); 
  return true;
}

// connexion à la box
// attention : l'adresse du serveur n'est plus 192.168.4.17,
// mais celle choisie par votre box, accessible pour les linuxiens
// par la commande user@user:~$ nmap -sP 192.168.1.* (si vos adresses privées sont 192.168.1.x)
bool toBox() {
  int counter = 0;
  bool isOk = false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(boxSsid.c_str(), boxPass.c_str());

  // 20 essais de connection
  while (counter < 20 && WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    counter++;
    wait(500);
  }
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    isOk = true;
  }
  return isOk;
}

// charge un fichier .html de la SPIFFS
// dans une chaîne de caractères
String createFromData(String qHtml) {
  String str = "";
  File dataFile = SPIFFS.open(qHtml, "r");
  for (int i = 0; i < dataFile.size(); i++)
  {
    str += (char)dataFile.read();
  }
  dataFile.close();
  return str;
}

// remplace la variable ssid de boxPage.html dans la section javascript par sa valeur = eprom.m_ssid
// cette valeur est mémorisée dans ssidHtml, en cas de son changement ultérieur
void updateBoxPage() {
  if (ssidHtml != c_eprom.ssid()) { 
    boxPage.replace(ssidHtml, c_eprom.ssid());
    ssidHtml = c_eprom.ssid();
  }    
}

// connexion à la racine
void handleRoot() {
  server.send(200, "text/html", webPage);
}

// appui sur un bouton de la form "/switch"
// du fichier webPage.html
// server.arg prend la valeur de l'attribut name comme paramètre, 
// et renvoie une chaîne contenant la valeur de l'attribut value
// Ici, il faut 2 boutons ayant name="button", 1 avec value="On", 1 avec value="Off" (case sensitive)
void handleSwitch() {
  String s = server.arg("button");
  if (s == "On") {
    digitalWrite(GPIO, HIGH);
  }
  if (s == "Off") {
    digitalWrite(GPIO, LOW);
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "");
}

// appui sur le bouton "Box" de la form "box"
// du fichier webPage.html
void handleBox() {
  updateBoxPage(); // pour afficher la valeur c_eprom.ssid() dans le champ Ssid de boxPage.html
  server.send(302, "text/html", boxPage);
}

// appui sur le bouton "valider" de la form "check" du fichier boxPage.html
// Le fichier boxPage.html se charge de valider la saisie (javascript)
// mais ne peut contrôler la panne de box, le changement de box, l'erreur de saisie de password ...
// Après saisie, il faut réinitialiser l'esp8266-01 pour rendre les modifications opérationnelles
void handleCheck() {
  String s = server.arg("button");
  if (s == "Valider") {
    String strSsid = server.arg("ssid");
    String strPass = server.arg("pass");
    c_eprom.writeAll(strSsid, strPass); // Mémorisation dans l'eeprom
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/html", "");
}

void loop() {
  server.handleClient();
}
