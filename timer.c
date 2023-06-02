#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "vale_os.h"
#include "list.h"
#include "tcb.h"

#include "uart.h"

/* CPU Frequency */
#ifdef F_CPU
#define AVR_CPU_HZ F_CPU
#else
#define AVR_CPU_HZ 16000000
#endif

extern list_t ready_list;
extern list_t sleep_list;

void timer_start(void)
{
    /* Set timer 1 compare match value for configured system tick,
     * with a prescaler of 256. We will get a compare match 1A
     * interrupt on every system tick, in which we must call the
     * OS's system tick handler. */
    OCR1A = (AVR_CPU_HZ / 256 / SYSTEM_TICKS_PER_SEC);

    /* Enable compare match 1A interrupt */
#ifdef TIMSK
    TIMSK = _BV(OCIE5A);
#else
    TIMSK1 = _BV(OCIE1A);
#endif
    /* Set prescaler 256 */
    TCCR1B = _BV(CS12) | _BV(WGM12);
}

int current_time = 0;

int get_current_time(void)
{
    return current_time;
}

void increment_current_time(void)
{
    current_time++;
}

void awake_sleeping_threads(void)
{
    list_node_t *node = sleep_list.head;
    while (node != 0)
    {
        list_node_t *node_next = node->next;
        tcb_t *tcb = (tcb_t *)node->data;
        // my_printf("awake_sleeping_threads %d %d\n", tcb->sleep_until, current_time);
        if (tcb->sleep_until <= current_time)
        {
            tcb->status = THREAD_STATUS_READY;
            list_remove(&sleep_list, node);
            list_enqueue(&ready_list, node);
        }
        node = node_next;
    }
}