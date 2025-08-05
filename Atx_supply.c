/*
 * File:   Atx_supply.c
 * Author: Its Me
 *
 * Created on May 21, 2025, 11:36 AM
 */


#include <xc.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define STATUS_PIN 5
#define PS_ON 4
#define BUZZER_PIN 0
#define GREEN_LED_SCROLL 2
#define RED_LED_SCROLL 1
#define GREEN_LED_SELECT 3
#define RED_LED_SELECT 2
#define BUTTON_SCROLL_PIN 3
#define BUTTON_SELECT_PIN 4
#define OUTPUT_12V_PIN 2
#define OUTPUT_5V_PIN 1
#define OUTPUT_3V3_PIN 0
#define LONG_PRESS_SEC 5
#define PS_ON 4
unsigned volatile long sec_count = 0;
void timer_init(void);
void buzzer_sound(void);
unsigned long sec(void);
void status_update(void);
void led_srl_update(void);
void button_logic(void);
void update_outputs(void);
void blinking(int);
void init_io(void);
void enter_standby();
void start_buzzer();
void fancy_beeps();
typedef enum { ON, OFF,STANDBY } SystemState;
typedef enum { GREEN, RED, ORANGE } states;
states STATE = RED;
SystemState system_state = OFF;
uint8_t red_scroll_on = 0;
uint8_t green_scroll_on = 0;
uint8_t orange_scroll_on = 0;
uint8_t red_scroll_backup = 0;
uint8_t green_scroll_backup = 0;
uint8_t orange_scroll_backup = 0;
uint8_t button_sel_press = 0, button_srl_press = 0;
unsigned long button_sel_time = 0, button_srl_time = 0;
unsigned long standby_enter_time,standby_in_time;
uint8_t in_standby =0 ; 
int blinking_1 = 0;
int long_press_time = 4;
void init_io(void) {
    DDRB |= (1 << GREEN_LED_SCROLL) | (1 << RED_LED_SCROLL) | (1 << BUZZER_PIN);
    DDRC |= ((1 << GREEN_LED_SELECT) | (1 << RED_LED_SELECT) | (1<<PS_ON));
    DDRD |= (1 << OUTPUT_12V_PIN) | (1 << OUTPUT_5V_PIN) | (1 << OUTPUT_3V3_PIN);
    DDRD &= ~((1 << BUTTON_SCROLL_PIN) | (1 << BUTTON_SELECT_PIN));
}
unsigned long sec(void) {
    return sec_count;
}
void status_update(void) {
    uint8_t active = 0;
    switch (STATE) {
        case GREEN: active = green_scroll_on; break;
        case RED: active = red_scroll_on; break;
        case ORANGE: active = orange_scroll_on; break;
    }
    if (active && system_state == ON) {
        PORTC |= (1 << GREEN_LED_SELECT);
        PORTC &= ~(1 << RED_LED_SELECT);
    } else {
        PORTC |= (1 << RED_LED_SELECT);
        PORTC &= ~(1 << GREEN_LED_SELECT);
    }
}
void led_srl_update(void) {
    PORTB &= ~((1 << GREEN_LED_SCROLL) | (1 << RED_LED_SCROLL));
    switch (STATE) {
        case GREEN: PORTB |= (1 << GREEN_LED_SCROLL); break;
        case RED: PORTB |= (1 << RED_LED_SCROLL); break;
        case ORANGE: PORTB |= (1 << GREEN_LED_SCROLL) | (1 << RED_LED_SCROLL); break;
    }
}
void update_outputs(void) {
    if ((red_scroll_on)&&(system_state == ON)) {
        PORTD |= (1 << OUTPUT_12V_PIN);
    }
    else{ 
        PORTD &= ~(1 << OUTPUT_12V_PIN);
    }
    if ((green_scroll_on)&& (system_state == ON)){
        PORTD |= (1 << OUTPUT_5V_PIN);
    }
    else{
        PORTD &= ~(1 << OUTPUT_5V_PIN);
    }
    if ((orange_scroll_on)&&(system_state == ON)){
        PORTD |= (1 << OUTPUT_3V3_PIN);
    }
    else{
        PORTD &= ~(1 << OUTPUT_3V3_PIN);
    }
}
void buzzer_sound(void) {
    PORTB |= (1 << BUZZER_PIN);
    _delay_ms(200);
    PORTB &= ~(1 << BUZZER_PIN);
}
void timer_init(void) {
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 7600;
    TIMSK1 |= (1 << OCIE1A);
    sei();
}
ISR(TIMER1_COMPA_vect) {
    sec_count++;
}
void start_buzzer(void){
PORTB |= (1 << BUZZER_PIN);
    _delay_ms(400);
    PORTB &= ~(1 << BUZZER_PIN);
_delay_ms(400);
PORTB |= (1 << BUZZER_PIN);
    _delay_ms(400);
    PORTB &= ~(1 << BUZZER_PIN);
_delay_ms(400);

}
void blinking(int count) {
    if (blinking_1) {
        for (int i = 0; i < count; i++) {
            PORTC = (1 << GREEN_LED_SELECT);
            PORTB = (1 << GREEN_LED_SCROLL);
            buzzer_sound();
            _delay_ms(1000);
            PORTC = (1<<RED_LED_SELECT);
            PORTB = (1<<RED_LED_SCROLL);
            buzzer_sound();
            _delay_ms(1000);
        }
        blinking_1 = 0;
    }
}
void button_logic(void) {
    unsigned long prst_time = sec();
    static uint8_t long_press_executed = 0;
    static uint8_t long_press_detected = 0;
    if (system_state == STANDBY) {
        if (button_srl_press || button_sel_press) {
            system_state = OFF;
            STATE = RED;
            PORTC |= (1 << PS_ON);
            in_standby = 0;
            status_update();
            led_srl_update();
            update_outputs();
        }
    }

    if (!(PIND & (1 << BUTTON_SCROLL_PIN))) {
        if (!button_srl_press && !long_press_detected) {
            button_srl_press = 1;
            button_srl_time = prst_time;
        }
    } else if (button_srl_press) {
        if ((prst_time - button_srl_time < long_press_time) &&
            !button_sel_press &&
            !long_press_detected) {
            switch (STATE) {
                case RED: STATE = GREEN; break;
                case GREEN: STATE = ORANGE; break;
                case ORANGE: STATE = RED; break;
            }
            led_srl_update();
            status_update();
            buzzer_sound();
        }
        button_srl_press = 0;
    }

 
    if (!(PIND & (1 << BUTTON_SELECT_PIN))) {
        if (!button_sel_press && !long_press_detected) {
            button_sel_press = 1;
            button_sel_time = prst_time;
        }
    } else if (button_sel_press) {
        if ((prst_time - button_sel_time < long_press_time) &&
            !button_srl_press &&
            !long_press_detected) {
            switch (STATE) {
                case GREEN: green_scroll_on = !green_scroll_on; break;
                case RED: red_scroll_on = !red_scroll_on; break;
                case ORANGE: orange_scroll_on = !orange_scroll_on; break;
            }

            if (green_scroll_on || red_scroll_on || orange_scroll_on) {
                system_state = ON;
            }

            led_srl_update();
            status_update();
            update_outputs();
            buzzer_sound();
        }
        button_sel_press = 0;
    }

    
    if (button_sel_press && button_srl_press &&
        (prst_time - button_srl_time >= long_press_time) &&
        (prst_time - button_sel_time >= long_press_time) &&
        !long_press_executed) {

        long_press_executed = 1;
        long_press_detected = 1;

        blinking_1 = 1;
        enter_standby();

        button_srl_press = 0;
        button_sel_press = 0;
        button_srl_time = 0;
        button_sel_time = 0;

        for (int i = 0; i < 6; i++) {
            buzzer_sound();
        }
    }

 
    if ((PIND & (1 << BUTTON_SCROLL_PIN)) && (PIND & (1 << BUTTON_SELECT_PIN))) {
        long_press_executed = 0;
        long_press_detected = 0;
    }
}
void fancy_beeps(void){
    for(int i=0 ; i<4 ; i++){
        PORTB |= (1<<GREEN_LED_SCROLL);
        PORTC    |= (1<<GREEN_LED_SELECT);
        buzzer_sound();
                _delay_ms(1000);
        PORTB = (1<<RED_LED_SCROLL);
        PORTC = (1<<RED_LED_SELECT);
        buzzer_sound();
    _delay_ms(1000);
    
    }



}
void enter_standby(void){
    standby_enter_time = sec();
    system_state = STANDBY;
    red_scroll_on = 0;
    green_scroll_on = 0;
    orange_scroll_on =0;
    STATE = RED;
    PORTC &= ~(1<<PS_ON);
    status_update();
    led_srl_update();
    update_outputs();
    in_standby=1;
}
void main(void) {
    init_io();
    timer_init();    
    DDRD |= (1<<STATUS_PIN);
    PORTD |= (1<<STATUS_PIN);
    
    start_buzzer();
    enter_standby();
    PORTC |= (1<<PS_ON);
    status_update();
    led_srl_update();
    update_outputs();

    while (1) {
        button_logic();
        blinking(3);
        
        
        if((system_state == STANDBY)&&(in_standby)){
            standby_in_time = sec()- standby_enter_time;
            if((standby_in_time >= 120)&&(standby_in_time%20 == 0)){
                fancy_beeps();
            
            }
        
        }
    }
}



        
        
        


//        led_srl_update();
//        update_outputs();
//        buzzer_sound();
//    }

