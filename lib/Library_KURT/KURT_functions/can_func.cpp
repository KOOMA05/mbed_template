/*
 * can_func.cpp
 *
 *  Created on: 2014/08/09
 *      Author: junya_s
 */

#include "can_func.h"
#include "init.h"

int QEI_CAN::QEI_IDnum;
unsigned int QEI_CAN::QEI_IDs[QEI_max];
char QEI_CAN::QEI_DATAs[QEI_max][4];
char QEI_CAN::QEI_pDATAs[QEI_max][4];
int QEI_CAN::QEI_TIMEs[QEI_max];
int QEI_CAN::QEI_CYCLEs[QEI_max];

int LS_CAN::LS_IDnum;
unsigned int LS_CAN::LS_IDs[LS_max];
char LS_CAN::LS_DATAs[LS_max][8];
char LS_CAN::LS_DATAsH[LS_max][8];
char LS_CAN::LS_DATAsL[LS_max][8];
char LS_CAN::LS_DATAsS[LS_max][8];

char DeadReckoning_CAN::DR_DATA[6];
char DeadReckoning_CAN::DR_DATA2[8];
int DeadReckoning_CAN::DR_TIME;
int DeadReckoning_CAN::DR_CYCLE;

char PS3_CAN::PS3_DATA[6];

Timer canrcv;
int rcvtime;

union IntAndFloat{
	int ival;
	float fval;
};

//can受信用割り込み関数
void can_irq(){
	CANMessage rxmsg;
	if(can0.read(rxmsg,0)){
//		pc.printf("time:%dus ",canrcv.read_us()-rcvtime);
		rcvtime=canrcv.read_us();
		switch(rxmsg.id>>6&0x1f){ //受信IDの上位５ケタでswitch
		case 0x03: //FREE (000-11XX-XXXX)
			if(DR_ID==rxmsg.id){ //DeadReckoning
				for(int j=0;j<6;j++){
					DeadReckoning_CAN::DR_DATA[j]=rxmsg.data[j];
				}
				DeadReckoning_CAN::DR_CYCLE=rcvtime-DeadReckoning_CAN::DR_TIME;
				DeadReckoning_CAN::DR_TIME=rcvtime;
			}else if(DR_ID2==rxmsg.id){
				for(int j=0;j<8;j++){
					DeadReckoning_CAN::DR_DATA2[j]=rxmsg.data[j];
				}
			}else if(PS3_ID==rxmsg.id){ //PS3_CAN
				for(int j=0;j<6;j++){
					PS3_CAN::PS3_DATA[j]=rxmsg.data[j];
				}
			}
			break;
		case 0x04: //QEI_CAN (001-00XX-XXXX)
			for(int i=0;i<QEI_CAN::QEI_IDnum;i++){
				if(QEI_CAN::QEI_IDs[i]==rxmsg.id){
					for(int j=0;j<4;j++){
						QEI_CAN::QEI_pDATAs[i][j]=QEI_CAN::QEI_DATAs[i][j];
						QEI_CAN::QEI_DATAs[i][j]=rxmsg.data[j];
					}
					QEI_CAN::QEI_CYCLEs[i]=rcvtime-QEI_CAN::QEI_TIMEs[i];
					QEI_CAN::QEI_TIMEs[i]=rcvtime;
					break;
				}
			}
			break;
		case 0x08: //LS_CAN (Value)(010-00XX-XXXX)
			for(int i=0;i<LS_CAN::LS_IDnum;i++){
				if(LS_CAN::LS_IDs[i]==rxmsg.id){
					for(int j=0;j<8;j++){
						LS_CAN::LS_DATAs[i][j]=rxmsg.data[j];
					}
					break;
				}
			}
			break;
		case 0x09: //LS_CAN (HValue)(010-01XX-XXXX)
			for(int i=0;i<LS_CAN::LS_IDnum;i++){
				if((LS_CAN::LS_IDs[i]|0x40)==rxmsg.id){
					for(int j=0;j<8;j++){
						LS_CAN::LS_DATAsH[i][j]=rxmsg.data[j];
					}
					break;
				}
			}
			break;
		case 0x0a: //LS_CAN (LValue)(010-10XX-XXXX)
			for(int i=0;i<LS_CAN::LS_IDnum;i++){
				if((LS_CAN::LS_IDs[i]|0x80)==rxmsg.id){
					for(int j=0;j<8;j++){
						LS_CAN::LS_DATAsL[i][j]=rxmsg.data[j];
					}
					break;
				}
			}
			break;
		case 0x0b: //LS_CAN (SValue)(010-11XX-XXXX)
			for(int i=0;i<LS_CAN::LS_IDnum;i++){
				if((LS_CAN::LS_IDs[i]|0xc0)==rxmsg.id){
					for(int j=0;j<8;j++){
						LS_CAN::LS_DATAsS[i][j]=rxmsg.data[j];
					}
					break;
				}
			}
			break;
		}
	}
}

