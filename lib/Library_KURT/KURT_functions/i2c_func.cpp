#include <mbed.h>
#include "i2c_func.h"
#include <limits.h>
#include "init.h"

#define DEBUG_MODE 0

Ticker timeout;
Ticker timeout2;
Ticker  SuperTimeout;


extern I2C i2c0;
extern I2C i2c1;


void TERMINATE_i2c0(void){
//  mbed_reset();
    //main();
    i2c0.stop();//stop()使ってもこの関数自体がスリーズする。
    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
//    i2c0.start();
//    i2c0.stop();
}

void TERMINATE_i2c1(void){
    i2c1.stop();
    i2c1.start();
}

int LS4_I2C::update(){
    char data[4];
    int stat[2];
    stat[0] = stat[1] = 1;
    data[0] = MODE_LS4|LS4_READ|0;
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    SetTimeOut;
    stat[1] = m_i2c->read(m_addr,data,4);
    EndTimeOut;
    EndSuperTimeOut;

    if(stat[0]!=0 || stat[1]!=0 )return 1;
    for(int i=0;i<4;i++){
        VAL[i] = data[i];
    }
    return 0;
}

int LS4_I2C::getHighSide(){
    char data[4];
    int stat[2];
    stat[0] = stat[1] = 1;
    data[0] = MODE_LS4|LS4_HighSide|0;
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    SetTimeOut;
    stat[1] = m_i2c->read(m_addr,data,4);
    EndTimeOut;
    EndSuperTimeOut;

    if(stat[0]!=0 || stat[1]!=0 )return 1;
    for(int i=0;i<4;i++){
        HighSide[i] = data[i];
    }
    return 0;
}

int LS4_I2C::getLowSide(){
    char data[4];
    int stat[2];
    stat[0] = stat[1] = 1;
    data[0] = MODE_LS4|LS4_LowSide|0;
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    SetTimeOut;
    stat[1] = m_i2c->read(m_addr,data,4);
    EndTimeOut;
    EndSuperTimeOut;

    if(stat[0]!=0 || stat[1]!=0 )return 1;
    for(int i=0;i<4;i++){
        LowSide[i] = data[i];
    }
    return 0;
}



int QEI_I2C::getPulses(){
    char data[8];
    int stat[2] = {-1,-1};
//  int v = ValueLog[0] - ValueLog[1];          //前回の速度
////    int a = v - (ValueLog[1] - ValueLog[2]);    //前回の加速度
//    ValueLog[2] = ValueLog[1];
//  ValueLog[1] = ValueLog[0];
//  ValueLog[0] = ValueLog[1]+v;//+a;               //通信に失敗した時のために、今までの値・速度・加速度から現在値を予想

    data[0] = MODE_QEI|QEI_READ|m_pin_num;      //bit7~5=(001)でQEI指定,bit3=0で値読み取り指定,bit2~0=ピン番号指定
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
#if 1==DEBUG_MODE
    Timer timer;int time;
    timer.start();
#endif
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    //wait_us(50);
    stat[1] = m_i2c->read(m_addr,data,8);
    EndTimeOut;
    EndSuperTimeOut;
    int   tmp_i = *(int *)&data[0];
    float tmp_f = *(float *)&data[4];

    if(0==stat[0] && 0==stat[1] && tmp_f == (float)tmp_i)ValueLog[0] = tmp_i;////両方の通信に成功した時、受信した値を利用
    else pc.printf("enci2cERROR stat[0]=%d stat[1]=%d",stat[0],stat[1]);

#if 1==DEBUG_MODE
    time = timer.read_us();
    pc.printf("gp_time:%4d st1:%3d st2:%3d  ",time,stat[0],stat[1]);
    pc.printf("i:%5d f:%5f ",tmp_i,tmp_f);
#endif
    return ValueLog[0];
}

void QEI_I2C::reset(){
    char *data;char tmp = 0;
    data = &tmp;
    int stat;
    data[0] = MODE_QEI|QEI_RESET|m_pin_num;     //bit7~5=0x1でQEI指定,bit3=1で値のリセット指定,bit2~0=ピン番号指定
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    stat = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    EndSuperTimeOut;
}


