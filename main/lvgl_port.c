/**************************************************************************************************/
/** @file     lvgl_port.c
 *  @brief    x
 *  @details  x
 */
/**************************************************************************************************/


//************************************************************************************************//
//                                            INCLUDES                                            //
//************************************************************************************************//

//Library Includes
#include "lvgl.h"
#include "lvgl_port.h"

//SDK Includes
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_touch.h"
#include "esp_timer.h"
#include "esp_log.h"

//FreeRTOS Includes
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"


//************************************************************************************************//
//                                            VARIABLES                                           //
//************************************************************************************************//

static const char *TAG = "lv_port";                 // Tag for logging

static SemaphoreHandle_t lvgl_mux;                  // LVGL mutex for synchronization

static TaskHandle_t lvgl_task_handle = NULL;        // Handle for the LVGL task


//************************************************************************************************//
//                                         PRIVATE FUNCTIONS                                      //
//************************************************************************************************//

/**************************************************************************************************/
/** @fcn        static void flush_callback(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  name    descrip
 */
/**************************************************************************************************/
static void flush_callback(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
	
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data; // Get the panel handle from driver user data
    const int offsetx1 = area->x1;                  // Start X coordinate of the area to flush
    const int offsetx2 = area->x2;                  // End X coordinate of the area to flush
    const int offsety1 = area->y1;                  // Start Y coordinate of the area to flush
    const int offsety2 = area->y2;                  // End Y coordinate of the area to flush

    /* Action after last area refresh */
    if (lv_disp_flush_is_last(drv)) {
		
        /* Switch the current RGB frame buffer to `color_map` */
        esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);

        /* Wait for the last frame buffer to complete transmission */
        ulTaskNotifyValueClear(NULL, ULONG_MAX);
        
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    lv_disp_flush_ready(drv); // Mark the display flush as complete
}


/**************************************************************************************************/
/** @fcn        static lv_disp_t *display_init(esp_lcd_panel_handle_t panel_handle)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (esp_lcd_panel_handle_t) panel_handle - x
 *
 *  @return   (lv_disp_t *) descrip
 */
/**************************************************************************************************/
static lv_disp_t *display_init(esp_lcd_panel_handle_t panel_handle) {
	
    assert(panel_handle); // Ensure the panel handle is valid

    static lv_disp_draw_buf_t disp_buf = { 0 };     // Contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv = { 0 };          // Contains LCD panel handle and callback functions

    // Allocate draw buffers used by LVGL
    void *buf1 = NULL; 								// Pointer for the first buffer
    void *buf2 = NULL; 								// Pointer for the second buffer
    
    int buffer_size = 0; 							// Size of the buffer

    ESP_LOGD(TAG, "Malloc memory for LVGL buffer");
#if LVGL_PORT_AVOID_TEAR_ENABLE
    // To avoid tearing effect, at least two frame buffers are needed: one for LVGL rendering and another for RGB output
    buffer_size = LVGL_PORT_H_RES * LVGL_PORT_V_RES;
#if (LVGL_PORT_LCD_RGB_BUFFER_NUMS == 3) && (EXAMPLE_LVGL_PORT_ROTATION_DEGREE == 0) && LVGL_PORT_FULL_REFRESH
    // With three buffers and full-refresh, one buffer is always available for rendering
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 3, &lvgl_port_rgb_last_buf, &buf1, &buf2));
    lvgl_port_rgb_next_buf = lvgl_port_rgb_last_buf; // Set the next RGB buffer
    lvgl_port_flush_next_buf = buf2; // Set the flush next buffer
#elif (LVGL_PORT_LCD_RGB_BUFFER_NUMS == 3) && (EXAMPLE_LVGL_PORT_ROTATION_DEGREE != 0)
    // Using three frame buffers, one for LVGL rendering and two for RGB driver (one used for rotation)
    void *fbs[3];
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 3, &fbs[0], &fbs[1], &fbs[2]));
    buf1 = fbs[2]; // Set buf1 to the third frame buffer
#else
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2)); // Get two frame buffers
#endif
#else
    // Normally, for RGB LCD, just one buffer is used for LVGL rendering
    buffer_size = LVGL_PORT_H_RES * LVGL_PORT_BUFFER_HEIGHT; // Calculate buffer size
    buf1 = heap_caps_malloc(buffer_size * sizeof(lv_color_t), LVGL_PORT_BUFFER_MALLOC_CAPS); // Allocate memory
    assert(buf1); // Ensure allocation succeeded
    ESP_LOGI(TAG, "LVGL buffer size: %dKB", buffer_size * sizeof(lv_color_t) / 1024); // Log buffer size
