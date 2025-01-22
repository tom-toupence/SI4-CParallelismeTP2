#include <stdio.h>
#include <stdlib.h>


int isAPrime(int i) {
   int divider = 2;
   while(divider <i) {
     if (i%divider ==0) {
       return 0;
     }
     divider++;
   }
   return 1;
}

void simpleLoop() {
   for(int i=0;i<1000;i++) {
     if (i==500) {
       printf("I'm halfway there !\n");
     }
   }
    printf("I'm done !\n");
}

void cpuIntensiveLoop() {
  for(int i=0;i<200000;i++) {
      isAPrime(i);
  }


}




//This is the main function of your program
//it will be called automatically when you start
int main(int argc, char* argv[]) {
   printf("Hello world \n");

   int a = 10;
   int size = 1000;
   int* table = malloc(size*sizeof(int));
   int* res = malloc(size*sizeof(int));
   printf("The value of a is %d \n", a);

   simpleLoop();

  //testing number to see if they are isAPrime
  //in C booleans can be implemeted with int (0 is false, any other value is true)
  double t1_omp,t2_omp;
  for (int i=3;i<size;i++) {
    int result = isAPrime(i);
    if (result) {
      printf("%d is a prime \n" , i);
    } else {
      printf("%d is not a prime \n", i);
    }
  }



  cpuIntensiveLoop();

}