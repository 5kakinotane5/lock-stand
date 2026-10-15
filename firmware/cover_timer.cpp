// C++ code
//一秒経過ごとにシリアルモニタに経過秒数を表示するプログラム
const int COVER_PIN=2;//カバーのスイッチをつないだピン

bool timerRunning=false;//タイマがうごいているか
unsigned long startTime=0;//タイマの始めた時刻ミリ秒
unsigned long lastPrint=0;//最後に画面に表示した時刻

void setup()
{
  pinMode(COVER_PIN,INPUT_PULLUP);//内部の抵抗でHIGHにしておく
  Serial.begin(9600);//PCとの通信を始める(速さ:9600)
  Serial.println("ready");
}

void loop()
{
  //スイッチがGNDに繋がっている(LOW)＝カバーが閉まっている
  bool coverCloesd=(digitalRead(COVER_PIN)==LOW);

  //閉まっていて、まだタイマーが動いていない→今が「スイッチが閉まった瞬間」
  if(coverCloesd&&!timerRunning){
    timerRunning=true;
    startTime=millis();//今の時刻を覚えておく
    Serial.println("cover closed -> timer start");
  }

  //開いていて、タイマーが動いている→今が「スイッチが開いた瞬間」
  if(!coverCloesd&&timerRunning){
    timerRunning=false;
    Serial.println("cover opened -> timer stop");
  }

  //タイマが動いている間は一秒ごとに経過時間を表示
  if(timerRunning&&millis()-lastPrint>=1000){
    lastPrint=millis();
    Serial.print("elapsed: ")//elapsed=経過という意味
    Serial.print((millis()-startTime)/1000);
    Serial.println(" s");
  }
}