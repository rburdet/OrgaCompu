#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

extern void write_int(int, int);

/*
void write_int(int number, int fd) {
  if (number == 0) {
    char aux = '0';
    write(fd, &aux, 1);
    return;
  }
  int cociente = number /  10;
  int resto = number % 10;
  if (cociente != 0) {
    write_int(cociente, fd);
  }
  char c = resto + 48;
  write(fd, &c, 1);
}
*/

int main(int argc, char** argv) {
  int number = atoi(argv[1]);
  write_int(number, 1);
  printf("\n");
  return 0;
}

