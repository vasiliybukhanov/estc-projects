#include "main.h"

#define COLOR_COUNT 3

/* global LED-switching vars */
static int k, dir, pin_color, prev_color;

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
  
  /* Init Buttons */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void EXTI_ConfLine0(void)
{
  /* Enable clocl for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);

  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);

  /* setting nvic to work with the timer */
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = EXTI0_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0x00;
  nvic_struct.NVIC_IRQChannelSubPriority = 0x01;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct); 
}

void EXTI_ConfLine1(void)
{
  /* Enable clocl for SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

  EXTI_InitTypeDef EXTI_InitStruct;
  EXTI_InitStruct.EXTI_Line = EXTI_Line1;
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_Init(&EXTI_InitStruct);

  /* setting nvic to work with the timer */
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = EXTI1_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0x00;
  nvic_struct.NVIC_IRQChannelSubPriority = 0x01;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct); 
}

void Timer_Init(void)
{
  /* Init timer */  	
  TIM_TimeBaseInitTypeDef tim_struct;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  tim_struct.TIM_Period = 1000;
  tim_struct.TIM_Prescaler = 42000;
  tim_struct.TIM_ClockDivision = 0;
  tim_struct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &tim_struct);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);

  /* setting nvic to work with the timer */
  NVIC_InitTypeDef nvic_struct;
  nvic_struct.NVIC_IRQChannel = TIM2_IRQn;
  nvic_struct.NVIC_IRQChannelPreemptionPriority = 0;
  nvic_struct.NVIC_IRQChannelSubPriority = 1;
  nvic_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic_struct); 
}

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) !=RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    /* turn off the old color and turn on the new one */
    GPIO_ToggleBits(GPIOA, prev_color);
    GPIO_ToggleBits(GPIOA, pin_color);
    /* memorizing the old color to turn it off properly */
    prev_color = pin_color;
    if (dir)
    {
	pin_color = GPIO_Pin_8 << k;
    }
    else
    {
	pin_color = GPIO_Pin_10 >> k;
    }
    k = (k + 1) % COLOR_COUNT;    
  }
}

void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    pin_color = GPIO_Pin_8;
    k = 0; 
    dir = 0;
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void EXTI1_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line1) != RESET)
  {
    pin_color = GPIO_Pin_10;
    k = 0; 
    dir = 1;
    EXTI_ClearITPendingBit(EXTI_Line1);
  }
}


int main(void)
{
  k = 0; dir = 0; 
  /* setting red as the initial color */
  pin_color = GPIO_Pin_10;
  /* additional variable for correct color toggling */
  prev_color = pin_color;
  
  LEDs_Init();
  /* Turn all the leds off */
  GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
  /* Turn blue on */
  GPIO_ResetBits(GPIOA, GPIO_Pin_10);

  Buttons_Init();
  Timer_Init();
  EXTI_ConfLine0();
  EXTI_ConfLine1();

  while (1)
  { }
}
