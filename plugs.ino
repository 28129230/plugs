

#include <Enerlib.h>//电源管理函数库
Energy energy;//实例
int ledPin = 3;//LED灯端口
int buttonPin = 2;//按钮信号端口
int LedState = HIGH;//初始化Led灯
String contComm="";//收集串口发出的命令标签字符串，用于串口控制
int plugs[8] = {4,5,6,7,8,9,11,10};//分配控制插座的开关引脚
int currState[8];

void INT0_ISR(void)
{
	if(energy.WasSleeping())//唤醒
	{
		Serial.println("PowerOn");//输出表示已经唤醒
	}
}

void setup()
{
	for(int i=0;i<8;i++)//初始化8个数字引脚，控制8个继电器状态为关闭
	{
		pinMode(plugs[i],OUTPUT);//引脚为输出
		digitalWrite(plugs[i],HIGH);//引脚为高电平
	}
	pinMode(ledPin, OUTPUT);
    pinMode(buttonPin,INPUT);
	Serial.begin(9600);//打开串口通讯
	attachInterrupt(0, INT0_ISR, RISING);//中断端口2，升压出发中断
    energy.PowerDown();//进入断电睡眠状态
}

void plugContral(int plugNum,char state)//插口控制函数，带2个参数，插口编号从0-7，状态为‘c’或‘o’
{
	if(state=='c'||state=='o')//判断一下字符是不是'c'或'o'
	{
		if(state == 'c')//如果State是'c'，就是close的缩写，表示端口通电
		{
			digitalWrite(plugs[plugNum],LOW);//继电器用的是低电平有效，低电平表示通电
			delay(200);//等待一下，有节奏感
		}
		else
		{
			digitalWrite(plugs[plugNum],HIGH);//关闭一般用字符'o'，控制继电器断电
			delay(200);
		}
	}
}

void loop()
{
	if(digitalRead(buttonPin) == HIGH)//接受按钮信号，如果高电平代表关机
	{
		delay(150);//消抖
		if(digitalRead(buttonPin) == HIGH)
		{
		LedState = !LedState;//改变led灯状态值
		}
	}
	
	digitalWrite(ledPin, LedState);//灭灯
	
	if(LedState==LOW)
	{
		Serial.println("PowerDown");//断电休眠
		energy.PowerDown();
	}
	
	while(Serial.available())//串口有数据的时候
	{
		contComm += char(Serial.read());//读取串口数据并加入到字符串变量中
		delay(2);//停顿一下等待接收，很关键不然数据收不全
	}
	if(contComm.length() > 0)//收到完整的字符串数据后
	{
		for(int i=0;i<=7;i++)//遍历字符串（继电器只有8个口）
		{
			plugContral(i,contComm[i]);//按字符串对应字符控制继电器
			currState[i]=digitalRead(plugs[i]);//把当前数字端口的状态写入数组
			Serial.print(currState[i],DEC);//把当前状态值发送到串口
		}
		contComm="";//把字符串清空
	}
}