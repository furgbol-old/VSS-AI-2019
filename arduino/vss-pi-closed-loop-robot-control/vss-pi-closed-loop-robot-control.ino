// ® Copyright FURGBot 2019



#include "MsTimer2.h"
#include "nRF24L01.h"
#include "RF24.h"
#include "SPI.h"



#define TIME 50 // Intervalo de tempo da interrupção interna (em milissegundos)
#define MAX_ATTEMPTS 2 // Número máximo de tentativas de leitura de dados do rádio
#define ERROR_SLIDING_WINDOW_QUEUE_SIZE 2

#define ENCODER_A_RIGHT 3 // Pino de interrupção 1
#define ENCODER_A_LEFT 2 // Pino de interrupção 0
#define ENCODER_B_RIGHT 1
#define ENCODER_B_LEFT 0

#define CE A1 // Define o pino como CE
#define CSN A0 // Define o pino como CSN
#define CHANNEL 111 // Define o canal do rádio (de 0 até 125)

#define ENABLE_RIGHT 9
#define INPUT_RIGHT_FORWARD 8
#define INPUT_RIGHT_BACK 7

#define ENABLE_LEFT 6
#define INPUT_LEFT_FORWARD 5
#define INPUT_LEFT_BACK 4

#define ID_SWITCH_TWO A4
#define ID_SWITCH_THREE A5

#define PIPE_ONE 0xF1F1F1F1E1LL
#define PIPE_TWO 0xF0F0F0F0E2LL
#define PIPE_THREE 0xF0F0F0F0E3LL
#define PIPE_FOUR 0xF0F0F0F0E4LL
#define PIPE_FIVE 0xF0F0F0F0E5LL
#define PIPE_SIX 0xF0F0F0F0E6LL

#define PACKAGE_SIZE 5

#define WHEEL_RADIUS 0.03
#define ROBOT_RADIUS 0.04

#define KP 0.091039 / 1
#define KI 0.33885 / 2

#define WHEEL_TO_ENCODER_CONVERSION_RATIO 6 * 51.45

#define MAX_LINEAR_VELOCITY 1.5
#define MAX_ANGULAR_VELOCITY 37.5



typedef volatile long long int VolatileType;
typedef long long int LongType;

const uint64_t pipes[6] = {PIPE_ONE, PIPE_TWO, PIPE_THREE, PIPE_FOUR, PIPE_FIVE, PIPE_SIX};

RF24 radio(CE, CSN);

uint8_t received_package[PACKAGE_SIZE];
uint8_t robot_id;
int number_of_attempts;

double linear_velocity;
double angular_velocity;
int linear_direction;
int angular_direction;

double right_motor_input;
double right_motor_setpoint;
double right_motor_current_output;
double right_motor_previous_output;

double left_motor_input;
double left_motor_setpoint;
double left_motor_current_output;
double left_motor_previous_output;

double right_motor_current_error;
double right_motor_previous_error;
double left_motor_current_error;
double left_motor_previous_error;

VolatileType right_wheel_pulses_counter;
VolatileType right_wheel_previous_pulses;
LongType right_wheel_final_pulses;

VolatileType left_wheel_pulses_counter;
VolatileType left_wheel_previous_pulses;
LongType left_wheel_final_pulses;

int right_motor_encoder_current_state;
int right_motor_encoder_previous_state;
int left_motor_encoder_current_state;
int left_motor_encoder_previous_state;

double right_motor_velocity;
double left_motor_velocity;

int right_motor_pwm_output;
int left_motor_pwm_output;

int queue_counter;
double right_motor_error_queue[ERROR_SLIDING_WINDOW_QUEUE_SIZE];
double left_motor_error_queue[ERROR_SLIDING_WINDOW_QUEUE_SIZE];

enum AngularDirectionsLabels {
    COUNTERCLOCKWISE = 1, CLOCKWISE = 3
};

enum LinearDirectionsLabels {
    BACK = 1, FORWARD = 3
};

enum PackageLabels {
    ROBOT_ID, LINEAR_VELOCITY, ANGULAR_VELOCITY, LINEAR_DIRECTION, ANGULAR_DIRECTION
};



