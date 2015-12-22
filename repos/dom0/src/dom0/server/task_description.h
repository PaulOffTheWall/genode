#pragma once

//Since pthread_create only accepts functions with a single void* argument,
//all arguments need to be packed into a structure.

struct taskDescription{
  int id;
  int executionTime;
  int criticalTime;
  int priority;
  int period;
  int offset;
  int matrixSize;
  char* binaryName;
};

