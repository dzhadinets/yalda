#include <stdio.h>
#include <unistd.h>
#include <sys/reboot.h>

int main(int argc, char *argv[])
{
  printf("Hello from simplest initrd!\n");
  sleep(5);
  printf("YALDA is going to shutdown\n");
  reboot(RB_POWER_OFF);
  return 0;
}
