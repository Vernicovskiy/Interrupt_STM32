#include "main.h"

#define TIM_EnableIT_UPDATE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_UIE)
#define TIM_EnableCounter(TIMx) SET_BIT(TIMx->CR1, TIM_CR1_CEN)


	void SystemClock_Config(void)
     {
		// Включаем HSI
		RCC->CR |= ((uint32_t)RCC_CR_HSION);
		while (!(RCC->CR & RCC_CR_HSIRDY));

		// Настраиваем Flash prefetch, Instruction cache, Data cache и wait state
		FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

		// Настраиваем источник частоты и множитель PLL
		RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLM_4 | (336 << RCC_PLLCFGR_PLLN_Pos) | (((RCC_PLLCFGR_PLLP_0 | RCC_PLLCFGR_PLLP_1) >> 16) << RCC_PLLCFGR_PLLP_Pos));
		//RCC->PLLCFGR = (RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLM_3 | (84 << RCC_PLLCFGR_PLLN_Pos));
		// Включаем PLL
		RCC->CR |= RCC_CR_PLLON;
		while(!(RCC->CR & RCC_CR_PLLRDY));

		// Настраиваем делители шин AHB, APB1 и APB2
		RCC->CFGR |= (RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV1 | RCC_CFGR_PPRE2_DIV1);

		// Настраиваем источник системных часов
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
     }


// Настройка таймера TIM1 в режиме PWM
	void TIM1_PWM_Init(void) {
		// Включение тактирования TIM1
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		// Настройка вывода PA8 в режим альтернативной функции AF1 (TIM1_CH1)
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		GPIOA->MODER &= ~GPIO_MODER_MODER8;
		GPIOA->MODER |= GPIO_MODER_MODER8_1;
		GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8_0 | GPIO_OSPEEDR_OSPEED8_1 );
		GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_0; /////////////////////////
		// Установка предделителя таймера TIM1
		TIM1->PSC = 0;
		// Установка периода таймера TIM1
		TIM1->ARR = 40-1;
		// Установка коэффициента заполнения таймера TIM1
		TIM1->CCR1 = 20-1;
		// Включение канала 1 таймера TIM1 в режиме PWM
		TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Output Compare 1 mode 110: PWM mode 1 - In upcounting
		TIM1->CCER |= TIM_CCER_CC1E; // Capture/Compare 1 output enable
		// Включение таймера TIM1
		TIM1->CR1 |= TIM_CR1_CEN;
		TIM1->BDTR |= TIM_BDTR_MOE;
	}

	void tim10_init(){
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN; // тактирование
		NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 1);
		NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn); // глоб прер
		TIM10->PSC = 16000-1;// преддел
		TIM10->ARR = 1000-1;
		TIM10->EGR |= TIM_EGR_UG; // Принудительно обновляем shadow-регистры

	}

	void exti_init(){

		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // тактирование на порт
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_1;
		SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC ;
		EXTI->IMR |= EXTI_IMR_MR13;
		EXTI->FTSR |= EXTI_FTSR_TR13;
		//EXTI->RTSR |= EXTI_RTSR_TR13;
		NVIC_SetPriority(EXTI15_10_IRQn, 2);
	}

// Переменная для хранения частоты
	volatile uint32_t frequency = 0;

	void TIM1_UP_TIM10_IRQHandler(void){
		if(READ_BIT(TIM10->SR, TIM_SR_UIF)){
			NVIC_DisableIRQ(EXTI15_10_IRQn);
			CLEAR_BIT(TIM10->CR1, TIM_CR1_CEN);
			CLEAR_BIT(TIM10->SR,TIM_SR_UIF);

			}
		}


	void EXTI15_10_IRQHandler(void){


		frequency++;
		EXTI->PR |= EXTI_PR_PR13;
		 }


	int main(void) {
		DWT->CTRL |= 1;
		//SystemClock_Config();
		TIM1_PWM_Init();
		//tim10_init();
		exti_init();
		//TIM_EnableIT_UPDATE(TIM10);
		CLEAR_BIT(TIM10->SR,TIM_SR_UIF);
		TIM_EnableCounter(TIM10);
		DWT->CYCCNT = 0;
		NVIC_EnableIRQ(EXTI15_10_IRQn);


  while (1) {
  }
}

