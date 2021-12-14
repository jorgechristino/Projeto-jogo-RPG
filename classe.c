#include "config.h"
#include "bits.h"
#include "serial.h"
#include "lcd.h"
#include "ssd.h"
#include "ds1307.h"
#include "keypad.h"
#include "i2c.h"

#define L_L1 0x80
#define L_L2 0xC0

//0 - mago
//1 - guerreiro
int EscolhaDeClasse(void){
    unsigned char tmp;
    int a, b;
    int classe;
    int aux = 0;
    
    while (aux < 2) {
        tmp = serial_rx(2000);
        if ((tmp != 0xA5) && (aux == 0)) {
            a = tmp;
            aux++;
        }else if(tmp != 0xA5) {
            b = tmp;
            aux++;
        }
        serial_tx(tmp);
    }
    a = a - 48;
    b = b - 48;
    classe = (a*10) + b;
    return classe;
}

//Exibe numero no lcd com 3 algarismos
void lcdNumber3(int value){
    int i = 100;
    while (i > 0) {
		lcdChar((value / i) % 10 + 48);
		i /= 10;
	}  
}

//Exibe numero no lcd com 2 algarismos
void lcdNumber2(int value){
    int i = 10;
    while (i > 0) {
		lcdChar((value / i) % 10 + 48);
		i /= 10;
	}  
}