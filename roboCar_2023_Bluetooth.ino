
/**
 * Author: Vo Van Hoang. ID: 19CE017
 * Github: https://github.com/vanhoang1809/
 * Date: 30/03/2023
 * @version: V1.0
 * File: robotCar_2023_Bluetooth.ino
 */
/*****************************************
 * Includes Libary
 * TODO: Install ServoTimer2.h "https://github.com/nabontra/ServoTimer2"
 *****************************************/
#include <stdio.h>
#include <stdint.h>
#include <ServoTimer2.h>
#include <SoftwareSerial.h>

/**
 * Khai báo Pin cho Arduino để đọc Data từ Module HC-05 gửi về.
 * TODO: Pin 4 chỉ cần khai báo không cần phải nối dây.
 */
SoftwareSerial mySerial(2, 4); // RX = 2, TX = 4
/**
 * Gọi Chức năng ServoTimer2 cho 3 Servo
 */
ServoTimer2 SERVO_1, SERVO_2, SERVO_3;
/**
 * Khai báo Pin cho 3 Servo
 */
#define Servo_PIN_1 6
#define Servo_PIN_2 3
#define Servo_PIN_3 9

/**
 * Khai báo Pin để Arduino điều khiển L298N
 */
#define ENA 16  // A2
#define IN_1 14 // A0
#define IN_2 15 // A1
#define IN_3 18 // A5
#define IN_4 19 // A4
#define ENB 17  // A3
/**
 * Global Variables
 * Khởi tạo giá trị biến đếm cho 3 Servo
 * TODO: Giá trị servo từ 0 - 180 độ tương ứng với xung PWM trong thư viện ServoTimer2.h là 900 - 2250
 *  (750) Quay đến góc 0 độ
    (1125) Quay đến góc 50 độ
    (1200) Quay đến góc 60 độ
    (1500) Quay đến góc 90 độ
    (1777) Quay đến góc 130 độ
    (1925) Quay đến góc 145 độ
    (1950) Quay đến góc 150 độ
    (2100) Quay đến góc 160 độ
    (2175) Quay đến góc 170 độ
    (2250) Quay đến góc 180 độ
 * !Có thể tuỳ chỉnh giá trị xung tuỳ thuộc vào từng loại Servo và góc quay cụ thể
 */
uint16_t count_Servo_1 = 900;
uint16_t count_Servo_2 = 1200;
uint16_t count_Servo_3 = 1500;

/**
 * Khởi tạo giá trị tốc độ động cơ cho 4 bánh xe
 */
int speedCar = 500;
int speedDiv = 3;

/****************************************
 * Function Header
 *****************************************/
void Control_Servo();
void Control_Motor();
void goAhead(int sp);
void goBack(int sp);
void turnLeft(int sp);
void turnRight(int sp);
void goAheadLeft(int sp);
void goAheadRight(int sp);
void goBackLeft(int sp);
void goBackRight(int sp);
void stopCar();
/**
 * @brief: Hàm thiết lập các chức năng cho chương trình
 * @param: Thiết lập Pin cho 3 Servo
 *         Quay tính hiệu cho Servo các góc với giá trị đã được khởi tạo
 *         Khởi tạo Monitor ở Arduino IDE: Serial
 *         Khởi tạo bluetooth mySerial với Baudrate: 9600
 */
void setup()
{
    SERVO_1.attach(Servo_PIN_1);
    SERVO_2.attach(Servo_PIN_2);
    SERVO_3.attach(Servo_PIN_3);
    Delay_MS(1);
    SERVO_1.write(count_Servo_1);
    SERVO_2.write(count_Servo_2);
    SERVO_3.write(count_Servo_3);
    mySerial.begin(9600);
    Serial.begin(9600);
}
/**
 * @brief: Hàm vòng lặp vô hạn
 * @param: Gọi Function Control_Motor(); vô hạn
 */
void loop()
{
    Control_Motor();
}
/**
 * @brief: Hàm tạo delay với một giá trị ms
 * @param: Sử dụng millis() để tạo delay cho chương trình
 */
