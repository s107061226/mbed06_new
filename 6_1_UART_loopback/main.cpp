#include "mbed.h"

DigitalIn button0(SW2);         // sw2 digital input
DigitalIn button1(SW3);         // sw3 digital input
DigitalOut red_led(LED1);       // red LED digital output 
DigitalOut green_led(LED2);     // green LED digitla output

Thread thread1;                 // parallel execute thread 1 for master
Thread thread2;                 // parallel execute thread 2 for slave

/* UART connection: pc <--> master <--> slave */
Serial pc(USBTX, USBRX);        // UART tx, rx to pc
Serial device1(D12, D11);       // UART tx, rx of master
Serial device2(D1, D0);         // UART tx, rx of slave

char recv_from_pc;              // data receive by pc
char recv_from_k66f;            // data receive by k66f

/* k66f master receive data from pc */
/* UART pc <--> master */
void master_thread() 
{
    while (1) {
        if (pc.readable()) {                // if there are some data can read from pc
            recv_from_pc = pc.getc();       // K66F master receive data from pc
            device1.putc(recv_from_pc);     // K66F master transmit the data out from pc
        } else {
            device1.putc('0');              // K66F master transmit 0 if no data from pc
        }
        wait(0.05);
    }
}

/* k66f slave control the led */
/* UART master <--> slave */
void slave_thread() 
{
    red_led = 1;                                // turn off the red led
    green_led = 1;                              // turn off the green led
    while (1) {
        if (device2.readable()) {               // if there are some data can read from slave RX
            recv_from_k66f = device2.getc();    // slave get data from slave RX <-- master TX
            if (recv_from_k66f == '1') {        // if data is 1, turn on green
                red_led = 1;
                green_led = 0;
            } else if (recv_from_k66f == '2') { // if data is 2, turn on red
                red_led = 0;
                green_led = 1;
            } else {                            // turn off leds
                red_led = 1;
                green_led =1;
            }
            pc.putc(recv_from_k66f);            // transmit data to pc screen
        }
    }
}

/* turn on two threads */
int main() 
{
    thread1.start(master_thread);
    thread2.start(slave_thread);
}