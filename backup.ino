
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
//sensor ---begin
#include <Arduino.h>
//Cảm biến DTHH1
#include <DHT.h>
#define DHT_PIN 2       // Chân kết nối cảm biến DHT11 chân D6
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
const int pinBuzzer = 4;//D4 CÒI
const int pinLed = 16;//
//sensor ----end

//--------------JSON--------
#include <ArduinoJson.h>
const int JSON_SIZE = 200;
StaticJsonDocument<JSON_SIZE> jsonDoc;

//-------END JSON-----------

const int temper = 50;
const int humity = 100;

const String dataTopic = "environment/data";
const String receiveTopic = "environment/control";

//Update these with values suitable for your network.

const char* ssid = "Batonnnn";//tên wifi
const char* password = "12345678";//mật khẩu
const char* mqtt_server = "192.168.88.32";//server IP
const char* clientId = "raspberry/pi";
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;//biến kết nối wifi
PubSubClient client(espClient);//kết nối MQTT
unsigned long lastMsg = 0;//thời gian của lần gửi tin nhắn cuối cùng
#define MSG_BUFFER_SIZE  (255)//kích thước tối đa của tin nhắn
char msg[MSG_BUFFER_SIZE];//mảng lưu trữ kí tự nội dung tin nhắn
int value = 0;//biến đếm

//-----Hàm thiết lập kết nối wifi---------//
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);//hiển thị tên wifi
  WiFi.mode(WIFI_STA);//thiết lập chế độ esp đang kết nối wifi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void controlSensor(){
 int duration = 3000;//10s
      unsigned long startTime = millis(); // Thời điểm bắt đầu
      unsigned const  long interval = 500;//thời gian nháy
      unsigned long previousMillis = 0;
      bool flag = false;
      while (millis() - startTime < duration) {
          unsigned long currentMillis = millis();
          if (currentMillis - previousMillis >= interval) {
              previousMillis = currentMillis; // Cập nhật thời gian trước đó
             if(flag){
               digitalWrite(pinLed,HIGH);
               digitalWrite(pinBuzzer,HIGH);
               flag=false;
             }else{
               digitalWrite(pinLed,LOW);
               digitalWrite(pinBuzzer,LOW);
               flag=true; 
             }
            }
        yield();
    }
}


