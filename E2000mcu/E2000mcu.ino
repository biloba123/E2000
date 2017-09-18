//#include <Wire.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "biloba";//连接的路由器的名字
const char* password = "123456";//连接的路由器的密码
const char* mqtt_server = "XXXXXX";//服务器的地址 
const int port=61613;//服务器端口号
const char* inTopic = "E2000";
const char* outTopic = "Code"; 

WiFiClient espClient;
PubSubClient client(espClient);

String data = "";

void setup_wifi() {//自动连WIFI接入网络
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {//用于接收数据
  String str((char*)payload);
  Serial.println(str.substring(0,length));
}

void reconnect() {//等待，直到连接上服务器
  while (!client.connected()) {//如果没有连接上
    if (client.connect("biloba")+random(999999999)) {//接入时的用户名，尽量取一个很不常用的用户名
      client.subscribe(inTopic);//接收外来的数据时的intopic
    } else {
      Serial.print("failed, rc=");//连接失败
      Serial.print(client.state());//重新连接
      Serial.println(" try again in 5 seconds");//延时5秒后重新连接
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);//设置串口波特率
  setup_wifi();//自动连WIFI接入网络
  client.setServer(mqtt_server, port);//端口号
  client.setCallback(callback); //用于接收服务器接收的数据
}

void loop() {
  reconnect();//确保连上服务器，否则一直等待。
  client.loop();//MUC接收数据的主循环函数。
  if (receive()) {
    Serial.println(data);
    char msg[200];
    data.toCharArray(msg,data.length()+1);
    client.publish(outTopic, msg);//发送数据
  }
}

bool receive() {
  bool op = 0;
  while (Serial.available() > 0)
  {
    if (!op) {
      data = "";
    }
    data += char(Serial.read());
    delay(2);//一个一个接收数据，每次停顿2毫秒
    op = 1;
  }
  return op;
}
