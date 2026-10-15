#include<Servo.h>

const int COVER_PIN=2;//カバーのスイッチ
const int COIN_PIN=3;//コインのスイッチ
const int UNLOCK_PIN=9;//解除用サーボの信号線
const int DISPENSE_PIN=10;//払い出し用サーボの信号線

const unsigned long COIN_INTERVAL=10000UL;

const int DISPENSE_HOME=0;
const int DISPENSE_PUSH=90;
const int UNLOCK_HOME=0;//爪がかかったままの角度(ロックしている時)
const int UNLOCK_PULL=90;//爪を弾いてロックを外す角度

Servo dispenseServo;
Servo unlockServo;
bool timerRunning=false;
unsigned long startTime=0;
bool lastCoin=false;//前回見た時にコインのスイッチが押されていたか

void dispenseCoin(){
    dispenseServo.write(DISPENSE_PUSH);
    delay(500);
    dispenseServo.write(DISPENSE_HOME);
    delay(500);
}

//爪を引いてロックを外し、2秒後に戻す
void unlock(){
    unlockServo.write(UNLOCK_PULL);
    delay(2000);//ロック解除。使用者はこの間にカバーを開ける
    unlockServo.write(UNLOCK_HOME);
}

void setup(){
    pinMode(COVER_PIN,INPUT_PULLUP);
    pinMode(COIN_PIN,INPUT_PULLUP);
    dispenseServo.attach(DISPENSE_PIN);
    unlockServo.attach(UNLOCK_PIN);
    dispenseServo.write(DISPENSE_HOME);
    unlockServo.write(UNLOCK_HOME);
    Serial.begin(9600);
    Serial.println("ready");
}

void loop(){
    bool coverClosed=(digitalRead(COVER_PIN)==LOW);
    bool coinNow=(digitalRead(COIN_PIN)==LOW);

    if(coverClosed&&!timerRunning){
        timerRunning=true;
        startTime=millis();
        Serial.println("cover closed -> timer start");
    }
    if(!coverClosed&&timerRunning){
        timerRunning=false;
        Serial.println("cover opened -> timer stop");
    }

    //前回は押されていなくて、今は押されている。今押された瞬間
    if(coinNow&&!lastCoin){
        delay(50);
        if(coverClosed){
            Serial.println("coin in -> unlock");
            timerRunning=false;//タイマーを0に戻す
            unlock();
        }
    }
    lastCoin=coinNow;//次に比べる為に、今の状態を覚えておく

    //時間がたったら鍵を一個放出
    if(timerRunning&&millis()-startTime>=COIN_INTERVAL){
        Serial.println("time up -> dispense 1 coin");
        dispenseCoin();
        startTime=millis();
    }

}