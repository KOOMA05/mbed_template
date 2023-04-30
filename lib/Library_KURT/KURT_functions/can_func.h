/*
 * can_func.h
 *
 *  Created on: 2014/08/09
 *      Author: junya_s
 */

#ifndef CAN_FUNC_H_
#define CAN_FUNC_H_

#include "mbed.h"
#include "init.h"
#include <math.h>

extern Timer canrcv; //can受信の頻度などを測るタイマ
extern void can_irq(); //can受信割り込み関数

union IntAndFloat; //float型変数を論理演算したい時に使う共用体

/*
 * クラス名	PwmOut_CAN
 *
 * 用途		外部マイコンのPWMをCAN通信で動かすためのクラス
 *
 * 引数		int id:can通信のID、周期を変更するためのIDを入力する
 * 			CAN　*can:CANクラスのポインタ
 *
 * 備考		PwmOut_CAN PWM1_1(0x500,&can0)のように定義する
 */
class PwmOut_CAN{
	int m_id_period; //周期を変更するためのID
	int m_id_duty; //dutyを変更するためのID
	CAN* m_can;
	unsigned int m_period;
	float m_duty;
	CANMessage msg;
public:
	PwmOut_CAN(int id,CAN *can){
		m_id_period = id;
		m_id_duty = m_id_period | 0x40; //dutyを変更するためのIDは周期を変更するためのIDの３桁目を1にしたもの
		m_can = can;
		m_period = 50;
		m_duty = 0.5;
		msg.len = 2;
		msg.type = CANData;
		msg.format = CANStandard;
	}
	void write(float value);
	float read(void);
	void period(float seconds);
	void period_ms(int ms);
	void period_us(int us);
	void pulsewidth(float seconds);
	void pulsewidth_ms(unsigned int ms);
	void pulsewidth_us(unsigned int us);

	/** An operator shorthand for write()*/
	PwmOut_CAN& operator= (float value){
		write(value);
		return *this;
	}
	PwmOut_CAN& operator= (PwmOut_CAN& rhs){
		write(rhs.read());
		return *this;
	}
	/** An operator shorthand for read()*/
	operator float(){
		return read();
	}
};

#define QEI_max 12 //つなげるエンコーダーの最大数　足りなかったら増やしていい

/*
 * クラス名	QEI_CAN
 *
 * 用途		外部マイコンからcan通信で送られてくるエンコーダの値を読む
 *
 * 引数		int id:can通信のID
 *
 * 備考		QEI_CAN　C_ENC0(0x100)のように定義する
 */
class QEI_CAN{
	int m_num;
	int Value;
	int pValue;
	int value_ofs; //resetした際の値を保存しておく
public:
	static int QEI_IDnum; //定義されたIDの数を保管する
	static unsigned int QEI_IDs[QEI_max]; //引数として受け取ったIDを保管する
	static char QEI_DATAs[QEI_max][4]; //受け取った値
	static char QEI_pDATAs[QEI_max][4]; //前の値
	static int QEI_TIMEs[QEI_max]; //データを受け取った時間
	static int QEI_CYCLEs[QEI_max]; //データ受信の間隔
	QEI_CAN(int id){
		value_ofs=0;
		if(QEI_IDnum>=QEI_max) {
			//error QEI_maxを大きくした方がいい
		}else{
			Value=0;
			m_num=QEI_IDnum;
			QEI_IDs[m_num]=id;
			QEI_IDnum++;
		}
	}
	int getPulses();
	int getSpeed(); //1秒当たりのエンコーダ回転数
	void reset();
};


#define LS_max 5 //つなげるラインセンサの最大数　足りなかったら増やしていい

/*
 * クラス名	LS_CAN
 *
 * 用途		ラインセンサ用基板から各フォトリフレクタの値を受け取る
 *
 * 引数		int id:can通信のID
 *
 * 備考		LS_CAN LS1(0x200)のように定義する
 * 			whileループの最初でupdate()を実行すると良い
 * 			値が４種類あるが、基本的にValue[0]~[7]を使う
 */
class LS_CAN{
	int m_num;
public:
	unsigned char Value[8]; //明るさの割合 0=0% 255=100%
	unsigned char HValue[8]; //LED点灯時の値（デバッグモード時のみ）
	unsigned char LValue[8]; //LED消灯時の値（デバッグモード時のみ）
	unsigned char SValue[8]; //外乱除去した値（デバッグモード時のみ）
	static int LS_IDnum; //定義されたIDの数を保管する
	static unsigned int LS_IDs[LS_max]; //IDを保管する
	static char LS_DATAs[LS_max][8]; //受け取ったValueの値を保存する
	static char LS_DATAsH[LS_max][8]; //受け取ったHValueの値を保存する
	static char LS_DATAsL[LS_max][8]; //受け取ったLValueの値を保存する
	static char LS_DATAsS[LS_max][8]; //受け取ったSValueの値を保存する
	LS_CAN(int id){
		if(LS_IDnum>=LS_max){
			//error　LS_maxを大きくした方がいい
		}
		else{
			m_num=LS_IDnum;
			LS_IDs[m_num]=id;
			LS_IDnum++;
		}
	}
	void update();
};