//----------hàm này được gọi khi nhận một tin nhắn từ MQTT---------//
//topic: chủ đề của tin nhắn, payload: nội dung của tin nhắn, lenght: độ dài của tin nhắn
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived OK[");
  Serial.print(topic);//chủ đề
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    //digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    controlSensor();
  } else {
    //digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}

//----Thiết lập lại kết nối MQTT nếu mất kết nối
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "raspberry_pi";
   // clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("begin", "hello world");//gửi một tin nhắn đến MQTT với topic begin
      // ... and resubscribe
      client.subscribe(receiveTopic.c_str());//đăng ký topic theo giõi
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


String collectSensorData(){
//   String result = "";
//    //BEGIN GAS SENSOR
//    float sensor_volt;// giá trị điện áp đọc được từ cảm biến (đọc được từ 0-1023 tương đương cho 0-5V) trong kk sạch thì sấp sỉ = 5V
//    float RS_air; //   giá trị của điện trở RS trong không khí sạch
//    float R0;  // Get the value of R0 via in LPG
//    float sensorValue;
//
//    /*--- Get a average data by testing 100 times ---*/
//    for(int x = 0 ; x < 100 ; x++)
//    {
//        sensorValue = sensorValue + analogRead(A0);
//    }
//    sensorValue = sensorValue/100.0;//tính trung bình cộng của cảm biến
//    /*-----------------------------------------------*/
//    sensor_volt = sensorValue/1024*5.0;
//    RS_air = (5.0-sensor_volt)/sensor_volt; // nếu trong kk sạch thì sẽ bằng 0 -> có gas sẽ tăng
//    R0 = RS_air/9.9; // The ratio of RS/R0 is 9.9 in LPG gas from Graph (Found using WebPlotDigitizer)
//
//    Serial.print("sensor_volt = ");
//  //  result += "sensor_volt = ";
//    Serial.print(sensor_volt);
//    Serial.println("V");
//  //  result += String(sensor_volt) + "V\n";
//
//    Serial.print("R0 = ");
//  //  result += "R0 = ";
//    Serial.println(R0);
// //   result += String(R0) + "\n";
//    sensorValue = 0;
//    //END GASSENSOR
//  
//    //DTH11 BEGIN
//    // Đọc giá trị nhiệt độ và độ ẩm từ cảm biến
//    float temperature = dht.readTemperature();
//    float humidity = dht.readHumidity();
//
//   if (isnan(temperature) || isnan(humidity)) {
//      Serial.println("Failed to read from DHT sensor!");
//    } else {
//      // In giá trị nhiệt độ và độ ẩm lên Serial Monitor
//      Serial.print("Temperature: ");
//  //    result += "Temperature: ";;
//      Serial.print(temperature);
//      Serial.print(" °C");
// //     result += String(temperature) + " °C";
//      Serial.print(" - Humidity: ");
//      Serial.print(humidity);
//      Serial.println(" %");
//    //  result += " - Humidity: ";
//  //    result += String(humidity) + " %\n";
//    }
//  //BTH11 END

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    boolean isNan = isnan(temperature) || isnan(humidity);
   if (isNan) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      // In giá trị nhiệt độ và độ ẩm lên Serial Monitor
      Serial.print("Temperature: ");
  //    result += "Temperature: ";;
      Serial.print(temperature);
      Serial.print(" °C");
 //     result += String(temperature) + " °C";
      Serial.print(" - Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    //  result += " - Humidity: ";
  //    result += String(humidity) + " %\n";  
    }


    float sensor_volt;
    float RS_gas; // Get value of RS in a GAS
    float ratio; // Get ratio RS_GAS/RS_air
    int sensorValue = analogRead(A0);
    sensor_volt=(float)sensorValue/1024*5.0;
    RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL

          /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
  //  ratio = RS_gas/R0;  // ratio = RS/R0
          /*-----------------------------------------------------------------------*/

    Serial.print("sensor_volt = ");
    Serial.println(sensor_volt);
    Serial.print("RS_ratio = ");
    Serial.println(RS_gas);
    Serial.print("Rs/R0 = ");
   // Serial.println(ratio);

    Serial.print("\n\n");

    delay(1000);

    String jsonString;
  //  jsonDoc["sensor_volt"] = sensor_volt;
    jsonDoc["RS_gas"] = RS_gas;
    if(isNan){
       jsonDoc["Temperature"] = 0;
       jsonDoc["Humidity"] = 0;
    }else{
      jsonDoc["Temperature"] = temperature;
      jsonDoc["Humidity"] = humidity;
    }
    
    serializeJson(jsonDoc, jsonString);
    Serial.print(jsonString);
    return jsonString; // Trả về chuỗi kết quả
}


void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

   dht.begin(); //BHT11
   pinMode(pinLed, OUTPUT);//đèn
   pinMode(pinBuzzer, OUTPUT);//còi
  
}


//method này dùng để kiểm tra nhiệt độ và độ ẩm. Nếu nhiệt độ và độ ẩm vượt quá ngưỡng quy định thì sẽ báo động

void loop() {
  //nếu nhiệt độ thay đổi thì hú còi
   // Đọc giá trị nhiệt độ và độ ẩm từ cảm biến
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if(temperature>=temper|| humidity>=humity){
      controlSensor();
    }
  

//nếu chưa kết nối được thì gọi hàm kết nối lại 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  unsigned long now = millis();
  //nếu đã được 2s thì gửi tin nhắn
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;

    String mess = collectSensorData();
    //char* charMess = mess.c_str();
//    char charMess[mess.length() + 1];
//    mess.toCharArray(charMess, sizeof(charMess));
    //snprintf (msg, MSG_BUFFER_SIZE,charMess, value);//nội dung tin nhắn gửi đến broker


    //R0 = 3
    Serial.print("Publish message: ");
    Serial.println(mess);
    client.publish(dataTopic.c_str(), mess.c_str());//gửi nội dung với chủ đề đăng ký chủ đề với tên esp1 và nội dung
  }
}
