#ifndef _APPLICATION8_H_
#define _APPLICATION8_H_


#include <stdio.h>
#include <windows.h>


int application8(int *delay_time_1,int *delay_time_2)
{
 int i=0;

 char A[5]={'A',' ','C','\0'};
 char B[5]={' ','B',' ','D','\0'};


 for (i=0;i<20;i++){

     if (i%2==0){
        printf(A);
        f1Data(delay_time_1);

     };

     if (i%2==1){
        printf(B);
        f1Data(delay_time_1);
     };


     system("cls");
     f2Clear(delay_time_2);


 };
 return 0;
}

#endif
