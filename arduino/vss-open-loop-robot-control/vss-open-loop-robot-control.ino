// ® Copyright FURGBot 2019

#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <MsTimer2.h>


#define TIME 100 // Intervalo de tempo para a interrupção interna (em ms)
#define MAX_ATTEMPTS 3 // Número de tentativas de leitura do rádio antes do robô parar

#define CE A1 // Define o pino como CE
#define CSN A0 // Define o pino como CS
#define CHANNEL 111 // Define o canal do rádio (de 0 até 125)

#define ENABLE_RIGHT 9
#define ENABLE_LEFT 6

#define INPUT_RIGHT_FORWARD 7
#define INPUT_RIGHT_BACK 8

#define INPUT_LEFT_FORWARD 4
#define INPUT_LEFT_BACK 5

#define ID_SWITCH_TWO A4
#define ID_SWITCH_THREE A5

#define PACKAGE_SIZE 5

#define WHEEL_RADIUS 0.03
#define ROBOT_RADIUS 0.04

#define MAX_ROBOT_LINEAR_VELOCITY 1.5
#define MAX_ROBOT_ANGULAR_VELOCITY 37.5

#define PIPE_ONE 0xF1F1F1F1E1LL
#define PIPE_TWO 0xF0F0F0F0E2LL
#define PIPE_THREE 0xF0F0F0F0E3LL
#define PIPE_FOUR 0xF0F0F0F0E4LL
#define PIPE_FIVE 0xF0F0F0F0E5LL
#define PIPE_SIX 0xF0F0F0F0E6LL

#define CONVERSION_CONSTANT 30/PI


enum AngularDirectionsLabels {
    COUNTERCLOCKWISE = 1, CLOCKWISE = 3
};

enum LinearDirectionsLabels {
    BACK = 1, FORWARD = 3
};

enum PackageLabels {
    ROBOT_ID, LINEAR_VELOCITY, ANGULAR_VELOCITY, LINEAR_DIRECTION, ANGULAR_DIRECTION
};


uint8_t robot_id;
int number_of_failed_attempts;

uint8_t received_package[PACKAGE_SIZE];
const uint64_t pipes[6] = {PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR, PIPE_FIVE, PIPE_SIX};

double linear_velocity;
double angular_velocity;
int linear_direction;
int angular_direction;

double right_wheel_velocity;
double left_wheel_velocity;

double right_motor_voltage;
double left_motor_voltage;

int right_motor_pwm;
int left_motor_pwm;

RF24 radio(CE, CSN);


void setup() {
  pinMode(ID_SWITCH_TWO, INPUT_PULLUP);
  pinMode(ID_SWITCH_THREE, INPUT_PULLUP);
  pinMode(ENABLE_RIGHT, OUTPUT);
  pinMode(ENABLE_LEFT, OUTPUT);
  pinMode(INPUT_RIGHT_FORWARD, OUTPUT);
  pinMode(INPUT_RIGHT_BACK, OUTPUT);
  pinMode(INPUT_LEFT_FORWARD, OUTPUT);
  pinMode(INPUT_LEFT_BACK, OUTPUT);

  Serial.begin(9600);
  
  radio.begin(); // Inicia o rádio
  radio.openReadingPipe(1, pipes[PIPE_ONE]); // Define o pipe de comunicação
  radio.setPALevel(RF24_PA_MIN); // Define o nível PA para mínimo
  radio.setChannel(CHANNEL); // Canal selecionado para ocorrer a transmissão de dados de 0 a 125
  radio.startListening(); // Começa a escutar

  configure(); // Função para definir ID do robô

  MsTimer2::set(TIME, exec);
  MsTimer2::start();
}


void configure() {
  if ((digitalRead(ID_SWITCH_TWO) == 1) && (digitalRead(ID_SWITCH_THREE) == 1)) robot_id = 1;
  else if ((digitalRead(ID_SWITCH_TWO) == 0) && (digitalRead(ID_SWITCH_THREE) == 1)) robot_id = 2;
  else if ((digitalRead(ID_SWITCH_TWO) == 1) && (digitalRead(ID_SWITCH_THREE) == 0)) robot_id = 3;
  else robot_id = 0;
}


void exec() {
  if (getRadioData()) {
    deserialize();
    calculateWheelsVelocity();
    getVoltage();
    getPwm();
    moveRobot();
  } else stopRobot();
}


bool getRadioData() {
  number_of_failed_attempts = 0;
  while (number_of_failed_attempts < MAX_ATTEMPTS) {
    Serial.println(radio.available());
    if (radio.available()) {
      radio.read(&received_package, PACKAGE_SIZE);
      if (received_package[ROBOT_ID] == robot_id) return true;
      else number_of_failed_attempts++;
    } else number_of_failed_attempts++;
  }
  return false;
}


