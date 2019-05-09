
#include <stdio.h> 
#include <stdlib.h>
typedef struct TCB{
   int val;
   struct TCB* next;
   struct TCB* prev;
}TCB;


void insert(TCB* node,TCB* head,TCB* tail){
    if (NULL==head){
        head=node;
        tail=node;

    }else{
        tail->next=node;
        node->prev=tail;
        node->next=NULL;
        tail=node;
        printf("tail %d \n",tail->val);
        printf("node %d \n",node->val);  
    }
    return;
}

void delete(TCB* node,TCB* head,TCB* tail){

    if (head->next ==NULL){
        return;
    }else if (head==tail){
        head=tail=NULL;
        free(head);
        free(tail);
    }else if(head==node){
        head= head->next;
    }else if (tail==node){
        tail=tail->prev;
    }else{
       
        node->prev->next=node->next;
        node->next->prev=node->prev;
        node=NULL;
        free(node);
    }
    


}
     




int main(int argc,char** argv){
    TCB* head=malloc(sizeof(TCB)); 
    TCB* tail=malloc(sizeof(TCB));
    TCB* Node=malloc(sizeof(TCB));
    head->val=0;
    tail->val=1;
    Node->val=2;

    head->next=tail;
    head->prev=NULL;
    tail->prev=head;
    tail->next=NULL;

    insert(Node,head,tail);
    printf("outside %d",Node->val);
    printf("\n");
    delete(Node,head,tail);
    printf("%d",tail->val);
    printf("\n");
    return 0;
}

