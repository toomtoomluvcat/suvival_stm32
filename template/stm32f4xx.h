/*
 * stm32f4xx.h (MINI STUB สำหรับซ้อมคอมไพล์เท่านั้น)
 * ------------------------------------------------------------
 * ไฟล์นี้ไม่ใช่ของจริงจาก ST — เป็นไฟล์ย่อที่ผมทำขึ้นมาเอง
 * ให้พอมี struct/macro ครบสำหรับแบบฝึกหัด GPIO/EXTI/ADC/UART/PWM
 * เพื่อให้ compile เช็ค syntax ได้บนเครื่องนี้ (ไม่มีบอร์ดจริง)
 *
 * ตอนสอบจริง ให้ใช้ stm32f4xx.h ตัวจริงจาก STM32CubeIDE/CMSIS
 * ชื่อ struct/macro ด้านล่างนี้ตั้งตามของจริงเป๊ะ ๆ แล้ว โค้ดที่ผ่านที่นี่
 * เอาไป build กับของจริงได้เลยไม่ต้องแก้อะไร
 * ------------------------------------------------------------
 */
#ifndef STM32F4XX_H
#define STM32F4XX_H

#include <stdint.h>

#define __IO volatile
#define __I  volatile const
#define __O  volatile

/* ================= GPIO ================= */
typedef struct
{
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_TypeDef;

#define GPIOA_BASE   (0x40020000UL)
#define GPIOB_BASE   (0x40020400UL)
#define GPIOC_BASE   (0x40020800UL)
#define GPIOA        ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB        ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC        ((GPIO_TypeDef *)GPIOC_BASE)

/* MODER bit-field macros (เฉพาะพินที่ใช้บ่อยในแบบฝึกหัด) */
#define GPIO_MODER_MODER0        (0x3UL << 0)
#define GPIO_MODER_MODER0_Pos    (0U)
#define GPIO_MODER_MODER1        (0x3UL << 2)
#define GPIO_MODER_MODER1_Pos    (2U)
#define GPIO_MODER_MODER2        (0x3UL << 4)
#define GPIO_MODER_MODER2_Pos    (4U)
#define GPIO_MODER_MODER3        (0x3UL << 6)
#define GPIO_MODER_MODER3_Pos    (6U)
#define GPIO_MODER_MODER4        (0x3UL << 8)
#define GPIO_MODER_MODER4_Pos    (8U)
#define GPIO_MODER_MODER5        (0x3UL << 10)
#define GPIO_MODER_MODER5_Pos    (10U)
#define GPIO_MODER_MODER6        (0x3UL << 12)
#define GPIO_MODER_MODER6_Pos    (12U)
#define GPIO_MODER_MODER7        (0x3UL << 14)
#define GPIO_MODER_MODER7_Pos    (14U)
#define GPIO_MODER_MODER8        (0x3UL << 16)
#define GPIO_MODER_MODER8_Pos    (16U)
#define GPIO_MODER_MODER9        (0x3UL << 18)
#define GPIO_MODER_MODER9_Pos    (18U)
#define GPIO_MODER_MODER10        (0x3UL << 20)
#define GPIO_MODER_MODER10_Pos    (20U)
#define GPIO_MODER_MODER11        (0x3UL << 22)
#define GPIO_MODER_MODER11_Pos    (22U)
#define GPIO_MODER_MODER12        (0x3UL << 24)
#define GPIO_MODER_MODER12_Pos    (24U)
#define GPIO_MODER_MODER13        (0x3UL << 26)
#define GPIO_MODER_MODER13_Pos    (26U)
#define GPIO_MODER_MODER14        (0x3UL << 28)
#define GPIO_MODER_MODER14_Pos    (28U)
#define GPIO_MODER_MODER15        (0x3UL << 30)
#define GPIO_MODER_MODER15_Pos    (30U)

#define GPIO_OTYPER_OT0          (0x1UL << 0)
#define GPIO_OTYPER_OT1          (0x1UL << 1)
#define GPIO_OTYPER_OT2          (0x1UL << 2)
#define GPIO_OTYPER_OT3          (0x1UL << 3)
#define GPIO_OTYPER_OT4          (0x1UL << 4)
#define GPIO_OTYPER_OT5          (0x1UL << 5)
#define GPIO_OTYPER_OT6          (0x1UL << 6)
#define GPIO_OTYPER_OT7          (0x1UL << 7)
#define GPIO_OTYPER_OT8          (0x1UL << 8)
#define GPIO_OTYPER_OT9          (0x1UL << 9)
#define GPIO_OTYPER_OT10          (0x1UL << 10)
#define GPIO_OTYPER_OT11          (0x1UL << 11)
#define GPIO_OTYPER_OT12          (0x1UL << 12)
#define GPIO_OTYPER_OT13          (0x1UL << 13)
#define GPIO_OTYPER_OT14          (0x1UL << 14)
#define GPIO_OTYPER_OT15          (0x1UL << 15)

#define GPIO_OSPEEDR_OSPEED5     (0x3UL << 10)

#define GPIO_PUPDR_PUPD0         (0x3UL << 0)
#define GPIO_PUPDR_PUPD0_Pos     (0U)
#define GPIO_PUPDR_PUPD1         (0x3UL << 2)
#define GPIO_PUPDR_PUPD1_Pos     (2U)
#define GPIO_PUPDR_PUPD2         (0x3UL << 4)
#define GPIO_PUPDR_PUPD2_Pos     (4U)
#define GPIO_PUPDR_PUPD3         (0x3UL << 6)
#define GPIO_PUPDR_PUPD3_Pos     (6U)
#define GPIO_PUPDR_PUPD4         (0x3UL << 8)
#define GPIO_PUPDR_PUPD4_Pos     (8U)
#define GPIO_PUPDR_PUPD5         (0x3UL << 10)
#define GPIO_PUPDR_PUPD5_Pos     (10U)
#define GPIO_PUPDR_PUPD6         (0x3UL << 12)
#define GPIO_PUPDR_PUPD6_Pos     (12U)
#define GPIO_PUPDR_PUPD7         (0x3UL << 14)
#define GPIO_PUPDR_PUPD7_Pos     (14U)
#define GPIO_PUPDR_PUPD8         (0x3UL << 16)
#define GPIO_PUPDR_PUPD8_Pos     (16U)
#define GPIO_PUPDR_PUPD9         (0x3UL << 18)
#define GPIO_PUPDR_PUPD9_Pos     (18U)
#define GPIO_PUPDR_PUPD10         (0x3UL << 20)
#define GPIO_PUPDR_PUPD10_Pos     (20U)
#define GPIO_PUPDR_PUPD11         (0x3UL << 22)
#define GPIO_PUPDR_PUPD11_Pos     (22U)
#define GPIO_PUPDR_PUPD12         (0x3UL << 24)
#define GPIO_PUPDR_PUPD12_Pos     (24U)
#define GPIO_PUPDR_PUPD13         (0x3UL << 26)
#define GPIO_PUPDR_PUPD13_Pos     (26U)
#define GPIO_PUPDR_PUPD14         (0x3UL << 28)
#define GPIO_PUPDR_PUPD14_Pos     (28U)
#define GPIO_PUPDR_PUPD15         (0x3UL << 30)
#define GPIO_PUPDR_PUPD15_Pos     (30U)

#define GPIO_IDR_ID0              (0x1UL << 0)
#define GPIO_IDR_ID1              (0x1UL << 1)
#define GPIO_IDR_ID2              (0x1UL << 2)
#define GPIO_IDR_ID3              (0x1UL << 3)
#define GPIO_IDR_ID4              (0x1UL << 4)
#define GPIO_IDR_ID5              (0x1UL << 5)
#define GPIO_IDR_ID6              (0x1UL << 6)
#define GPIO_IDR_ID7              (0x1UL << 7)
#define GPIO_IDR_ID8              (0x1UL << 8)
#define GPIO_IDR_ID9              (0x1UL << 9)
#define GPIO_IDR_ID10              (0x1UL << 10)
#define GPIO_IDR_ID11              (0x1UL << 11)
#define GPIO_IDR_ID12              (0x1UL << 12)
#define GPIO_IDR_ID13              (0x1UL << 13)
#define GPIO_IDR_ID14              (0x1UL << 14)
#define GPIO_IDR_ID15              (0x1UL << 15)
#define GPIO_ODR_OD0              (0x1UL << 0)
#define GPIO_ODR_OD1              (0x1UL << 1)
#define GPIO_ODR_OD2              (0x1UL << 2)
#define GPIO_ODR_OD3              (0x1UL << 3)
#define GPIO_ODR_OD4              (0x1UL << 4)
#define GPIO_ODR_OD5              (0x1UL << 5)
#define GPIO_ODR_OD6              (0x1UL << 6)
#define GPIO_ODR_OD7              (0x1UL << 7)
#define GPIO_ODR_OD8              (0x1UL << 8)
#define GPIO_ODR_OD9              (0x1UL << 9)
#define GPIO_ODR_OD10              (0x1UL << 10)
#define GPIO_ODR_OD11              (0x1UL << 11)
#define GPIO_ODR_OD12              (0x1UL << 12)
#define GPIO_ODR_OD13              (0x1UL << 13)
#define GPIO_ODR_OD14              (0x1UL << 14)
#define GPIO_ODR_OD15              (0x1UL << 15)

#define GPIO_AFRL_AFRL2           (0xFUL << 8)
#define GPIO_AFRL_AFSEL2_Pos      (8U)
#define GPIO_AFRL_AFRL3           (0xFUL << 12)
#define GPIO_AFRL_AFSEL3_Pos      (12U)

/* ================= RCC ================= */
typedef struct
{
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    uint32_t RESERVED0[2];
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    uint32_t RESERVED2[2];
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
} RCC_TypeDef;

#define RCC_BASE   (0x40023800UL)
#define RCC        ((RCC_TypeDef *)RCC_BASE)

#define RCC_AHB1ENR_GPIOAEN   (0x1UL << 0)
#define RCC_AHB1ENR_GPIOBEN   (0x1UL << 1)
#define RCC_AHB1ENR_GPIOCEN   (0x1UL << 2)
#define RCC_APB2ENR_SYSCFGEN  (0x1UL << 14)
#define RCC_APB2ENR_ADC1EN    (0x1UL << 8)
#define RCC_APB1ENR_USART2EN  (0x1UL << 17)
#define RCC_APB1ENR_TIM3EN    (0x1UL << 1)
#define RCC_APB1ENR_TIM2EN    (0x1UL << 0)

/* ================= EXTI / SYSCFG ================= */
typedef struct
{
    __IO uint32_t IMR;
    __IO uint32_t EMR;
    __IO uint32_t RTSR;
    __IO uint32_t FTSR;
    __IO uint32_t SWIER;
    __IO uint32_t PR;
} EXTI_TypeDef;

#define EXTI_BASE  (0x40013C00UL)
#define EXTI       ((EXTI_TypeDef *)EXTI_BASE)

#define EXTI_IMR_MR0     (0x1UL << 0)
#define EXTI_IMR_MR1     (0x1UL << 1)
#define EXTI_IMR_MR2     (0x1UL << 2)
#define EXTI_IMR_MR3     (0x1UL << 3)
#define EXTI_IMR_MR4     (0x1UL << 4)
#define EXTI_IMR_MR5     (0x1UL << 5)
#define EXTI_IMR_MR6     (0x1UL << 6)
#define EXTI_IMR_MR7     (0x1UL << 7)
#define EXTI_IMR_MR8     (0x1UL << 8)
#define EXTI_IMR_MR9     (0x1UL << 9)
#define EXTI_IMR_MR10    (0x1UL << 10)
#define EXTI_IMR_MR15    (0x1UL << 15)

#define EXTI_FTSR_TR0    (0x1UL << 0)
#define EXTI_FTSR_TR3    (0x1UL << 3)
#define EXTI_FTSR_TR4    (0x1UL << 4)
#define EXTI_FTSR_TR5    (0x1UL << 5)
#define EXTI_FTSR_TR7    (0x1UL << 7)
#define EXTI_FTSR_TR10   (0x1UL << 10)

#define EXTI_PR_PR0      (0x1UL << 0)
#define EXTI_PR_PR3      (0x1UL << 3)
#define EXTI_PR_PR4      (0x1UL << 4)
#define EXTI_PR_PR5      (0x1UL << 5)
#define EXTI_PR_PR7      (0x1UL << 7)
#define EXTI_PR_PR10     (0x1UL << 10)

typedef struct
{
    __IO uint32_t MEMRMP;
    __IO uint32_t PMC;
    __IO uint32_t EXTICR[4];
} SYSCFG_TypeDef;

#define SYSCFG_BASE  (0x40013800UL)
#define SYSCFG       ((SYSCFG_TypeDef *)SYSCFG_BASE)

#define SYSCFG_EXTICR2_EXTI4       (0xFUL << 0)
#define SYSCFG_EXTICR2_EXTI4_Pos   (0U)

/* ================= ADC ================= */
typedef struct
{
    __IO uint32_t SR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMPR1;
    __IO uint32_t SMPR2;
    __IO uint32_t JOFR[4];
    __IO uint32_t HTR;
    __IO uint32_t LTR;
    __IO uint32_t SQR1;
    __IO uint32_t SQR2;
    __IO uint32_t SQR3;
    __IO uint32_t JSQR;
    __IO uint32_t JDR[4];
    __IO uint32_t DR;
} ADC_TypeDef;

#define ADC1_BASE  (0x40012000UL)
#define ADC1       ((ADC_TypeDef *)ADC1_BASE)

#define ADC_SR_EOC        (0x1UL << 1)
#define ADC_CR1_EOCIE     (0x1UL << 5)
#define ADC_CR2_ADON      (0x1UL << 0)
#define ADC_CR2_SWSTART   (0x1UL << 30)
#define ADC_SMPR2_SMP0    (0x7UL << 0)
#define ADC_SMPR2_SMP1    (0x7UL << 3)
#define ADC_SMPR2_SMP4    (0x7UL << 12)
#define ADC_SQR1_L        (0xFUL << 20)
#define ADC_SQR1_L_Pos    (20U)
#define ADC_SQR3_SQ1      (0x1FUL << 0)
#define ADC_SQR3_SQ1_Pos  (0U)

/* ================= USART ================= */
typedef struct
{
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t GTPR;
} USART_TypeDef;

#define USART2_BASE  (0x40004400UL)
#define USART2       ((USART_TypeDef *)USART2_BASE)

#define USART_SR_TXE     (0x1UL << 7)
#define USART_SR_TC      (0x1UL << 6)
#define USART_SR_RXNE    (0x1UL << 5)
#define USART_CR1_UE     (0x1UL << 13)
#define USART_CR1_M      (0x1UL << 12)
#define USART_CR1_TE     (0x1UL << 3)
#define USART_CR1_RE     (0x1UL << 2)
#define USART_CR1_RXNEIE (0x1UL << 5)
#define USART_CR2_STOP   (0x3UL << 12)

/* ================= TIMER ================= */
typedef struct
{
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMCR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCMR2;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
    uint32_t RESERVED0;
    __IO uint32_t CCR1;
    __IO uint32_t CCR2;
    __IO uint32_t CCR3;
    __IO uint32_t CCR4;
} TIM_TypeDef;

#define TIM3_BASE  (0x40000400UL)
#define TIM3       ((TIM_TypeDef *)TIM3_BASE)
#define TIM2_BASE  (0x40000000UL)
#define TIM2       ((TIM_TypeDef *)TIM2_BASE)

#define TIM_CR1_CEN     (0x1UL << 0)
#define TIM_CCER_CC1E   (0x1UL << 0)
#define TIM_CCMR1_OC1M  (0x7UL << 4)
#define TIM_CCMR1_OC1PE (0x1UL << 3)

/* ================= NVIC (stub - ไม่ทำอะไรจริง แค่ให้ compile ผ่าน) ================= */
typedef enum
{
    EXTI0_IRQn          = 6,
    EXTI1_IRQn          = 7,
    EXTI2_IRQn          = 8,
    EXTI3_IRQn          = 9,
    EXTI4_IRQn          = 10,
    EXTI9_5_IRQn        = 23,
    EXTI15_10_IRQn      = 40,
    ADC_IRQn            = 18,
    TIM2_IRQn           = 28,
    TIM3_IRQn           = 29,
    TIM4_IRQn           = 30,
    USART1_IRQn         = 37,
    USART2_IRQn         = 38
} IRQn_Type;

static inline void NVIC_EnableIRQ(IRQn_Type irq) { (void)irq; }
static inline void NVIC_SetPriority(IRQn_Type irq, uint32_t prio) { (void)irq; (void)prio; }

/* ================= SysTick / SCB (stub) ================= */
typedef struct
{
    __IO uint32_t CTRL;
    __IO uint32_t LOAD;
    __IO uint32_t VAL;
    __IO uint32_t CALIB;
} SysTick_TypeDef;

#define SysTick_BASE (0xE000E010UL)
#define SysTick      ((SysTick_TypeDef *)SysTick_BASE)

#define SysTick_CTRL_CLKSOURCE_Msk (0x1UL << 2)
#define SysTick_CTRL_ENABLE_Msk    (0x1UL << 0)
#define SysTick_CTRL_COUNTFLAG_Msk (0x1UL << 16)

typedef struct
{
    __IO uint32_t CPACR;
} SCB_TypeDef;
#define SCB_BASE (0xE000ED88UL)
#define SCB      ((SCB_TypeDef *)SCB_BASE)

/* Interrupt handler prototypes ที่ผู้เรียนจะไป define เอง ต้องมี prototype ก่อน (MISRA rule 11) */
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void ADC_IRQHandler(void);
void USART2_IRQHandler(void);

#endif /* STM32F4XX_H */
