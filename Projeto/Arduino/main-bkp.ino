#include <Servo.h>

#define Qtd_Amostras 10    // ***Quantas Amostras o filtro terá para filtrar, mude para testar outros filtros***
#define Intervalo_Amostragem 10 // ***definindo o intervalo de amostragem em ms. Mude para testar novos filtros***

int Leitura_analogica = 0; // Variável global que salva o dado bruto lido da porta serial.
unsigned long timer1=0; // A variável que irá contar o útimo 

// Sensor vars
int EMGPin = A0;
float EMGVal = 0;
int EMGTrigger = 110;

// Hand vars
int activeHand = 0;
bool handStatus = false;

Servo servo1;  // Finger 1
Servo servo2;  // Finger 2
Servo servo3;  // Finger 3
Servo servo4;  // Finger 4
Servo servo5;  // Finger 5

int openValues[] = {180, 10, 0, 180, 60};
int closeValues[] = {0, 180, 180, 0, 180};

// Control vars
int finger = 0;
int fingerDelay = 400;
long int lastReadFingerTime = 0;
long int delayTrigger = 1000 +  5 * fingerDelay;
long int lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  lastReadTime = millis();
  lastReadFingerTime = millis();
  servo1.attach(11);
  servo2.attach(10);
  servo3.attach(9);
  servo4.attach(6);
  servo5.attach(5);

  setupHand();
}

void loop() {
 
  Leitura_analogica = analogRead(EMGPin);  // Leitura_analogica aqui é o valor bruto
  //Leitura_analogica = random(100, 150);
  Amostragem(); // Essa é a função que fará a amostragem no tempo que determinamos no intervalo de amostragem que definimos na segunda linha de código.
  
  //Serial.print(Leitura_analogica); // Imprime o dado bruto
  //Serial.print(",");
  //Serial.println(filtroLogico(0)); // Imprime o dado filtrado


  EMGVal = filtroLogico(0);

  //Serial.println(millis());
  if (EMGVal >= EMGTrigger) {
    long int actualReadTime = millis();
    if (actualReadTime - lastReadTime >= delayTrigger) {
      activeHand += 1;
      lastReadTime = actualReadTime;
    }
  }


  if (activeHand >= 2) {
    if ((millis() - lastReadFingerTime) >=  fingerDelay) {
      lastReadFingerTime = millis();
      handStatus = changeHandStatus(handStatus);
    }
    
  }

  // Format string
  String output = String(activeHand) + "|" + String(convertValue(EMGVal)) + "|" + String(handStatus) + "|" + String(finger);

  Serial.println(output);  // Send to Serial
  delay(100);
}

float convertValue(float val) {
    return val * ( (5 * 1000000) / (1023.0 * 4140));
}

void Amostragem(){ // Essa função verifica se o tempo de amostragem  selecionado ocorreu
  if(millis() - timer1>Intervalo_Amostragem){ // Caso o tempo de amostragem tenha ocorrido, ele envia 1 para a função de filtro de media movel
    //Dessa forma a função sabe que é para atualizar o valor de saída para um novo valor filtrado
    filtroLogico(1);
    timer1 = millis(); // atualiza para contar o tempo mais uma vez
  }
}

float filtroLogico(bool atualiza_saida){ // Igual nos outros exemplos, ele usa variavel estática.
  int diferenca; // Variavel que salvará a diferenca entre o valor do filtro e a saída
  static float Saida_Filtro = 0; // A variavel 
  if(atualiza_saida == 0) return((double)Saida_Filtro);
  
  else{
    if(Leitura_analogica<Saida_Filtro){
      diferenca = abs(Leitura_analogica - Saida_Filtro);
      Saida_Filtro -= (float)diferenca/(float)Qtd_Amostras;
    }
    else if(Leitura_analogica>Saida_Filtro){
      diferenca = abs(Leitura_analogica - Saida_Filtro);
      Saida_Filtro += (float)diferenca/(float)Qtd_Amostras;
    }
  }
}

bool changeHandStatus(bool actualStatus) {
    
    if (actualStatus) {
      // Closing hand
      if (finger == 0){
        servo5.write(closeValues[4]);
        finger += 1;

      }else if (finger == 1){
        servo4.write(closeValues[3]);
        finger += 1;

      }else if (finger == 2){
        servo3.write(closeValues[2]);
        finger += 1;
        
      }else if (finger == 3){
        servo2.write(closeValues[1]);
        finger += 1;

      }else if (finger == 4){
        servo1.write(closeValues[0]);
        finger = 0;
        activeHand = 0;
        actualStatus = true; 
      }

    } else {
      // Opening hand
      if (finger == 0){
        servo1.write(openValues[0]);
        finger += 1;

      }else if (finger == 1){
        servo2.write(openValues[1]);
        finger += 1;

      }else if (finger == 2){
        servo3.write(openValues[2]);
        finger += 1;
        
      }else if (finger == 3){
        servo4.write(openValues[3]);
        finger += 1;


      }else if (finger == 4){
        servo5.write(openValues[4]);
        finger = 0;
        activeHand = 0;
        actualStatus = true; 
      }        
    }

    return actualStatus;
}

void setupHand() {
    
    // Opening hand
    servo1.write(openValues[0]);
    delay(fingerDelay);
    servo2.write(openValues[1]);
    delay(fingerDelay);
    servo3.write(openValues[2]);
    delay(fingerDelay);
    servo4.write(openValues[3]);
    delay(fingerDelay);
    servo5.write(openValues[4]);

    return;
}

