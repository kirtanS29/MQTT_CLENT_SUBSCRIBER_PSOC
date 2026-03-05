#include "cybsp.h"
#include "string.h"
#include "FreeRTOS.h"

#include "subscriber_task.h"
#include "mqtt_task.h"
#include "mqtt_client_config.h"

#include "cy_mqtt_api.h"
#include "cy_retarget_io.h"

#define MAX_SUBSCRIBE_RETRIES            (3U)
#define MQTT_SUBSCRIBE_RETRY_INTERVAL_MS (1000U)
#define SUBSCRIPTION_COUNT               (3U)
#define SUBSCRIBER_TASK_QUEUE_LENGTH     (1U)

/* Task handle */
TaskHandle_t subscriber_task_handle;
QueueHandle_t subscriber_task_q;

/* Device states */
uint32_t current_device_state_led1 = DEVICE_OFF_STATE;
uint32_t current_device_state_led2 = DEVICE_OFF_STATE;
uint32_t current_device_state_led3 = DEVICE_OFF_STATE;


/* Subscription list (2 topics) */
static cy_mqtt_subscribe_info_t subscribe_info[SUBSCRIPTION_COUNT] =
{
    {
        .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
        .topic = MQTT_SUB_TOPIC1,
        .topic_len = (sizeof(MQTT_SUB_TOPIC1) - 1)
    },
    {
        .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
        .topic = MQTT_SUB_TOPIC2,
        .topic_len = (sizeof(MQTT_SUB_TOPIC2) - 1)
    },
        {
        .qos = (cy_mqtt_qos_t) MQTT_MESSAGES_QOS,
        .topic = MQTT_SUB_TOPIC3,
        .topic_len = (sizeof(MQTT_SUB_TOPIC2) - 1)
    }

    
};

/******************************************************************************/
/* Subscribe to multiple topics */
static void subscribe_to_topic(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    mqtt_task_cmd_t mqtt_task_cmd;

    for (uint32_t retry = 0; retry < MAX_SUBSCRIBE_RETRIES; retry++)
    {
        result = cy_mqtt_subscribe(mqtt_connection,
                                   subscribe_info,
                                   SUBSCRIPTION_COUNT);

        if (result == CY_RSLT_SUCCESS)
        {
            printf("\nSubscribed to both topics successfully\n");
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(MQTT_SUBSCRIBE_RETRY_INTERVAL_MS));
    }

    if (CY_RSLT_SUCCESS != result)
    {
        printf("\nMQTT Subscribe failed!\n");
        mqtt_task_cmd = HANDLE_MQTT_SUBSCRIBE_FAILURE;
        xQueueSend(mqtt_task_q, &mqtt_task_cmd, portMAX_DELAY);
    }
}

/******************************************************************************/
static void unsubscribe_from_topic(void)
{
    cy_rslt_t result = cy_mqtt_unsubscribe(
        mqtt_connection,
        (cy_mqtt_unsubscribe_info_t *)subscribe_info,
        SUBSCRIPTION_COUNT);

    if (CY_RSLT_SUCCESS != result)
    {
        printf("Unsubscribe failed!\n");
    }
}

/******************************************************************************/
void subscriber_task(void *pvParameters)
{
    subscriber_data_t subscriber_q_data;
    (void) pvParameters;

    /* Initialize second LED GPIO if needed */
//    Cy_GPIO_Pin_Init(CYBSP_USER_LED2_PORT,
//                     CYBSP_USER_LED2_NUM,
//                     &CYBSP_LED_CONFIG);

    subscribe_to_topic();

    subscriber_task_q = xQueueCreate(
        SUBSCRIBER_TASK_QUEUE_LENGTH,
        sizeof(subscriber_data_t));

    while (true)
    {
        if (pdTRUE == xQueueReceive(subscriber_task_q,
                                    &subscriber_q_data,
                                    portMAX_DELAY))
        {
            switch(subscriber_q_data.cmd)
            {
                case SUBSCRIBE_TO_TOPIC:
                    subscribe_to_topic();
                    break;

                case UNSUBSCRIBE_FROM_TOPIC:
                    unsubscribe_from_topic();
                    break;

                case UPDATE_DEVICE_STATE:
                {
                    /* Update LED1 */
                    if (subscriber_q_data.led_id == 1)
                    {
                        Cy_GPIO_Write(CYBSP_USER_LED_PORT,
                                      CYBSP_USER_LED_NUM,
                                      subscriber_q_data.data);

                        current_device_state_led1 = subscriber_q_data.data;
                    }
                    /* Update LED2 */
                    else if (subscriber_q_data.led_id == 2)
                    {
                        Cy_GPIO_Write(CYBSP_USER_LED2_PORT,
                                      CYBSP_USER_LED2_NUM,
                                      subscriber_q_data.data);

                        current_device_state_led2 = subscriber_q_data.data;
                    }
                      else if (subscriber_q_data.led_id == 3)
                    {
                        Cy_GPIO_Write(CYBSP_USER_LED_PORT,
                                      CYBSP_USER_LED3_NUM,
                                      subscriber_q_data.data);

                        current_device_state_led3 = subscriber_q_data.data;
                    }
                    break;
                }
            }
        }
    }
}

/******************************************************************************/
/* MQTT subscription callback */
void mqtt_subscription_callback(cy_mqtt_publish_info_t *received_msg_info)
{
    const char *received_msg = received_msg_info->payload;
    int received_msg_len = received_msg_info->payload_len;

    subscriber_data_t subscriber_q_data;

    printf("\nIncoming MQTT message:\n"
           "Topic: %.*s\n"
           "Payload: %.*s\n",
           received_msg_info->topic_len,
           received_msg_info->topic,
           received_msg_len,
           received_msg);

    subscriber_q_data.cmd = UPDATE_DEVICE_STATE;

    /* Check payload */
    if ((strlen(MQTT_DEVICE_ON_MESSAGE) == received_msg_len) &&
        (strncmp(MQTT_DEVICE_ON_MESSAGE, received_msg, received_msg_len) == 0))
    {
        subscriber_q_data.data = DEVICE_ON_STATE;
    }
    else if ((strlen(MQTT_DEVICE_OFF_MESSAGE) == received_msg_len) &&
             (strncmp(MQTT_DEVICE_OFF_MESSAGE, received_msg, received_msg_len) == 0))
    {
        subscriber_q_data.data = DEVICE_OFF_STATE;
    }
    else
    {
        printf("Invalid payload!\n");
        return;
    }

    /* Decide LED based on topic */
    if (strncmp(received_msg_info->topic,
                MQTT_SUB_TOPIC1,
                received_msg_info->topic_len) == 0)
    {
        subscriber_q_data.led_id = 1;
    }
    else if (strncmp(received_msg_info->topic,
                     MQTT_SUB_TOPIC2,
                     received_msg_info->topic_len) == 0)
    {
        subscriber_q_data.led_id = 2;
    }
     else if (strncmp(received_msg_info->topic,
                     MQTT_SUB_TOPIC3,
                     received_msg_info->topic_len) == 0)
    {
        subscriber_q_data.led_id = 3;
    }
    else
    {
        printf("Unknown topic!\n");
        return;
    }

    xQueueSend(subscriber_task_q,
               &subscriber_q_data,
               portMAX_DELAY);
}