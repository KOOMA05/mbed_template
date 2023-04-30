#ifndef I2C_FUNC_H_
#define I2C_FUNC_H_

/*----------------------------
 ---I2C通信の1byte目で使われる値---
 -----------------------------*/
#define MODE_PWM    (0<<5)
#define PWM_PERIOD  (0<<3)
#define PWM_DUTY    (1<<3)
#define PWM_MD_OFF  (0<<4)//未実装
#define PWM_MD_ON   (1<<4)//未実装

#define MODE_QEI    (1<<5)
#define QEI_READ    (0<<3)
#define QEI_RESET   (1<<3)

#define MODE_LS4   (2<<5)
#define LS4_READ   (0<<3)
#define LS4_HighSide    (1<<3)
#define LS4_LowSide     (2<<3)

#define MODE_PAD   (3<<5)

#define MODE_IN   (4<<5)
#define IN_READ   (0<<3)
#define IN_MODE   (1<<3)

#define MODE_OUT  (5<<5)
#define OUT_WRITE_0 (0<<3)
#define OUT_WRITE_1 (1<<3)
#define OUT_READ  (2<<3)


/*-----------------------------
 ---通信エラー時のためのタイムアウト---
 ------------------------------*/
extern void TERMINATE_i2c0(void);
#define SetTimeOut  timeout.attach_us(&TERMINATE_i2c0,3500)
#define EndTimeOut  timeout.detach()

extern void TERMINATE_i2c1(void);
#define SetTimeOut2 timeout2.attach_us(&TERMINATE_i2c1,3500)
#define EndTimeOut2 timeout2.detach();

extern "C" void mbed_reset();
#define SetSuperTimeOut SuperTimeout.attach(&mbed_reset,1.0)
#define EndSuperTimeOut SuperTimeout.detach()

class LS4_I2C{
    char m_addr;I2C* m_i2c;
public:
    //float VAL[4];
    unsigned char VAL[4];
    unsigned char HighSide[4];
    unsigned char LowSide[4];
    LS4_I2C(char addr,I2C *i2c){
        m_addr = addr;
        m_i2c = i2c;
        VAL[0] = VAL[1] = VAL[2] = VAL[3] = 0;
        HighSide[0] = HighSide[1] = HighSide[2] = HighSide[3] = 0;
        LowSide[0] = LowSide[1] = LowSide[2] = LowSide[3] = 0;
    }
    int update();
    int getHighSide();
    int getLowSide();
};


class QEI_I2C{
    char m_addr,m_pin_num;I2C* m_i2c;int ValueLog[3];
public:
    QEI_I2C(char addr,char pin_num,I2C *i2c){
        m_addr = addr;
        m_pin_num = pin_num;
        m_i2c = i2c;
        ValueLog[0] = ValueLog[1] = ValueLog[2] = 0;
    }
    int getPulses();
    void reset();
};

class PwmOut_I2C{
    char m_addr,m_pin_num;I2C* m_i2c;
    unsigned int m_period;
    float m_pulsewidth;
public:
    PwmOut_I2C(char addr,char pin_num,I2C *i2c){
        m_addr = addr;
        m_pin_num = pin_num;
        m_i2c = i2c;
        m_period = 50;
        m_pulsewidth = 0.5;
    }
    void write(float value);
    float read(void);
    void period(float seconds);
    void period_ms(int ms);
    void period_us(int us);
    void pulsewidth(float seconds);
    void pulsewidth_ms(unsigned int ms);
    void pulsewidth_us(unsigned int us);

    /** A operator shorthand for write()*/
    PwmOut_I2C& operator= (float value) {
        write(value);
        return *this;
    }
    PwmOut_I2C& operator= (PwmOut_I2C& rhs) {
        write(rhs.read());
        return *this;
    }
    /** An operator shorthand for read()*/
    operator float() {
        return read();
    }
};

class IN_I2C{
	char m_addr,m_pin_num;I2C* m_i2c;
	int i;
	int m_value;
public:
	IN_I2C(char addr,char pin_num,I2C *i2c){
		m_addr=addr;
		m_pin_num=pin_num;
		m_i2c=i2c;
		m_value=0;
	}
	int read(void);
	operator int() {
		return read();
	}
};

class OUT_I2C{
	char m_addr,m_pin_num;I2C* m_i2c;
	int m_value;
public:
	OUT_I2C(char addr,char pin_num,I2C *i2c){
		m_addr=addr;
		m_pin_num=pin_num;
		m_i2c=i2c;
		m_value=0;
	}
	void write(int value);
	int read(void);
	/** A shorthand for write()
	 */
    OUT_I2C& operator= (int value) {
		write(value);
		return *this;
	}

	OUT_I2C& operator= (OUT_I2C& rhs) {
		write(rhs.read());
		return *this;
	}
	/** A shorthand for read()
	 */
	operator int() {
		return read();
	}
};

