/*
 * a single linked list
 *
 */

#ifndef LIST_H
#define LIST_H

typedef struct list
{
  int          value;
  struct list *next;
} list;

typedef list list_node;

extern list *insert(list *head, list_node *node);
extern void  print(list *head);
extern list *reverse(list *head);

#endif  /* LIST_H */
