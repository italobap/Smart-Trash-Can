//Trabalho de Gustavo, Italo e Lucas

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>


Servo servo;
//Configuração do ultrassônico
int trigPin = 14;    
int echoPin = 12;   
int servoPin = 13;
long duracao, distancia;   
long distmed[3];   //vetor de distancia media

// ============= Você precisa preencher com seus dados ===================================
const char* ssid = "VIVOFIBRA-1911";
const char* password = "essaehruim";
 
const char* PushBulletAPIKEY = "v9663055D5F9F8BA"; //A sua chave de API. Veja na sua conta Pushbullet.
const String message_title = "Enviado do ESP8266";
const String message_body = "Funciona!";
const char* host = "api.pushbullet.com";

// Pushingbox API
char *api_server = "api.pushingbox.com";
char *deviceId = "v9663055D5F9F8BA";  //Usar o seu codigo depois.

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

//Manda notificação
void sendNotification() 
{
  WiFiClient client; 
  Serial.println("Enviando notificação para " + String(api_server));
  if (client.connect(api_server, 80)) { 
     Serial.println("Conectado ao servidor");
     String message = "devid=" + String(deviceId) + "Teste" +"\r\n\r\n";
     
     client.print("POST /pushingbox HTTP/1.1\n");
     client.print("Host: api.pushingbox.com\n");
     client.print("Connection: close\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(message.length());
     client.print("\n\n");
     client.print(message);
  }
  client.stop();
  Serial.println("Notificação enviada!");
}

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
    servo.attach(servoPin); //Anexa o servo a um pino
    delay(1);
    servo.write(0);  
    delay(3000);       
    servo.write(150); //Grava um valor no servo controlando o seu eixo  
    delay(3000);
    servo.detach(); // Desconecte o servo do seu pino
    sendNotification();    
  }
  Serial.print(distancia);
}
