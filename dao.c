#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"
#include "dao.h"

static sqlite3 *db = NULL;
static char* file = ""; /* default to temp db */

void dao_init() {
  sqlite3_initialize();

  int rc = 0;
  rc = sqlite3_open_v2(file, &db, SQLITE_OPEN_READWRITE |
    SQLITE_OPEN_CREATE, NULL);

  if (rc != SQLITE_OK) {
    sqlite3_close(db);
    fprintf(stderr, "error open db\n");
    exit(-1);
  }
}

void dao_deinit() {
  sqlite3_close(db);
  sqlite3_shutdonw();
}