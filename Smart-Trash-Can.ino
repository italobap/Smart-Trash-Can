//Trabalho de Gustavo, Italo e Lucas

//Instalação de Esp8266
//https://www.robocore.net/tutoriais/como-programar-nodemcu-arduino-ide#:~:text=Configurando%20a%20Arduino%20IDE&text=Agora%20clique%20em%20Ferramentas%20%2D%3E%20Placa,placas%20da%20sua%20Arduino%20IDE.
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>4
#include <Wire.h>

//Push safer = Notificação via push safer
// https://androidgeek.pt/enviar-e-receber-notificacoes-push-em-tempo-real-no-android-ou-ios
#include <Pushsafer.h>
#include <ArduinoJson.h>
#define PushsaferKey "HxqogWuaFBM6vx8LfEqW"

/*WiFiClientSecure client;*/
WiFiClient client;
Pushsafer pushsafer(PushsaferKey, client);


//Configuração do servo
Servo servo;
int servoPin = 13;

//Configuração do ultrassônico
int trigPin = 14;    
int echoPin = 12;   

long duracao, distancia;   
long distmed[3];   //vetor de distancia media


// ============= Você precisa preencher com seus dados ===================================
const char* ssid = "Redmi Note 7";
const char* password = "12345678";

//Pushbulet = Notificação via push bullet
//https://capsistema.com.br/index.php/2020/10/27/notificacao-pushbullet-de-internet-das-coisas-pushingbox-e-arduino/#Implementar_a_notificacao_usando_ESP8266_ou_Arduino
//https://www.automalabs.com.br/esp8266-como-enviar-notificacoes-usando-o-servico-pushbullet/
/*const char* PushBulletAPIKEY = "v9663055D5F9F8BA"; //A sua chave de API. Veja na sua conta Pushbullet.
const String message_title = "Enviado do ESP8266";
const String message_body = "Funciona!";
const char* host = "api.pushbullet.com";

// Pushingbox API
char *api_server = "api.pushingbox.com";
char *deviceId = "v9663055D5F9F8BA";  //Usar o seu codigo depois.*/

void setup() {       
    Serial.begin(9600);
    servo.attach(servoPin);  
    pinMode(trigPin, OUTPUT);  
    pinMode(echoPin, INPUT);  
    servo.write(0); 
    delay(100);
    servo.detach(); 

    //Conexão wifi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi conetado.");
} 

void medicao() {  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duracao = pulseIn(echoPin, HIGH); // Captura a duração de um pulso
  distancia = (duracao/2) / 29.1;   
  //Você divide por dois porque vai e volta, então o tempo seria o dobro de uma viagem só de ida.
  //O 29,1 é a velocidade do som (que é 343,5 m/s => 1 / 0,03435 = 29,1). Portanto, o resultado está em cm, não em polegadas
}

//Manda notificação via push bullet
/*void sendNotification() 
{
  struct PushSaferInput input;
  input.message = "This is a test message";
  input.title = "Hello!";
}*/

void loop() {
  //Distancia media
  for (int i=0;i<=2;i++) {   
    medicao();               
    distmed[i]=distancia;            
    delay(10); 
  }
  
  distancia=(distmed[0]+distmed[1]+distmed[2])/3; 
  
  //Abre tampa lixeira
  if ( distancia<30 ) {
    struct PushSaferInput input;
    input.message = "This is a test message";
    input.title = "Hello!";
    
    servo.attach(servoPin); //Anexa o servo a um pino
    delay(1);
    servo.write(0);  
    delay(3000);       
    servo.write(150); //Grava um valor no servo controlando o seu eixo  
    delay(3000);
    Serial.println(pushsafer.sendEvent(input));
    Serial.println("Sent");
  }
  Serial.print(distancia);
}
