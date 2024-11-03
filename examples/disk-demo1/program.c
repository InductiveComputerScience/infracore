#include <Disk.h>

#include "program.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void Program(DiskStructure *disk){
  NumberReference entries, entrysize;
  _Bool success;
  ByteArrayReference data;
  int i;
  char str[100];

  Entries(disk, &entries, &entrysize);

  printf("Disk size: %ld bytes\n", (long)(entries.numberValue * entrysize.numberValue));

  // Allocate for a block:
  data.byteArrayLength = entries.numberValue * entrysize.numberValue;
  data.byteArray = malloc(data.byteArrayLength);

  success = Read(disk, 0, &data);

  for(i = 0; i < 13; i++){
    str[i] = (char)data.byteArray[i];
  }
  str[i] = 0;

  printf("Disk contents first 13 characters: %s\n", str);

  free(data.byteArray);
}