#define DR_ID 0x0f0 //DeadReckoningのcanID
#define DR_ID2 0x0f1

#define DR_ALPHA_XY 0.3 //ローパスフィルタ係数
#define DR_ALPHA_W 0.3

#define GYRO_CYCLE 10000.0 //ジャイロ動作周波数(us)

/*
 * クラス名	DeadReckoning_CAN
 *
 * 用途		自己位置推定用基板から座標と速度を受け取る
 *
 * 引数		CAN　*can:CANクラスのポインタ
 * 			(short xofs:X座標のオフセット)
 * 			(short yofs:Y座標のオフセット)
 *
 * 備考		DeadReckoning_CAN DR(&can0)のように定義
 * 			whileループの最初で毎回updateを実行すると良い
 */
class DeadReckoning_CAN{
	CANMessage msg;
	CAN* m_can;
	short pSpeed;
	short W; //角度（度数×100）
	short pW;
	int dW;
	short X_ofs;
	short Y_ofs;
	int p_DR_TIME;
	float p_WSpeed;
	int WSpeed_flag;
public:
	short X; //X座標
	short Y; //Y座標
	float deg; //角度（度数法） -180.00~179.99
	float rad; //角度（弧度法） -PI~PI

	float XSpeed; //X方向速度（mm/s）
	float YSpeed; //Y方向速度（mm/s）
	float WSpeed; //W方向速度(deg/s)

	float Speed; //速度（mm/s）
	float Acceleration; //加速度（mm/s^2）
	float Direction; //速度方向(-180.00~179.99)

	static char DR_DATA[6];
	static char DR_DATA2[8];
	static int DR_TIME;
	static int DR_CYCLE;
	DeadReckoning_CAN(CAN *can){
		X_ofs=0;
		Y_ofs=0;
		X=0;
		Y=0;
		deg=0;
		rad=0;
		m_can=can;
		msg.len = 1;
		msg.type = CANData;
		msg.format = CANStandard;
		msg.id=0x0f8;
		msg.data[0]=0xaa;
		WSpeed_flag=0;
	}
	DeadReckoning_CAN(CAN *can,short xofs,short yofs){
		X_ofs=xofs;
		Y_ofs=yofs;
		X=0;
		Y=0;
		deg=0;
		rad=0;
		m_can=can;
		msg.len = 1;
		msg.type = CANData;
		msg.format = CANStandard;
		msg.id=0x0f8;
		msg.data[0]=0xaa;
		WSpeed_flag=0;
	}
	void update();
	void reset();
	void reset(short xofs,short yofs);
};

#define PS3_ID 0x0d0 //PS3_CANのID

/*
 * クラス名	PS3_CAN
 *
 * 用途		外部マイコンからPS3コントローラーの状態を受け取る
 *
 * 引数		CAN　*can:CANクラスのポインタ
 *
 * 備考		PS3_CAN ps3(&can0)のように定義する
 * 			whileループの最初でupdate()を実行すると良い
 */

class PS3_CAN{
	CANMessage msg;
	CAN* m_can;
public:
	static char PS3_DATA[6];
	union {
		uint32_t WORD;
		struct {
			unsigned char L2       :1;
			unsigned char R2       :1;
			unsigned char L1       :1;
			unsigned char R1       :1;

			unsigned char SANKAKU  :1;
			unsigned char MARU     :1;
			unsigned char BATU     :1;
			unsigned char SIKAKU   :1;

			unsigned char SELECT   :1;
			unsigned char L3       :1;
			unsigned char R3       :1;
			unsigned char START    :1;

			unsigned char UP       :1;
			unsigned char RIGHT    :1;
			unsigned char DOWN     :1;
			unsigned char LEFT     :1;
		} BIT;
	} BUTTON;
	int8_t right_x, right_y, left_x, left_y;
	unsigned char CONNECT;//繋がっているとき1、繋がってないとき0
	struct {
		union {
			uint32_t WORD;
			struct {
				unsigned char L2       :1;
				unsigned char R2       :1;
				unsigned char L1       :1;
				unsigned char R1       :1;

				unsigned char SANKAKU  :1;
				unsigned char MARU     :1;
				unsigned char BATU     :1;
				unsigned char SIKAKU   :1;

				unsigned char SELECT   :1;
				unsigned char L3       :1;
				unsigned char R3       :1;
				unsigned char START    :1;

				unsigned char UP       :1;
				unsigned char RIGHT    :1;
				unsigned char DOWN     :1;
				unsigned char LEFT     :1;
			} BIT;
		} BUTTON;
		int8_t right_x, right_y, left_x, left_y;
	} PREV;
	PS3_CAN(CAN *can){
		m_can=can;
		msg.id=PS3_ID;
		msg.len=6;
		msg.type=CANData;
		msg.format=CANStandard;
	}
	void update();
};


#endif /* CAN_FUNC_H_ */
