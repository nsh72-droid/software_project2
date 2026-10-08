#include <Servo.h>

// =========================
// 핀 설정
// =========================
#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13
#define PIN_SERVO 10


// =========================
// 초음파 센서 설정
// =========================
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)


// =========================
// 거리 측정 범위
// 18cm ~ 36cm
// 단위: mm
// =========================
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0


// =========================
// EMA 필터
// =========================
#define _EMA_ALPHA 0.1


// =========================
// 서보 PWM 설정
// 실제 측정한 값으로 수정
// =========================
#define _DUTY_MIN 600
#define _DUTY_MAX 2400

// 0°와 180°의 중간값 = 90°
#define _DUTY_NEU ((_DUTY_MIN + _DUTY_MAX) / 2)


// =========================
// 전역 변수
// =========================
float dist_ema;
float dist_prev = _DIST_MIN;

unsigned long last_sampling_time;

Servo myservo;


// =========================
// setup()
// =========================
void setup() {

  // 핀 설정
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // LED OFF
  // LED가 Active LOW라고 가정
  digitalWrite(PIN_LED, HIGH);


  // 서보 연결
  myservo.attach(PIN_SERVO);


  // 서보 초기 위치
  // 90°
  myservo.writeMicroseconds(_DUTY_MIN);


  // 거리 초기값
  dist_prev = _DIST_MIN;
  dist_ema = dist_prev;


  // 시리얼 통신
  Serial.begin(57600);
}


// =========================
// loop()
// =========================
void loop() {

  float dist_raw;
  float dist_filtered;

  // 일정한 시간 간격으로 측정
  if (millis() < last_sampling_time + INTERVAL)
    return;


  // =========================
  // 초음파 거리 측정
  // =========================
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);


  // =========================
  // 범위 필터
  // 18~36cm 밖의 값은
  // 이전 정상 측정값을 사용
  // =========================
  if (dist_raw == 0.0 ||
      dist_raw < _DIST_MIN ||
      dist_raw > _DIST_MAX) {

    dist_filtered = dist_prev;

  }
  else {

    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  }


  // =========================
  // EMA 필터
  // =========================
  dist_ema = _EMA_ALPHA * dist_filtered
           + (1.0 - _EMA_ALPHA) * dist_ema;


  // =========================
  // 거리 → 서보 각도
  // =========================

  float angle;

  // 18cm 이하
  if (dist_ema <= _DIST_MIN) {

    angle = 0.0;
  }

  // 36cm 이상
  else if (dist_ema >= _DIST_MAX) {

    angle = 180.0;
  }

  // 18~36cm
  else {

    // 18~36cm를
    // 0~180°로 선형 변환
    angle = (dist_ema - _DIST_MIN)
          * 180.0
          / (_DIST_MAX - _DIST_MIN);
  }


  // =========================
  // 각도 → PWM
  // =========================

  int duty;

  duty = _DUTY_MIN
       + (angle / 180.0)
       * (_DUTY_MAX - _DUTY_MIN);


  // =========================
  // 서보 이동
  // =========================
  myservo.writeMicroseconds(duty);


  // =========================
  // LED 제어
  // 측정값이 18~36cm 안에 있으면 ON
  // =========================

  if (dist_raw >= _DIST_MIN &&
      dist_raw <= _DIST_MAX) {

    digitalWrite(PIN_LED, LOW);

  }
  else {

    digitalWrite(PIN_LED, HIGH);
  }


  // =========================
  // 시리얼 모니터 출력
  // =========================

  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",dist:");
  Serial.print(dist_raw);

  Serial.print(",ema:");
  Serial.print(dist_ema);

  Serial.print(",Servo:");
  Serial.print(angle);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.println("");


  // 다음 측정 시간
  last_sampling_time += INTERVAL;
}


// =========================
// 초음파 거리 측정 함수
// =========================
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);

  delayMicroseconds(PULSE_DURATION);

  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
