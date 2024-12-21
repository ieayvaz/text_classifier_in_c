#include <stdio.h>

double *gradient(double (*func)(double*,int),double *w,int size){
  double *diff = malloc(sizeof(double)*size);
  for(int i = 0;i < size;i++){
    w[i] += 0.000001;
    double sumy = func(w,size);
    w[i] -= 0.000001;
    double y = func(w,size);
    double ydiff = sumy - y;
    diff[i] = (double)(ydiff / 0.000001);
  }
  return diff;
}