/*
 * 関数名	PwmOut_CAN::write
 *
 * 用途		外部マイコンのPWMのdutyを書き換える
 *
 * 引数		float value:PWMのduty
 *
 * 戻り値		なし
 *
 * 備考		連続で実行すると受信側の処理が追いつかず失敗する可能性がちょっとだけある
 */
void PwmOut_CAN::write(float value){
	m_duty=value;
	if(m_duty>1) m_duty=1;
	else if(m_duty<0) m_duty=0;
	int sendvalue=(int)(m_duty*65535); //dutyに65535を掛けた値を送る

	msg.data[0]=(sendvalue)&0xff;
	msg.data[1]=(sendvalue>>8)&0xff;
	msg.id=m_id_duty;
	char stat=m_can->write(msg);

	//送信に失敗した場合、何回かやり直す
	if(stat==0){
		wait_us(100);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(300);
		stat=m_can->write(msg);
	}
	if(stat==0){
		pc.printf("エラー!!　PwmOut_CAN::write失敗  ID:%x\r\n",m_id_duty);
	}

	wait_us(10); //連続送信による失敗を防止するためのwait
}


/*
 * 関数名	PwmOut_CAN::read
 *
 * 用途		現在のdutyの値を返す
 *
 * 引数		なし
 *
 * 戻り値		float:現在のdutyの値
 *
 * 備考		正確には最後に書き込んだdutyの値なので、受信側マイコンがリセットしたり、送信に失敗してたりすると間違った値となる
 */
float PwmOut_CAN::read(){
	return m_duty;
}

void PwmOut_CAN::period(float seconds){
	this->period_us(seconds*1000000);
}

void PwmOut_CAN::period_ms(int ms){
	this->period_us(ms*1000);
}

/*
 * 関数名	PwmOut_CAN::period_us
 *
 * 用途		外部マイコンのPWMの周期を書き換える
 *
 * 引数		int us:PWMの周期(us)
 *
 * 戻り値		なし
 *
 * 備考		連続で実行すると受信側の処理が追いつかず失敗する可能性がちょっとだけある
 * 			現在の仕様では65535us以上の周期にはできない
 */
void PwmOut_CAN::period_us(int us){
	m_period = (unsigned int)us;
	if(m_period>65535) m_period=65535;

	msg.data[0]=(m_period)&0xff;
	msg.data[1]=(m_period>>8)&0xff;
	msg.id=m_id_period;
	char stat=m_can->write(msg);

	//送信に失敗した場合、何回かやり直す
	if(stat==0){
		wait_us(100);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(300);
		stat=m_can->write(msg);
	}
	if(stat==0){
			pc.printf("エラー!!　PwmOut_CAN::period_us失敗  ID:%x\r\n",m_id_period);
	}

	wait_us(10); //連続送信による失敗を防止するためのwait
}

void PwmOut_CAN::pulsewidth(float seconds){
	this->write(((float)seconds*1000000.0)/((float)m_period));
}

void PwmOut_CAN::pulsewidth_ms(unsigned int ms){
	this->write(((float)ms*1000.0)/((float)m_period));
}

