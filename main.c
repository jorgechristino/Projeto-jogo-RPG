//Autor: Jorge Christino dos Santos Ferreira
#include <pic18f4520.h>
#include "config.h"
#include "atraso.h"
#include "bits.h"
#include "lcd.h"
#include "keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "ssd.h"
#include "serial.h"
#include "classe.h"

#define L_CLR 0x01
#define L_L1 0x80
#define L_L2 0xC0

//player 1
int classe1, vida1, ataque1, defesa1;

//player 2 
int classe2, vida2, ataque2, defesa2;

int classe;
int diferenca;
long int segundos = 0;
unsigned char tecla;
unsigned char teclaAntiga = 0;

//vez do jogador x
unsigned char x = 0;

void main(void) {
    TRISA = 0xC3;
    TRISB = 0x03;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;
    ADCON1 = 0x06;
    
    TRISCbits.TRISC7 = 1; 
    TRISCbits.TRISC6 = 0; 
    PORTB = 0;
    
    lcdInit();
    serial_init();
    kpInit();
    ssdInit();
    
    //escolha de classse dos players 
    lcdCommand(L_CLR);
    lcdCommand(L_L1);
    lcdString("Escolha as");
    lcdCommand(L_L2);
    lcdString("classes [p1p2]");
    serial_tx_str("\nClasses: [0]Mago, [1]Guerreiro");
    serial_tx_str("\nEscolha as classes dos jogadores:");
    classe = EscolhaDeClasse();
    atraso_ms(2000);
    classe1 = (classe/10) % 10;
    classe2 = classe % 10;
    
    //atributos de cada classe
    if(classe1 == 0){
        vida1 = 160;
        ataque1 = 65;
        defesa1 = 15;
    }else if(classe1 == 1){
        vida1 = 220;
        ataque1 = 45;
        defesa1 = 25;
    }
    
    if(classe2 == 0){
        vida2 = 160;
        ataque2 = 65;
        defesa2 = 15;
    }else if(classe2 == 1){
        vida2 = 220;
        ataque2 = 45;
        defesa2 = 25;
    }
    
    lcdCommand(L_CLR);
    
    while((vida1 > 0) && (vida2 > 0)){
        lcdCommand(L_L1);
        lcdString("P1:");
        lcdString("V");
        lcdNumber3(vida1);
        lcdString(" A");
        lcdNumber2(ataque1);
        lcdString(" D");
        lcdNumber2(defesa1);

        lcdCommand(L_L2);
        lcdString("P2:");
        lcdString("V");
        lcdNumber3(vida2);
        lcdString(" A");
        lcdNumber2(ataque2);
        lcdString(" D");
        lcdNumber2(defesa2);
        
        //turno jogador x
        serial_tx_str("\n\nTurno do Player ");
        serial_tx(x+49);
        for(segundos = 0; segundos < 4270; segundos++){
            kpDebounce();
            tecla = kpReadKey();
            if (tecla != teclaAntiga){
                teclaAntiga = tecla;
                //cura vida
                if (tecla == 'L') {
                    if(x == 0){
                        vida1 += 10;
                        serial_tx_str("\nPlayer 1 curou 10 de vida");
                    }
                    if(x == 1){
                        vida2 += 10;
                        serial_tx_str("\nPlayer 2 curou 10 de vida");
                    }
                }
                //causar dano ao inimigo
                if (tecla == 'R') {
                    //o dano causado será a diferença entre o ataque e a defesa do inimigo
                    if(x == 0){ 
                        diferenca = ataque1 - defesa2;
                        vida2 -= diferenca; 
                        serial_tx_str("\nPlayer 1 causou ");
                        serial_tx((diferenca/10) % 10 + 48);
                        serial_tx(diferenca% 10 + 48);
                        serial_tx_str(" de dano");
                    }
                    if(x == 1){
                        diferenca = ataque2 - defesa1;
                        vida1 -= diferenca;
                        serial_tx_str("\nPlayer 2 causou ");
                        serial_tx((diferenca/10) % 10 + 48);
                        serial_tx(diferenca% 10 + 48);
                        serial_tx_str(" de dano");
                    } 
                }
                //aumentar ataque
                if (tecla == 'U') {
                    if(x == 0){
                        ataque1 += 5; 
                        serial_tx_str("\nPlayer 1 aumentou seu ataque");
                    }
                    if(x == 1){
                        ataque2 += 5;
                        serial_tx_str("\nPlayer 2 aumentou seu ataque");
                    } 
                }
                //aumentar defesa
                if (tecla == 'D') {
                    if(x == 0){
                        defesa1 += 5; 
                        serial_tx_str("\nPlayer 1 aumentou sua defesa");
                    }
                    if(x == 1){
                        defesa2 += 5; 
                        serial_tx_str("\nPlayer 2 aumentou sua defesa");
                    } 
                }
                if(tecla != 0){
                    break;  
                }
            }
            //mostra o jogador da vez
            if(x == 0){
                ssdDigit(1, 0);
            }else{
                ssdDigit(2, 0);
            }
            ssdDigit(16, 1);
            //mostra o tempo do turno
            ssdDigit(((segundos/700) % 10), 2);
            ssdDigit(((segundos/70) % 10), 3);
            ssdUpdate(); 
            segundos++;   
            
            for (float tempo = 0; tempo < 65; tempo++);            
        }
        
        //alteração do jogador
        if(x == 0){
            x = 1;
        }else{          
            x = 0;
        }
        tecla = 0;
        atraso_ms(2000);       
    }
    
    //declaração do vencedor
    lcdCommand(L_CLR);
    if(vida1 <= 0){
        lcdCommand(L_L1);
        lcdString("Player 2 venceu!");  
    }else{
        lcdCommand(L_L1);
        lcdString("Player 1 venceu!"); 
    }
    while(1);
}
