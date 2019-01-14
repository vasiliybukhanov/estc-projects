#include "main.h"

#define SWITCH_DELAY 1000000
#define COLOR_COUNT 3

void LEDs_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable peripheral clock for LEDs port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Init LEDs */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Buttons_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable peripheral clock for GPIOE port */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

int main(void)
{
  int left_button, right_button;
  /* direction of color toggling and shift counter */
  int i, dir = 0, k = 0;
  /* setting red as the initial color */
  int pin_color = GPIO_Pin_8;

  LEDs_Init();
  /* Turn all the leds off */
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
  Buttons_Init();

  while (1)
  {
    if (dir)
    {
	pin_color = GPIO_Pin_8 << k;
    }
    else
    {
	pin_color = GPIO_Pin_10 >> k;
    }
    GPIO_ToggleBits(GPIOA, pin_color);
    for (i=0; i< SWITCH_DELAY; i++) ;
    
    GPIO_ToggleBits(GPIOA, pin_color);    
    for (i=0; i< SWITCH_DELAY; i++);

    k = (k + 1) % COLOR_COUNT;

    left_button = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0);
    right_button = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1);
    //changing toggle direction
    if (!left_button)
    {
	pin_color = GPIO_Pin_8;
	dir = 0;
	k = 0;
    }
    if (!right_button)
    {
	pin_color = GPIO_Pin_10;
        dir = 1;
	k = 0;
    }
  }
}
