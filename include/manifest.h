#pragma once
#include <Arduino.h>

#define MF_NAME_LEN 40
#define MF_PATH_LEN 128

struct Manifest {
  char name[MF_NAME_LEN];
  char version[16];
  char app[48];
  char dir[MF_PATH_LEN];
  uint32_t size;
  char md5[33];
  bool has_md5;
  bool built_for_slayerx;
};

bool manifest_load(const char* dir, Manifest* out);
bool manifest_verify_md5(const Manifest* m);