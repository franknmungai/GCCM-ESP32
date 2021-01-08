#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DT 25
#define SCK 26
#define sw 23

//Network credentials
const char* ssid = "FRANK-BVTQT4E 9087";
const char* password = "12345678";

char jsonOutput[128];

long sample=0;
float val=0;
long count=0;

unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  pinMode(DT, OUTPUT);
  digitalWrite(DT,HIGH);
  digitalWrite(SCK,LOW);
  Count=0;
  pinMode(DT, INPUT);
  while(digitalRead(DT));
  for (i=0;i<24;i++)
  {
    digitalWrite(SCK,HIGH);
    Count=Count<<1;
    digitalWrite(SCK,LOW);
    if(digitalRead(DT)) 
    Count++;
  }
  digitalWrite(SCK,HIGH);
  Count=Count^0x800000;
  digitalWrite(SCK,LOW);
  return(Count);
}

void setup()
{
  Serial.begin(115200);
  pinMode(SCK, OUTPUT);
  pinMode(sw, INPUT_PULLUP);
  delay(1000);
  calibrate();

  //  Connect to wifi network with ssd and password
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop()
{
  count= readCount();
  int w=(((count-sample)/val)-2*((count-sample)/val));
  Serial.print("weight:");
  Serial.print((int)w);
  Serial.println("g");

  if(digitalRead(sw)==0)
  {
    val=0;
    sample=0;
    w=0;
    count=0;
    calibrate();
  }

//MAKE HTTP CALL TO API
if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
  //Send a request every 5 seconds

if(w<0)
{
  //to do
}
else{
  sendDataToApi((int)w);
  delay(3000);
  }
} else{
    Serial.println("Error in WiFi connection");   
  }

}

void sendDataToApi(int co)
{
    HTTPClient http;   
 
   http.begin("http://jsonplaceholder.typicode.com/posts");  
   http.addHeader("Content-Type", "application/json");             //set content-type header

   const size_t CAPACITY = JSON_OBJECT_SIZE(1);
   StaticJsonDocument<CAPACITY> doc;

   JsonObject object = doc.to<JsonObject>();
   object["count"] = co;
   serializeJson(doc, jsonOutput);
 
   int httpResponseCode = http.POST(String(jsonOutput));   //Send the actual POST request: Replace with json holding mass
 
   if(httpResponseCode>0){
 
    String response = http.getString();                       //Get the response to the request
 
    Serial.println(httpResponseCode);   
    Serial.println(response);           
   }else{
 
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode); 
   }
 
   http.end();  //Free resources
 
} 

 
void calibrate()
{
  for(int i=0;i<100;i++)
  {
    count=readCount();
    sample+=count;
//    Serial.println(count);
  }
  sample/=100;
//  Serial.print("Avg:");
//  Serial.println(sample);
  count=0;
  while(count<1000)
  {
    count=readCount();
    count=sample-count;
    Serial.println(count);
  }
  delay(2000);
  for(int i=0;i<100;i++)
  {
    count=readCount();
    val+=sample-count;
    Serial.println(sample-count);
  }
  val=val/100.0;
  val=val/100.0;        // calibrating weight
}

  
