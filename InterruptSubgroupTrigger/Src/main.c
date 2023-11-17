#include "main.h"



uint32_t  priority1 = 0;
uint32_t  priority2 = 0;

void led_init(){

		RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOAEN; //RCC on for GPIO C
		GPIOA->MODER &= ~0x00000C00; /* clear pin mode */
		GPIOA->MODER |= GPIO_MODER_MODER5_0; /* set pin to output mode */

		}



void TIM2_init(void)
{
    /* Enable clock for TIM2 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* Initialize TIM2 */
    TIM2->PSC = 160000 - 1; // Prescaler
    TIM2->ARR = 100 - 1; // Auto-reload value

    /* Force update generation to update registers */
    TIM2->EGR |= TIM_EGR_UG;

    /* Clear update interrupt flag */
    TIM2->SR &= ~TIM_SR_UIF;

    /* Enable TIM2 global interrupt */
    NVIC_EnableIRQ(TIM2_IRQn);

    /* Configure TIM2 as master timer */
    TIM2->CR2 |= TIM_CR2_MMS_1; // Update event is selected as trigger output (TRGO)
}

void TIM3_init(void)
{
    /* Enable clock for TIM3 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    /* Initialize TIM3 */
    TIM3->PSC = 160000 - 1; // Prescaler
    TIM3->ARR = 100 - 1; // Auto-reload value

    /* Clear update interrupt flag */
    TIM3->SR &= ~TIM_SR_UIF;

    TIM3->DIER |= TIM_DIER_UIE;


    /* Enable TIM3 global interrupt */
    NVIC_EnableIRQ(TIM3_IRQn);

    /* Configure TIM3 as slave timer */
    TIM3->SMCR |= TIM_SMCR_TS_0; // ITR0 (TIM2) is selected as trigger input
    TIM3->SMCR |= TIM_SMCR_SMS_2; // Trigger mode is selected
}

void TIM4_init(void)
{
    /* Enable clock for TIM4 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    /* Initialize TIM4 */
    TIM4->PSC = 160000 - 1; // Prescaler
    TIM4->ARR = 100 - 1; // Auto-reload value

    TIM4->DIER |= TIM_DIER_UIE;


    /* Clear update interrupt flag */
    TIM4->SR &= ~TIM_SR_UIF;

    /* Enable TIM4 global interrupt */
    NVIC_EnableIRQ(TIM4_IRQn);

    /* Configure TIM4 as slave timer */
    TIM4->SMCR |= TIM_SMCR_TS_0; // ITR0 (TIM2) is selected as trigger input
    TIM4->SMCR |= TIM_SMCR_SMS_2; // Trigger mode is selected
}

void TIM4_IRQHandler(void){
	if(READ_BIT(TIM4->SR, TIM_SR_UIF)){
		GPIOA->BSRR = GPIO_BSRR_BS5;
		TIM4->SR &= ~TIM_SR_UIF;

	}

}

void TIM3_IRQHandler(void){
	if(READ_BIT(TIM3->SR, TIM_SR_UIF)){
		GPIOA->ODR ^= GPIO_ODR_OD5;
		TIM3->SR &= ~TIM_SR_UIF;

	}

}


int main(void)
{
    /* Initialize timers */
    TIM2_init();
    TIM3_init();
    TIM4_init();
    led_init();
    NVIC_SetPriorityGrouping(4);
    priority1 = NVIC_EncodePriority(4,2,1);
    priority2 = NVIC_EncodePriority(4,2,2);
    NVIC_SetPriority(TIM4_IRQn  , priority2);
    NVIC_SetPriority(TIM3_IRQn, priority1);

    /* Start TIM2 */
    TIM2->CR1 |= TIM_CR1_CEN;

    while (1)
    {
    }
}




