/*
 * init.cpp
 *
 *  Created on: 2014/08/09
 *      Author: junya_s
 */

#include "init.h"
#include "can_func.h"

/*----------------------
 -----機能選択するピン-----
 ----------------------*/
//QEI 		ENC0(p13,p14 , NC, 624, QEI::X4_ENCODING);
Serial	serial0(p13,p14);
QEI		ENC1(p27,p28 , NC, 624, QEI::X4_ENCODING);
//I2C		i2c0(p28,p27);

DigitalOut OUT0(p5);
//DigitalIn IN0(p5);
DigitalOut OUT1(p6);
//DigitalIn IN1(p6);
DigitalOut OUT2(p7);
//DigitalIn IN2(p7);
DigitalOut OUT3(p8);
//DigitalIn IN3(p8);
DigitalOut OUT4(p11);
//DigitalIn IN4(p11);
DigitalOut OUT5(p12);
//DigitalIn IN5(p12);
DigitalOut OUT6(p15);
//DigitalIn IN6(p15);
DigitalOut OUT7(p16);
//DigitalIn IN7(p16);
//DigitalOut OUT8(p17);
//DigitalIn IN8(p17);
InterruptIn IN8(p17);
DigitalOut OUT9(p18);
//DigitalIn IN9(p18);
DigitalOut OUT10(p19);
//DigitalIn IN10(p19);
DigitalOut OUT11(p20);
//DigitalIn IN11(p20);

/*----------------------
 --あまり機能選択しないピン--
 ----------------------*/
PwmOut	 	PWM0(p21);//PWMはリセットの周期が全ピン同じなので注意
PwmOut 		PWM1(p22);
PwmOut 		PWM2(p23);
PwmOut 		PWM3(p24);
PwmOut 		PWM4(p25);
PwmOut 		PWM5(p26);
DigitalOut 	MOTOR_POWER(p29);//1にするとモータドライバが起動
DigitalIn   SW0(p30);

CAN			can0(p9,p10);

/*----------------------
 ----CAN通信外付けピン----
 ----------------------*/
//PwmOut_CAN PWM1_1(0x500,&can0);
//PwmOut_CAN PWM1_2(0x501,&can0);
//PwmOut_CAN PWM1_3(0x502,&can0);
//PwmOut_CAN PWM2_1(0x504,&can0);
//PwmOut_CAN PWM2_2(0x505,&can0);
//PwmOut_CAN PWM2_3(0x506,&can0);
//PwmOut_CAN PWM2_4(0x507,&can0);
//PwmOut_CAN PWM3_1(0x508,&can0);
//PwmOut_CAN PWM3_2(0x509,&can0);
//PwmOut_CAN PWM3_3(0x50a,&can0);
//PwmOut_CAN PWM3_4(0x50b,&can0);
//PwmOut_CAN PWM4_1(0x50c,&can0);
//PwmOut_CAN PWM4_2(0x50d,&can0);
//PwmOut_CAN PWM4_3(0x50e,&can0);
//PwmOut_CAN PWM4_4(0x50f,&can0);
//
//QEI_CAN ENC0(0x100);
//QEI_CAN ENC1(0x101);
//QEI_CAN ENC2(0x102);
//QEI_CAN ENC3(0x103);
//
//DeadReckoning_CAN dr(&can0);
//
//LS_CAN LS1(0x200);
//LS_CAN LS2(0x201);


/*----------------------
 ----mbed本体上のピン-----
 ----------------------*/
Serial pc(USBTX, USBRX);
DigitalOut led0(LED1);
DigitalOut led1(LED2);
DigitalOut led2(LED3);
DigitalOut led3(LED4);
Timer AdCycle;

Timer global_timer;

/*
 * 関数名	InitBoard
 *
 * 用途		色々な機能を初期化する
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		mainで一回だけ実行する
 * 			can通信を使うならこの関数を実行した後じゃないと失敗するので注意
 */
void InitBoard(){
	MOTOR_POWER = MOTOR_POWER_ON;
	SW0.mode(PullDown); //基板上のスイッチとつながっているピンをプルダウンする

//	i2c0.frequency(400000/*1000000*/);	//青mbedではI2Cの最高速度は400Kbpsまで、黄なら1Mbpsまで 1114の初期設定100kHz

	can0.frequency(1000000); //can通信動作周期の設定1MHz　can通信を行うならこの後からじゃないと動かない
	can0.attach(&can_irq); //can受信割り込み関数の設定
	canrcv.start();

	global_timer.start();
}

/*
 * 関数名	AdjustCycle
 *
 * 用途		マイコンの動作周期を調整する
 *
 * 引数		int t_us:目的の動作周期(us)
 *
 * 戻り値		なし
 *
 * 備考		関数実行時、前の実行時からt_us経っていない場合、t_us経つまで待つ
 * 			すでにt_us経っている場合、led3を点灯する
 */
void AdjustCycle(int t_us){
    if(AdCycle.read_us() == 0) AdCycle.start();

    if(AdCycle.read_us()>t_us){
    	led3=1;
//    	pc.printf("AdCycle=%dus\r\n",AdCycle.read_us());
    }
    else {
    	led3=0;
//    	pc.printf("AdCycle=%dus\r\n",AdCycle.read_us());
    }
    while(AdCycle.read_us()<=t_us);
    AdCycle.reset();
}

/*
 * 関数名	Limit
 *
 * 用途		int型の値を範囲内に収める
 *
 * 引数		int value:範囲内に収めたい値
 * 			int max:最大値
 * 			int min:最小値
 *
 * 戻り値		int:範囲内に収めた値
 *
 * 備考		なし
 */
int Limit(int value,int max,int min) {
    if (value>max)return max;
    if (value<min)return min;
    else          return value;
}

/*
 * 関数名	Limit_d
 *
 * 用途		double型の値を範囲内に収める
 *
 * 引数		double value:範囲内に収めたい値
 * 			double max:最大値
 * 			double min:最小値
 *
 * 戻り値		double:範囲内に収めた値
 *
 * 備考		pwm0.write(Limit_d(duty,0.9,0.1))のように使う
 */
double Limit_d(double value,double max,double min) {
    if (value>max)return max;
    if (value<min)return min;
    else          return value;
}
