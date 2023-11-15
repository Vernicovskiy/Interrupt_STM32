#include "main.h"

#define TIM_EnableIT_UPDATE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_UIE)
#define TIM_EnableCounter(TIMx) SET_BIT(TIMx->CR1, TIM_CR1_CEN)
#define LED_ON() GPIOA->BSRR = GPIO_BSRR_BS5;
#define LED_OFF() GPIOA->BSRR = GPIO_BSRR_BR5;

uint8_t counter = 0;
volatile uint32_t  SysTick_Count = 0;




	void tim10_init(){
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN; // тактирование
		NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 2);
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); // глоб прер
		TIM10->PSC = 16000;// преддел
		TIM10->ARR = 5000;
		TIM10->EGR |= TIM_EGR_UG; // Принудительно обновляем shadow-регистры
		TIM10->SR &= ~TIM_SR_UIF;

	}

	void led_init(){

		RCC->AHB1ENR	|= RCC_AHB1ENR_GPIOAEN; //RCC on for GPIO C
		GPIOA->MODER &= ~0x00000C00; /* clear pin mode */
		GPIOA->MODER |= GPIO_MODER_MODER5_0; /* set pin to output mode */

		}

	void exti_init(){

		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // тактирование на порт
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		GPIOC->MODER &= ~GPIO_MODER_MODER13;
		GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;
		SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC ;
		EXTI->IMR |= EXTI_IMR_MR13;
		EXTI->FTSR |= EXTI_FTSR_TR13;
		//EXTI->RTSR |= EXTI_RTSR_TR13;
		NVIC_SetPriority(EXTI15_10_IRQn, 1);
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}

	 void  SysTick_Handler(void)
	     {
	    	if (SysTick_Count > 0){
	    		SysTick_Count --;
	    	}
	    	}

	 void delay_ms(int ms)
	    	{
	    		SysTick_Count = ms;
	    		while (SysTick_Count){}
	    	}

	void TIM1_UP_TIM10_IRQHandler(void){
			if(READ_BIT(TIM10->SR, TIM_SR_UIF)){
				SysTick->CTRL &= 0b101; // запретил прерывание системного таймера
				while(1){
					LED_ON();
				}
			}
	}

	void EXTI15_10_IRQHandler(void){
			GPIOA->ODR ^= GPIO_ODR_OD5;
			SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
			delay_ms(1000);
			EXTI->PR |= EXTI_PR_PR13;
		 }


	int main(void){
		uint32_t SystemCoreClock = 16000000;
		tim10_init();
		led_init();
		exti_init();
		SysTick_Config(SystemCoreClock/1000);
		SysTick->CTRL &= 0b101; // запретил прерывание системного таймера

		NVIC_SetPriority(SysTick_IRQn, 0);

		TIM_EnableIT_UPDATE(TIM10);
		TIM_EnableCounter(TIM10);


		while(1){

		}

	}
