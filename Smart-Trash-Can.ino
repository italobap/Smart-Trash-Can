#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiClientSecure.h>

#include <Servo.h>

//Configuração do servo
Servo servo;
int servoPin = 13;

//Configuração do ultrassônico abertura tampa
int trigPin0 = 14;    
int echoPin0 = 12;   

//Configuração do ultrassônico nivel lixeira
int trigPin1 = 4;    
int echoPin1 = 5;   

long duracao, distancia;   
long distmed[3];   //vetor de distancia media
long duracao2, distancia2;   
long distmed2[3];   //vetor de distancia media

BlynkTimer timer; //OBJETO DO TIPO BlynkTimer
 
//CREDENCIAIS DO WIFI E AUTH TOKEN.
//O AUTH TOKEN É ENVIADO PARA SEU EMAIL QUANDO CRIA O PROJETO NO APP. BASTA COPIAR O TOKEN E COLAR AQUI NO CÓDIGO
char auth[] = "Your Auth Token";
char ssid[] = "Redmi Note 7";
char pass[] = "12345678";
const char* ssid2 = "Redmi Note 7";
const char* password2 = "12345678";

WidgetLED green(V1);
WidgetLED orange(V2);
WidgetLED red(V3);

int distance = 0;
int thresh [3] = {20,12,4};

void setup() {       
    Serial.begin(115200);

    //INICIA A COMUNICAÇÃO COM O BLYNK
    Blynk.begin(auth, ssid, pass);
    
    servo.attach(servoPin);  
    pinMode(trigPin0, OUTPUT);  
    pinMode(echoPin0, INPUT);
    pinMode(trigPin1, OUTPUT);  
    pinMode(echoPin1, INPUT);    
    servo.write(0); 
    delay(100);
    servo.detach(); 

    WiFi.begin(ssid2, password2);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi conetado.");

} 

void medicao() {  
  digitalWrite(trigPin0, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin0, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin0, LOW);
  pinMode(echoPin0, INPUT);
  duracao = pulseIn(echoPin0, HIGH); // Captura a duração de um pulso
  distancia = (duracao/2) / 29.1;   
  //Você divide por dois porque vai e volta, então o tempo seria o dobro de uma viagem só de ida.
  //O 29,1 é a velocidade do som (que é 343,5 m/s => 1 / 0,03435 = 29,1). Portanto, o resultado está em cm, não em polegadas
}

void medicao2() {  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin1, LOW);
  pinMode(echoPin1, INPUT);
  duracao2 = pulseIn(echoPin1, HIGH); // Captura a duração de um pulso
  distancia2 = (duracao2/2) / 29.1;   
  //Você divide por dois porque vai e volta, então o tempo seria o dobro de uma viagem só de ida.
  //O 29,1 é a velocidade do som (que é 343,5 m/s => 1 / 0,03435 = 29,1). Portanto, o resultado está em cm, não em polegadas
}

void loop() {
  Blynk.run();
  //Distancia media
  for (int i=0;i<=2;i++) {   
    medicao();               
    distmed[i]=distancia; 
    medicao2();               
    distmed2[i]=distancia2;              
    delay(10); 
  }
  
  distancia=(distmed[0]+distmed[1]+distmed[2])/3; 
  
  //Abre tampa lixeira
  if ( distancia<30 ) {
    
    servo.attach(servoPin); //Anexa o servo a um pino
    delay(1);
    servo.write(0);  
    delay(3000);       
    servo.write(150); //Grava um valor no servo controlando o seu eixo  
    delay(3000);

  }
  Serial.print(distancia);
  
  if(distancia2<=thresh[0] && distancia2>=thresh[1] && distancia2>=thresh[2]){
    green.on();
    //Serial.println(1);  //debugging sake
    }
  else if(distancia2<=thresh[0] && distancia2<=thresh[1] && distancia2>=thresh[2]){
    green.on();
    orange.on();
    //Serial.println(2);
  }
  else if(distancia2<=thresh[0] && distancia2<=thresh[1] && distancia<=thresh[2]){
    green.on();
    orange.on();
    red.on();
    //Serial.println(3);
  }
  else{
    green.off();
    orange.off();
    red.off();
    //Serial.println(0);
  }
  delay(100);
 
}