void deserialize() {
  linear_velocity = (double)received_package[LINEAR_VELOCITY];
  angular_velocity = (double)received_package[ANGULAR_VELOCITY];
  linear_direction = (int)received_package[LINEAR_DIRECTION];
  angular_direction = (int)received_package[ANGULAR_DIRECTION];
}


void calculateWheelsVelocity() {
  // Calcula velocidades linear e angular reais
  linear_velocity = (MAX_ROBOT_LINEAR_VELOCITY * linear_velocity) / 127.0;
  angular_velocity = (MAX_ROBOT_ANGULAR_VELOCITY * angular_velocity) / 127.0;

  // Modelo cinemático para passar de velocidades linear e angular para velocidade das rodas em rpm
  right_wheel_velocity = ((linear_velocity / WHEEL_RADIUS) + (angular_velocity * ROBOT_RADIUS) / WHEEL_RADIUS) * CONVERSION_CONSTANT;
  left_wheel_velocity = ((linear_velocity / WHEEL_RADIUS) - (angular_velocity * ROBOT_RADIUS) / WHEEL_RADIUS) * CONVERSION_CONSTANT;
}


void getVoltage() {
  right_motor_voltage = 0.548 + (0.00724 * right_wheel_velocity) + (0.00000142 * (right_wheel_velocity * right_wheel_velocity));
  left_motor_voltage = 0.548 + (0.00724 * left_wheel_velocity) + (0.00000142 * (left_wheel_velocity * left_wheel_velocity));
}


void getPwm() {
  right_motor_pwm = (int)((right_motor_voltage * 255) / 6);
  left_motor_pwm = (int)((left_motor_voltage * 255) / 6);

  if (right_motor_pwm < 0) right_motor_pwm = right_motor_pwm * (-1);
  if (left_motor_pwm < 0) left_motor_pwm = left_motor_pwm * (-1);
  if (right_motor_pwm > 255) right_motor_pwm = 255;
  if (left_motor_pwm > 255) left_motor_pwm = 255;
}


void moveRobot() {
  if ((linear_velocity == 0) && (angular_velocity > 0)) {
    if (angular_direction == CLOCKWISE) turnClockwise();
    else if (angular_direction == COUNTERCLOCKWISE) turnCounterclockwise();
  } else if ((linear_velocity > 0) && (angular_velocity == 0)) {
    if (linear_direction == FORWARD) goForward();
    else if (linear_direction == BACK) goBack();
  } else stopRobot();
}


void turnClockwise() {
  analogWrite(ENABLE_RIGHT, right_motor_pwm);
  analogWrite(ENABLE_LEFT, left_motor_pwm);
  digitalWrite(INPUT_RIGHT_FORWARD, LOW);
  digitalWrite(INPUT_LEFT_BACK, LOW);
  digitalWrite(INPUT_RIGHT_BACK, HIGH);
  digitalWrite(INPUT_LEFT_FORWARD, HIGH);
}


void turnCounterclockwise() {
  analogWrite(ENABLE_RIGHT, right_motor_pwm);
  analogWrite(ENABLE_LEFT, left_motor_pwm);
  digitalWrite(INPUT_RIGHT_BACK, LOW);
  digitalWrite(INPUT_LEFT_FORWARD, LOW);
  digitalWrite(INPUT_RIGHT_FORWARD, HIGH);
  digitalWrite(INPUT_LEFT_BACK, HIGH);
}


void goForward() {
  analogWrite(ENABLE_RIGHT, right_motor_pwm);
  analogWrite(ENABLE_LEFT, left_motor_pwm);
  digitalWrite(INPUT_RIGHT_BACK, LOW);
  digitalWrite(INPUT_LEFT_BACK, LOW);
  digitalWrite(INPUT_RIGHT_FORWARD, HIGH);
  digitalWrite(INPUT_LEFT_FORWARD, HIGH);
}


void goBack() {
  analogWrite(ENABLE_RIGHT, right_motor_pwm);
  analogWrite(ENABLE_LEFT, left_motor_pwm);
  digitalWrite(INPUT_RIGHT_FORWARD, LOW);
  digitalWrite(INPUT_LEFT_FORWARD, LOW);
  digitalWrite(INPUT_RIGHT_BACK, HIGH);
  digitalWrite(INPUT_LEFT_BACK, HIGH);
}


void stopRobot() {
  analogWrite(ENABLE_RIGHT, 0);
  analogWrite(ENABLE_LEFT, 0);
  digitalWrite(INPUT_RIGHT_FORWARD, 0);
  digitalWrite(INPUT_RIGHT_BACK, 0);
  digitalWrite(INPUT_LEFT_FORWARD, 0);
  digitalWrite(INPUT_LEFT_BACK, 0);
}


void loop() {}
