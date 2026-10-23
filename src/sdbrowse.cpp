#include "sdbrowse.h"
#include "hal.h"
#include "ui.h"
#include "config.h"
#include "manifest.h"
#include "flasher.h"
#include <SD.h>
#include <vector>

struct Entry { String name; bool isDir; };

static bool list_dir(const char* path, std::vector<Entry>& out) {
  File d = SD.open(path);
  if (!d || !d.isDirectory()) return false;
  while (File e = d.openNextFile()) {
    String n = e.name();
    int slash = n.lastIndexOf('/');
    if (slash >= 0) n = n.substring(slash + 1);
    if (!n.startsWith(".")) out.push_back({ n, e.isDirectory() });
    e.close();
  }
  d.close();
  return true;
}

static void boot_flow(const char* dir) {
  Manifest m;
  if (!manifest_load(dir, &m)) { ui_message("bad manifest"); return; }

  char l1[64], l2[64];
  snprintf(l1, sizeof(l1), "%s v%s", m.name, m.version);
  snprintf(l2, sizeof(l2), "%lu KB", (unsigned long)(m.size / 1024));

  const char* opts[] = { "Boot", "Check md5", "Back" };
  while (true) {
    int c = ui_pick(l1, opts, 3);
    if (c < 0 || c == 2) return;
    if (c == 1) {
      ui_toast(manifest_verify_md5(&m) ? "md5 OK" : "md5 MISMATCH");
      continue;
    }
    if (!m.built_for_slayerx && !ui_confirm("not built for slayerx")) continue;
    if (!manifest_verify_md5(&m) && !ui_confirm("md5 mismatch, force?")) continue;
    if (flasher_write(&m)) flasher_boot_slot();
    else ui_message("flash failed", l1);
  }
}

void browse_path(const char* start) {
  char cur[128];
  strlcpy(cur, start, sizeof(cur));

  while (true) {
    std::vector<Entry> entries;
    if (!list_dir(cur, entries)) { ui_message("cannot open", cur); return; }

    std::vector<String> labels;
    for (auto& e : entries) labels.push_back(e.isDir ? ("[" + e.name + "]") : e.name);
    labels.push_back("< back >");

    std::vector<const char*> cstr;
    for (auto& l : labels) cstr.push_back(l.c_str());

    int sel = ui_pick(cur, cstr.data(), (int)cstr.size());
    if (sel < 0) return;
    if (sel == (int)entries.size()) return;

    if (entries[sel].isDir) {
      char next[160];
      snprintf(next, sizeof(next), "%s/%s", cur, entries[sel].name.c_str());
      strlcpy(cur, next, sizeof(cur));
    } else {
      ui_message(entries[sel].name.c_str());
    }
  }
}

void browse_firmware_dir() {
  std::vector<Entry> dirs;
  if (!list_dir(PATH_FW, dirs)) {
    SD.mkdir(PATH_ROOT);
    SD.mkdir(PATH_FW);
    ui_message("created /slayerx/firmware");
    return;
  }
  std::vector<String> labels;
  for (auto& e : dirs) if (e.isDir) labels.push_back(e.name);
  if (labels.empty()) { ui_message("no firmware found"); return; }
  std::vector<const char*> cstr;
  for (auto& l : labels) cstr.push_back(l.c_str());

  int sel = ui_pick("Firmware", cstr.data(), (int)cstr.size());
  if (sel < 0) return;

  char dir[160];
  snprintf(dir, sizeof(dir), "%s/%s", PATH_FW, labels[sel].c_str());
  boot_flow(dir);
}