 #include <SoftwareSerial.h>

//软串口定义
SoftwareSerial mySerial(2,3);//2 : rx ,3 : tx
//扫码结果
String data = "";

//光强
const int pinLight=5;
const int lightBound=400;//光强界限
bool isLight=false;
int lastMillis=0;

void setup() {
  Serial.begin(9600);//设置串口波特率
  mySerial.begin(9600);//设置软串口波特率

  light(false);
}

void loop() {
  
  if ( receive()) {
    Serial.println(data);//串口输出
    if(isOrderCode()||isBookBarCode()){
      Serial.println(",,,,,,,");
      mySerial.print(data);//软串口输出
    }
  }

  controlLight();
}

//串口是否收到二维码模块数据
bool receive(){
  bool op=0;
  while (Serial.available() > 0){
  if(!op){
    data="";
  }
    data += char(Serial.read());
    delay(2);//一个一个接收数据，每次停顿2毫秒
    op=1;
}
  return op;
}

//是否是借还书二维码
bool isOrderCode(){
  return data.startsWith("{\"Id\":")&&data.endsWith("\"}");
}

bool isBookBarCode(){
  return data.charAt(0)=='A'&&data.length()==10;
}

//照明灯
void light(bool isLight){
  Serial.write(0x08);
  Serial.write(0xC6);
  Serial.write(0x04);
  Serial.write(0x08);
  Serial.write(0x00);
  Serial.write(0xF2);
  Serial.write(0x02);
  if(isLight){//识读时亮
    Serial.write(0x00);
    Serial.write(0xFE);
    Serial.write(0x32);
  }else{//长灭
    Serial.write(0x02);
    Serial.write(0xFE);
    Serial.write(0x30);
  }
}

//照明灯控制
void controlLight(){
  if(millis()-lastMillis>2000){
    if(analogRead(pinLight)>lightBound&&isLight){
      isLight=false;
      light(false);
    }else if(analogRead(pinLight)<=lightBound&&!isLight){
      isLight=true;
      light(true);
    }
    lastMillis=millis();
  }
}


