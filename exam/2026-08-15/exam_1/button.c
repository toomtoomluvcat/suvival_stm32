/*******************************************************************************
 * File Name    : button.c
 * Description  : อ่านปุ่มกด 3 ปุ่ม (PA10=เพิ่ม, PB3=ลด, PB5=รีเซ็ต) แบบ
 *                polling + falling-edge detect เพื่อให้กดค้างแล้วค่าขยับ
 *                ทีละ 1 ครั้งต่อการกด 1 ครั้งเท่านั้น (ไม่ไหลรัวตอนกดค้าง)
 *                แต่ละปุ่มเขียนแยกฟังก์ชันตรง ๆ ไม่ผ่านฟังก์ชันกลาง
 *                เพื่อให้อ่านฟังก์ชันเดียวจบ ไม่ต้องกระโดดไปดูที่อื่น
 * Date         : 2026-08-15
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "button.h"
#include "stm32f4xx.h"

/* Private includes ------------------------------------------------------------*/

/* Private typedef ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define BUTTON_RELEASED      (1u)    /* active-low: ไม่กด = 1 (pull-up) */
#define BUTTON_PRESSED       (0u)

/* Private macro ------------------------------------------------------------*/

/* Private constants ------------------------------------------------------------*/

/* Private variables ------------------------------------------------------------*/

/* External variables ------------------------------------------------------------*/

/* Private function prototypes ------------------------------------------------*/

/* Private user code ------------------------------------------------------------*/

/* Public functions ------------------------------------------------------------*/

/*
 * ตั้งค่าขาปุ่มกดทั้ง 3 ขาเป็น input + internal pull-up (เขียนตรง ๆ ทีละขา)
 * เหตุผล: ปุ่มบนบอร์ดเป็น active-low ไม่มีตัวต้านทานภายนอก ต้องเปิด
 * pull-up เอง ไม่งั้นตอนไม่กดขาจะลอย (floating) อ่านค่าไม่แน่นอน
 */
void button_init(void)
{
    /* PA10 = ปุ่มเพิ่มค่า (D2) */
    GPIOA->MODER &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR &= ~(0x3u << (10u * 2u));
    GPIOA->PUPDR |= (0x1u << (10u * 2u));

    /* PB3 = ปุ่มลดค่า (D3) */
    GPIOB->MODER &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (3u * 2u));
    GPIOB->PUPDR |= (0x1u << (3u * 2u));

    /* PB5 = ปุ่มรีเซ็ต (D4) */
    GPIOB->MODER &= ~(0x3u << (5u * 2u));
    GPIOB->PUPDR &= ~(0x3u << (5u * 2u));
    GPIOB->PUPDR |= (0x1u << (5u * 2u));
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มเพิ่มค่าเพิ่งถูกกด (edge จาก 1->0)
 * เหตุผล: เก็บสถานะรอบก่อนไว้ในตัวแปร static ของฟังก์ชันตัวเอง แล้วเทียบ
 * กับค่าที่อ่านได้รอบนี้ ถ้าเปลี่ยนจาก "ไม่กด" เป็น "กด" ถือว่าเพิ่งกด 1
 * ครั้ง ใช้ edge-detect แบบนี้เพื่อให้กดค้างแล้วค่าเพิ่มทีละ 1 เท่านั้น
 * ไม่ใช่เพิ่มรัว ๆ ตลอดเวลาที่ยังกดปุ่มค้างอยู่
 */
uint8_t button_increase_pressed(void)
{
    static uint8_t prev_state = BUTTON_RELEASED;
    uint8_t now_state;
    uint8_t edge_detected;

    now_state = (uint8_t)((GPIOA->IDR >> 10u) & 1u);
    edge_detected = 0u;

    if ((prev_state == BUTTON_RELEASED) && (now_state == BUTTON_PRESSED))
    {
        edge_detected = 1u;
    }
    else
    {
        /* No action */
    }

    prev_state = now_state;

    return edge_detected;
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มลดค่าเพิ่งถูกกด (edge จาก 1->0)
 * (logic เหมือน button_increase_pressed ทุกอย่าง เปลี่ยนแค่ขาที่อ่าน)
 */
uint8_t button_decrease_pressed(void)
{
    static uint8_t prev_state = BUTTON_RELEASED;
    uint8_t now_state;
    uint8_t edge_detected;

    now_state = (uint8_t)((GPIOB->IDR >> 3u) & 1u);
    edge_detected = 0u;

    if ((prev_state == BUTTON_RELEASED) && (now_state == BUTTON_PRESSED))
    {
        edge_detected = 1u;
    }
    else
    {
        /* No action */
    }

    prev_state = now_state;

    return edge_detected;
}

/*
 * คืนค่า 1 ครั้งเดียวตอนตรวจพบว่าปุ่มรีเซ็ตเพิ่งถูกกด (edge จาก 1->0)
 * (logic เหมือน button_increase_pressed ทุกอย่าง เปลี่ยนแค่ขาที่อ่าน)
 */
uint8_t button_reset_pressed(void)
{
    static uint8_t prev_state = BUTTON_RELEASED;
    uint8_t now_state;
    uint8_t edge_detected;

    now_state = (uint8_t)((GPIOB->IDR >> 5u) & 1u);
    edge_detected = 0u;

    if ((prev_state == BUTTON_RELEASED) && (now_state == BUTTON_PRESSED))
    {
        edge_detected = 1u;
    }
    else
    {
        /* No action */
    }

    prev_state = now_state;

    return edge_detected;
}

/* Callback functions ------------------------------------------------------------*/

/* Private functions ------------------------------------------------------------*/
