/*
 * init.h
 *
 *  Created on: 2014/08/09
 *      Author: junya_s
 */
#ifndef INIT_H_
#define INIT_H_

#include "mbed.h"
#include "QEI.h"
#include "i2c_func.h"
#include "pspad.h"
#include "can_func.h"

/*-----------------------------------------
 * （たぶん）確認しやすくなるので変数名の変更はできれば
 * #define DUTY_R PWM1
 * などの「#define 変数名 命名対象」でやってみてください
 * ---------------------------------------*/

#define CYCLE 5000 //動作周期(us)
#define PI 3.14159265358979323846 //円周率

/*-----------------------
 * -------定数----------
 -----------------------*/
#define MOTOR_POWER_OFF	0
#define MOTOR_POWER_ON	1

/*----------------------
 -----機能選択するピン-----
 ----------------------*/
extern QEI ENC0;
extern Serial serial0;
extern QEI ENC1;
extern I2C i2c0;

extern DigitalOut 	OUT0;
extern DigitalIn	IN0;
extern DigitalOut 	OUT1;
extern DigitalIn	IN1;
extern DigitalOut	OUT2;
extern DigitalIn	IN2;
extern DigitalOut 	OUT3;
extern DigitalIn	IN3;
extern DigitalOut 	OUT4;
extern DigitalIn	IN4;
extern DigitalOut	OUT5;
extern DigitalIn	IN5;
extern DigitalOut 	OUT6;
extern DigitalIn	IN6;
extern DigitalOut 	OUT7;
extern DigitalIn	IN7;
extern DigitalOut	OUT8;
//extern DigitalIn	IN8;
extern InterruptIn	IN8;
extern DigitalOut 	OUT9;
extern DigitalIn	IN9;
extern DigitalOut 	OUT10;
extern DigitalIn	IN10;
extern DigitalOut	OUT11;
extern DigitalIn	IN11;

/*----------------------
 --あまり機能選択しないピン--
 ----------------------*/
extern PwmOut	 	PWM0;
extern PwmOut	 	PWM1;
extern PwmOut 		PWM2;
extern PwmOut 		PWM3;
extern PwmOut 		PWM4;
extern PwmOut 		PWM5;
extern DigitalOut 	MOTOR_POWER;
extern DigitalIn    SW0;

extern CAN can0;

/*----------------------
 ----CAN通信外付けピン----
 ----------------------*/
//extern PwmOut_CAN PWM1_1;
//extern PwmOut_CAN PWM1_2;
//extern PwmOut_CAN PWM1_3;
//extern PwmOut_CAN PWM2_1;
//extern PwmOut_CAN PWM2_2;
//extern PwmOut_CAN PWM2_3;
//extern PwmOut_CAN PWM2_4;
//extern PwmOut_CAN PWM3_1;
//extern PwmOut_CAN PWM3_2;
//extern PwmOut_CAN PWM3_3;
//extern PwmOut_CAN PWM3_4;
//extern PwmOut_CAN PWM4_1;
//extern PwmOut_CAN PWM4_2;
//extern PwmOut_CAN PWM4_3;
//extern PwmOut_CAN PWM4_4;
//
//extern QEI_CAN ENC0;
//extern QEI_CAN ENC1;
//extern QEI_CAN ENC2;
//extern QEI_CAN ENC3;

//extern DeadReckoning_CAN dr;
//
//extern LS_CAN LS1;
//extern LS_CAN LS2;


/*----------------------
 ----mbed本体上のピン-----
 ----------------------*/
extern Serial pc;
extern DigitalOut led0;
extern DigitalOut led1;
extern DigitalOut led2;
extern DigitalOut led3;
extern Timer AdCycle; //AdjustCycleで使うタイマ

extern Timer global_timer; //電源投入後からの経過時間を測る用のグローバルタイマ

/*----------------------
 ---------関数---------
 ----------------------*/
extern void InitBoard(void);
extern void AdjustCycle(int t_us);
extern int Limit(int value,int max,int min);
extern double Limit_d(double value,double max,double min);

#endif /* INIT_H_ */

