#include<Servo.h>

const int COVER_PIN=2;
const int COIN_PIN=3;
const int UNLOCK_PIN=9;
const int DISPENSE_PIN=10;

const unsigned long COIN_INTERVAL=10000UL;//本番は3600000UL
const unsigned long OPEN_WAIT_TIME=5000UL;//爪を弾いてから（解除してから）カバーが開くのを待つ時間
const unsigned long PUSH_TIME=500UL;//払い出し用サーボが押し出している時間
const unsigned long DEBOUNCE=200UL;

const int DISPENSE_HOME=0;
const int DISPENSE_PUSH=90;
const int UNLOCK_HOME=0;
const int UNLOCK_PULL=90;

//状態は3つ:OPEN,LOCKED,UNLOCKING
enum State {OPEN,LOCKED,UNLOCKING};
State state=OPEN;
Servo dispenseServo;
Servo unlockServo;

unsigned long startTime=0;//タイマーを始めた時刻
unsigned long unlockStartTime=0;//爪を弾き始めた（解除）時刻
unsigned long pushStartTime=0;//押し出しを始めた時間(鍵を出す)
bool pushing=false;//今押し出し中か
bool lastCoin=false;//前回コインのスイッチがおされていたか
unsigned long lastCoinTime=0;//最後にコインに反応した時刻

void setup(){
    pinMode(COVER_PIN,INPUT_PULLUP);
    pinMode(COIN_PIN,INPUT_PULLUP);
    dispenseServo.attach(DISPENSE_PIN);
    unlockServo.attach(UNLOCK_PIN);
    dispenseServo.write(DISPENSE_HOME);
    unlockServo.write(UNLOCK_HOME);
    Serial.begin(9600);
    Serial.println("setup ready");
}

void loop(){
    unsigned long now=millis();
    bool coverClosed=(digitalRead(COVER_PIN)==LOW);
    bool coinNow=(digitalRead(COIN_PIN)==LOW);

    //コインが入った瞬間(前の反応からデバウンス(スイッチのチャタリングを除去する処理)以上経過しているときだけ)
    bool coinIn=false;
    if(coinNow&&!lastCoin&&now-lastCoinTime>=DEBOUNCE){
        coinIn=true;
        lastCoinTime=now;
    }
    lastCoin=coinNow;

    //押し出してからPUSH_TIME経過で元に戻す
    if(pushing&&now-pushStartTime>=PUSH_TIME){
        dispenseServo.write(DISPENSE_HOME);
        pushing=false;
    }

    switch(state){
        case OPEN://カバーが閉められるのを待つ
            if(coinIn){
                Serial.println("coin ignored(not locked)");
            }
            if(coverClosed){
                state=LOCKED;
                startTime=now;
                Serial.println("OPEN -> LOCKED (timer start)");
            }
            break;
        
        case LOCKED: //時間を数え、コイン（鍵）を待つ
            if(now-startTime>=COIN_INTERVAL&&!pushing){
                dispenseServo.write(DISPENSE_PUSH);
                pushStartTime=now;
                pushing=true;
                startTime=now;
                Serial.println("time up -> dispense 1 coin");
            }
            if(coinIn){
                unlockServo.write(UNLOCK_PULL);
                unlockStartTime=now;
                state=UNLOCKING;
                Serial.println("coin in -> UNLOCKING");
            }
            if(!coverClosed&&state==LOCKED){//例外処理
                state=OPEN;
                Serial.println("cover opened while locked?");
            }
            break;
        
        case UNLOCKING://爪を弾いたままカバーが開くのを待つ
            if(!coverClosed){
                unlockServo.write(UNLOCK_HOME);
                state=OPEN;
                Serial.println("cover opened -> OPEN");
            }else if(now-unlockStartTime>=OPEN_WAIT_TIME){
                unlockServo.write(UNLOCK_HOME);
                state=LOCKED;
                startTime=now;
                Serial.println("not opened -> back to LOCKED");
            }
            break;
    }
}