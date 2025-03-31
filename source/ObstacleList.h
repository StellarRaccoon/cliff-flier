#include <stdio.h>
#include <stdlib.h>
//#include <ndstypes.h>

struct Obstacle
{
    int ID, X,Y,Frame;
    struct Obstacle *next;
};

struct Obstacle *createObstacle(int ID,int X,int Y, int Frame){
    struct Obstacle *newObstacle = (struct Obstacle *)malloc(sizeof(struct Obstacle));
    newObstacle->ID=ID;
    newObstacle->X=X;
    newObstacle->Y=Y;
    newObstacle->Frame=Frame;
    newObstacle->next=NULL;
    return newObstacle;
}
struct Obstacle *insertEnd( struct Obstacle *tail,int X,int Y, int Frame){
    int ID;
    if (tail != NULL && tail->ID >= 3) {
        ID = 0;
    } else if (tail != NULL) {
        ID = (tail->ID) + 1;
    } else {
        ID = 0; // If the list is empty, start with ID 0
    }

    struct Obstacle *newObstacle = createObstacle(ID,X,Y,Frame);
    if(tail==NULL){ //if list is empty
        tail =newObstacle;
        newObstacle->next=newObstacle;
    }else{ //insert after current tail, update the pointer of the previous node
        newObstacle->next = tail->next;
        tail->next = newObstacle;
        tail = newObstacle;
    }
    fprintf(stderr, "ID: %d\n\n", ID);
    return tail;
}
struct Obstacle* deleteObstacle(struct Obstacle* head, struct Obstacle* toDelete) {
    if (head == NULL || toDelete == NULL) {
        return head; // Nothing to delete
    }

    // If the list has only one node
    if (head == toDelete && head->next == head) {
        free(toDelete);
        return NULL;
    }

    struct Obstacle* prev = head;
    while (prev->next != toDelete) {
        prev = prev->next;
    }

    // If the node to delete is the head
    if (toDelete == head) {
        head = head->next;
    }

    prev->next = toDelete->next;
    free(toDelete);

    return head;
}
