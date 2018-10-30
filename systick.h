//****************************************************************************** 
// Имя файла    :  'SysTick.h'
// заголовок    :  Постоянный глобальный таймер
// Автор        :  Маньянов Р.Р.
// Контакты     :  
// Дата         :  13.05.2014 - 15.05.2014 
//******************************************************************************

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <stm32f10x.h>

//==============================================================================
//                      Прототипы функций
//==============================================================================
    
    // Инициализация глобального таймера
    uint8_t SysTickInit (uint16_t frequence);

    // Денициализация глобального таймера
    void SysTickDeinit();

    // Пауза в отсчётах глобального таймера
    void delay_ms(uint16_t delay);
    
    // Микросекундная пауза
    void delay_us(uint32_t delay);
        
    // Получение текущего системного времени
    uint32_t SysTickGet (void);
//    void SysTick_Handler(void);
    
#endif // __SYSTICK_H
   
