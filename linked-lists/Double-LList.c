#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "DLList.h"

// data structures representing DLList

typedef struct DLListNode {
	int    value;  // value of this list item (string)
	struct DLListNode *prev;
	               // pointer previous node in list
	struct DLListNode *next;
	               // pointer to next node in list
} DLListNode;

typedef struct DLListRep {
	int  nitems;      // count of items in list
	DLListNode *first; // first node in list
	DLListNode *curr;  // current node in list
	DLListNode *last;  // last node in list
} DLListRep;

// create a new DLListNode (private function)
static DLListNode *newDLListNode(int it)
{
	DLListNode *new;
	new = malloc(sizeof(DLListNode));
	assert(new != NULL);
	new->value = it;
	new->prev = new->next = NULL;
	return new;
}

// create a new empty DLList
DLList newDLList()
{
	struct DLListRep *L;

	L = malloc(sizeof (struct DLListRep));
	assert (L != NULL);
	L->nitems = 0;
	L->first = NULL;
	L->last = NULL;
	L->curr = NULL;
	return L;
}

// free up all space associated with list
void freeDLList(DLList L)
{
	assert(L != NULL);
	DLListNode *curr, *prev;
	curr = L->first;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(L);
}

// trim off \n from strings (private function)
// this is needed for getDLList() because of fgets()
// alternatively, we could use the evil gets() function
/*
static char *trim(char *s)
{
	int end = strlen(s)-1;
	if (s[end] == '\n') s[end] = '\0';
	return s;
}
*/
// display list to file, one item per line
// assumes that the file is open for writing
void putDLList(FILE *out, DLList L)
{
	assert(out != NULL); assert(L != NULL);
	DLListNode *curr;
	for (curr = L->first; curr != NULL; curr = curr->next)
		fprintf(out,"%d\n",curr->value);
}

// check sanity of a DLList (for testing)
int validDLList(DLList L)
{
	if (L == NULL) {
		fprintf(stderr,"DLList is null\n");
		return 0;
	}
	if (L->first == NULL) {
		// list is empty; curr and last should be null
		if (L->last != NULL || L->curr != NULL) {
			fprintf(stderr,"Non-null pointers in empty list\n");
			return 0;
		}
	}
	else {
		// list is not empty; curr and last should be non-null
		if (L->last == NULL || L->curr == NULL) {
			fprintf(stderr,"Null pointers in non-empty list\n");
			return 0;
		}
	}
	int count;
	DLListNode *curr;
	// check scanning forward through list
	count = 0;
	for (curr = L->first; curr != NULL; curr = curr->next) {
		if (curr->prev != NULL && curr->prev->next != curr) {
			fprintf(stderr, "Invalid forward link into node (%d)\n",curr->value);
			return 0;
		}
		if (curr->next != NULL && curr->next->prev != curr) {
			fprintf(stderr, "Invalid backward link into node (%d)\n",curr->value);
			return 0;
		}
		count++;
	}
	if (count != L->nitems) {
		fprintf(stderr, "Forward count mismatch; counted=%d, nitems=%d\n",
		        count, L->nitems);
		return 0;
	}
	// check scanning backward through list
	count = 0;
	for (curr = L->last; curr != NULL; curr = curr->prev) {
		count++;
	}
	if (count != L->nitems) {
		fprintf(stderr, "Backward count mismatch; counted=%d, nitems=%d\n",
		        count, L->nitems);
		return 0;
	}
	// nothing went wrong => must be ok
	return 1;
}

// return item at current position
int DLListCurrent(DLList L)
{
	assert(L != NULL); assert(L->curr != NULL);
	return L->curr->value;
}

// move current position (+ve forward, -ve backward)
// return 1 if reach end of list during move
// if current is currently null, return 1
int DLListMove(DLList L, int n)
{
	assert(L != NULL);
	if (L->curr == NULL)
		return 1;
	else if (n > 0) {
		while (n > 0 && L->curr->next != NULL) {
			L->curr = L->curr->next;
			n--;
		}
	}
	else if (n < 0) {
		while (n < 0 && L->curr->prev != NULL) {
			L->curr = L->curr->prev;
			n++;
		}
	}
	return (L->curr == L->first || L->curr == L->last) ? 1 : 0;
}

// move to specified position in list
// i'th node, assuming first node has i==1
int DLListMoveTo(DLList L, int i)
{
	assert(L != NULL); assert(i > 0);
	L->curr = L->first;
	return DLListMove(L, i-1);
}

