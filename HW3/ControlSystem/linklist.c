
#include <stdio.h> 
#include <stdlib.h>
typedef struct TCB{
   int val;
   struct TCB* next;
   struct TCB* prev;
}TCB;

TCB* head=NULL;
TCB* tail=NULL;

void Insert(TCB* node){
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

    Insert(Node2);
    printf(" 0 head %d ",head->val);
    printf(" 2 next %d ",head->next->val);
    printf(" 1 tail %d\n",head->next->next->val);
    Delete(Node1);
    printf(" 0 head1 %d ",head->val);
    printf(" 2 tail1 %d\n",head->next->val);
    //printfln("\n");
    return 0;
}