//-------------------------------------------------------------------------
// Class
//-------------------------------------------------------------------------
//class Pspad
//{
//public:
//    Pspad();
//    void PsRead(void);
//
//    union {
//        uint32_t WORD;
//        struct {
//            unsigned char L2       :1;
//            unsigned char R2       :1;
//            unsigned char L1       :1;
//            unsigned char R1       :1;
//
//            unsigned char SANKAKU  :1;
//            unsigned char MARU     :1;
//            unsigned char BATU     :1;
//            unsigned char SIKAKU   :1;
//
//            unsigned char SELECT   :1;
//            unsigned char L3       :1;
//            unsigned char R3       :1;
//            unsigned char START    :1;
//
//            unsigned char UP       :1;
//            unsigned char RIGHT    :1;
//            unsigned char DOWN     :1;
//            unsigned char LEFT     :1;
//        } BIT;
//    } BUTTON;
//    int8_t right_x, right_y, left_x, left_y;
//    unsigned char VIBRATE;
//    unsigned char ANALOG_MODE;
//    struct {
//        union {
//            uint32_t WORD;
//            struct {
//                unsigned char L2       :1;
//                unsigned char R2       :1;
//                unsigned char L1       :1;
//                unsigned char R1       :1;
//
//                unsigned char SANKAKU  :1;
//                unsigned char MARU     :1;
//                unsigned char BATU     :1;
//                unsigned char SIKAKU   :1;
//
//                unsigned char SELECT   :1;
//                unsigned char L3       :1;
//                unsigned char R3       :1;
//                unsigned char START    :1;
//
//                unsigned char UP       :1;
//                unsigned char RIGHT    :1;
//                unsigned char DOWN     :1;
//                unsigned char LEFT     :1;
//            } BIT;
//        } BUTTON;
//        int8_t right_x, right_y, left_x, left_y;
//    } PREV;
//
//private:
//    uint8_t PsComm(uint8_t send_data);
//    Timer tm;
//    DigitalIn  datpin;
//    DigitalOut cmdpin;
//    DigitalOut selpin;
//    DigitalOut clkpin;
//};

class PAD_I2C{
	char m_addr;
	I2C* m_i2c;
public:
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
	    unsigned char VIBRATE;
	    unsigned char ANALOG_MODE;
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
	PAD_I2C(char addr,I2C *i2c){
		m_addr=addr;
		m_i2c=i2c;
	}
	void PsRead();
};
////以下PSPAD用のプログラム
//struct PAD_ANALOG {
//  unsigned char R3H;
//  unsigned char L3H;
//  unsigned char R3V;
//  unsigned char L3V;
//};//4byte
//
//union PAD_BUTTON {
//  unsigned short WORD;
//  struct {
//    unsigned char H;
//    unsigned char L;
//  } BYTE;
//  struct {
//      unsigned char SELECT   :1;//元１バイト目最下位ビット
//      unsigned char L3       :1;
//      unsigned char R3       :1;
//      unsigned char START    :1;
//
//      unsigned char UP       :1;
//      unsigned char RIGHT    :1;
//      unsigned char DOWN     :1;
//      unsigned char LEFT     :1;//元１バイト目最上ビット
//
//      unsigned char L2       :1;//元2バイト目最下位ビット
//      unsigned char R2       :1;
//      unsigned char L1       :1;
//      unsigned char R1       :1;
//
//      unsigned char TRIANGLE :1;
//      unsigned char ROUND    :1;
//      unsigned char CROSS    :1;//読み込むときCROSSとR2がひっくり返ってる？
//      unsigned char SQUARE   :1;//元２バイト目最上位ビット
//  } BIT;//20byte
//}  ;
//
//struct PAD_STAT {
//  union PAD_BUTTON BUTTON;
//  struct PAD_ANALOG ANALOG;
//  unsigned char MODE;
//};
//
//class PAD_I2C{
//    char m_addr;I2C* m_i2c;
//public:
//    union PAD_BUTTON BUTTON;
//    struct PAD_ANALOG ANALOG;
//    unsigned char MODE;
//    PAD_I2C(char addr,I2C *i2c){
//        m_addr = addr;
//        m_i2c = i2c;
//        MODE = BUTTON.WORD = 0;
//        ANALOG.R3H = ANALOG.R3V = ANALOG.L3H = ANALOG.L3V = 127;
//    }
//    int rcvPad();
//};

//mbedでI2Cを経由してPSPADを読み込むライブラリ

/*-------サンプルプログラム--------
//main.cpp
 #include "mbed.h"
 #include "i2c_func.h"
 #define PAD_ADDRESS 0x02
 Serial pc(USBTX, USBRX);
 I2C i2c2(p9, p10);

 int main(void){
     i2c2.frequency(1000000);       // Fast Mode +
     initPad();
     while(1){
         I2CReadPad(PAD_ADDRESS,i2c2);
         pc.printf("cross = %d \n\r",PAD.BUTTON.BIT.CROSS);
     }

     return 0;
 }

----------------------*/


#endif /* I2C_FUNC_H_ */

