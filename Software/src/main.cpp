#include <Arduino.h>
#include <TFT_eSPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void task(void *val) {
    while (true) {
        Serial.println((char *) val);
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("hello");
    xTaskCreate(task,           //任务函数
                "task1",         //这个参数没有什么作用，仅作为任务的描述
                2048,            //任务栈的大小
                (void *)"task1",         //传给任务函数的参数
                2,              //优先级，数字越大优先级越高
                NULL          //传回的句柄，不需要的话可以为NULL
                ); //指定运行任务的ＣＰＵ，使用这个宏表示不会固定到任何核上
    xTaskCreate(task, "task2", 2048, (void*)"task2", 3, NULL);
    xTaskCreate(task, "task3", 2048, (void*)"task3", 2, NULL);
}

void loop() {
}