/*
* Gestion ssid et password actifs
*/
#ifndef CEPROM
#define CEPROM
#include <Arduino.h>
#include <EEPROM.h>
#include <IPAddress.h>

#define idAddr  0x00  // adresse eeprom de l'identifiant actif
#define pwAddr  0x50  // adresse eeprom du mot de passe actif
#define oiAddr  0xA0  // adresse eeprom de l'identifiant d'origine fixé par eepromPutEsp.ino
#define opAddr  0xC0  // adresse eeprom du mot de passe d'origine fixé par eepromPutEsp.ino

class C_Eprom {
	public :	
	C_Eprom();
	String m_ssid;  // identifiant actif
	String m_pass;  // mot de passe actif
  String m_orig;  // identifiant d'origine
	
	void setSsid(String s) {m_ssid = s;}
	void setPass(String s) {m_pass = s;}

	String ssid() {return m_ssid;}
	String pass() {return m_pass;}
  String orig() {return m_orig;}

	String readId_Pw(int addr);
	void writeId_Pw(int addr, String s);
	void readAll();
	void writeAll(String id, String pw);
  void toOrigine();
  IPAddress setIP();
};
#endif