// insert an item before current item
// new item becomes current item
void DLListBefore(DLList L, int it)
{
    //printf("Inserting a line before curr\n");
	assert(L != NULL); 
	// COMPLETE THIS FUNCTION
	
	struct DLListNode *newNode = newDLListNode(it);
	
	if (L->nitems == 0) {
	    L->first = newNode;
        L->last = newNode;
	} else if (L->curr == L->first) { //inserting before the first node
	    L->first = newNode;
	    newNode->next = L->curr;
	    L->curr->prev = newNode;
	} else { //inserting literally anywhere else
	    L->curr->prev->next = newNode;
	    newNode->next = L->curr;
	
	    newNode->prev = L->curr->prev;
	    L->curr->prev = newNode;
	}
	
	L->curr = newNode;
	L->nitems++;
}

// insert an item after current item
// new item becomes current item
void DLListAfter(DLList L, int it)
{
	assert(L != NULL); 
	// COMPLETE THIS FUNCTION
	
	struct DLListNode *newNode = newDLListNode(it);
	
	if (L->nitems == 0) {
	    L->first = newNode;
        L->last = newNode;
	} else if (L->curr == L->last) {
	    L->last = newNode;
	    newNode->prev = L->curr;
	    L->curr->next = newNode;
	} else {
        L->curr->next->prev = newNode;
	    newNode->prev = L->curr;
	
	    newNode->next = L->curr->next;
	    L->curr->next = newNode;
    }
    
    L->curr = newNode;
	L->nitems++;
	
}


//delete nodes with a value inside the range, inclusive
//this should do nothing if lo>hi
//ie. don't change anything, but don't fail
void deleteInRange(DLList L, int lo, int hi) {
    //need a front back current
    struct DLListNode *back;
    struct DLListNode *front;
    struct DLListNode *deletethis;

    if(L->nitems == 0) {
    	return;
    } else {
    	L->curr = L->first;
    	while(L->curr != NULL) {
    		if(L->curr->value >= lo && L->curr->value <= hi) {
    			//delete
    			back = L->curr->prev;
    			front = L->curr->next;
    			
                if(L->nitems == 1) {
                    //deleting only node
                    deletethis = L->curr;
                    free(deletethis);
                    L->first = NULL;
                    L->last = NULL;
                    L->curr = NULL;
                    L->nitems--;
                    return;

                } else if(back == NULL) {
    				//assume first node deletion
    				deletethis = L->curr;
    				L->curr = L->curr->next;
    				L->first = L->curr;
    				free(deletethis);
    			} else if(front == NULL) {
    				//assume last node deletion
    				deletethis = L->curr;
    				L->curr = L->curr->prev;
    				L->last = L->curr;
    				free(deletethis);
    			} else {
    				back->next = front;
    				front->prev = back;
    				deletethis = L->curr;
    				L->curr = L->curr->next;
    				free(deletethis);	
    			}
    			L->nitems--;
    		} else {
    			L->curr = L->curr->next;
    		}
    	}
    }

    L->curr = L->first;
}


// delete current item
// new item becomes item following current
// if current was last, current becomes new last
// if current was only item, current becomes null
void DLListDelete(DLList L)
{
	assert (L != NULL);
	// COMPLETE THIS FUNCTION
    //case 1: if list was empty...
    
    DLListNode *toDelete;
    
    if(DLListIsEmpty(L)) {
        return;
    } else if(L->nitems == 1) {
        free(L->curr);
        L->nitems = 0;
        L->first = NULL;
        L->last = NULL;
        L->curr = NULL;
        return;
    } else if (L->curr == L->first){
        //case: L being first item
        L->curr->next->prev = NULL;
        L->first = L->curr->next;
        free(L->curr);
        L->curr = L->first;
        L->nitems--;
        return;
    } else if (L->curr == L->last) {
        //case: L being last item
        L->curr->prev->next = NULL;
        L->last = L->curr->prev;
        free(L->curr);
        L->curr = L->last;
        L->nitems--;
        return;
    } else {
        //case: L is somewhere in the middle :0
        L->curr->next->prev = L->curr->prev;
        L->curr->prev->next = L->curr->next;
        toDelete = L->curr;
        L->curr = L->curr->next;
        free(toDelete);
        L->nitems--;
        return;
    }
}

// return number of elements in a list
int DLListLength(DLList L)
{
	return (L->nitems);
}

// is the list empty?
int DLListIsEmpty(DLList L)
{
	return (L->nitems == 0);
}

