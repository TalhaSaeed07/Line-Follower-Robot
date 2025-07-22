#define enA 11
#define enB 3
#define in1 12
#define in2 8
#define in3 7
#define in4 4

#define lmf in1
#define lmb in2
#define rmf in3
#define rmb in4
#define lms enA
#define rms enB

#define LINE_THRESHOLD 600

const int numSensors = 5;
int sensorPins[numSensors] = {A0, A1, A2, A3, A4};

int threshold[numSensors];
int whiteValues[numSensors] = {979, 979, 979, 979, 979};  
int blackValues[numSensors] = {280, 280, 280, 280, 280};  
int sensorReadings[numSensors];
float c = 0;

int baseSpeedA = 60;
int baseSpeedB = 60;
int maxSpeed = 100;

float kp = 25;
float kd = 15;
float ki = 0;

float p = 0, i = 0, d = 0;
int last_error = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numSensors; i++) {
    pinMode(sensorPins[i], INPUT);
    threshold[i] = (whiteValues[i] + blackValues[i]) / 2;
  }

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  stop();
  delay(4000);
}

void loop() {
  followLine();
}

void sensor_reading() {
  float a = 0, b = 0;
  int black_count = 0;

  for (int i = 0; i < numSensors; i++) {
    int reading = analogRead(sensorPins[i]);

    if (reading < LINE_THRESHOLD) {
      sensorReadings[i] = 1;
      black_count++;
    } else {
      sensorReadings[i] = 0;
    }
  }

  if (black_count == 0) {
    stop();
    delay(1000);
    return;
  }

  a = (sensorReadings[0] * 1 + sensorReadings[1] * 2 + sensorReadings[2] * 3 +
       sensorReadings[3] * 4 + sensorReadings[4] * 5);
  b = (sensorReadings[0] + sensorReadings[1] + sensorReadings[2] +
       sensorReadings[3] + sensorReadings[4]);

  if (b > 0) {
    c = a / b;
  }
}

void followLine() {
  while (true) {
    sensor_reading();

    float error = c - 3;
    p = error;
    i += error;
    d = error - last_error;
    last_error = error;

    float correction = kp * p + ki * i + kd * d;

    int left_motor = baseSpeedA + correction;
    int right_motor = baseSpeedB - correction;

    motor(left_motor, right_motor);
  }
}

void motor(int a, int b) {
  if (a > 0) {
    digitalWrite(lmf, LOW);
    digitalWrite(lmb, HIGH);
  } else {
    digitalWrite(lmf, HIGH);
    digitalWrite(lmb, LOW);
    a = -a;
  }

  if (b > 0) {
    digitalWrite(rmf, HIGH);
    digitalWrite(rmb, LOW);

  } else {
    digitalWrite(rmf, LOW);
    digitalWrite(rmb, HIGH);
    b = -b;
  }

  a = constrain(a, 0, 90);
  b = constrain(b, 0, 90);
  analogWrite(lms, a);
  analogWrite(rms, b);


}

void stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}
