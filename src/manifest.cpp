#include "manifest.h"
#include "config.h"
#include <SD.h>
#include <ArduinoJson.h>
#include <MD5Builder.h>

bool manifest_load(const char* dir, Manifest* out) {
  memset(out, 0, sizeof(*out));
  strlcpy(out->dir, dir, MF_PATH_LEN);

  char path[MF_PATH_LEN + 24];
  snprintf(path, sizeof(path), "%s/manifest.json", dir);
  File f = SD.open(path, FILE_READ);
  if (!f) return false;

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;

  strlcpy(out->name, doc["name"] | "unknown", MF_NAME_LEN);
  strlcpy(out->version, doc["version"] | "?", sizeof(out->version));
  strlcpy(out->app, doc["app"] | "app.bin", sizeof(out->app));
  out->size = doc["size"] | 0;
  const char* md5 = doc["md5"] | (const char*)nullptr;
  if (md5) { strlcpy(out->md5, md5, sizeof(out->md5)); out->has_md5 = true; }
  const char* bf = doc["built_for"] | "";
  out->built_for_slayerx = (strcmp(bf, "slayerx") == 0);
  return out->size > 0;
}

bool manifest_verify_md5(const Manifest* m) {
  if (!m->has_md5) return true;
  char path[MF_PATH_LEN + 24];
  snprintf(path, sizeof(path), "%s/%s", m->dir, m->app);
  File f = SD.open(path, FILE_READ);
  if (!f) return false;

  MD5Builder md5;
  md5.begin();
  uint8_t buf[2048];
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    md5.add(buf, n);
  }
  f.close();
  md5.calculate();
  return md5.toString().equalsIgnoreCase(m->md5);
}