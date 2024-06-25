#define DELEY 500

int ledPin = 4;
int polinomio = 0x07;
  
unsigned char crc8(const String &data) {
  unsigned char crc = 0x00;
  for (int i = 0; i < data.length(); i += 8) {
    String byteString = data.substring(i, i + 8); // Extrai uma substring de 8 caracteres, representando um byte
    byte byteValue = (byte)strtol(byteString.c_str(), NULL, 2); // Converte a string binÃ¡ria para um byte
    crc ^= byteValue; // Faz XOR do byte atual com o CRC
    for (int j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ polinomio;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

String toBinary(String palavra) {
  String binary = "";
  for (int i = 0; i < palavra.length(); i++) {
    for (int j = 7; j >= 0; j--) {
      binary += ((palavra[i] >> j) & 1) ? '1' : '0';
    }
  }
  return binary;
}

String encodeNRZ_L_Normal(String binario) {
  String nrzL = "";
  for (int i = 0; i < binario.length(); i++) {
    if (binario.charAt(i) == '0') {
      nrzL += '0';
    } else {
      nrzL += '1';
    }
  }
  return nrzL;
}

String encodeNRZ_L_Invertido(String binario) {
  String nrzL = "";
  for (int i = 0; i < binario.length(); i++) {
    if (binario.charAt(i) == '0') {
      nrzL += '1';
    } else {
      nrzL += '0';
    }
  }
  return nrzL;
}

String encodeNRZ_I_H(String binario) {
  String nrzI = "";
  bool nivel = HIGH;
  for (int i = 0; i < binario.length(); i++) {
    if (binario.charAt(i) == '1') {
      nivel = !nivel;
    }
    nrzI += String(nivel);
  }
  return nrzI;
}

String encodeNRZ_I_L(String binario) {
  String nrzI = "";
  bool nivel = LOW;
  for (int i = 0; i < binario.length(); i++) {
    if (binario.charAt(i) == '1') {
      nivel = !nivel;
    }
    nrzI += String(nivel);
  }
  return nrzI;
}

void outputToLED(String codigo) {
  digitalWrite(ledPin, LOW);
  delay(50);
  for (int i = 0; i < codigo.length(); i++) {
    if (codigo.charAt(i) == '1') {
      digitalWrite(ledPin, HIGH); 
    } else {
      digitalWrite(ledPin, LOW);
    }
    delay(DELEY);
  }
}



void setup() {
  Serial.begin(9600);

  digitalWrite(ledPin, HIGH); 
  Serial.println("Digite uma palavra, maximo 8 caracteres:");
  while (Serial.available() == 0) {}
  String palavra = Serial.readStringUntil('\n');
  String binario = toBinary(palavra);
  
  unsigned char crc;
  String crcBin;
  
  String nrzL_Normal = encodeNRZ_L_Normal(binario);
  String nrzL_Invertido = encodeNRZ_L_Invertido(binario);
  
  String nrzI_H = encodeNRZ_I_H(binario);
  String nrzI_L = encodeNRZ_I_L(binario);

  Serial.println("Palavra: " + palavra);
  Serial.println();
  Serial.println("Qual metodo gostaria de decodificar?\n1) NZR-L Valores iguais\n2) NZR-L Valores invertidos\n3) NZR-I Alto\n4) NZR-I Baixo");
  while (Serial.available() == 0) {}
  int codifica = Serial.parseInt();
  Serial.println();
  
  switch(codifica){
  	case 1:
      crc = crc8(nrzL_Normal);
      crcBin = toBinary(String(char(crc)));
      Serial.println("CRC : " + crcBin);
      Serial.println("NRZ-L Normal: " + nrzL_Normal);
      outputToLED("00"+nrzL_Normal+crcBin);
    break;
    case 2:
      crc = crc8(nrzL_Invertido);
      crcBin = toBinary(String(char(crc)));
      Serial.println("CRC : " + crcBin);
      Serial.println("NRZ-L Invertido: " + nrzL_Invertido);
      outputToLED("01"+nrzL_Invertido+crcBin);
    break;
    case 3:
      crc = crc8(nrzI_H);
      Serial.println("Valor do CRC: "+crc);
      crcBin = toBinary(String(char(crc)));
      Serial.println("CRC : " + crcBin);
      Serial.println("NRZ-I High: " + nrzI_H);
      outputToLED("10"+nrzI_H+crcBin);
    break;
    case 4:
      crc = crc8(nrzI_L);
      crcBin = toBinary(String(char(crc)));
      Serial.println("CRC : " + crcBin);
      Serial.println("NRZ-I Low: " + nrzI_L);
      outputToLED("11"+nrzI_L+crcBin);
    break;
  }
  Serial.println();
  Serial.println("Mensagem enviada");
  digitalWrite(ledPin, LOW);
  //delay(DELEY*8)
}

void loop() {
  
}