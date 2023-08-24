#include <stdio.h>

#include "list.h"


/*
 * insert a node to the list identified by head
 */
list *insert(list *head, list_node *node)
{
  if (node == NULL)
    return head;

  node->next = head;
  head       = node;

  return head;
}

/*
 * display each node
 */
void print(list *head)
{
  while (head != NULL) {
    printf("%d, ", head->value);
    head = head->next;
  }
  printf("\n");
}


list *reverse(list *head)
{
  if (head == NULL)
    return NULL;

  list *reversed = head;
  head = head->next;
  reversed->next = NULL;

  while (head != NULL) {
    list *head_next = head->next;

    head->next = reversed;
    reversed = head;

    head = head_next;
  }

  return reversed;
}    
