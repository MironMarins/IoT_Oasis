#include <FastLED.h> // Biblioteca para neopixel
#include <RTClib.h> //biblioteca para sensor rtc

// Inclusão de bibliotecas
// Já instaladas:
#include <WiFi.h> 
#include <time.h>

// Instalar:
#include <AsyncTCP.h>
#include <PubSubClient.h>   
#include <ArduinoJson.h>

//definição dos pinos
#define boardLED 2
#define NUM_LEDS 16
#define LED_PIN 2 
#define BUZZER_PIN 18
#define BUTTON_PIN 15

bool chuveiro_ligado;
bool tempo_acabando;
bool estrapolou;
bool BT;
bool flag;
bool ligaled = LOW;
int faltam2 = 1;
int acabou_tempo = 0;
int azul =0;
int verde = 0;
int amarelo = 0;
int laranja = 0;
int pisca_vermelho =0;
int inicio = 0;
int iniciosec =0;
int timer =0;
int agora =0;
String status;
String uso;
RTC_DS1307 rtc;

CRGB leds[NUM_LEDS];


// Crie um ID para o seu módulo 
const char* ID = "Oasis"; 
// Crie um ID para o seu sensor
const char* SensorID = "chuveiro_alarme";

// Credenciais WiFi para uso no Wokwi:
const char* SSID = "Wokwi-GUEST"; // 
const char* PASSWORD = ""; // 

// CONFIGURE AQUI OS DADOS DO SERVIDOR
// MQTT
const char* BROKER_MQTT = "74.179.84.66"; // URL ou IP do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT (padrão 1883)
const char* mqttServer = "74.179.84.66"; // URL ou IP do servidor onde roda o Node-RED
const int mqttPort = 1880; // Porta para acessar o servidor onde roda o Node-RED
const char* mqttUser = "gs2024"; // Usuário do Broker
const char* mqttPassword = "q1w2e3r4"; // Senha do Broker
#define TOPICO_SUBSCRIBE "GS_2TDSPV"     // Tópico MQTT de escuta
#define TOPICO_PUBLISH   "GS_2TDSPV"    // Tópico MQTT de envio de informações para Broker
 
// Configurações MQTT
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  // Variável que armazena o estado atual da saída
char buffer[256];


void setup() {
  Serial.begin(115200);
  rtc.begin();
  DateTime now = rtc.now();
  DateTime newTime = DateTime(now.year(), now.month(), now.day(), now.hour(), 0, 0);
  rtc.adjust(newTime);
  //para o led
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200);
  //buzzer
  pinMode(BUZZER_PIN, OUTPUT);
    
  //botão
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // MQTT setup
  pinMode(boardLED, OUTPUT);    
  digitalWrite(boardLED, LOW); 

  initWiFi();
  initMQTT(); 
  
  
}

//-------------------------------Rotina principal------------------------

void loop() {
  
 
  //vermelho_total();
  //DateTime now = rtc.now();
  //Serial.print(now.minute(),DEC);
  //Serial.print(":");
  //Serial.print(now.second(),DEC);
  //Serial.println("");
  //delay(1000);
  initWiFi();
    delay(1000);
    
    StaticJsonDocument<200> doc;
    doc["ID"] = ID;
    doc["Sensor"] = SensorID;
    doc["IP"] = WiFi.localIP();  
    doc["estrapolou"] = estrapolou;
    doc["tempo_acabando"] = tempo_acabando;
    doc["status"] = status;
    doc["tempo_de_uso"] =uso;
    // doc["Nome da grandeza 3 lida"] = variavel3_a_ser_enviada;

    serializeJson(doc, buffer);
    Serial.println(buffer);
    
    initMQTT();
    EnviaEstadoOutputMQTT();    
    delay(10000);     
    piscaLED();  
  
    
    liga_desliga();
    timer_cor();
    buzzer();
  
}

//Função: inicializa parâmetros de conexão MQTT
// (endereço do broker, porta e função de callback)



//-------------------------Conectar e reconectar ao wifi----------------------------

//Função: inicializa e conecta-se na rede WiFi desejada
void initWiFi(){
WiFi.begin(SSID, PASSWORD);
delay(2000);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
Serial.println("---------------------");  
Serial.println("WiFi conectado...    ");
Serial.println("---------------------");
Serial.print("ESP Board MAC Address:  ");
Serial.println(WiFi.macAddress());
Serial.print("Conectado a rede ");
Serial.println(SSID);
Serial.print("O endereço IP é: ");
Serial.println(WiFi.localIP());

}

void reconectWiFi(){
    WiFi.begin(SSID, PASSWORD);
    delay(3000);
    Serial.println("---------------------");  
    Serial.println("WiFi conectado...    ");
    Serial.println("---------------------");
    Serial.print("Conectado em: ");
    Serial.println(SSID);
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP()); 
    Serial.print("ESP Board MAC Address:  ");
    Serial.println(WiFi.macAddress()); 
    Serial.println("----------------------");    
}
//--------------------------MQTT--------------------------------------------
//Função: inicializa parâmetros de conexão MQTT
// (endereço do broker, porta e função de callback)
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   
    while (!MQTT.connected())
    {
        Serial.println("Conectando ao Broker MQTT...");
        if(MQTT.connect(SensorID, mqttUser, mqttPassword ))
            {
                Serial.println("Conectado!!");
            } 
        else
            {
                Serial.print("failed with state ");
                Serial.print(MQTT.state());
                delay(2000);
            }
    }
}