#endif /* LVGL_PORT_AVOID_TEAR_ENABLE */

    // Initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, buffer_size); // Initialize the draw buffer

    ESP_LOGD(TAG, "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv); // Initialize the display driver
#if EXAMPLE_LVGL_PORT_ROTATION_90 || EXAMPLE_LVGL_PORT_ROTATION_270
    disp_drv.hor_res = LVGL_PORT_V_RES; // Set horizontal resolution for rotation
    disp_drv.ver_res = LVGL_PORT_H_RES; // Set vertical resolution for rotation
#else
    disp_drv.hor_res = LVGL_PORT_H_RES; // Set horizontal resolution
    disp_drv.ver_res = LVGL_PORT_V_RES; // Set vertical resolution
#endif
    disp_drv.flush_cb = flush_callback; // Set the flush callback
    disp_drv.draw_buf = &disp_buf; // Set the draw buffer
    disp_drv.user_data = panel_handle; // Set user data to panel handle
#if LVGL_PORT_FULL_REFRESH
    disp_drv.full_refresh = 1; // Enable full refresh
#elif LVGL_PORT_DIRECT_MODE
    disp_drv.direct_mode = 1; // Enable direct mode
#endif
    return lv_disp_drv_register(&disp_drv); // Register the display driver
}

/**************************************************************************************************/
/** @fcn        static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (lv_indev_drv_t *) indev_drv - x
 *  @param    [in]  (lv_indev_data_t *) data - x
 */
/**************************************************************************************************/
static void touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
	
	//LoOcals
    esp_lcd_touch_handle_t tp = (esp_lcd_touch_handle_t)indev_drv->user_data; // Get touchpad handle from user data
    assert(tp); // Ensure touchpad handle is valid

    uint16_t touchpad_x; 							// Variable for X coordinate
    uint16_t touchpad_y; 							// Variable for Y coordinate
    uint8_t touchpad_cnt = 0; 						// Variable for touch count

    /* Read data from touch controller into memory */
    esp_lcd_touch_read_data(tp); 					// Read data from touch controller

    /* Read data from touch controller */
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(tp, &touchpad_x, &touchpad_y, NULL, &touchpad_cnt, 1); // Get touch coordinates
    
    if (touchpad_pressed && touchpad_cnt > 0) {
        data->point.x = touchpad_x; 				// Set the X coordinate
        data->point.y = touchpad_y; 				// Set the Y coordinate
        data->state = LV_INDEV_STATE_PRESSED; 		// Set state to pressed
        ESP_LOGD(TAG, "Touch position: %d,%d", touchpad_x, touchpad_y); // Log touch position
    } else {
        data->state = LV_INDEV_STATE_RELEASED; 		// Set state to released
    }
}


/**************************************************************************************************/
/** @fcn        static lv_indev_t *indev_init(esp_lcd_touch_handle_t tp)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (esp_lcd_touch_handle_t) tp - x
 *
 *  @return   (lv_indev_t *) descrip
 */
/**************************************************************************************************/
static lv_indev_t *indev_init(esp_lcd_touch_handle_t tp) {
	
    assert(tp); 									// Ensure the touch panel handle is valid

    static lv_indev_drv_t indev_drv_tp; 			// Static input device driver

    /* Register a touchpad input device */
    lv_indev_drv_init(&indev_drv_tp); 		// Initialize the input device driver
    indev_drv_tp.type = LV_INDEV_TYPE_POINTER; 		// Set the device type to pointer (touchpad)
    indev_drv_tp.read_cb = touchpad_read; 			// Set the read callback function
    indev_drv_tp.user_data = tp; 					// Set user data to the touch panel handle

    return lv_indev_drv_register(&indev_drv_tp); // Register the input device driver
}


/**************************************************************************************************/
/** @fcn        static void tick_increment(void *arg)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (void *) arg - x
 */
/**************************************************************************************************/
static void tick_increment(void *arg) {
	
    /* Tell LVGL how many milliseconds have elapsed */
    lv_tick_inc(LVGL_PORT_TICK_PERIOD_MS); // Increment the LVGL tick count
    
    return;
}


/**************************************************************************************************/
/** @fcn        static esp_err_t tick_init(void)
 *  @brief      x
 *  @details    x
 */
/**************************************************************************************************/
static esp_err_t tick_init(void) {
	
	//Locals
	esp_err_t stat;									// API resp
	
	
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &tick_increment,                // Set the callback function for the timer
        .name     = "LVGL tick"                     // Name of the timer
    };
    
    esp_timer_handle_t lvgl_tick_timer = NULL;      // Timer handle
    
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer)); // Create the timer
    
    stat = esp_timer_start_periodic(lvgl_tick_timer, LVGL_PORT_TICK_PERIOD_MS * 1000); // Start the timer
    
    
    return stat;
}


/**************************************************************************************************/
/** @fcn        static void lvgl_port_task(void *arg)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (void *) arg - x
 */
