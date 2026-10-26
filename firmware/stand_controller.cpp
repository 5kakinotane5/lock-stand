#include <Servo.h>

const int COVER_PIN    = 2;
const int COIN_PIN     = 3;   // コイン＝今回の「鍵」
const int UNLOCK_PIN   = 9;
const int DISPENSE_PIN = 10;

const unsigned long COIN_INTERVAL = 10000UL;  // 試し用 10秒

// 変更：シャトル式の払い出しに合わせた角度（dispenser_test で調べた値に書き換える）
const int DISPENSE_HOME = 10;    // ポケットが通路の真下（休止）
const int DISPENSE_PUSH = 160;   // ポケットが落下穴の真上（払い出し）
const int UNLOCK_HOME   = 0;
const int UNLOCK_PULL   = 90;

Servo dispenseServo;
Servo unlockServo;
bool timerRunning = false;
unsigned long startTime = 0;
bool lastCoin = false;

void dispenseCoin() {
  dispenseServo.write(DISPENSE_PUSH);
  delay(1000);   // 変更：150°回り切って、鍵が穴から落ちるまで待つ（前は500）
  dispenseServo.write(DISPENSE_HOME);
  delay(1000);   // 変更：戻り切って、次の鍵がポケットに落ちるまで待つ（前は500）
}

void unlock() {
  unlockServo.write(UNLOCK_PULL);
  delay(2000);
  unlockServo.write(UNLOCK_HOME);
}

void setup() {
  pinMode(COVER_PIN, INPUT_PULLUP);
  pinMode(COIN_PIN,  INPUT_PULLUP);
  dispenseServo.attach(DISPENSE_PIN);
  unlockServo.attach(UNLOCK_PIN);
  dispenseServo.write(DISPENSE_HOME);
  unlockServo.write(UNLOCK_HOME);
  Serial.begin(9600);
  Serial.println("ready");
}

void loop() {
  bool coverClosed = (digitalRead(COVER_PIN) == LOW);
  bool coinNow     = (digitalRead(COIN_PIN) == LOW);

  if (coverClosed && !timerRunning) {
    timerRunning = true;
    startTime = millis();
    Serial.println("cover closed -> timer start");
  }
  if (!coverClosed && timerRunning) {
    timerRunning = false;
    Serial.println("cover opened -> timer stop");
  }

  if (coinNow && !lastCoin) {
    delay(50);                                         // チャタリングが収まるまで待つ
    if (digitalRead(COIN_PIN) == LOW && coverClosed) { // まだ押されているか確認
      Serial.println("coin in -> unlock");
      timerRunning = false;
      unlock();
    }
  }
  lastCoin = coinNow;

  if (timerRunning && millis() - startTime >= COIN_INTERVAL) {
    Serial.println("time up -> dispense 1 coin");
    dispenseCoin();
    startTime = millis();
  }
}
