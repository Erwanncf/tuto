//Titre: Programme de base Mqtt LiveObject       //
//                                               //
//Auteur: Erwann Caroff                          //
//Date de création: 05/05/2017                   //
//                                               //
//Description: Connexion du kit                  //
//NodeMcu à un point wifi                        //
///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          Library                                                                     //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          Set configuration                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Entrez vos identifiants WIFI
const char* ssid     = "Livebox-1ffd";
const char* password = "0123456789";

//configuration pour LiveObject 
const char* topic_online ="dev/data";
const char* username ="json+device";
const char* ApiKey ="7ef7949fb37c454aa3087f63c276573d";
const char* myDevice = "Test";
const char* mqttHost = "liveobjects.orange-business.com";

String JsonLiveObjectString = "{\"s\":\"urn:lo:nsid:Objet:Id!DataType\",\"m\":\"version\",\"v\":{\"Value1\":12,\"Value2\":\"text\"},\"tags\": [\"TestTag\"]}";

//Configuration Lecture donnée sur Liveobject
String url = "https://liveobjects.orange-business.com/api/v0/data/streams/urn%3Alo%3Ansid%3AObjet%3AId!DataType?limit=1";
String UserAgent = "SMART DISPLAY";

const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
const char* fingerprint = "53 7F E0 E9 1B E9 F1 84 DD 07 5D 5D F7 B2 73 53 CD C3 37 81";
String LiveObjectData;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          on Mqtt Connect                                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttConnect(bool sessionPresent) {
  Serial.println("** Connected to the broker **");
//  Serial.print("Session present: ");
//  Serial.println(sessionPresent);
//  uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
//  Serial.print("Subscribing at QoS 2, packetId: ");
//  Serial.println(packetIdSub);
//  mqttClient.publish("test/lol", 0, true, "test 1");
//  Serial.println("Publishing at QoS 0");
//  uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
//  Serial.print("Publishing at QoS 1, packetId: ");
//  Serial.println(packetIdPub1);
//  uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
//  Serial.print("Publishing at QoS 2, packetId: ");
//  Serial.println(packetIdPub2);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          on Mqtt Disconnect                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("** Disconnected from the broker **");
  Serial.println("Reconnecting to MQTT...");
  mqttClient.connect();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          on Mqtt Subscribe                                                           //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("** Subscribe acknowledged **");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         on Mqtt Unsubscribe                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("** Unsubscribe acknowledged **");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          on Mqtt Message                                                             //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("** Publish received **");
//  Serial.print("  topic: ");
//  Serial.println(topic);
//  Serial.print("  qos: ");
//  Serial.println(properties.qos);
//  Serial.print("  dup: ");
//  Serial.println(properties.dup);
//  Serial.print("  retain: ");
//  Serial.println(properties.retain);
//  Serial.print("  len: ");
//  Serial.println(len);
//  Serial.print("  index: ");
//  Serial.println(index);
//  Serial.print("  total: ");
//  Serial.println(total);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         on Mqtt Publish                                                              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void onMqttPublish(uint16_t packetId) {
  Serial.println("** Publish acknowledged **");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                          Get Live Object Data                                                     //
//      Cherche les données sous format json sur Live Obect                                                                          //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetLiveObjectData()
{
    // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(mqttHost);
  if (!client.connect(mqttHost, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, mqttHost)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

   client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + mqttHost + "\r\n" +
                 "User-Agent:"+ UserAgent +"\r\n"  +
                 "X-API-KEY:"+ String(ApiKey) + "\r\n" + 
                 "Connection: close\r\n\r\n");
                  
  String line= "";

  while(client.available())
  {
     line += client.readStringUntil('\r');
  }
  
  int stringLength = line.length();
  
  char jsonAray[1000] = "";
  int cpt = 0;
  bool jsonlock = 0;

    for(int i=0;i<stringLength;i++)
  {

    //si début de json, on commence à écrire dans le tableau
    if((line[i] =='[') &&  (line[i+2] ='{') )
    {
      jsonlock = 1;
      i = i +1;
    }
    
    //si fin de json, on arrete de écrire dans le tableau
    if((line[i] ==']') &&  (line[i+1] =='\n'))
    {
      jsonlock = 0;
    }

    //Condition d'écriture dans le tableau
    if (jsonlock == 1)
    {
      jsonAray[cpt] = line [i];
      cpt = cpt +1;
    }
  }
  char jsonArayOK [cpt];
  memset(jsonArayOK,'\0',cpt);
  
  int h = 0;
  
   for(int i=0;i<cpt;i++)
  {
    if ((jsonAray[i] == '\n') || (jsonAray[i] == '\r') || (jsonAray[i] == ' '))
    {
      //Suprime les caracteres indésirables
      //Serial.println("Spression caractere");
      //cpt = cpt -1;
    }
    else
    {
      jsonArayOK[h] = jsonAray[i];
      h = h+1;
    }
  }
  
  //Convert local char array data to global data string
  LiveObjectData = String(jsonArayOK);
  
  Serial.print("Live object json: ");
  Serial.println(LiveObjectData);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                           Live Object Data Parser                                                 //
//Selectionne la températur cafetiere et le bruit en salle de créativité (permet de notifier la présence de persone ou de café prêt) //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LiveObjectDataParser()
{
 
    int Jsonlength = LiveObjectData.length();
    char jsonArayOK[Jsonlength]; 

    LiveObjectData.toCharArray(jsonArayOK, Jsonlength);   

    char charValue[1];
    int intValue=0;
    memset(charValue,'\0',1);
    
     for(int i=0;i<Jsonlength;i++)
     { 
       if(jsonArayOK[i]=='V'&& jsonArayOK[i+1]=='a'&& jsonArayOK[i+2]=='l'&& jsonArayOK[i+3]=='u'&& jsonArayOK[i+4]=='e'&& jsonArayOK[i+5]=='1'&& jsonArayOK[i+6]=='"'&& jsonArayOK[i+7]==':')
       {
          Serial.println("Value1 match in Json");
          charValue[0]=jsonArayOK[i+8];
          
          if (isDigit(jsonArayOK[i+9]))
          {
            charValue[1]=jsonArayOK[i+9];
          }
          else 
          {
            charValue[1]='\0';
          }
       }
     }
    int Value = atoi(charValue);
    Serial.print("Value1: ");
    Serial.println(Value);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          Setup                                                                       //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" OK");

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(mqttHost, 1883);
  mqttClient.setKeepAlive(20).setCleanSession(false).setWill(topic_online, 2, true, "no").setCredentials(username, ApiKey).setClientId(myDevice);
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}
void PublishOnLiveObect ()
{
    int JsonLiveObjectStringLength = JsonLiveObjectString.length()+1;
    char JsonLiveObjectArray[JsonLiveObjectStringLength];
    
    Serial.print("Json: ");
    Serial.println(JsonLiveObjectString);
    memset(JsonLiveObjectArray,'\0', sizeof(JsonLiveObjectArray)); 
    
    JsonLiveObjectString.toCharArray(JsonLiveObjectArray, JsonLiveObjectStringLength);
    mqttClient.publish (topic_online, 1, true, JsonLiveObjectArray) ; //Envoie des données
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          Loop                                                                        //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  PublishOnLiveObect ();
  GetLiveObjectData();
  LiveObjectDataParser();
  delay (4000);
}
