#include "LCD16x2.h"

void _send2LCD(char data, Mode mode)
{
    shiftOut(HC_DS_PIN, HC_SH_PIN, MSBFIRST, data); //MSB
    delay(10);
    digitalWrite(HC_ST_PIN, HIGH);
    digitalWrite(HC_ST_PIN, LOW);
    digitalWrite(LCD_RS_PIN, mode == Mode::cmd?LOW:HIGH);
    digitalWrite(LCD_E_PIN, LOW);
    digitalWrite(LCD_E_PIN, HIGH);
    delay(2);
}

void _cmd(char cmd)
{
    _send2LCD(cmd, Mode::cmd);
}

void LCD_showALetter(char letter)
{
    _send2LCD(letter, Mode::data);
}

void LCD_showString(const char *str, int posi)
{
    if (posi == 1) // Position::bottom
        _send2LCD(0xC0, Mode::cmd);  // Mode:: cmd
    else _send2LCD(0x80, Mode::cmd); // Mode:: cmd
    int count = 0;
    while (str[count] != '\0')
    {
        LCD_showALetter(str[count]);
        ++count;
    }
}

void LCD_clear()
{
    _send2LCD(0x01, Mode::cmd); // Mode::cmd
}

void LCD_init()
{
    for (int i = 0; i < 4; i++)
    {
        _send2LCD(0x38, Mode::cmd); // Mode::cmd
        delay(5);
    }
    _send2LCD(0x0C, Mode::cmd); // Mode::cmd
    LCD_clear();
}

void LCD_goto(unsigned char x, unsigned char y)
{   
    if (x == 0x00) // line 1
        _send2LCD(0x80 + y, Mode::cmd);
    else _send2LCD(0xC0 + y, Mode::cmd); // line 2
}