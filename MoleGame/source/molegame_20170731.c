#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>

#define F_CPU 16000000
#define CLOCK_TICK 1000
#define CORRECT_SW 1
#define INCORRECT_SW 2
#define NONE_SW 0
#define DISPLAY_SCORE 1
#define DISPLAY_DIFFICULTY 2
#define DISPLAY_TIME 3
#define DISPLAY_LIFE 4
#define MAX_DIFFICULTY 4
#define DDR_LED DDRF
#define PORT_LED PORTF
#define DDR_MOLE_SW0 DDRC
#define DDR_MOLE_SW_ELSE DDRD
#define PIN_MOLE_SW0 PINC
#define PIN_MOLE_SW_ELSE PIND
#define DDR_GOT_IT_LED DDRF
#define PORT_GOT_IT_LED PORTF
#define DDR_BUZZER DDRF
#define PORT_BUZZER PORTF
#define DDR_TENS_FND DDRD
#define PORT_TENS_FND PORTD
#define DDR_UNITS_FND DDRB
#define PORT_UNITS_FND PORTB

void init_io_port();
void init_timer();
void delay_ms(unsigned int m_sec);
void fnd_display(unsigned char display_mode);
void change_score(unsigned char pressed_sw);
unsigned char check_caught_mole(unsigned int now_led);
unsigned char led_random_output();
void all_led_off();

volatile unsigned int time = 0;
volatile unsigned char difficulty = 0;
volatile unsigned char life = 3;
unsigned char max_score_of_stage[5] = {10, 30, 50, 80};
unsigned char fnd_digit[10] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
unsigned char score_of_stage[5] = {1, 2, 3, 4, 5};
unsigned int max_delay_time[10] = {1000, 800, 600, 400, 200};
unsigned int score = 0;

//Timer0 Overflow ISR
ISR(TIMER0_OVF_vect)
{
    time++;
    TCNT0 = 256 - (F_CPU / CLOCK_TICK / 64);
}

//INT6(Difficulty Increasing Button) ISR
ISR(INT6_vect)
{
    cli();
    //for prevent chattering
    EIMSK &= (0 << INT6);
    //for enable Timer0 Overflow Interrupt
    sei();
    //Increase difficulty
    difficulty = ++difficulty >= MAX_DIFFICULTY + 1 ? MAX_DIFFICULTY : difficulty;
    //increase current life
    life += 2;
    //Mole Led All On
    PORT_LED &= ~0xF0;
    //display now difficulty
    fnd_display(DISPLAY_DIFFICULTY);
    //Buzzer and Mole LED All On
    for(int i = 0; i < 250; i++)
    {
      PORT_BUZZER |= ((1 << PORT0) | 0x00);
      delay_ms(1);
      PORT_BUZZER &= ((0 << PORT0) | 0xFE);
      delay_ms(1);
    }
    //display now score again
    fnd_display(DISPLAY_SCORE);
    //Mole Led All OFF
    PORT_LED |= 0xF0;
    //enable INT6 again if only difficulty is not max difficulty
    if(difficulty != MAX_DIFFICULTY) EIMSK |= (1 << INT6);
}

//Initialize IO Port
void init_io_port()
{
    DDR_LED |= (1 << PORT7) | (1 << PORT6) | (1 << PORT5) | (1 << PORT4);
    PORT_LED |= (1 << PORT7) | (1 << PORT6) | (1 << PORT5) | (1 << PORT4);
    DDR_MOLE_SW0 &= (0 << PORT6);
    DDR_MOLE_SW_ELSE &= (0 << PORT7) | (0 << PORT6) | (0 << PORT4);
    DDR_GOT_IT_LED |= (1 << PORT1);
    PORT_GOT_IT_LED |= (1 << PORT1);
    DDR_BUZZER |= (1 << PORT0);
    PORT_BUZZER &= (0 << PORT0);
    DDR_TENS_FND |= (1 << PORT3) | (1 << PORT2) | (1 << PORT1) | (1 << PORT0);
    PORT_TENS_FND |= (1 << PORT3) | (1 << PORT2) | (1 << PORT1) | (1 << PORT0);
    DDR_UNITS_FND |= (1 << PORT7) | (1 << PORT6) | (1 << PORT5) | (1 << PORT4);
    PORT_UNITS_FND |= (1 << PORT7) | (1 << PORT6) | (1 << PORT5) | (1 << PORT4);
}

//Initialize Timer/Counter0
void init_timer()
{
    //Waveform Generation Mode : Normal mode, Clock : F_CPU/64
    TCCR0A = (0 << WGM01) | (0 << WGM00);
    TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00);
    //CLOCK_TICK = (1/F_CPU) * 64 * n
    //TCNT0 = 256 - n
    TCNT0 = 256 - (F_CPU / CLOCK_TICK / 64);
    //Timer/Counter0 Overflow Interrupt Enable
    TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
}

//Initialize Externel Interrupt
void init_externel_interrupt()
{
    //Use INT6(PE6)
    //Interrupt request occur at the falling edge
    EICRB = (1 << ISC61) | (0 << ISC60);
    //enable INT6
    EIMSK |= (1 << INT6);
}

//delay for m_sec(ms)
void delay_ms(unsigned int m_sec)
{
    time = 0;
    while(!(time >= m_sec));
}

