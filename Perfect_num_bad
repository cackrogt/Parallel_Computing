#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

long long MAX = 9223372036854775807;
int check_prime(long long k){
   long long x = 1;
   long long sqt = sqrt(k);
   for(x =1; x<=sqt; x++){
      if(k%x == 0){
         return 0;
      }
   }
   return 1;
}

int main(){
   int N;
   printf("enter the number Perfect Numbers to be found(int) \n");
   scanf("%d", &N);
   if(N>8){
      printf("too big for long long\n");
      return 0;
   }

   int req = 0;
   long long k = 4;
   while (req<N && ((k-1)*k/2) < MAX){
      long long x = 1;
      long long sum =0;
      long long sqt = sqrt(k);
      omp_set_num_threads(10);
#pragma omp parallel for reduction(+:sum)
      for(x =2; x<=sqt; x++){
         if((k-1)%x == 0){
            sum++;
         }
      }
      if(sum==0){
         req++;
         printf("%lld \n",(k-1)*(k/2));
      }

      k*=2;
   }
   return 0;
}
