#include <SoftwareSerial.h>
#include <Servo.h>
#include <cppQueue.h>

Servo servo[7];
Queue queue(sizeof(int));
SoftwareSerial BT(11, 10);
String voice;
int decpos = 0;

const unsigned char digits[] = {
  1 << 0 | 1 << 1 | 1 << 2 | 1 << 4 | 1 << 5 | 1 << 6, // 0
  1 << 0 | 1 << 2 | 1 << 5, // 1
  1 << 0 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 6, // 2
  1 << 0 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6, // 3
  1 << 1 | 1 << 2 | 1 << 3 | 1 << 5, // 4
  1 << 0 | 1 << 1 | 1 << 3 | 1 << 5 | 1 << 6, // 5
  1 << 0 | 1 << 1 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6, // 6
  1 << 0 | 1 << 2 | 1 << 5, // 7
  1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6, // 8
  1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6 // 9
};

void setup() {
  for (int i = 0; i < 7; i++) {
    servo[i].attach(9 - i);
  }

  BT.begin(9600);
  Serial.begin(9600);

  countdown();
}

void loop() {
  while (BT.available()) {
    delay(10);
    char c = BT.read();

    if (c == '#')
      break;

    voice += c;
  }

  int ch;
  if (voice.length() > 1) {
    Serial.print(voice);
    if (voice == "*стоп") {
      queue.clean();
    } else if (voice == "*сброс") {
      queue.clean();
      show_digit(0);
    } else if (queue.isEmpty()){
      if (voice == "*ель") {
        decpos = 0;
        ch = -1;
        queue.push(&ch);
      } else if (voice == "*порт") {
        decpos = 0;
        ch = -2;
        queue.push(&ch);
      } else if (voice == "*кора") {
        countdown();
      }else {
        for (int i = 1; i < voice.length(); i++) {
          if (voice[i] < '0' || voice[i] > '9')
            continue;

          ch = voice[i] - '0';
          queue.push(&ch);
        }
      }
    }
    voice = "";
  }

  if (!queue.isEmpty()) {
    int digit;
    queue.peek(&digit);

    if (digit >= 0) {
      show_digit(digit);
      queue.pop(&digit);
    } else {
      show_digit(next_digit(digit));
      decpos++;
    }
  }
}

void clear_digit() {
  for (int i = 0; i < 7; i++) {
    servo[i].write(0);
  }
}

void set_digit(int digit) {
  for (int i = 0; i < 7; i++) {
    bool bt = (digits[digit] & ( 1 << i )) >> i;
    servo[i].write(bt * 90);
  }
}

void show_digit(int digit) {
  clear_digit();
  delay(900);

  set_digit(digit);
  delay(1500);
}

int next_digit(int number) { // TODO:
  if (decpos > 50) {
    queue.clean();
    return;
  }
  
  if (number == -1) { // e number
    char e[] = "71828182845904523536028747135266249775724709369995";
    return e[decpos] - '0';
  } else if (number == -2) { // pi number
    char pi[] = "14159265358979323846264338327950288419716939937510";
    return pi[decpos] - '0';
  }
}

void countdown() {
  for (int i = 9; i >= 0; i--) {
    int fuck = i;
    queue.push(&fuck);
  }
}
