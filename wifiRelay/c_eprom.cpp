#include "c_eprom.h"

C_Eprom::C_Eprom()
{
  EEPROM.begin(512);
  readAll();
}

String C_Eprom::readId_Pw(int addr) {
  int l; // longueur de chaîne
  EEPROM.get(addr, l);
  String s = "";
  for (int i = 0;  i < l; ++i) {
    char c = EEPROM.read(addr+i+4);
    s += c;
  }
  return s;
}

void C_Eprom::writeId_Pw(int addr, String s) {
  EEPROM.put(addr, s.length());
  for (int i = 0; i < s.length(); ++i) {
    char c = (char) s.charAt(i);
    if (i < 64) {
      EEPROM.write(addr+i+4, c);
    }
  }
  EEPROM.commit();
}

void C_Eprom::readAll() {
  m_ssid = readId_Pw(idAddr);
  m_pass = readId_Pw(pwAddr);
  m_orig = readId_Pw(oiAddr);
}

void C_Eprom::writeAll(String id, String pw) {
  // jocker permet de revenir volontairement aux valeurs d'origine
  if (id == String("JOCKER") || id == String("Jocker") || id == String("jocker")) {
    toOrigine();
  }
  else {
    m_ssid = id;    
    m_pass = pw;
    writeId_Pw(idAddr, m_ssid);
    writeId_Pw(pwAddr, m_pass);
  }
}

// retour à la configurtion d'origine, fixée dans eepromPutEsp.ino
void C_Eprom::toOrigine() {
  m_ssid = readId_Pw(oiAddr);
  m_pass = readId_Pw(opAddr);
  writeId_Pw(idAddr, m_ssid);
  writeId_Pw(pwAddr, m_pass);
}

// transforme m_ssid en 4 entiers
IPAddress C_Eprom::setIP() {
  static int v[4];
  static String s = m_ssid;
  static int ndx = 0;
  int p = s.indexOf(".");
  if (p != -1) {
    v[ndx++] = s.substring(0, p).toInt();
    s = s.substring(p + 1);
    setIP();
  }
  else {
    v[ndx] = s.toInt();
  }
  return IPAddress(v[0], v[1], v[2], v[3]); // local_IP
}






