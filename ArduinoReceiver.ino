//#include <string>

#define THRESHOLD 400
#define PIN_LDR A0
#define N_DATA 90
#define DELEY 500
int N = N_DATA;
bool enable_read = true;
int sensorValue;
bool ledApagou = false;
int countBits = 72;
int count = 0;
int polinomio = 0x07;

void setup() {
  Serial.begin(9600);

}

//decodificar
String tipo(String msg) {
    String bits = msg.substring(0, 2);
    return bits;
}

String processarMensagem(String msg) {
    msg = msg.substring(2);

    bool lastByteIsZero = true;
    for (int i = msg.length() - 8; i < msg.length(); i++) {
        if (msg[i] == '1') {
            lastByteIsZero = false;
            break;
        }
    }

    String msgRecebida = msg;
    if (lastByteIsZero) {
        msgRecebida = msg.substring(0, msg.length() - 8);
    }

    while (msgRecebida.endsWith("00000000")) {
        msgRecebida = msgRecebida.substring(0, msgRecebida.length() - 8);
    }

    return msgRecebida;
}


int calcularCRC(String msg) {
  int resto = 0;
  for (int i = 0; i < msg.length(); i++) {
    resto ^= msg[i] - '0'; // XOR com o próximo bit da mensagem
    for (int j = 0; j < 8; j++) {
      if ((resto & 0x80) != 0) {
        resto = (resto << 1) ^ polinomio;
      } else {
        resto <<= 1;
      }
    }
  }
  return resto;
}

String decodeNRZ_L(String binario) {
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

String decodeNRZ_I_H(String nrzI) {
  String binario = "";
  bool nivel = HIGH;
  for (int i = 0; i < nrzI.length(); i++) {
    if (nrzI.charAt(i) != (nivel == HIGH ? '1' : '0')) {
      binario += "1";
      nivel = !nivel;
    } else {
      binario += "0";
    }
  }
  return binario;
}

String decodeNRZ_I_L(String nrzI) {
  String binario = "";
  bool nivel = LOW;
  for (int i = 0; i < nrzI.length(); i++) {
    if (nrzI.charAt(i) != (nivel == HIGH ? '1' : '0')) {
      binario += "1";
      nivel = !nivel;
    } else {
      binario += "0";
    }
  }
  return binario;
}

String binarioParaCaractere(String binario) {
  String caractere;
  for (int i = 0; i < binario.length(); i += 8) {
    int decimal = 0;
    for (int j = 0; j < 8; j++) {
      decimal <<= 1;
      decimal |= binario[i + j] - '0';
    }
    if (decimal != 0) {
      caractere += (char)decimal;
    }
  }
  return caractere;
}


//receber dados
String recebeDados(){
  String mensagem = "";
  int sinal = 0;
  delay(DELEY);
  for(int indice = 0;indice < N;indice ++){
    sinal = analogRead(PIN_LDR);
    Serial.println(sinal);
    if(sinal > 60){
      mensagem += "1";
    } else {
      mensagem += "0";
    }
    delay(DELEY);
  }
  return mensagem;
}

void loop() {
  int sig = analogRead(PIN_LDR);
  if(sig < THRESHOLD && enable_read == true){
    Serial.println("INICIO DA LEITURA:");
    String msg = recebeDados();
    Serial.print("Mensagem Recebida: ");
    Serial.println(msg);
    String msgRecebida = processarMensagem(msg);
    int resto = calcularCRC(msgRecebida);

    if (resto != 0) {
      Serial.println("Erro: Resto da divisao diferente de zero. Programa finalizado.");
      while (true) {}
    }

    Serial.println("CRC verificado com sucesso!");
    String msgSemCRC = msgRecebida.substring(0, msgRecebida.length() - 8);
    
    if(tipo(msg) == "00"){
      Serial.println("Mensagem: " + binarioParaCaractere(msgSemCRC));
    }else if(tipo(msg) == "01"){
      Serial.println("Mensagem: " + binarioParaCaractere(decodeNRZ_L(msgSemCRC)));
    }else if(tipo(msg) == "10"){
      Serial.println("Mensagem: " + binarioParaCaractere(decodeNRZ_I_H(msgSemCRC)));
    }else if(tipo(msg) == "11"){
      Serial.println("Mensagem: " + binarioParaCaractere(decodeNRZ_I_L(msgSemCRC)));
    }
  


    


    enable_read = false;
  }
}
