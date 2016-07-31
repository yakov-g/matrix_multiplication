#include <stdlib.h>
#include "queue.h"


typedef struct _Queue_Node Queue_Node;
struct _Queue_Node
{
   Queue_Node *next;
   const void *data;
};

struct _Queue
{
   Queue_Node *head;
   Queue_Node *tail;
};

static Queue_Node*
_queue_node_create()
{
   return calloc (1, sizeof(Queue_Node));
}

static void
_queue_node_destroy(Queue_Node *node)
{
   if (!node) return;
   free(node);
}

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

const void *
queue_peek(const Queue *queue)
{
   if (!queue) return NULL;
   if (!queue->head) return NULL;
   return queue->head->data;
}

Queue *
queue_create()
{
   return calloc(1, sizeof(Queue));
}

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
