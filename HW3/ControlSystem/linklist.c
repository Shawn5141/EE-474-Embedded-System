
#include <stdio.h> 
#include <stdlib.h>
typedef struct TCB{
   int val;
   struct TCB* next;
   struct TCB* prev;
}TCB;


void insert(TCB* node,TCB* head,TCB* tail){
   if(NULL==head){
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

void Delete(TCB* node,TCB* head,TCB* tail){
   
    if (head->next ==NULL){
        return;
    }else if (head==tail){
        head=tail=NULL;
    }else if(head==node){
        head= head->next;
    }else if (tail==node){
        tail=tail->prev;
    }else{
        printf("what\n");
        if (node->next==NULL){
           printf("null");
        }
        printf("test %d",node->next->val);
        printf("\n");      
        node->next->prev=node->prev;
        node->prev->next=node->next;
        
    }
    free(node);
    return;


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
    printf("outsideiiii %d",Node->val);
    printf("hey");
    Delete(Node,head,tail);
    printf("%d",tail->val);
    printf("\n");
    return 0;
}