void Delay_MS(unsigned long ms)
{
    unsigned long start_time = millis();
    while (millis() - start_time < ms)
    {
        // do nothing
    }
}
/**
 * @brief: Hàm điều khiển Servo
 * @param: 1. Đọc giá trị từ module HC-05 gửi về
 *         2. Nếu là ký tự 'W' thì chuyển đổi qua chức năng điều khiển động cơ
 *         3. Nếu là các ký tự khác như 'U', 'u', 'L', 'R', 'F', 'B', 'S' thì thực hiện
 *              các chức năng có trong điều kiện đó
 */
void Control_Servo()
{
    while (1)
    {
        if (mySerial.available() > 0)
        {
            char data = mySerial.read();
            Serial.println(data);
            switch (data)
            {
            case 'W':
                Serial.println("Goto Motor channel.");
                // Set Rotation Angle of Servo and call Function write Servo with PWM
                count_Servo_1 = 900;
                count_Servo_2 = 1200;
                count_Servo_3 = 1500;
                Control_Servo_1(count_Servo_1);
                Control_Servo_2(count_Servo_2);
                Control_Servo_3(count_Servo_3);
                // Using goto: to function Control_Motor();
                goto Out;
                break;
            case 'U':
                Serial.println("S1-F");
                // Set PWM value of servo_1 = 90 deg
                count_Servo_1 = 1500;
                Control_Servo_1(count_Servo_1);
                break;
            case 'u':
                Serial.println("S1-B");
                // Set PWM value of servo_1 about 40 deg
                count_Servo_1 = 900;
                Control_Servo_1(count_Servo_1);
                break;
            case 'L':
                Serial.println("S2-F");
                // Check if the value of the PWM servo_2 is more than 2100 (160 deg).
                if (count_Servo_2 >= 2100)
                {
                    // Set PWM value of servo_2 about 160 deg
                    count_Servo_2 = 2100;
                    Control_Servo_2(count_Servo_2);
                }
                // If not: Count value PWM +=45
                else
                {
                    count_Servo_2 += 45;
                    Control_Servo_2(count_Servo_2);
                }
                break;
            case 'R':
                Serial.println("S2-B");
                // Check if the value of the PWM servo_2 is less than 1200 (60 deg).
                if (count_Servo_2 <= 1200)
                {
                    // Set PWM value of servo_2 about 60 deg
                    count_Servo_2 = 1200;
                    Control_Servo_2(count_Servo_2);
                }
                else
                {
                    // If not: Count value PWM -=45
                    count_Servo_2 -= 45;
                    Control_Servo_2(count_Servo_2);
                }
                break;
            case 'F':
                Serial.println("S3-F");
                if (count_Servo_3 >= 2250)
                {
                    // Set PWM value of servo_3 about 180 deg
                    count_Servo_3 = 2250;
                    Control_Servo_3(count_Servo_3);
                }
                else
                {
                    // If not: Count value PWM +=50
                    count_Servo_3 += 50;
                    Control_Servo_3(count_Servo_3);
                }
                break;
            case 'B':
                Serial.println("S3-B");
                if (count_Servo_3 <= 1200)
                {
                    // Set PWM value of servo_3 about 60 deg
                    count_Servo_3 = 1200;
                    Control_Servo_3(count_Servo_3);
                }
                else
                {
                    // If not : Count value PWM += 50
                    count_Servo_3 -= 50;
                    Control_Servo_3(count_Servo_3);
                    // Control_Servo_2(count_Servo_2);
                }
                break;
            case 'S':
                Serial.println("NoSer");
                break;
            }
        }
    }
Out:
    Control_Motor();
}
/**
 * @brief: Hàm điều khiển động cơ thông qua Module L298N
 * @param: 1. Đọc giá trị từ module HC-05 gửi về
 *         2. Nếu là ký tự 'w' thì chuyển đổi qua chức năng điều khiển Servo
 *         3. Nếu là các ký tự khác như 'U', 'u', 'L', 'R', 'F', 'B', 'S' thì thực hiện
 *              các chức năng có trong điều kiện đó
 */
