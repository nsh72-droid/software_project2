// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25        // 샘플링 주기 25ms
#define PULSE_DURATION 10
#define _DIST_MIN 100.0    // 최소 거리 100mm
#define _DIST_MAX 300.0    // 최대 거리 300mm

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float distance;
  int brightness;

  // 25ms마다 측정
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // 거리 범위를 벗어난 경우
  if (distance == 0.0) {
    brightness = 255;       // LED OFF
  }
  else if (distance <= 100.0) {
    brightness = 255;       // LED OFF
  }
  else if (distance < 200.0) {
    // 100mm → 255
    // 150mm → 약 127
    // 200mm → 0
    brightness = (int)(255.0 * (200.0 - distance) / 100.0);
  }
  else if (distance == 200.0) {
    brightness = 0;         // 최대 밝기
  }
  else if (distance < 300.0) {
    // 200mm → 0
    // 250mm → 약 127
    // 300mm → 255
    brightness = (int)(255.0 * (distance - 200.0) / 100.0);
  }
  else {
    brightness = 255;       // LED OFF
  }

  // LED 밝기 제어
  analogWrite(PIN_LED, brightness);

  // 거리와 PWM 값 출력
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" mm, PWM: ");
  Serial.println(brightness);

  // delay(50) 삭제

  // 다음 샘플링 시간
  last_sampling_time += INTERVAL;
}


// 초음파 센서 거리 측정
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
