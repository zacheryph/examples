/*
 * Cidr converter
 *    By bysin@efnet. Copyright (C) bysin 2008, All rights reserved.
 *
 */

#include <stdio.h>
unsigned int mpow(unsigned int x, unsigned int n) {
  return (!n)?1:(n&1)?x*mpow(x,n-1):mpow(x*x,n/2);
}
int bit(unsigned long x,int pos) {
  return !!(x&(1<<pos));
}
void cidr(unsigned long first, unsigned long last) {
  if (first < last) {
    unsigned int idx1=31,idx2=0,prefix;
    while (bit(first,idx1) == bit(last,idx1)) idx1--;
    prefix=(first>>idx1+1)<<idx1+1;
    while (idx2 <= idx1 && bit(first,idx2) == 0 && bit(last,idx2) == 1) idx2++;
    if (idx2 <= idx1) {
      cidr(first,prefix|(unsigned long)mpow(2,idx1)-1);
      cidr(prefix|1<<idx1,last);
    }
    else printf("%s/%d\n",inet_ntoa(htonl(prefix)),32-idx2);
  }
  else printf("%s/%d\n",inet_ntoa(htonl(first)),32);
}
int main(int argc, char **argv) {
  if (argc <= 2) {
    printf("%s [first] [last]\n",argv[0]);
    return 0;
  }
  cidr(htonl(inet_addr(argv[1])),htonl(inet_addr(argv[2])));
  return 0;
}

