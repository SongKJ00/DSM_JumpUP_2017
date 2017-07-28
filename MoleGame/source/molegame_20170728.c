#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>

#define F_CPU 16000000
#define true 1
#define false 0
#define DDR_LED DDRB
#define PORT_LED PORTB
#define DDR_MOLE_SW0 DDRC
#define DDR_MOLE_SW_ELSE DDRD
#define PIN_MOLE_SW0 PINC
#define PIN_MOLE_SW_ELSE PIND
#define DDR_GOT_IT_LED DDRF
#define PORT_GOT_IT_LED PORTF

volatile unsigned int time = 0;

//Timer0 Overflow ISR
ISR(TIMER0_OVF_vect)
{
    time++;
    TCNT0 = 256 - (F_CPU / 1000 / 64);
}

//Initialize IO Port
void init_io_port()
{
    DDR_LED |= 0xF0;
    PORT_LED = 0xFF;
    DDRC &= (0 << PORT6);
    DDRD &= (0 << PORT7) | (0 << PORT6) | (0 << PORT4);
    DDR_GOT_IT_LED |= (1 << PORT1);
    PORT_GOT_IT_LED |= (1 << PORT1);
}

//Initialize Timer/Counter0
void init_timer()
{
    //Waveform Generation Mode : Normal mode, Clock : F_CPU/64
    TCCR0A = (0 << WGM01) | (0 << WGM00);
    TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00);
    //1ms = (1/F_CPU) * 64 * n
    //TCNT0 = 256 - n
    TCNT0 = 256 - (F_CPU / 1000 / 64);
    //Timer/Counter0 Overflow Interrupt Enable
    TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
}

//delay for m_sec(ms)
void delay_ms(unsigned int m_sec)
{
    time = 0;
    while(!(time >= m_sec));
}

//function of checking whether switch of the led pressed
unsigned char check_caught_mole(unsigned int now_led)
{
    unsigned char sw[4] = {0x40, 0x10, 0x40, 0x80};
    //0번 LED가 켜져있으면
    if(now_led == 0)
    {
        //해당 스위치가 눌리면
	    if(~PIN_MOLE_SW0 & sw[now_led])
            return true;
        else
            return false;
    }
    //0번 이외에 1번, 2번, 3번 LED 중 하나가 켜져있으면
    else
    {
        //해당 스위치가 눌리면
        if(~PIN_MOLE_SW_ELSE & sw[now_led])
            return true;
        else
            return false;
    }
}

//random led output function
void led_random()
{
    //for led random, PORT_LED = ~0x80 or ~0x40 or ~0x20 or ~0x10
    //2 power n(n = 0~3) will be 0x08 or 0x04 or 0x02 or 0x01
    unsigned int led_status = 1, now_led, i;
    unsigned int random = rand()%4;
    //2 power random
    for(i = 0; i < random; i++)
        led_status *= 2;
    now_led = i;
    PORT_LED = ~led_status << 4;
    for(i = 0; i < 1000; i++)
    {
        //지금 켜진 LED에 해당하는 스위치가 눌리면 break
        if(check_caught_mole(now_led))
        {
            //GOT_IT LED ON
            PORT_GOT_IT_LED &= (0 << PORT1);
            delay_ms(100);
            PORT_GOT_IT_LED |= (1 << PORT1);
            break;
        }
        //1ms 동안 대기 및 반복
        delay_ms(1);
    }
    //모든 LED OFF
    PORT_LED = 0xFF;
    delay_ms(300);
}

int main(void)
{
    cli();
    init_io_port();
    init_timer();
    sei();
    while(1)
    {
        led_random();
    }

}
