﻿/**
  @page PWR_EnterStopMode PWR standby example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_EnterStopMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR STOP 0 mode example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to enter the STOP 0 mode.

After start-up LED4 is toggling during 5 seconds, then the system automatically 
enter in STOP 0 mode (Final state).

LED4 is used to monitor the system state as follows:
 - LED4 toggling : system in Run mode
 - LED4 off : system in STOP 0 mode

This example does not implement a wake-up source from any peripheral: to wake-up the device,
press on Reset button.

@note To measure the current consumption in STOP0 mode, remove JP3 jumper 
      and connect an ampere meter to JP3 to measure IDD current.

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M0+ core is no longer clocked during low power mode 
      so debugging features are disabled.

@par Keywords

Power, PWR, Stop mode, Interrupt, Low Power

@par Directory contents 

  - PWR/PWR_EnterStopMode/Inc/stm32g0xx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStopMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_EnterStopMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_EnterStopMode/Src/stm32g0xx_it.c          Interrupt handlers
  - PWR/PWR_EnterStopMode/Src/main.c                  Main program
  - PWR/PWR_EnterStopMode/Src/system_stm32g0xx.c      STM32G0xx system source file

@par Hardware and Software environment

  - This example runs on STM32G0B1RETx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-G0B1RE
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-G0B1RE Set-up
    - LED4 connected to PA.05 pin

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 */