void setup() {
    right_motor_current_error = 0;
    right_motor_previous_error = 0;
    left_motor_current_error = 0;
    left_motor_previous_error = 0;

    right_wheel_pulses_counter = 0;
    right_wheel_previous_pulses = 0;
    right_wheel_final_pulses = 0;

    left_wheel_pulses_counter = 0;
    left_wheel_previous_pulses = 0;
    left_wheel_final_pulses = 0;

    right_motor_velocity = 0;
    left_motor_velocity = 0;

    queue_counter = 0;

    
    radio.begin();
    radio.openReadingPipe(1, pipes[PIPE_ONE]);
    radio.setPALevel(RF24_PA_MIN);
    radio.setChannel(CHANNEL);
    radio.startListening();


    pinMode(ID_SWITCH_TWO, INPUT_PULLUP);
    pinMode(ID_SWITCH_THREE, INPUT_PULLUP);
    pinMode(ENABLE_RIGHT, OUTPUT);
    pinMode(ENABLE_LEFT, OUTPUT);
    pinMode(INPUT_RIGHT_FORWARD, OUTPUT);
    pinMode(INPUT_RIGHT_BACK, OUTPUT);
    pinMode(INPUT_LEFT_FORWARD, OUTPUT);
    pinMode(INPUT_LEFT_BACK, OUTPUT);
    pinMode(ENCODER_A_RIGHT, INPUT);
    pinMode(ENCODER_A_LEFT, INPUT);
    pinMode(ENCODER_B_RIGHT, INPUT);
    pinMode(ENCODER_B_LEFT, INPUT);

    configure();

    MsTimer2::set(TIME, exec);
    MsTimer2::start();
    attachInterrupt(1, countRightWheelPulses, CHANGE);
    attachInterrupt(0, countLeftWheelPulses, CHANGE);
}



void countRightWheelPulses() {
    right_motor_encoder_current_state = ((digitalRead(ENCODER_A_RIGHT) << 1) | digitalRead(ENCODER_B_RIGHT));
    if ((right_motor_encoder_current_state == 0) || (right_motor_encoder_current_state == 3)) right_wheel_pulses_counter++;
    else if ((right_motor_encoder_current_state == 1) || (right_motor_encoder_current_state == 2)) right_wheel_pulses_counter--;
    right_motor_encoder_previous_state = right_motor_encoder_current_state;
}



void countLeftWheelPulses() {
    left_motor_encoder_current_state = ((digitalRead(ENCODER_A_LEFT) << 1) | digitalRead(ENCODER_B_LEFT));
    if ((left_motor_encoder_current_state == 0) || (left_motor_encoder_current_state == 3)) left_wheel_pulses_counter++;
    else if ((left_motor_encoder_current_state == 1) || (left_motor_encoder_current_state == 2)) left_wheel_pulses_counter--;
    left_motor_encoder_previous_state = left_motor_encoder_current_state;
}



void configure() {
    if (digitalRead(ID_SWITCH_TWO == 1) && digitalRead(ID_SWITCH_THREE == 1)) robot_id = 1;
    else if (digitalRead(ID_SWITCH_TWO == 0) && digitalRead(ID_SWITCH_THREE == 1)) robot_id = 2;
    else if (digitalRead(ID_SWITCH_TWO == 1) && digitalRead(ID_SWITCH_THREE == 0)) robot_id = 3;
    else robot_id = 0;
}



void exec() {
    if (getRadioData()) {
        readyMovement();
        moveRobot();
        updateControlParameters();
    } else stopRobot();
}



void readyMovement() {
    calculateRobotActualVelocity();
    deserialize();
    calculateRealVelocities();
    calculateSetpoints();
    getInputs();
    calculateCurrentErrors();
    calculateOutputs();
}



void updateControlParameters() {
    updateQueues();
    updatePreviousOutputs();
    updatePreviousErrors();
}



bool getRadioData() {
    number_of_attempts = 0;
    while (number_of_attempts < MAX_ATTEMPTS) {
        if (radio.available()) {
            radio.read(&received_package, PACKAGE_SIZE);
            if (received_package[ROBOT_ID] == robot_id) return true;
            else number_of_attempts++;
        } else number_of_attempts++;
    }
    return false;
}



void calculateRobotActualVelocity() {
    right_wheel_final_pulses = right_wheel_pulses_counter - right_wheel_final_pulses;
    left_wheel_final_pulses = left_wheel_pulses_counter - left_wheel_final_pulses;

    right_motor_velocity = ((2 * PI * right_wheel_final_pulses) / (WHEEL_TO_ENCODER_CONVERSION_RATIO * TIME / 1000));
    left_motor_velocity = ((2 * PI * left_wheel_final_pulses) / (WHEEL_TO_ENCODER_CONVERSION_RATIO * TIME / 1000));

    right_wheel_previous_pulses = right_wheel_pulses_counter;
    left_wheel_previous_pulses = left_wheel_pulses_counter;
}



void deserialize() {
    linear_velocity = (double)received_package[LINEAR_VELOCITY];
    angular_velocity = (double)received_package[ANGULAR_VELOCITY];
    linear_direction = (int)received_package[LINEAR_DIRECTION];
    angular_direction = (int)received_package[ANGULAR_DIRECTION];
}



void calculateRealVelocities() {
    linear_velocity = ((MAX_LINEAR_VELOCITY * linear_velocity) * (linear_direction - 2));
    angular_direction = ((MAX_ANGULAR_VELOCITY * angular_velocity) * (angular_direction - 2));
}



