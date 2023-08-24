#include <stdio.h>

#include "list.h"

int main()
{
  list_node test_data[] = {
    {.value = 0},
    {.value = 1},
    {.value = 2},
    {.value = 3},
    {.value = 4},    };
  list *head = NULL;

  for (int i = 0; i < sizeof(test_data) / sizeof(test_data[0]); i++)
	 head = insert(head, test_data + i);
  print(head);

  head = reverse(head);
  print(head);  
}    