void PwmOut_CAN::pulsewidth_us(unsigned int us){
	this->write(((float)us)/((float)m_period));
}


/*
 * 関数名	QEI_CAN::getPulses
 *
 * 用途		外部マイコンから送られてくるエンコーダの値を返す
 *
 * 引数		なし
 *
 * 戻り値		int:現在のエンコーダの値
 *
 * 備考		なし
 */
int QEI_CAN::getPulses(){
	Value=(QEI_DATAs[m_num][0])|(QEI_DATAs[m_num][1]<<8)|(QEI_DATAs[m_num][2]<<16)|(QEI_DATAs[m_num][3]<<24);
	return Value-value_ofs;
}


/*
 * 関数名	QEI_CAN::getSpeed
 *
 * 用途		外部マイコンから送られてくるエンコーダの値と、受信周期からエンコーダの速度を計算して返す
 *
 * 引数		なし
 *
 * 戻り値		int:１秒当たりのエンコーダの回転数
 *
 * 備考		なし
 */
int QEI_CAN::getSpeed(){
	Value=(QEI_DATAs[m_num][0])|(QEI_DATAs[m_num][1]<<8)|(QEI_DATAs[m_num][2]<<16)|(QEI_DATAs[m_num][3]<<24);
	pValue=(QEI_pDATAs[m_num][0])|(QEI_pDATAs[m_num][1]<<8)|(QEI_pDATAs[m_num][2]<<16)|(QEI_pDATAs[m_num][3]<<24);
	if(QEI_CYCLEs[m_num]!=0) return (int)(((float)(Value-pValue)/(float)QEI_CYCLEs[m_num])*1000000.0);
	else return 0;
}


/*
 * 関数名	QEI_CAN::reset
 *
 * 用途		エンコーダの値をリセットする
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		現在の値を保存しておき、これから受け取る値からその値を引くことで擬似的にリセットとみなす
 */
void QEI_CAN::reset(){
	value_ofs=Value;
}


/*
 * 関数名	LS_CAN::update
 *
 * 用途		ラインセンサの値を更新する
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		whileループの先頭で実行すること
 */
void LS_CAN::update(){
	for(int k=0;k<8;k++){
		this->Value[k]=this->LS_DATAs[m_num][k];
		this->HValue[k]=this->LS_DATAsH[m_num][k];
		this->LValue[k]=this->LS_DATAsL[m_num][k];
		this->SValue[k]=this->LS_DATAsS[m_num][k];
	}
}


union IntAndFloat dX;
union IntAndFloat dY;

/*
 * 関数名	DeadReckoning_CAN::update
 *
 * 用途		自己位置推定用マイコンから受け取った値で自己位置と速度を更新する
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		なし
 */