void Control_Motor()
{
    while (1)
    {
        if (mySerial.available() > 0)
        {
            char data = mySerial.read();
            Serial.println(data);
            switch (data)
            {
            case 'w':
                // Using goto: to function Control_Servo();
                Serial.println("Goto Servo channel.");
                goto Out;
                break;
            case '0':
                // Serial.println("Motor Speed 400.");
                speedCar = 400;
                break;
            case '1':
                // Serial.println("Motor Speed 450.");
                speedCar = 450;
                break;
            case '2':
                // Serial.println("Motor Speed 540.");
                speedCar = 540;
                break;
            case '3':
                // Serial.println("Motor Speed 610.");
                speedCar = 610;
                break;
            case '4':
                // Serial.println("Motor Speed 680.");
                speedCar = 680;
                break;
            case '5':
                // Serial.println("Motor Speed 820.");
                speedCar = 820;
                break;
            case '6':
                // Serial.println("Motor Speed 880.");
                speedCar = 880;
                break;
            case '7':
                // Serial.println("Motor Speed 900.");
                speedCar = 900;
                break;
            case '8':
                // Serial.println("Motor Speed 980.");
                speedCar = 980;
                break;
            case '9':
                // Serial.println("Motor Speed 500.");
                speedCar = 500;
                break;
            case 'q':
                // Serial.println("Motor Speed 1023.");
                speedCar = 1023;
                break;
            case 'F':
                // Serial.println("Motor --1--Forward");
                goAhead(speedCar);
                break;
            case 'B':
                // Serial.println("Motor --1--Back");
                goBack(speedCar);
                break;
            case 'L':
                // Serial.println("Motor --1--Left");
                turnLeft(speedCar);
                break;
            case 'R':
                // Serial.println("Motor --1--Right");
                turnRight(speedCar);
                break;
            case 'G':
                // Serial.println("Motor --1--Forward Left");
                goAheadLeft(speedCar);
                break;
            case 'I':
                // Serial.println("Motor --1--Forward Right");
                goAheadRight(speedCar);
                break;
            case 'H':
                // Serial.println("Motor --1--Back Left ");
                goBackLeft(speedCar);
                break;
            case 'J':
                // Serial.println("Motor --1--Back Right");
                goBackRight(speedCar);
                break;
            case 'S':
                // Serial.println("No Control Motor");
                stopCar();
                break;
            }
        }
    }
Out:
    Control_Servo();
}
/****************************************
 * Servo.Write(Value) function
 *****************************************/
void Control_Servo_1(uint16_t rotation)
{
    uint16_t temp = 900;

    if (temp < rotation)
    {
        for (uint16_t i = temp; i <= rotation; i += 50)
        {
            /* code */
            Delay_MS(10);
            SERVO_1.write(i);
            // Serial.print("S1F1 = ");
            // Serial.println(i);
            Delay_MS(10);
        }
    }
    else
    {
        SERVO_1.write(temp);
        // Delay_MS(10);
        Serial.print("s1 = ");
        Serial.println(temp);
    }
    Serial.print("R-s1 = ");
    Serial.println(rotation);
}
void Control_Servo_2(uint16_t rotation)
{
    Delay_MS(5);
    SERVO_2.write(rotation);
    Serial.print("S2F1 = ");
    Serial.println(rotation);
}
void Control_Servo_3(uint16_t rotation)
{
    Delay_MS(5);
    SERVO_3.write(rotation);
    Serial.print("S3F1 = ");
    Serial.println(rotation);
}
/****************************************
 * Control the direction of the vehicle's movement
 *****************************************/
void goAhead(int sp)
{
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, sp);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void goBack(int sp)
{
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, sp);
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void turnLeft(int sp)
{
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, sp);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void turnRight(int sp)
{
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, sp);
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void goAheadLeft(int sp)
{
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, sp / speedDiv);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void goAheadRight(int sp)
{
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, HIGH);
    analogWrite(ENB, sp / speedDiv);

    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, HIGH);
    analogWrite(ENA, sp);
    Serial.println(sp);
}
void goBackLeft(int sp)
{
    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, sp / speedDiv);
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, sp);
    Serial.println(sp);
}
void goBackRight(int sp)
{
    digitalWrite(IN_3, HIGH);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, sp / speedDiv);

    digitalWrite(IN_1, HIGH);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, sp);
    Serial.println(sp);
}
void stopCar()
{
    digitalWrite(IN_1, LOW);
    digitalWrite(IN_2, LOW);
    analogWrite(ENA, 0);
    digitalWrite(IN_3, LOW);
    digitalWrite(IN_4, LOW);
    analogWrite(ENB, 0);
}
