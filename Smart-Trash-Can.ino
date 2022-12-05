//Projeto oficina integração 1
//Grupo: Gustavo, Italo e Lucas

//Wifi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

//Blynk
#define BLYNK_TEMPLATE_ID "TMPLBYoCcCbW"
#define BLYNK_DEVICE_NAME "Lixeira"
#define BLYNK_AUTH_TOKEN 
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

//Led
int led = 0;

//Servo
#include <Servo.h>

//Definição do servo
Servo servo;
int servoPin = 13;

//Configuração do ultrassônico de abertura
int trigPin = 14;    
int echoPin = 12;   
long duracao, distancia;   
long distmed[3];   //vetor de distancia media

//Configuração do ultrassônico nivel lixeira
#define trigPin1 5
#define echoPin1 4
long duracao2, distancia2;   
long distmed2[3];   //vetor de distancia media
//Nivel da lixeira
int MaxLevel = 30;

//Conexão wifi
const char* ssid = "Redmi Note 7";
const char* password = "12345678";

// Notificação
char *host = "api.pushingbox.com";
char *devid = "v9663055D5F9F8BA";  

//Autenticação Blynk
char auth[] = "F0312baSytZRIF6p-cuvv8kqEKWP0S5V";

BlynkTimer timer;

void setup() {       
    Serial.begin(115200);

    //Inicialização dos sensores ultrassonicos 
    pinMode(trigPin, OUTPUT);  
    pinMode(echoPin, INPUT);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(led, OUTPUT);

    //Inicialização do servo motor
    servo.attach(servoPin);  
    servo.write(0); 
    delay(100);
    servo.detach(); 

    //Conexão wifi e autenticação blynk
    WiFi.begin(ssid, password);
    Blynk.begin(auth, ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");

    //A cada 500ms chama função ultrasonic
    timer.setInterval(500L, ultrasonic);
} 

void yield(void);

//Nivel da lixeira
void ultrasonic() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  long t = pulseIn(echoPin1, HIGH);
  int distance = t / 29 / 2;

  int blynkDistance = (distance - MaxLevel) * -1;
  delay(2);
  if (distance <= MaxLevel && blynkDistance <= 22) {
    Blynk.virtualWrite(V2, (blynkDistance));
    Serial.print(blynkDistance);
   }
  else if(distance <= MaxLevel &&(blynkDistance >= 23)){
        Blynk.virtualWrite(V2, (blynkDistance));
          //Serial.print(blynkDistance);
         notificacao();
         while(1){
          digitalWrite(led, HIGH);
          yield();
         }
  }
  else {
    Blynk.virtualWrite(V2, 0);
  }
}

//Calculo da distancia para abertura
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

void notificacao(){
    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/pushingbox";
    url += "?devid=";
    url += devid;
    
    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }
    Serial.println();
    Serial.println("closing connection");
  
}

void loop() {

  Blynk.run();
  timer.run();
  
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
    delay(500);       
    servo.write(180); //Grava um valor no servo controlando o seu eixo  
    delay(3000);
    servo.write(0); //Grava um valor no servo controlando o seu eixo  
    delay(3000);
    servo.detach(); // Desconecte o servo do seu pino    
  }
}