//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//Em caso de desconexão (qualquer uma das duas), a conexão é refeita.

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
    Serial.println("Conectando ao Broker MQTT...");
    
    initMQTT()  ;
    reconectWiFi(); //se não há conexão com o WiFi, a conexão é refeita
}

//Função: envia ao Broker o estado atual do output 
void EnviaEstadoOutputMQTT(void)
{
    MQTT.publish(TOPICO_PUBLISH, buffer);
    Serial.println("Informação enviada ao Broker MQTT!!");
}

//------------------------LED----------------

void piscaLED()
{
    digitalWrite(boardLED, HIGH);
    delay(300);
    digitalWrite(boardLED, LOW);  
}

// Crie uma função para ler o sensor
void leituraSensor(){
  // instruções para ler o valor do sensor
}


//-------------------------- Dispositivo -----------------------------------



void preto_total(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  delay(200);
}
  
void liga_desliga(){

  BT = digitalRead(BUTTON_PIN);
  delay(50);

   if(BT== LOW && flag == LOW){
     estrapolou=false;
     flag =HIGH;
     timer=timer*0;
     
     Serial.println("o Botão 1 foi pressionado!!");
   }
   if(BT == HIGH && flag ==HIGH){
       DateTime now = rtc.now();
       DateTime newTime = DateTime(now.year(), now.month(), now.day(), now.hour(), 0, 0);
       rtc.adjust(newTime);
       inicio = now.minute();
       Serial.print("minuto inico");
       Serial.println(inicio);
       iniciosec = now.second();
       Serial.print("segundo inicio");
       Serial.println(iniciosec);
       faltam2 = now.minute() + 5;
       acabou_tempo = now.minute() + 7;
       azul = now.minute() +2;
       verde = now.minute() +3;
       amarelo = now.minute() +4;
       laranja = now.minute() +5;
       pisca_vermelho=now.minute() +6;
       
       iniciosec= now.second();
       flag=LOW;
       Serial.println("o Botão 1 foi solto!!");
       timer = timer+1;
               
       
       if(ligaled == LOW){
                  ligaled = HIGH;
                  chuveiro_ligado=true; }
       else{
                  ligaled = LOW;
                  Serial.print("preto1");
                  chuveiro_ligado=false;
                  uso=String(now.minute()) + "." + String(now.second());
                  Serial.print(uso);
                  preto_total();
                  
                  if(estrapolou==true){
                    uso="7.00";
                  }}

  
  }
  if(BT == HIGH && flag ==LOW && timer==1){
        DateTime now = rtc.now();    
        if(now.minute() >= faltam2) {
    tempo_acabando=true;
    }
  if(BT == HIGH && flag == LOW && timer ==1 && agora <= acabou_tempo && chuveiro_ligado ==true ){
       DateTime now = rtc.now();
       agora = now.minute();
       Serial.print(now.minute());
       Serial.print(":");
       Serial.print(now.second());
       Serial.println("");
       delay(1000);
       if(now.minute() >= acabou_tempo){
              estrapolou = true;
              chuveiro_ligado=false;
              tempo_acabando==false;
              digitalWrite(BUZZER_PIN, LOW);
              flag=HIGH;
              Serial.print("preto2");
              preto_total();
            }
            }

  
}}

void buzzer(){
  if(tempo_acabando==true && estrapolou==false){
    digitalWrite(BUZZER_PIN, HIGH); // Liga o buzzer
    delay(1000); // Mantém o buzzer ligado por 1 segundo
    digitalWrite(BUZZER_PIN, LOW); // Desliga o buzzer
    delay(1000);
    }
}

void timer_cor(){
  DateTime now = rtc.now();
  if(chuveiro_ligado==true){
     if(azul > now.minute()){
        status = "azul";
        Serial.print(status);
        fill_solid(leds, NUM_LEDS, CRGB::Blue);
        FastLED.show();
        delay(200);}
     else if(azul <= now.minute() && verde > now.minute()){
        status = "verde";
        Serial.print("status");
        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
        delay(200);}
     else if(verde <= now.minute() && amarelo >now.minute()){
        status = "amarelo";
        Serial.print(status);
        fill_solid(leds, NUM_LEDS, CRGB::Yellow);
        FastLED.show();
        delay(200);} 
     else if(amarelo <= now.minute() && laranja >now.minute()){
        status = "laranja";
        Serial.print(status);
            fill_solid(leds, NUM_LEDS, CRGB::Orange);
            FastLED.show();
            delay(200);} 
     else if(laranja <= now.minute() && pisca_vermelho >now.minute() ){
        status = "vermelho";
        Serial.print(status);
            fill_solid(leds, NUM_LEDS, CRGB::Red);
            FastLED.show();}
     else{
            status = "na linha";
            Serial.print(status);
            fill_solid(leds, NUM_LEDS, CRGB::Red);
            FastLED.show();
            delay(500); 
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            FastLED.show();
            delay(500);}
                          }
                }