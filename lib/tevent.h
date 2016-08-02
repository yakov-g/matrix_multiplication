
#ifndef _T_EVENT_H_
#define _T_EVENT_H_
/* T_Event is synchronization counter,
 * based on pthread.
 * T_Event object is released when counter reaches 0. */

typedef struct _T_Event T_Event;

/* Create T_Event object with initialization.
 * Retuns T_Event*, or NULL on fail*/
T_Event *
t_event_create(size_t counter);

/* Destroy T_Event object. */
void
t_event_destroy(T_Event *te);

/* Block process execution until counter
 * reaches 0. */
void
t_event_wait(T_Event *te);

/* Decrement counter. */
void
t_event_dec(T_Event *te);

#endif