void PwmOut_I2C::period_us(int us){
    int stat[2];
    char *data;char tmp = 0;
    data = &tmp;
    int tmp_us = us;
    if(0 > tmp_us)return;  //周期に負の値を書き込もうとしてる時
    m_period = (unsigned int)tmp_us;

    data[0] = MODE_PWM|PWM_MD_ON|PWM_PERIOD|m_pin_num;//bit7~5=(000)でPWM指定,bit3=0で周期の送信指定,bit2~0=ピン番号指定

#if 1==DEBUG_MODE
    Timer timer;int time;
    timer.start();
#endif
    EndTimeOut;
    SetSuperTimeOut;
    SetTimeOut;
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    EndSuperTimeOut;
#if 0==DEBUG_MODE
    if(0 != stat[0])return;
#endif
//  wait_us(10);
    data = (char *)&m_period;       //periodの値を設定
    SetSuperTimeOut;
    SetTimeOut;
    stat[1] = m_i2c->write(m_addr,data,sizeof(unsigned int));
    EndTimeOut;
    EndSuperTimeOut;
#if 1==DEBUG_MODE
    time = timer.read_us();
    pc.printf("pr_time:%4d st1:%3d st2:%3d  ",time,stat[0],stat[1]);
#endif
}


void PwmOut_I2C::period_ms(int ms){
    if(ms > INT_MAX/1000) this->period_us(INT_MAX);
    else this->period_us(ms*1000);
}

void PwmOut_I2C::period(float seconds){
    if(seconds > INT_MAX/1000000) this->period_us(INT_MAX);
    else this->period_us(seconds*1000000);
}

float PwmOut_I2C::read(){
    return m_pulsewidth;
}

void PwmOut_I2C::write(float value){
    SetSuperTimeOut;
    char *data;char tmp = 0;
    unsigned short int send_val;
    int stat[2];stat[0]=stat[1]=-1;
    data = &tmp;
    m_pulsewidth = value;
    if(m_pulsewidth>1.0)m_pulsewidth = 1.0;
    if(m_pulsewidth<0.0)m_pulsewidth = 0.0;
    send_val = (int)((m_pulsewidth*(float)USHRT_MAX)+0.5);

    data[0] = MODE_PWM|PWM_MD_ON|PWM_DUTY|m_pin_num;//bit7~5=0x0でPWM指定,bit3=1でdutyの送信指定,bit2~0=ピン番号指定
#if 1==DEBUG_MODE
    Timer timer;int time;
    timer.start();
#endif
    EndTimeOut;
    SetTimeOut;
    stat[0] = m_i2c->write(m_addr,data,1);
    EndTimeOut;
    EndSuperTimeOut;
#if 0==DEBUG_MODE
    if(0 != stat[0])return;
#endif
    data = (char *)&send_val;
    SetSuperTimeOut;
    SetTimeOut;
    stat[1] = m_i2c->write(m_addr,data,sizeof(send_val));
    EndTimeOut;
    EndSuperTimeOut;
#if 1==DEBUG_MODE
    time = timer.read_us();
    pc.printf("wr_time:%4d st1:%3d st2:%3d  ",time,stat[0],stat[1]);
#endif
}

void PwmOut_I2C::pulsewidth_us(unsigned int us){
    this->write(((float)us)/((float)m_period));
}
void PwmOut_I2C::pulsewidth_ms(unsigned int ms){
    this->write(((float)ms*1000.0)/((float)m_period));
}
void PwmOut_I2C::pulsewidth(float seconds){
    this->write(((float)seconds*1000000.0)/((float)m_period));
}


