
#include <stdio.h> 
#include <stdlib.h>
typedef struct TCB{
   int val;
   struct TCB* next;
   struct TCB* prev;
}TCB;

TCB* head=NULL;
TCB* tail=NULL;

void insert(TCB* node){
   if(NULL==head){
        head=node;
        tail=node;

    }else{
        head->prev=node;
        node->next=head;
        head=head->prev;
          
    }
    return;
}

void Delete(TCB* node){
   
    if (head->next ==NULL){
        return;
    }else if (head==tail){
        head=tail=NULL;
    }else if(head==node){
        head= head->next;


            
    }else if (tail==node){
        tail=tail->prev;
    }else{              
        node->prev->next=node->next;
        node->next->prev=node->prev;
        
    }
    free(node);
    return;


}
     




int main(int argc,char** argv){
     
    
    
    TCB* Node;
    Node=(TCB*)malloc(sizeof(TCB));
    Node->val=2;
    
    
    TCB* Node1;
    Node1=(TCB*)malloc(sizeof(TCB));
    Node1->val=1;
    TCB* Node2;
    Node2=(TCB*)malloc(sizeof(TCB));
    Node2->val=0;
    

    head=Node;
    tail=Node1;
    head->next=tail;
    head->prev=NULL;
    tail->prev=head;
    tail->next=NULL;

    insert(Node2);
    printf("head %d",head->val);
    Delete(Node);
    printf("tail%d",head->next->val);
    printf("\n");
    return 0;
}

