![Static Badge](https://img.shields.io/badge/Unic_Lab-green)
![Static Badge](https://img.shields.io/badge/STM32-red)
![GitHub last commit (by committer)](https://img.shields.io/github/last-commit/Vernicovskiy/STM32_TIM)
![GitHub Repo stars](https://img.shields.io/github/stars/Vernicovskiy/STM32_TIM)
![GitHub watchers](https://img.shields.io/github/watchers/Vernicovskiy/STM32_TIM)
![GitHub top language](https://img.shields.io/github/languages/top/Vernicovskiy/STM32_TIM)

# Приоритет прерываний на уровне подгрупп 
 * NUCLEO-F401RE
 * STM32F401RET6U
 * ARM Cortex M4
 * CMSIS
 * STM32 CubeIDE v1.13.2

>Программа демонстрирует как прерывание с одинаковым приоритетом, но с разными подгруппами (субприоритетами) выполняются в очередности друг за другом исходя из того : первое выполнится прерывание с максимальным субприоритетом.

```C
    NVIC_SetPriorityGrouping(4);

    priority1 = NVIC_EncodePriority(4,2,1);
    priority2 = NVIC_EncodePriority(4,2,2);

    NVIC_SetPriority(TIM4_IRQn  , priority1);
    NVIC_SetPriority(TIM3_IRQn, priority2);

    /* Start TIM2 */
    TIM2->CR1 |= TIM_CR1_CEN;

```




В случае одновременного возникновения двух прерываний из одной группы с одинаковыми подгруппами, первым выполнится прерывание с меньшим номером согласно таблице векторов прерываний.