/**************************************************************************************************/
static void lvgl_port_task(void *arg) {
	
    ESP_LOGD(TAG, "Starting LVGL task");       // Log the task start

    uint32_t task_delay_ms = LVGL_PORT_TASK_MAX_DELAY_MS; // Set initial task delay
    
    for(;;) {
		
        if (lvgl_port_lock(-1)) {       // Try to lock the LVGL mutex
            task_delay_ms = lv_timer_handler();     // Handle LVGL timer events
            lvgl_port_unlock();                     // Unlock the mutex
        }
        
        // Ensure the delay time is within limits
        if (task_delay_ms > LVGL_PORT_TASK_MAX_DELAY_MS) {
			
            task_delay_ms = LVGL_PORT_TASK_MAX_DELAY_MS;
            
        } else if (task_delay_ms < LVGL_PORT_TASK_MIN_DELAY_MS) {
			
            task_delay_ms = LVGL_PORT_TASK_MIN_DELAY_MS;
            
        }
        
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms)); // Delay the task for the calculated time
    }
}


/**************************************************************************************************/
/** @fcn        esp_err_t lvgl_port_init(esp_lcd_panel_handle_t lcd_handle, esp_lcd_touch_handle_t tp_handle)
 *  @brief      x
 *  @details    x
 *
 *  @param    [in]  (esp_lcd_panel_handle_t) lcd_handle - x
 *  @param    [in]  (esp_lcd_touch_handle_t) tp_handle - x
	 
 *  @return   (esp_err_t) descrip
 */
/**************************************************************************************************/
esp_err_t lvgl_port_init(esp_lcd_panel_handle_t lcd_handle, esp_lcd_touch_handle_t tp_handle) {
	
    lv_init(); 						// Initialize LVGL
    ESP_ERROR_CHECK(tick_init());	// Initialize the tick timer

    lv_disp_t *disp = display_init(lcd_handle); // Initialize the display
    assert(disp); 					// Ensure the display initialization was successful

    if (tp_handle) {
		
        lv_indev_t *indev = indev_init(tp_handle); // Initialize the touchpad input device
        
        assert(indev); 								   // Ensure input device init was successful

    }

    lvgl_mux = xSemaphoreCreateRecursiveMutex(); 	   // Create a recursive mutex for LVGL
    assert(lvgl_mux); // Ensure mutex creation was successful

    ESP_LOGI(TAG, "Create LVGL task");           // Log task creation
    BaseType_t core_id = (LVGL_PORT_TASK_CORE < 0) ? tskNO_AFFINITY : LVGL_PORT_TASK_CORE; // Determine core ID for the task
    BaseType_t ret = xTaskCreatePinnedToCore(lvgl_port_task, "lvgl", LVGL_PORT_TASK_STACK_SIZE, NULL,
                                             LVGL_PORT_TASK_PRIORITY, &lvgl_task_handle, core_id); // Create the LVGL task
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create LVGL task"); // Log error if task creation fails
        return ESP_FAIL;                              // Return failure
    }

    return ESP_OK;                                    // Return success
}


/**************************************************************************************************/
/** @fcn        bool lvgl_port_lock(int timeout_ms)
 *  @brief      x
 *  @details    x
 
 *  @param    [in]  (int) timeout_ms - x
 *
 *  @return   (bool) descrip
 */
/**************************************************************************************************/
bool lvgl_port_lock(int timeout_ms) {
	
    assert(lvgl_mux && "lvgl_port_init must be called first"); // Ensure the mutex is initialized

    const TickType_t timeout_ticks = (timeout_ms < 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms); // Convert timeout to ticks
    
    return xSemaphoreTakeRecursive(lvgl_mux, timeout_ticks) == pdTRUE; // Try to take the mutex
}


/**************************************************************************************************/
/** @fcn        void lvgl_port_unlock(void)
 *  @brief      x
 *  @details    x
 */
/**************************************************************************************************/
void lvgl_port_unlock(void) {
	
    assert(lvgl_mux && "lvgl_port_init must be called first"); // Ensure the mutex is initialized
    
    xSemaphoreGiveRecursive(lvgl_mux); 						   // Release the mutex
}


/**************************************************************************************************/
/** @fcn        bool lvgl_port_notify_rgb_vsync(void)
 *  @brief      x
 *  @details    x
 *
 *  @return   (bool) descrip
 */
/**************************************************************************************************/
bool lvgl_port_notify_rgb_vsync(void) {
	
    BaseType_t need_yield = pdFALSE; // Flag to check if a yield is needed
#if LVGL_PORT_FULL_REFRESH && (LVGL_PORT_LCD_RGB_BUFFER_NUMS == 3) && (EXAMPLE_LVGL_PORT_ROTATION_DEGREE == 0)
    if (lvgl_port_rgb_next_buf != lvgl_port_rgb_last_buf) {
        lvgl_port_flush_next_buf = lvgl_port_rgb_last_buf; // Set next buffer for flushing
        lvgl_port_rgb_last_buf = lvgl_port_rgb_next_buf; // Update the last buffer
    }
#elif LVGL_PORT_AVOID_TEAR_ENABLE
    // Notify that the current RGB frame buffer has been transmitted
    xTaskNotifyFromISR(lvgl_task_handle, ULONG_MAX, eNoAction, &need_yield); // Notify the LVGL task
#endif
    return (need_yield == pdTRUE); // Return whether a yield is needed
}
