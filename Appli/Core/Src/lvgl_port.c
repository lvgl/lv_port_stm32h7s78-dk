/*********************
 *      INCLUDES
 *********************/

#include "lvgl_port.h"
#include "lvgl/lvgl.h"
#include "stm32h7rsxx_hal.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

void touch_read(lv_indev_t * indev, lv_indev_data_t * data);

/**********************
 *  STATIC VARIABLES
 **********************/

static volatile bool do_sample_touch = false;
static lv_indev_state_t last_state = LV_INDEV_STATE_RELEASED;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_port_init(void)
{
    lv_init();

    lv_tick_set_cb(HAL_GetTick);

    // static __attribute__((aligned(32))) uint8_t main_fb[800 * 480 * 2];
    uint8_t * main_fb = (uint8_t *)(uintptr_t)0x90000000;
    HAL_LTDC_SetAddress(&hltdc, (uint32_t)main_fb, 0);

#if 1
    uint8_t * buf_direct_2 = main_fb + (800 * 480 * 2);
    lv_st_ltdc_create_direct(main_fb, buf_direct_2, 0);
#else
    static __attribute__((aligned(32))) uint8_t buf_partial_1[800 * 40 * 2];
    // static __attribute__((aligned(32))) uint8_t buf_partial_2[sizeof(buf_partial_1)];
    lv_st_ltdc_create_partial(buf_partial_1, NULL, sizeof(buf_partial_1), 0);
#endif

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == TP_IRQ_Pin)
    {
        do_sample_touch = true;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    NVIC_DisableIRQ(TP_IRQ_EXTI_IRQn);
    if (do_sample_touch)
    {
        uint8_t touches = 0;
        uint8_t buf[6];
        const uint16_t STATUS_REG = 0x814E;
        const uint16_t TOUCH_POS_REG = 0x8150;
        uint8_t ZERO = 0;

        HAL_I2C_Mem_Read(&hi2c1, 0xBA, STATUS_REG, 2, buf, 1, HAL_MAX_DELAY);
        touches = (0x0F & buf[0]);

        HAL_I2C_Mem_Write(&hi2c1, 0xBA, STATUS_REG, 2, &ZERO, 1, HAL_MAX_DELAY);

        do_sample_touch = false;

        if (touches > 0)
        {
            last_state = LV_INDEV_STATE_PRESSED;

            HAL_I2C_Mem_Read(&hi2c1, 0xBA, TOUCH_POS_REG, 2, buf, 4, HAL_MAX_DELAY);
            data->point.x = buf[0] + (buf[1] << 8);
            data->point.y = buf[2] + (buf[3] << 8);
        }
        else {
            last_state = LV_INDEV_STATE_RELEASED;
        }
    }
    NVIC_EnableIRQ(TP_IRQ_EXTI_IRQn);

    data->state = last_state;
}