void buzzer_output(unsigned int time)
{
  int i;
  //buzzer output
  for(i = 0; i < time/2; i++)
  {
      //LED using PORTF4~7
      PORT_BUZZER |= ((1 << PORT0) | 0x00);
      delay_ms(1);
      PORT_BUZZER &= ((0 << PORT0) | 0xFE);
      delay_ms(1);
  }
}
//display now score, life, time function
void fnd_display(unsigned char display_mode)
{
    unsigned int display_value;

    switch(display_mode)
    {
        case DISPLAY_SCORE:
            display_value = score > 99 ? 99 : score;
            break;
        case DISPLAY_LIFE:
            display_value = life;
            break;
        case DISPLAY_DIFFICULTY:
            //difficulty start value is 0 so plus 1
            display_value = difficulty + 1;
            break;

    }
    //for bit masking
    //TENS_FND uses PORTD 0~3, UNITS_FND uses PORTB4~7
    PORT_TENS_FND &= ((fnd_digit[display_value/10]) | 0xF0);
    PORT_TENS_FND |= fnd_digit[display_value/10];
    PORT_UNITS_FND |= fnd_digit[display_value%10] << 4;
    PORT_UNITS_FND &= (fnd_digit[display_value%10] << 4) | 0x0F;
}

//changing current score function
void change_score(unsigned char pressed_sw)
{
    //눌린 스위치 판별
    switch(pressed_sw)
    {
        //if correct switch was pressed
        case CORRECT_SW :
            //GOT_IT LED ON
            PORT_GOT_IT_LED &= ((0 << PORT1) | 0xFD);
            buzzer_output(100);
            //GOT IT LED OFF
            PORT_GOT_IT_LED |= ((1 << PORT1) | ~0xFD);
            //Increase current score
            score += score_of_stage[difficulty];
            fnd_display(DISPLAY_SCORE);
            break;
        //if incorrect swtich was pressed
        case INCORRECT_SW:
            //The End
            if(life == 1)
            {
              fnd_display(DISPLAY_SCORE);
              buzzer_output(100);
              show_gameover();
            }
            //decrease now life
            else
            {
              life -= 1;
              //Mole LED All On
              PORT_LED &= ~0xF0;
              fnd_display(DISPLAY_LIFE);
              buzzer_output(100);
              PORT_LED |= 0xF0;
            }
            break;
        default:
            break;
    }
}

//function of checking whether switch of the led pressed
unsigned char check_caught_mole(unsigned int now_led)
{
    unsigned char sw[4] = {0x40, 0x10, 0x40, 0x80};
    unsigned char sw_judge;
    //0번 LED가 켜져있으면
    if(now_led == 0)
    {
        //해당 스위치가 눌리면
        if(~PIN_MOLE_SW0 & sw[now_led])
            sw_judge =  CORRECT_SW;
        //해당 스위치가 아닌 다른 스위치가 눌리면
        else if((PIN_MOLE_SW_ELSE & 0xD0) != 0xD0)
            sw_judge = INCORRECT_SW;
        else
            sw_judge =  NONE_SW;
    }
    //0번 이외에 1번, 2번, 3번 LED 중 하나가 켜져있으면
    else
    {
        //해당 스위치가 눌리면
        if((~PIN_MOLE_SW_ELSE & sw[now_led]) == sw[now_led])
            sw_judge = CORRECT_SW;
        //해당 스위치가 아닌 다른 스위치(0번 스위치 포함)가 눌리면
        else if((~PIN_MOLE_SW0 & sw[0]) || (PIN_MOLE_SW_ELSE & 0xD0) != 0xD0)
            sw_judge =  INCORRECT_SW;
        else
            sw_judge =  NONE_SW;
    }
    return sw_judge;
}

//random led output function
unsigned char led_random_output()
{
    //for led random, PORT_LED = ~0x80 or ~0x40 or ~0x20 or ~0x10
    //2 power n(n = 0~3) will be 0x08 or 0x04 or 0x02 or 0x01
    unsigned char led_status = 1, now_led, i;
    unsigned int random = rand()%4;
    //2 power random
    for(i = 0; i < random; i++)
        led_status *= 2;
    now_led = i;
    PORT_LED = (~led_status << 4) | 0x0F;
    return now_led;
}

//all of led off function
void all_led_off()
{
  PORT_LED = 0xFF;
  delay_ms(200);
}

void show_gameover()
{
  unsigned char led_status = 0x8F, i;
  //All Mole Led On
  PORT_LED &= ~0xF0;
  //순차 점멸
  for(i = 0; i < 4; i++)
  {
    delay_ms(900);
    PORT_LED = led_status;
    led_status = (led_status >> 1) | 0x8F;
    buzzer_output(100);
  }
  delay_ms(900);
  led_status = 0x0F;
  //반전 점멸
  for(i = 0; i < 4; i++)
  {
    PORT_LED = led_status;
    PORT_GOT_IT_LED |= (1 << PORT1);
    led_status = ~led_status;
    buzzer_output(100);
    delay_ms(400);
  }
  //계속 물결 점등
  while(1)
  {
    led_status = 0x7F;
    for(i = 0; i < 4; i++)
    {
      PORT_LED = led_status;
      led_status = led_status >> 1;
      delay_ms(200);
    }
    for(i = 0; i < 4; i++)
    {
      led_status = led_status << 1 | 0x01;
      PORT_LED = led_status;
      delay_ms(200);
    }
  }
}

int main(void)
{
    unsigned char pressed_sw, now_led;
    unsigned int i;
    cli();
    init_io_port();
    init_timer();
    init_externel_interrupt();
    sei();
    while(1)
    {
        //led random output
        now_led = led_random_output();
        //두더지 포획 체킹하는 반복문
        for(i = 0; i < max_delay_time[difficulty]; i++)
        {
            //눌린 스위치 판별
            pressed_sw = check_caught_mole(now_led);
            change_score(pressed_sw);
            //아무 스위치나 눌렸으면 바로 다음 두더지 소환
            if(pressed_sw == CORRECT_SW || pressed_sw == INCORRECT_SW) break;
            //1ms 동안 대기 및 반복
            delay_ms(1);
        }
        //모든 LED OFF
        all_led_off();
    }

}
