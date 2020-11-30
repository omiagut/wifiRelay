/*
 * Initialisation de l'identifiant et du mot de passe pour wifiRelay
*/
#include <EEPROM.h>

#define idAddr 	0x00 // 4 octets pour la longueur de l'id, le reste pour l'id => 4 + maxi 64 < 80
#define pwAddr 	0x50 // 4 octets pour la longueur du pass, le reste pour le pass => 4 + maxi 64 < 80
#define oiAddr  0xA0 // la longueur maxi de l'id d'origine est celle de XXX.XXX.XXX.XXX + 4 => 19 < 32
#define opAddr  0xC0 // mot de passe d'origine conservé, longueur => 8

String id = "192.168.4.17";
String pw = "12345678";

int l; 

void writeId_Pw(int addr, String s) {
  EEPROM.put(addr, s.length());
  Serial.printf("%d %s", s.length()," - ");
  for (int i = 0; i < s.length(); ++i) {
    char c = (char) s.charAt(i);
    if (i < 64) { // 64 = longueur max identifiant ou pass
      EEPROM.write(addr+i+4, c);
      Serial.printf("%c, %s", c," ");
    }
  }
  Serial.println("");
}

void readId_Pw(int addr) {
  EEPROM.get(addr, l);
  Serial.printf("%d %s", l, " - ");
   for (int i = 0; i < l; ++i) {
    char c = EEPROM.read(addr+i+4);
    Serial.print(c);
  }
  Serial.println("");
}

void setup() {
  delay(10000);
  Serial.begin(9600);
  while (!Serial) {;}
  EEPROM.begin(512);
  writeId_Pw(idAddr, id);		// identifiant
  writeId_Pw(pwAddr, pw);		// password
  writeId_Pw(oiAddr, id);   // conserve l'id d'origine, le même que l'identifiant au départ
  writeId_Pw(opAddr, pw); 	// conserve le mot de passe d'origine, ibid
  EEPROM.commit();
}

void loop() { // seulement pour contrôler que l'écriture en eprom est bien faite
  delay(2000);
  Serial.println("read");
  readId_Pw(idAddr);
  readId_Pw(pwAddr);
  readId_Pw(oiAddr);
  readId_Pw(opAddr);
}
