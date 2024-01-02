#include <Arduino.h>
#include <SoftwareSerial.h>

// Requirement:  1.tick函数给出系统时间（非硬性要求）
//               2.蜂鸣器在一定时间间隔后开始以某个频率响
//               3.灯光在一定时间间隔下分颜色亮起，然后一起闪烁
//
// Lack:         1.不同颜色灯光分别亮起(Solved)
//
// Issue:        1.系统时间的间隔没法直接调用，应当可以写成一个函数封装调用，这样就可以把所有跟计时有关的内容统一用loop解决了，即中断函数
//               2.灯光会自己莫名关闭，已经减小到最小影响

/*********************************************
 Group:            AE-DIY-Group?
 Version:          V0.9.1Beta
 Editor:           Rededge
 Log:              Solved Buzzer issue
                   Police mode failed to execute correctly
 Date:             20231228
*********************************************/

SoftwareSerial BTSerial(7, 8);        // Rx | Tx 设置蓝牙串口
int BTr = 0;                          // 蓝牙反馈值
const int ledPins[] = {11, 10, 9, 6}; // 灯光输出引脚 6冷光，11暖光
const int pwmPins[] = {2};            // PWM输出引脚
int NledPin = 4;
int i = 0;
int getLed = 0;    // LED温柔模式的标记值
int BuzfreTmp = 0; // Buz计时器，初值为0
int Buzfre = 0;    // Buz计时器，初值为0
int Buzmax = 0;    // Buz占空比-（满-空）
int Buzmin = 0;    // Buz占空比-空
// int DelayBuz = 0;           //延时6s后蜂鸣器开始响
int bn = 0;
int brightness = 0;
int Hbri = 255;
int Lbri = 10;      // 此及前参数调节柔光模式下亮度范围
int fadeAmount = 0; // 渐变参数
int k_fA = 1;       // 符号转换器
int DelayLig = 0;   // 灯光延时时间，单位为毫秒
// int ticVal = 1;             //tick延时函数的输出间隔，单位为毫秒
int curMil = 0;
int preMil = 0;  // 此及前参数用于tick，无需设置
int DDL = 10000; // 用于薄纱模式倒计时，单位毫秒
int status = 0;

void setup()
{
  // 初始化设置
  Serial.begin(9600);
  BTSerial.begin(38400);

  for (int k = 0; k < 4; k++)
  {
    pinMode(ledPins[k], OUTPUT);
  }
  pinMode(pwmPins[0], OUTPUT);
}

// tick用于将内部时钟以自定义时间间隔输出模拟信号
//  int tick() {
//    curMil = millis();
//    if ((curMil - preMil) >= ticVal) {
//      preMil = curMil;
//      DDL += 1;
//      return 1;
//    } else {
//      return 0;
//    }
//  }

// void gettick() {
//   if (tick() == 1) {
//     DDL += 1;
//   }
// }

void buzzer()
{
  BuzfreTmp = millis();
  Buzmax = 400;
  Buzmin = 200;
  // if (millis() <= DelayBuz) {
  //   return;
  // }
  if ((BuzfreTmp - Buzfre) > Buzmax)
  {
    Buzfre = BuzfreTmp;
  } // 可优化
  // if (Buzfre >= Buzmin) {
  //   digitalWrite(pwmPins[0],HIGH);
  // } else {
  //   digitalWrite(pwmPins[0],LOW);
  // }
  if ((BuzfreTmp - Buzfre) <= Buzmin)
  {
    tone(pwmPins[0], 1760);
    Serial.println("High");
  }
  else
  {
    tone(pwmPins[0], 440);
    Serial.println("LOW");
  }
}

// 输出四路PWM灯光信号-温柔模式
int Led()
{
  // if (i > NledPin)
  // {
  //   bn = Hbri;
  //   for (int k = 0; k < NledPin; ++k)
  //   {
  //     analogWrite(ledPins[k], bn);
  //   }
  //   return getLed = 1;
  // }

  if (i > NledPin)
  {
    fadeAmount = 25;
    DelayLig = 100;
    if (curMil - preMil >= DelayLig)
    {
      preMil = curMil;

      bn = bn + k_fA * fadeAmount;

      if (bn <= Hbri && bn >= Lbri)
      {
        brightness = bn;
      }
      else if (bn > Hbri)
      {
        brightness = Hbri;
        k_fA = -k_fA;
      }
      else
      {
        brightness = Lbri;
        k_fA = -k_fA;
      }
    }
    for (int k = 0; k < NledPin; ++k)
    {
      analogWrite(ledPins[k], brightness);
    }
    return getLed = 1;
  }
  else
  {
    if (curMil - preMil >= DelayLig)
    {
      fadeAmount = 40;
      DelayLig = 100;
      preMil = curMil;

      bn = bn + k_fA * fadeAmount;
      if (bn > Hbri)
      {
        bn = 0;
        i++;
      }

      analogWrite(ledPins[i], bn);
      return getLed = 0;
      // for (int i = 0; i < NledPin; ++i) {
      //   analogWrite(ledPins[0], brightness);
      // }
    }
  }
}

// 输出四路PWM灯光信号-薄纱大家的麻麻模式
void Police()
{

  fadeAmount = 10;
  DelayLig = 10;

  if (curMil - preMil >= DelayLig)
  {
    preMil = curMil;

    bn = bn + k_fA * fadeAmount;

    if (bn <= Hbri && bn >= Lbri)
    {
      brightness = bn;
    }
    else if (bn > Hbri)
    {
      brightness = Hbri;
      k_fA = -k_fA;
    }
    else
    {
      brightness = Lbri;
      k_fA = -k_fA;
    }

    for (int k = 0; k < NledPin; k++)
    {
      analogWrite(ledPins[k], brightness);
    }
  }
}

void loop()
{
  curMil = millis();
  if (millis() <= DDL || getLed == 0)
  {
    Led();
  }
  else if (BTr == 0)
  {
    buzzer();
    Police();
    // Led();
  }

  Serial.println(float(millis() / 1000.0f), DEC);
  // 其他的循环操作
}