#include <Arduino.h>

char val;
const int ledPins[] = { 11, 10, 9, 6 };  // 灯光输出引脚 6冷光，11暖光
int NledPin = 4;
int i = 0;
int getLed = 0;
const int pwmPins[] = { 3 };  // PWM输出引脚
int Buzfre = 0;               // Buz计时器，初值为0
int Buzmax = 0;               // Buz占空比-（满-空）
int Buzmin = 0;               // Buz占空比-空
//int DelayBuz = 0;             // 延时6s后蜂鸣器开始响
int bn = 0;
int brightness = 0;
int Hbri = 255;             
int Lbri = 0;               //此及前调节柔光模式下亮度范围
int fadeAmount = 0;
int k_fA = 1;               // 符号转换器
int DelayLig = 0;           // 灯光延时时间，单位为毫秒
int curMil = 0;
int preMil = 0;    //此及前用于tick，无需设置
int DDL = 20000;   //用于薄纱模式倒计时，单位毫秒

void setup() {
  // 初始化设置
  Serial.begin(9600);
  pinMode(13,OUTPUT);        //观察蓝牙模块是否连接成功
  for (int k = 0; k < 4; k++) {
    pinMode(ledPins[k], OUTPUT);
  }
}

void buzzer() {
  Buzfre = millis();
  Buzmax = 400;
  Buzmin = 200;
  while (Buzfre > Buzmax) {
    Buzfre -= Buzmax;
  }//可优化
  if (Buzfre >= Buzmin) {
    pinMode(pwmPins[0], OUTPUT);
  } else {
    pinMode(pwmPins[0], INPUT);
  }
}

//输出四路PWM灯光信号-温柔模式
int Led() {
  if (i > NledPin) {
    bn = Hbri;
    for (int k = 0; k < NledPin; ++k) {
      analogWrite(ledPins[k], bn);
    }
    return 1;
  }
  fadeAmount = 40;
  DelayLig = 100;
  curMil = millis();
  k_fA = 1;

  if (curMil - preMil >= DelayLig) {
    preMil = curMil;

    bn = bn + k_fA * fadeAmount;
    if (bn > Hbri) {
      bn = 0;
      i++;
    }

    analogWrite(ledPins[i], bn);
    return 0;
  }
}

//输出四路PWM灯光信号-薄纱大家的麻麻模式
void Police() {

  fadeAmount = 256;
  DelayLig = 50;
  curMil = millis();

  if (curMil - preMil >= DelayLig) {
    preMil = curMil;

    bn = bn + k_fA * fadeAmount;

    if (bn <= Hbri && bn >= Lbri) {
      brightness = bn;
    } else if (bn > Hbri) {
      brightness = Hbri;
      k_fA = -k_fA;
    } else {
      brightness = Lbri;
      k_fA = -k_fA;
    }

    for (int i = 0; i < NledPin; ++i) {
      analogWrite(ledPins[i], brightness);
    }
  }
}

void loop() {
val = Serial.read();//读取串口缓冲区的数据
  
  if(Serial.available()>0){//如果串口有数据输入，根据输入的不同来反应
   if (val == 'n') //当接收到的数据为n时
  {  Police();
     digitalWrite(13, HIGH); //点亮LED灯表示蓝牙程序运行
     Serial.println("LED ON! Police!"); //串口输出LED ON!
  }
  else if (val == 'f') {//当接收到的数据为f时
    Led();
     digitalWrite(13, LOW);//熄灭LED灯
     Serial.println("LED OFF! Led!");//串口输出LED OFF！

  }
  else if (val == 'b') {//当接收到的数据为b时
    digitalWrite(13, HIGH);
    buzzer();
     Serial.println("LED ON! Buzzer!");
  }

  }
  else if (millis() <= DDL || getLed == 0) {
    getLed = Led();
  } 
  else {
    Police();
    //Led();
  }
  Serial.println("输出四路灯光信号和一路蜂鸣器信号");
}
  
