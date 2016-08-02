#include <stdlib.h>
#include "queue.h"

/* Queue data structure.
 * Implemented as singly linked list. */

typedef struct _Queue_Node Queue_Node;

/* Queue Node strucutre declaration */
struct _Queue_Node
{
   Queue_Node *next;
   const void *data;
};

/* Queue structure declaration */
struct _Queue
{
   Queue_Node *head;
   Queue_Node *tail;
};

/* Helper to create queue node */
static Queue_Node*
_queue_node_create()
{
   return calloc (1, sizeof(Queue_Node));
}

/* Helper to destroy queue node */
static void
_queue_node_destroy(Queue_Node *node)
{
   if (!node) return;
   free(node);
}

/* Create queue. */
Queue *
queue_create()
{
   return calloc(1, sizeof(Queue));
}

/* Destroy queue. */
void
queue_destroy(Queue *queue)
{
   if (!queue) return;
   while (queue->head)
     {
        queue_pop(queue);
     }
   free(queue);
}

/* Push data to queue. NULL data allowed. */
void
queue_push(Queue *queue, const void *data)
{
   if (!queue) return;
   Queue_Node *node = _queue_node_create();
   node->data = data;
   if (!node) return;

   if (!queue->head)
     {
        queue->head = node;
     }
   else
     {
        queue->tail->next = node;
     }
   queue->tail = node;
}

/* Pop data from queue. */
void
queue_pop(Queue *queue)
{
   if (!queue) return;
   if (!queue->head) return;

   Queue_Node *next = queue->head->next;
   _queue_node_destroy(queue->head);
   queue->head = next;
   if (!next)
     {
        queue->tail = NULL;
     }
}

/* Take top data from queue. */
const void *
queue_peek(const Queue *queue)
{
   if (!queue) return NULL;
   if (!queue->head) return NULL;
   return queue->head->data;
}

