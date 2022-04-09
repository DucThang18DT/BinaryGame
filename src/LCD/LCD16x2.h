#pragma once

#include <Arduino.h>
#include <Port.h>

enum Mode {cmd, data};

/// send a cmd to LCD
void _cmd(char cmd);

/// send a byte to LCD
/// data - byte to send
/// mode - cmd: data is a cmd; data: data is a character
void _send2LCD(char data, Mode mode);

/// Initials LCD display
void LCD_init();

/// Display the letter that passed into this function on LCD.
/// letter - 'A' -> 'Z' or 'a' -> 'z' or '0' -> '9'.
/// posi - row that the letter displayed.
void LCD_showALetter(char letter);

/// Display the string that into this function on LCD.
/// str - string that is displayed.
/// posi - row that the string displayed.
void LCD_showString(const char *str, int posi);

/// clear screen
void LCD_clear();
/// clear line
/// Position::top - clear line 1
/// Position::bottom - clear line 2
//void clear(int);

void LCD_goto(unsigned char x, unsigned char y);