int IN_I2C::read(void){
	char data[1];
	int stat[3]={1,1};

	data[0]=MODE_IN|IN_READ|m_pin_num;
	EndTimeOut;
	SetSuperTimeOut;
	SetTimeOut;
	stat[0]=m_i2c->write(m_addr,data,1);
	EndTimeOut;
	EndSuperTimeOut;
	SetSuperTimeOut;
	SetTimeOut;
	stat[1]=m_i2c->read(m_addr,data,1);
	EndTimeOut;
	EndSuperTimeOut;
	if(stat[0]==0&&stat[1]==0&&(int)data[0]==0){
		m_value=0;
		return 0;
	}
	else if(stat[0]==0&&stat[1]==0&&(int)data[0]==1){
		m_value=1;
		return 1;
	}
	return m_value;
}

void OUT_I2C::write(int value){
	char data[1];
	int stat[2]={1,1};
	if(m_value!=value){
		if(value==0){
			m_value=0;
			data[0]=MODE_OUT|OUT_WRITE_0|m_pin_num;
			EndTimeOut;
			SetTimeOut;
			stat[0]=m_i2c->write(m_addr,data,1);
			EndTimeOut;
		}
		else{
			m_value=1;
			data[0]=MODE_OUT|OUT_WRITE_1|m_pin_num;
			EndTimeOut;
			SetTimeOut;
			stat[0]=m_i2c->write(m_addr,data,1);
			EndTimeOut;
		}
	}
//	if(m_value!=value){
//		if(value==0) m_value=0;
//		else m_value=1;
//		data[0]=MODE_OUT|OUT_WRITE|m_pin_num;
//		EndTimeOut;
//		SetTimeOut;
//		stat[0]=m_i2c->write(m_addr,data,1);
//		EndTimeOut;
//		pc.printf("ws0=%d ",stat[0]);
//		data[0]=(char)m_value;
//		SetTimeOut;
//		stat[1]=m_i2c->write(m_addr,data,1);
//		EndTimeOut;
//		pc.printf("ws1=%d ",stat[1]);
//	}
}

int OUT_I2C::read(void){
	return m_value;
}

void PAD_I2C::PsRead(){
	char data[7];
	int stat[2]={1,1};
	data[0]=MODE_PAD;
	EndTimeOut;
	SetTimeOut;
	stat[0]=m_i2c->write(m_addr,data,1);
	EndTimeOut;
	SetTimeOut;
	stat[1]=m_i2c->read(m_addr,data,7);
	EndTimeOut;
	pc.printf("stat[0]=%d stat[1]=%d ",stat[0],stat[1]);
	if(stat[0]!=0||stat[1]!=0) return;
	PREV.BUTTON.WORD = BUTTON.WORD;
	BUTTON.WORD = ((data[0]<<8)|data[1]) ^ 0xFFFF;

	PREV.right_x = right_x;
	PREV.right_y = right_y;
	PREV.left_x = left_x;
	PREV.left_y = left_y;
    if(data[6] >= 6){   //Convert range to -128~127
        ANALOG_MODE = 1;
        right_x = data[2] - 128;
        right_y = -(data[3] - 128 +1);
        left_x  = data[4] - 128;
        left_y  = -(data[5] - 128 +1);
    }else{
        ANALOG_MODE = 0;
        right_x = 0;
        right_y = 0;
        left_x  = 0;
        left_y  = 0;
    }
}

//int PAD_I2C::rcvPad(){
//    SetSuperTimeOut;
//    int stat;
//    struct PAD_STAT *tmpPAD;
//    char data[7];
//    EndTimeOut;
//    SetTimeOut;
//    stat = m_i2c->read(m_addr,data,7);
//    EndTimeOut;
//    EndSuperTimeOut;
//    if(0 != stat){
//        MODE = BUTTON.WORD = 0;
//        ANALOG.R3H = ANALOG.R3V = ANALOG.L3H = ANALOG.L3V = 127;
//        return stat;
//    }
//    tmpPAD = (struct PAD_STAT *)data;
//    this->ANALOG = tmpPAD->ANALOG;
//    this->BUTTON = tmpPAD->BUTTON;
//    this->MODE = tmpPAD->MODE;
//    return stat;
//}

