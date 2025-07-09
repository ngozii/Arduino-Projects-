# A Traffic light stimulator using LEDs and a single digit 7-segment display
#Hardware: LEDs (red, green, yellow), 7-segment display, buzzer, resistors

from machine import Pin
import time

# Defining Pins
print('Setting up hardware...')

# LEDs
led_green = Pin(21, Pin.OUT)
led_yellow = Pin(20, Pin.OUT)
led_red = Pin(19, Pin.OUT)
buzzer = Pin(18, Pin.OUT)

# 7-segment pins
a = Pin(2, Pin.OUT)
b = Pin(3, Pin.OUT)
c = Pin(4, Pin.OUT)
d = Pin(5, Pin.OUT)
e = Pin(6, Pin.OUT)
f = Pin(7, Pin.OUT)
g = Pin(8, Pin.OUT)


segments = [a, b, c, d, e, f, g]

print('Hardware Setup!')

#Time setup
print('Timing setup')

green_time = 9
yellow_time = 6
red_time = 9
total_cycle = green_time + yellow_time + red_time

current_time = 0
last_tick = time.ticks_ms()

#buzzer control
buzz_counter = 0
buzz_interval = 100


print(f'Timing: Green Light= {green_time}s, Yellow Light = {yellow_time}s, Red Light = {red_time}s')
print(f'Total cycle: {total_cycle} seconds')

#Storing the values of the numbers to be displayed. 0 = off, 1 = on

digit_patterns = {
    0: (1, 1, 1, 1, 1, 1, 0),  # Display "0"
    1: (0, 1, 1, 0, 0, 0, 0),  # Display "1"
    2: (1, 1, 0, 1, 1, 0, 1),  # Display "2"
    3: (1, 1, 1, 1, 0, 0, 1),  # Display "3"
    4: (0, 1, 1, 0, 0, 1, 1),  # Display "4"
    5: (1, 0, 1, 1, 0, 1, 1),  # Display "5"
    6: (1, 0, 1, 1, 1, 1, 1),  # Display "6"
    7: (1, 1, 1, 0, 0, 0, 0),  # Display "7"
    8: (1, 1, 1, 1, 1, 1, 1),  # Display "8"
    9: (1, 1, 1, 1, 0, 1, 1),  # Display "9"
}


def display_digit(digit):
    
    if digit in digit_patterns:
            pattern = digit_patterns[digit]
            
            for i, segment in enumerate(segments):
                segment.value(pattern[i])
                
    else:
        
        for segment in segments:
            segment.value(0)

print('7-segment setup!')


def turn_off_leds():
    #turn off all LEDs and buzzer
    led_green.value(0)
    led_yellow.value(0)
    led_red.value(0)
    buzzer.value(0)
    
    
def get_current_state():
    # Check the state based on current time (green,red, yellow)
    
    if current_time < green_time:
        return 'Green'
    elif current_time < green_time + yellow_time:
        return 'Yellow'
    else:
        return 'Red'
    
def get_remaining_time():
    # Gets time for display on 7-segment
    
    state = get_current_state()
    
    if state == 'Green':
        remaining = green_time - current_time
        
    elif state == 'Yellow':
        remaining = green_time + yellow_time - current_time
    
    else:
        remaining = total_cycle - current_time


    return remaining
    
    
def control_buzzer():
    
#handle buzzer based on current state (off for green, on for red, beeping for yellow)
    global buzz_counter
    
    state = get_current_state()
    
    if state == 'Yellow':
        buzz_counter += 1
        if buzz_counter >= buzz_interval:
            current_buzz = buzzer.value()
            buzzer.value(1 if current_buzz == 0 else 0)
            buzz_counter = 0
            
    elif state == 'Red':
            buzzer.value(1)
            
    else:
            buzzer.value(0)
            
def update_traffic_lights():
    
# Updates the LED states based on current time

    state = get_current_state()
    turn_off_leds()
    
    if state == 'Green':
        led_green.value(1)
        
    elif state == 'Yellow':
        led_yellow.value(1)
    
    elif state == 'Red':
        led_red.value(1)
    
    control_buzzer()
    
print('Traffic Light control ready!')

def update_timer():
    #updates timer every second
    
    global current_time, last_tick, buzz_counter
    
    current_ms = time.ticks_ms()
    
    if time.ticks_diff(current_ms, last_tick) >= 1000:
        current_time += 1
        last_tick = current_ms
        
        #Reset cycle
        if current_time >= total_cycle:
            current_time = 0
            buzz_counter = 0
            
        return True  #check the time was updated
    return False #no update

print('Timer ready!')

try:
    while True:
    
        time_updated = update_timer() 
        update_traffic_lights() #update traffic lights 
        
        remaining = get_remaining_time() #update 7-segment display
        display_digit(remaining)
        
        time.sleep_ms(10)

except KeyboardInterrupt:
    print("\n\nStopping traffic light system...")
    turn_off_leds()
    display_digit(-1)  # Blank display
    print("System stopped!")