void DeadReckoning_CAN::update(){
	this->pSpeed=this->Speed;

	//受け取ったfloat型のデータを論理演算で元に戻すために一度int型に入れる
	dX.ival=((this->DR_DATA2[0]|this->DR_DATA2[1]<<8|this->DR_DATA2[2]<<16|this->DR_DATA2[3]<<24));
	dY.ival=((this->DR_DATA2[4]|this->DR_DATA2[5]<<8|this->DR_DATA2[6]<<16|this->DR_DATA2[7]<<24));

	//float型になおし、ローパスフィルタをかけてからXSpeed,YSpeedに入れる
	this->XSpeed=DR_ALPHA_XY*dX.fval+(1-DR_ALPHA_XY)*this->XSpeed;
	this->YSpeed=DR_ALPHA_XY*dY.fval+(1-DR_ALPHA_XY)*this->YSpeed;

	this->Speed=sqrt(this->XSpeed*this->XSpeed+this->YSpeed*this->YSpeed);
	this->Acceleration=this->Speed-this->pSpeed;

	this->X=(this->DR_DATA[0]|this->DR_DATA[1]<<8)+this->X_ofs;
	this->Y=(this->DR_DATA[2]|this->DR_DATA[3]<<8)+this->Y_ofs;

	this->pW=this->W;
	this->W=this->DR_DATA[4]|this->DR_DATA[5]<<8; //角度*100の値が入る
	this->deg=(float)(this->W)/100.0; //角度を度数法に直す
	this->rad=this->deg*PI/180.0; //角度を弧度法に直す

	//角度の増加量、ジャイロの動作周期から角速度を求める
	//ジャイロの動作周期が10msのため、角度の増加量が0のときは何回か続くまで速度の更新をしない
	if(this->W!=this->pW||WSpeed_flag>=2){
		WSpeed_flag=0;

		dW=(int)this->W-(int)this->pW;

		if(dW>15000) dW-=36000;
		else if(dW<-15000) dW+=36000;

		this->WSpeed=(float)((float)dW)/100.0/GYRO_CYCLE*1000000.0;

		this->WSpeed=DR_ALPHA_W*this->WSpeed+(1-DR_ALPHA_W)*this->p_WSpeed;

		p_WSpeed=WSpeed;
	}else{
		WSpeed_flag++;
	}

	//マシンの速度方向を求める
	if(this->Speed!=0){
		this->Direction=(float)((atan2(-this->XSpeed,this->YSpeed))*180/PI-this->deg);
		if(this->Direction<-180)this->Direction+=360;
		else if(this->Direction>=180)this->Direction-=360;
	}
	else{
		this->Direction=0;
	}
}


/*
 * 関数名	DeadReckoning_CAN::reset
 *
 * 用途		自己位置のリセットをする
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		なし
 */
void DeadReckoning_CAN::reset(){
	char stat=m_can->write(msg);

	//失敗したら何回かやり直す
	if(stat==0){
		wait_us(100);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(300);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(1000);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(1500);
		stat=m_can->write(msg);
	}
	if(stat==0){
		pc.printf("エラー!!　DeadReckoning_CAN::reset失敗\r\n");
	}
}

/*
 * 関数名	DeadReckoning_CAN::reset
 *
 * 用途		オフセット付きで自己位置のリセットをする
 *
 * 引数		short xofs:X座標のオフセット
 * 			short yofs:Y座標のオフセット
 *
 * 戻り値		なし
 *
 * 備考		なし
 */
void DeadReckoning_CAN::reset(short xofs,short yofs){
	X_ofs=xofs;
	Y_ofs=yofs;
	char stat=m_can->write(msg);
	if(stat==0){
		wait_us(100);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(300);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(1000);
		stat=m_can->write(msg);
	}
	if(stat==0){
		wait_us(1500);
		stat=m_can->write(msg);
	}
	if(stat==0){
		pc.printf("エラー!!　DeadReckoning_CAN::reset失敗\r\n");
	}
}

/*
 * 関数名	PS3_CAN::update
 *
 * 用途		PS3コントローラの状態をアップデートする
 *
 * 引数		なし
 *
 * 戻り値		なし
 *
 * 備考		whileループの最初で実行すると良い
 */
void PS3_CAN::update(){
	PREV.BUTTON.WORD = BUTTON.WORD;
	BUTTON.WORD = (this->PS3_DATA[5]<<8)|(this->PS3_DATA[4]);
	PREV.right_x=right_x;
	PREV.right_y=right_y;
	PREV.left_x=left_x;
	PREV.left_y=left_y;
	//アナログスティックの値がすべて0のときは繋がってないとみなす
	if(this->PS3_DATA[0]==0&&this->PS3_DATA[1]==0&&this->PS3_DATA[2]==0&&this->PS3_DATA[3]==0){
		left_x = 0;
		left_y = 0;
		right_x = 0;
		right_y = 0;
		CONNECT = 0;
	}
	else{
		left_x = this->PS3_DATA[0]-128;
		left_y = -(this->PS3_DATA[1] - 128 +1);
		right_x = this->PS3_DATA[2]-128;
		right_y = -(this->PS3_DATA[3] - 128 +1);
		CONNECT = 1;
	}
}
