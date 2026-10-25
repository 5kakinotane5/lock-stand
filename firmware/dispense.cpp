#include<Servo.h> //サーボを動かす為のライブラリ

const int COVER_PIN=2;
const int DISPENSE_PIN=10;

//コインを出す間隔。ミリ秒
const unsigned long COIN_INTERVAL=10000UL;

//サーボの角度(0~180)
const int DISPENSE_HOME=0;//待っているとき
const int DISPENSE_PUSH=90;//鍵を押し出す時

Servo dispenseServo;//払い出し用サーボを表す変数
bool timerRunning=false;
unsigned long startTime=0;

//鍵を一枚出す。押し出して戻す
void dispenseCoin(){
    dispenseServo.write(DISPENSE_PUSH);
    delay(500);//サーボが回り切るまで待つ
    dispenseServo.write(DISPENSE_HOME);
    delay(500);
}



void setup(){
    pinMode(COVER_PIN,INPUT_PULLUP);
    dispenseServo.attach(DISPENSE_PIN);//attach()でD10にサーボが繋がっていることをArduinoに教える。
    dispenseServo.write(DISPENSE_HOME);
    Serial.begin(9600);//ArduinoとPCがシリアル通信(USBを通した文字の通信)をはめるための命令。これで、ArduinoからPCに文字を送れる
    Serial.println("ready");
}

void loop(){
//スイッチがGNDに繋がっている(LOW)＝カバーが閉まっている
    bool coverClosed=(digitalRead(COVER_PIN)==LOW);
    
    //カバーが閉まっていて、スイッチが今閉まった瞬間
    if(coverClosed&&!timerRunning){
        timerRunning=true;
        startTime=millis();
        Serial.println("cover closed -> timer start");
    }

    //カバーが開いていて、スイッチが今開いた瞬間
    if(!coverClosed&&timerRunning){
        timerRunning=false;
        Serial.println("cover closed -> timer stop");
    }

    //決めた時間がたったら、鍵を一個出してまた０から数える
    if(timerRunning&&millis()-startTime>=COIN_INTERVAL){
        Serial.println("time up -> dispense 1 coin");
        dispenseCoin();
        startTime=millis();
    }
}