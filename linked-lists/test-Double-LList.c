#define VERY_BIG 100000
#define DELETE 0
#define BEFORE 1
#define AFTER 2
#define NOTHING -1

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "DLList.h"


void testStep(DLList inList, int function, int toEnter,
              int expectLen, int expectInt);

int main(int argc, char *argv[])
{

    //*
    DLList myList = newDLList();
    putDLList(stdout,myList);
    assert(validDLList(myList));

	//protocol:
	//print state of the list (probably the list, curr and nitems)
	//show whats about to happen
	//do it
	//print the state again
	//run validDLList()
	
	int i = 0;
	i = (i != 0);
	
	printf("Hello!\n"
	       //"Apparently getCurr doesn't work with empty lists...\n"
	       //"So we won't check curr if that is the case.\n\n"
	       "Testing before/after/delete now...\n\n");
	
	testStep(myList, DELETE, NOTHING, 0, NOTHING);
	testStep(myList, BEFORE, 1, 1, 1);
	testStep(myList, DELETE, 1, 0, NOTHING);
	testStep(myList, AFTER, 2, 1, 2);
	testStep(myList, BEFORE, 3, 2, 3);
	testStep(myList, DELETE, 3, 1, 2);
	testStep(myList, AFTER, 4, 2, 4);
	testStep(myList, DELETE, 4, 1, 2);
	testStep(myList, BEFORE, 5, 2, 5);
	testStep(myList, BEFORE, 6, 3, 6);
	testStep(myList, BEFORE, 7, 4, 7);
	testStep(myList, DELETE, 7, 3, 6);
	testStep(myList, AFTER, 8, 4, 8);
	testStep(myList, DELETE, 8, 3, 5);
	DLListMove(myList, -1);
	testStep(myList, DELETE, 6, 2, 5);
	testStep(myList, AFTER, 9, 3, 9);
	testStep(myList, BEFORE, 10, 4, 10);
	testStep(myList, DELETE, 10, 3, 9);
	testStep(myList, AFTER, 11, 4, 11);
	testStep(myList, DELETE, 11, 3, 2);
	testStep(myList, DELETE, 2, 2, 9);
	testStep(myList, BEFORE, 12, 3, 12);
	testStep(myList, DELETE, 12, 2, 9);
	testStep(myList, AFTER, 13, 3, 13);
	testStep(myList, BEFORE, 14, 4, 14);
	testStep(myList, DELETE, 14, 3, 13);
	testStep(myList, DELETE, 13, 2, 9);
	testStep(myList, AFTER, 15, 3, 15);
	testStep(myList, AFTER, 16, 4, 16);
	testStep(myList, DELETE, 16, 3, 15);
	
	printf("After/before/delete working!\n"
	       "Testing deleteInRange...\n\n");

	DLListAfter(myList, 1);
	DLListAfter(myList, 2);
	DLListAfter(myList, 3);
	DLListAfter(myList, 4);
	DLListAfter(myList, 5);
	DLListAfter(myList, 6);
	printf("List:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    printf("Calling deleteInRange, lo=2 hi=4\n");
	deleteInRange(myList, 2, 4);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    DLListAfter(myList, 6);
	DLListAfter(myList, 5);
	DLListAfter(myList, 4);
	DLListAfter(myList, 3);
	DLListAfter(myList, 2);
	DLListAfter(myList, 1);
	printf("List:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    
    printf("Calling deleteInRange, lo=4 hi=2 "
           "(which should do nothing...\n\n");
	deleteInRange(myList, 4, 2);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    
    printf("Calling deleteInRange, lo=1 hi=3\n");
	deleteInRange(myList, 1, 3);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    
    printf("Calling deleteInRange, lo=5 hi=6\n");
	deleteInRange(myList, 5, 6);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    
    printf("Calling deleteInRange, lo=4 hi=4\n");
	deleteInRange(myList, 4, 4);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
    
    printf("Calling deleteInRange again, lo=4 hi=4\n");
	deleteInRange(myList, 4, 4);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));


    printf("Calling deleteInRange again, lo=-2 hi=5\n");
	deleteInRange(myList, -2, 5);
	printf("After:\n");
    putDLList(stdout,myList);
    assert(validDLList(myList));
	
	printf("deleteInRange passed all tests!\n\n"
	       "Testing moveRisingEdge now...\n\n");
	
    DLList newList = newDLList();
    putDLList(stdout,newList);
    assert(validDLList(newList));
    
    printf("moveRisingEdge passed all tests!\n\n"
	       "All tests passed; you are awesome!\n");
	return 0;
	
}

void testStep(DLList inList, int function, int toEnter,
              int expectLen, int expectInt) {
              
    int len;
    
    switch (function) {
    case DELETE:
        //printf("Deleting \"%d\"...\n", toEnter);
	    DLListDelete(inList);
        break;
    case BEFORE:
        //printf("Inserting \"%d\" before curr...\n", toEnter);
	    DLListBefore(inList, toEnter);
        break;
    case AFTER:
        //printf("Inserting \"%d\" after curr...\n", toEnter);
	    DLListAfter(inList, toEnter);
        break;
    }
    //hello
    //printf("List:\n");
    //putDLList(stdout,inList);
    assert(validDLList(inList));
    
    len = DLListLength(inList);
    //printf("Length is %d\nShould be %d\n\n",
    //       len, expectLen);
    assert(len == expectLen);
    if (len != 0) {
        //printf("Curr is \"%d\"\nShould be \"%d\"\n\n",
        //       DLListCurrent(inList), expectInt);
        assert(DLListCurrent(inList) == expectInt);
    }
}
