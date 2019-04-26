// preprocessor directive to support printing to the display

#include <stdio.h>
#include <windows.h>
#include "C:\Users\s1991\Downloads\UW_19_Spring\Embed System\HW\hw1\Application8.h"
// the main program


int application1(void)
{
 int i=0;
 while (i<10){
     char A[5]={'A','B','C','D','\0'};
     i+=1;
     printf(A);
     Sleep(500);
     system("cls");
     Sleep(500);

 };
 return 0;
}

int application2(void)
{
 int i=0;
 while (i<10){
     char A[5]={'A','B','C','D','\0'};
     char B[5]={' ','B','C','D','\0'};
     char C[5]={' ',' ','C','D','\0'};
     char D[5]={' ',' ',' ','D','\0'};
     i+=1;
     printf(A);
     Sleep(300);
     system("cls");
     printf(B);
     Sleep(300);
     system("cls");
     printf(C);
     Sleep(300);
     system("cls");
     printf(D);
     Sleep(300);
     system("cls");
     Sleep(300);

 };
 return 0;
}

int application3(void)
{
 int i=0;
 int j=0;
 int k=0;
 char A[5]={'A',' ','C','\t','\0'};
 char B[5]={' ','B',' ','D','\0'};
 for (i=0;i<20;i++){

     if (i%2==0){
        printf(A);
        for(j=0;j<100000000;j++){};
     };

     if (i%2==1){
        printf(B);
        for(j=0;j<200000000;j++){};
     };


     system("cls");


 };
 return 0;
}

int application4(int delay_time_1,int delay_time_2)
{
 int i=0;
 int j=0;
 int k=0;
 char A[5]={'A',' ','C','\0'};
 char B[5]={' ','B',' ','D','\0'};
 for (i=0;i<20;i++){

     if (i%2==0){
        printf(A);
        for(j=0;j<delay_time_1*100000000;j++){};
     };

     if (i%2==1){
        printf(B);
        for(j=0;j<delay_time_2*100000000;j++){};
     };


     system("cls");


 };
 return 0;
}

void f1Data(unsigned long *delay1){
     int j=0;
     for(j=0;j<*delay1*100000000;j++){};}

void f2Clear(unsigned long *delay2){
    int j;
    for(j=0;j<*delay2*100000000;j++){};}



int application5(int delay_time_1,int delay_time_2)
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

int application6(int *delay_time_1,int *delay_time_2)
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

int application7(char A[],char B[],int *delay_time_1,int *delay_time_2)
{
 int i=0;

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

int main(void)
{
	//  declare, efine, and inbitializa some local variables
	int x =  9;
	int y = 10;
	int z =  0;
	float a = 0.0;

	//  preform a simple calculation
	z = x+y;

	//  print the results of the calculation to the display
	printf ("the sum of x and y is %d\n" ,z);

	//  ask the user for some data
	printf("please enter a value\n");

	//  get the data from the user
	//    the data will be a floating point number %f
	//    stored in the variable 'a'
	//    the & operator takes the address of the variable 'a'

	scanf ("%f", &a);

	//  remove the newline from input buffer
	//getchar();

	//  print the user data to the display
	//    the format will be xx.yy

	printf ("the data is %2.2f\n", a);
	//application1();
	int c=1;
	int d=2;
	char A[5]={'A',' ','C','\0'};
    char B[5]={' ','B',' ','D','\0'};
    application1();
    application2();
    application3();
    application4(c,d);
    application5(c,d);
    application6(&c,&d);
	application7(A,B,&c,&d);
	application8(&c,&d);
	return 0;
}
