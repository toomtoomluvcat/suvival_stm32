/*******************************************************************************
 * File Name    : button.c
 * Description  : อ่านปุ่มกด 3 ปุ่ม (PA10=เพิ่ม, PB3=ลด, PB5=รีเซ็ต) แบบ
 *                polling + falling-edge detect เพื่อให้กดค้างแล้วค่าขยับ
 *                ทีละ 1 ครั้งต่อการกด 1 ครั้งเท่านั้น (ไม่ไหลรัวตอนกดค้าง)
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BTN_INCREASE_PIN     (10u)   /* PA10 = D2 */
#define BTN_DECREASE_PIN     (3u)    /* PB3  = D3 */
#define BTN_RESET_PIN        (5u)    /* PB5  = D4 */

#define GPIO_MODER_INPUT     (0x0u)
#define GPIO_MODER_MASK      (0x3u)
#define GPIO_MODER_PIN_BITS  (2u)

#define GPIO_PUPDR_PULLUP    (0x1u)
#define GPIO_PUPDR_MASK      (0x3u)
#define GPIO_PUPDR_PIN_BITS  (2u)

#define BUTTON_RELEASED      (1u)    /* active-low: ไม่กด = 1 (pull-up) */
#define BUTTON_PRESSED       (0u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/
/* เก็บสถานะขาล่าสุดของแต่ละปุ่ม ไว้เทียบหา falling edge (เริ่มต้น = ไม่กด) */
static uint8_t prev_state_increase = BUTTON_RELEASED;
static uint8_t prev_state_decrease = BUTTON_RELEASED;
static uint8_t prev_state_reset = BUTTON_RELEASED;

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/
static void configure_input_pullup(GPIO_TypeDef * port, uint8_t pin);
static uint8_t check_falling_edge(GPIO_TypeDef * port, uint8_t pin, uint8_t * prev_state);

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขาปุ่มกดทั้ง 3 ขาเป็น input + internal pull-up
 * เหตุผล: ปุ่มบนบอร์ดเป็น active-low ไม่มีตัวต้านทานภายนอก ต้องเปิด
 * pull-up เอง ไม่งั้นตอนไม่กดขาจะลอย (floating) อ่านค่าไม่แน่นอน
 */
void button_init(void)
{
    configure_input_pullup(GPIOA, BTN_INCREASE_PIN);
    configure_input_pullup(GPIOB, BTN_DECREASE_PIN);
    configure_input_pullup(GPIOB, BTN_RESET_PIN);
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มเพิ่มค่าเพิ่งถูกกด (edge จาก 1->0)
 * เหตุผล: ใช้ edge-detect แทนการเช็คสถานะดิบ เพื่อให้กดค้างแล้วค่าเพิ่ม
 * ทีละ 1 เท่านั้น ไม่ใช่เพิ่มรัว ๆ ตลอดเวลาที่ยังกดปุ่มค้างอยู่
 */
uint8_t button_increase_pressed(void)
{
    return check_falling_edge(GPIOA, BTN_INCREASE_PIN, &prev_state_increase);
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มลดค่าเพิ่งถูกกด (edge จาก 1->0)
 */
uint8_t button_decrease_pressed(void)
{
    return check_falling_edge(GPIOB, BTN_DECREASE_PIN, &prev_state_decrease);
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มรีเซ็ตเพิ่งถูกกด (edge จาก 1->0)
 */
uint8_t button_reset_pressed(void)
{
    return check_falling_edge(GPIOB, BTN_RESET_PIN, &prev_state_reset);
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขาหนึ่งขาเป็น input mode พร้อมเปิด internal pull-up
 * เหตุผล: รวม logic ตั้งค่าขาปุ่มไว้ที่เดียว ใช้ clear-then-set กับทั้ง
 * MODER และ PUPDR เพื่อไม่ให้ค่าบิตเก่าปนกับค่าที่ตั้งใหม่
 */
static void configure_input_pullup(GPIO_TypeDef * port, uint8_t pin)
{
    port->MODER &= ~(GPIO_MODER_MASK << (pin * GPIO_MODER_PIN_BITS));
    port->MODER |= (GPIO_MODER_INPUT << (pin * GPIO_MODER_PIN_BITS));

    port->PUPDR &= ~(GPIO_PUPDR_MASK << (pin * GPIO_PUPDR_PIN_BITS));
    port->PUPDR |= (GPIO_PUPDR_PULLUP << (pin * GPIO_PUPDR_PIN_BITS));
}

/*
 * ตรวจ falling edge ของขาหนึ่งขา (สถานะเก่า=ไม่กด(1), สถานะใหม่=กด(0))
 * เหตุผล: ต้องเรียกฟังก์ชันนี้ทุกรอบ while(1) เพื่ออัปเดต prev_state
 * ตลอดเวลา ไม่งั้นการตรวจ edge ครั้งต่อไปจะผิดพลาด (เทียบกับค่าเก่าที่ค้างอยู่)
 */
static uint8_t check_falling_edge(GPIO_TypeDef * port, uint8_t pin, uint8_t * prev_state)
{
    uint8_t now_state;
    uint8_t edge_detected;

    now_state = (uint8_t)((port->IDR >> pin) & 1u);
    edge_detected = 0u;

    if ((*prev_state == BUTTON_RELEASED) && (now_state == BUTTON_PRESSED))
    {
        edge_detected = 1u;
    }
    else
    {
        /* No action */
    }

    *prev_state = now_state;

    return edge_detected;
}