void calculateSetpoints() {
    right_motor_setpoint = ((linear_velocity / WHEEL_RADIUS) + ((angular_velocity * ROBOT_RADIUS) / WHEEL_RADIUS));
    left_motor_setpoint = ((linear_velocity / WHEEL_RADIUS) - ((angular_velocity * ROBOT_RADIUS) / WHEEL_RADIUS));
}



void getInputs() {
    right_motor_input = right_motor_velocity;
    left_motor_input = left_motor_velocity;
}



void calculateCurrentErrors() {
    right_motor_current_error = right_motor_setpoint - right_motor_input;
    left_motor_current_error = left_motor_setpoint - left_motor_input;
}



void updateQueues() {
    right_motor_error_queue[queue_counter] = right_motor_current_error;
    left_motor_error_queue[queue_counter] = left_motor_current_error;
    queue_counter++;
    if (queue_counter >= ERROR_SLIDING_WINDOW_QUEUE_SIZE) queue_counter = 0;
}



void calculateOutputs() {
    right_motor_current_output = right_motor_previous_output + (KP * (right_motor_current_error - right_motor_previous_error)) + 
        (KI * right_motor_previous_error * TIME * 1000);
    left_motor_current_output = left_motor_previous_output + (KP * (left_motor_current_error - left_motor_previous_error)) + 
        (KI * left_motor_previous_error * TIME * 1000);

    right_motor_pwm_output = (int)(right_motor_current_output * 255 / 6);
    left_motor_pwm_output = (int)(left_motor_current_output * 255 / 6);

    verifyPWMOutputs();
}



void verifyPWMOutputs() {
    if (right_motor_pwm_output < 0) right_motor_pwm_output = right_motor_pwm_output * (-1);
    if (left_motor_pwm_output < 0) left_motor_pwm_output = left_motor_pwm_output * (-1);
    if (right_motor_pwm_output > 255) right_motor_pwm_output = 255;
    if (left_motor_pwm_output > 255) left_motor_pwm_output = 255;
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
    analogWrite(ENABLE_RIGHT, right_motor_pwm_output);
    analogWrite(ENABLE_LEFT, left_motor_pwm_output);
    digitalWrite(INPUT_RIGHT_FORWARD, LOW);
    digitalWrite(INPUT_LEFT_BACK, LOW);
    digitalWrite(INPUT_RIGHT_BACK, HIGH);
    digitalWrite(INPUT_LEFT_FORWARD, HIGH);
}



void turnCounterclockwise() {
    analogWrite(ENABLE_RIGHT, right_motor_pwm_output);
    analogWrite(ENABLE_LEFT, left_motor_pwm_output);
    digitalWrite(INPUT_RIGHT_BACK, LOW);
    digitalWrite(INPUT_LEFT_FORWARD, LOW);
    digitalWrite(INPUT_RIGHT_FORWARD, HIGH);
    digitalWrite(INPUT_LEFT_BACK, HIGH);
}



void goForward() {
    analogWrite(ENABLE_RIGHT, right_motor_pwm_output);
    analogWrite(ENABLE_LEFT, left_motor_pwm_output);
    digitalWrite(INPUT_RIGHT_BACK, LOW);
    digitalWrite(INPUT_LEFT_BACK, LOW);
    digitalWrite(INPUT_RIGHT_FORWARD, HIGH);
    digitalWrite(INPUT_LEFT_FORWARD, HIGH);
}



void goBack() {
    analogWrite(ENABLE_RIGHT, right_motor_pwm_output);
    analogWrite(ENABLE_LEFT, left_motor_pwm_output);
    digitalWrite(INPUT_RIGHT_FORWARD, LOW);
    digitalWrite(INPUT_LEFT_FORWARD, LOW);
    digitalWrite(INPUT_RIGHT_BACK, HIGH);
    digitalWrite(INPUT_LEFT_BACK, HIGH);
}



void updatePreviousOutputs() {
    right_motor_previous_output = right_motor_current_output;
    left_motor_previous_output = left_motor_current_output;
}



void updatePreviousErrors() {
    for (int i = 0; i < ERROR_SLIDING_WINDOW_QUEUE_SIZE; i++) {
        right_motor_previous_error = right_motor_previous_error + right_motor_error_queue[i];
        left_motor_previous_error = left_motor_previous_error + left_motor_error_queue[i];
    }
}



void stopRobot() {
    right_motor_setpoint = 0;
    left_motor_setpoint = 0;

    analogWrite(ENABLE_RIGHT, 0);
    analogWrite(ENABLE_LEFT, 0);
    digitalWrite(INPUT_RIGHT_FORWARD, LOW);
    digitalWrite(INPUT_LEFT_FORWARD, LOW);
    digitalWrite(INPUT_RIGHT_BACK, LOW);
    digitalWrite(INPUT_LEFT_BACK, LOW);

    for (int i = 0; i < ERROR_SLIDING_WINDOW_QUEUE_SIZE; i++) {
        right_motor_error_queue[i] = 0;
        left_motor_error_queue[i] = 0;
    }
}



void loop() {}
