#include "handle.h"

#define CHECK_BIT(variable, index)  \
    (((variable) >> (index)) & 0x01)

#define LOW 0
#define HIGH 1
#define LED_START 0

unsigned char dot_val = 0;

/**
      | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
      ---------------------------------
   0: | 1 | 1 | 1 | 0 | 0 | 1 | 1 | 1 | -> 0xE7
      ---------------------------------
   1: | 1 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | -> 0x21
      ---------------------------------
   2: | 1 | 1 | 0 | 1 | 0 | 0 | 1 | 1 | -> 0xCB
      ---------------------------------
   3: | 1 | 1 | 0 | 1 | 0 | 1 | 1 | 0 | -> 0x6B
      ---------------------------------
   4: | 1 | 0 | 1 | 1 | 0 | 1 | 0 | 0 | -> 0x2D
      ---------------------------------
   5: | 0 | 1 | 1 | 1 | 0 | 1 | 1 | 0 | -> 0x6E
      ---------------------------------
   6: | 0 | 1 | 1 | 1 | 0 | 1 | 1 | 1 | -> 0xEE
      ---------------------------------
   7: | 1 | 1 | 0 | 0 | 0 | 1 | 0 | 0 | -> 0x23
      ---------------------------------
   8: | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 1 | -> 0xEF
      ---------------------------------
   9: | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 0 | -> 0x6F
*/
unsigned char led_status[] = {0xE7, 0x21, 0xCB, 0x6B, 0x2D, 0x6E, 0xEE, 0x23, 0xEF, 0x6F, 0x00};

void display_digit(int digit)
{
    if (digit > 10 || digit < 0)
        return;
    unsigned char bit_map = led_status[digit];
    bit_map += (dot_val) << 4;
    int i = 0;
    for (; i < 8; i ++)
    {
        if (CHECK_BIT(bit_map, i))
        {
          digitalWrite(i + LED_START, LOW);
        } else
        {
          digitalWrite(i + LED_START, HIGH);
        }
    }
    printf("write %d\n", digit);
}

int handle_init()
{
    if (wiringPiSetup() == -1)
        return -1;
    int i = 0;
    for (; i < 8; i++)
    {
        pinMode (i + LED_START, OUTPUT);
    }
    display_digit(8);
}

int handle(char* command)
{
    char c;
    if (command == NULL || strcmp(command, "exit\n") == 0)
    {
        printf("exit\n");
        return 0;
    }
    for (c = *command; c != '\0'; c = *(++command))
    {
        if (c >= '0' && c <= '9')
        {
            display_digit(c - '0');
            delay(500);
        }
    }
    printf("finished displaying\n");
    return 1;
}
