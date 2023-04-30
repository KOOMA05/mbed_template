#include <mbed.h>
#include "init.h"
#include "init.h"
#include "i2c_func.h"
#include "can_func.h"
#include "QEI.h"

int main(){
  // put your setup code here, to run once:
	pc.baud(921600); //デバッグ用
	QEI qei(p11,p12,NC,0,QEI::X4_ENCODING);
	wait_ms(300); //全ての基板の電源が入るまで待つ
	InitBoard();

		while(true){
      // put your main code here, to run repeatedly:
			AdjustCycle(10000);
    }
}
