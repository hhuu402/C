#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct node {
    struct node *next;
    int       data;
};

struct node *delete_first(struct node *head);
struct node *delete_last(struct node *head);
struct node *delete_contains(int i, struct node *head);
void print_list(struct node *head);
struct node *reverse(struct node *list);

struct node *create_node(int data, struct node *next);
struct node *last(struct node *head);
struct node *append(struct node *head, int value);
void free_list(struct node *head);

#define MAX_LINE 4096

// simple main function to test delete_first, delete_last, delete_contains, reverse

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    struct node *list_head = NULL;

    while (1) {
        printf("list = ");
        print_list(list_head);
        printf("\n");

        printf("> ");
        if (fgets(line, MAX_LINE, stdin) == NULL) {
            free_list(list_head);
            return 0;
        }

        int i = 0;
        while (isalpha(line[i]) || line[i] == '_') {
            i++;
        }
        int argument = atoi(&line[i]);
        line[i] = '\0';

        if (strcmp(line, "quit") == 0) {
            free_list(list_head);
            return 0;
        } else if (strcmp(line, "append") == 0) {
            list_head = append(list_head, argument);
        } else if (strcmp(line, "delete_first") == 0) {
            list_head = delete_first(list_head);
        } else if (strcmp(line, "delete_last") == 0) {
            list_head = delete_last(list_head);
        } else if (strcmp(line, "delete_contains") == 0) {
            list_head = delete_contains(argument, list_head);
        } else if (strcmp(line, "reverse") == 0) {
            list_head = reverse(list_head);
        } else if (strcmp(line, "") != 0) {
            printf("Unknown command: '%s'\n", line);
        }
    }
}

// delete first node in list

struct node *delete_first(struct node *head) {

	struct node *temp = head;
	
	if(head != NULL) {
		head = head -> next;
		free(temp);
	} else {
		return NULL;
	}
	
    return head;
}

// delete last node in list

struct node *delete_last(struct node *head) {
    
	struct node *temp = head;
	struct node *lastnode = last(head);
	struct node *secondlast;
	
	if(temp == NULL) {
		return NULL;
	} else if(temp == lastnode) {
		free(head);
		return NULL;
	}
	
	while(temp -> next != lastnode) {
		temp = temp -> next;
	}
	
	secondlast = temp;
	free(lastnode);
	secondlast->next = NULL;
	
	/* Method has been improved upon, keeping this here for memories
	while(temp -> next != NULL) {
		temp = temp -> next;
		total_size++;
	}
	
	temp = head;
	while(count_size < total_size - 1) {
		temp = temp -> next;
	*/
	
    return head;
}

// delete first node containing specified int

struct node *delete_contains(int i, struct node *head) {
    
	struct node *temp = head;
	struct node *secondlast;
	
	int found_number = 0;
	
	if(temp == NULL) {
		return NULL;
	}
	
	if(temp->data == i) {
		found_number = 1;
	}
	
	while(temp != NULL) {
		if(temp->data == i) {
			found_number = 1;
			break;
		}
		secondlast = temp;
		temp = temp -> next;
	}
	
	if(found_number == 1) {
		if(temp == head) {
			temp = temp->next;
			secondlast = head;
			head = temp; //not pretty but it works
			free(secondlast);
		} else {
			secondlast->next = temp->next;
			free(temp);
		}
	}
		
    return head;
}

// reverse the nodes in list

struct node *reverse(struct node *head) {
    struct node *temp = head;
    struct node *lastnode = last(head);
    struct node *secondlast;
    struct node *newhead = lastnode;
    
    if(temp == NULL) {
		return NULL;
	}
	
	while(last(newhead) != head) {
		temp = head;
		lastnode = last(head);
		//printf("last node = %d\n",lastnode->data);
		while(temp->next != lastnode) {
			temp = temp -> next;
		}
		secondlast = temp;
		
		secondlast -> next = NULL;
		lastnode -> next = secondlast;
	}
	
	head = newhead;		
    return head;
}

// print contents of list in Python syntax

void print_list(struct node *head) {
    printf("[");
    for (struct node *n = head; n != NULL; n = n->next) {
        printf("%d", n->data);
        if (n->next != NULL) {
            printf(", ");
        }
    }
    printf("]");
}

// return pointer to last node in list
// NULL is returned if list is empty

struct node *last(struct node *head) {
    if (head == NULL) {
        return NULL;
    }

    struct node *n = head;
    while (n->next != NULL) {
        n = n->next;
    }
    return n;
}

// create a new list node containing value
// and append it to end of list

struct node *append(struct node *head, int value) {
    // new node will be last in list, so next field is NULL
    struct node *n =  create_node(value, NULL);
    if (head == NULL) {
        // new node is now  head of the list
        return n;
    } else {
        // change next field of last list node
        // from NULL to new node
        last(head)->next = n;  /* append node to list */
        return head;
    }
}

// Create a new struct node containing the specified data,
// and next fields, return a pointer to the new struct node.

struct node *create_node(int data, struct node *next) {
    struct node *n;

    n = malloc(sizeof (struct node));
    if (n == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    n->data = data;
    n->next = next;
    return n;
}

void free_list(struct node *head) {
    struct node *n = head;
    while (n != NULL) {
        struct node *next_node = n->next;
        free(n);
        n = next_node;
    }
}
