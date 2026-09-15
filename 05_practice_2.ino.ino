int LED = 7;

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  // 처음 1초 동안 켜기
  digitalWrite(LED, HIGH);
  delay(1000);

  // 1초 동안 5번 깜빡이기
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED, LOW);
    delay(100);

    digitalWrite(LED, HIGH);
    delay(100);
  }

  // LED 끄기
  digitalWrite(LED, LOW);

  // 무한 루프
  while (1) {
  }
}
