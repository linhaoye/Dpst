#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"
#include "dao.h"

#undef  S
#undef  STR
#define S(s)   #s
#define STR(s) S(s)

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

int dao_add_member(member_info *member) {
  sqlite3_stmt *stmt = NULL;
  int rc = 0, idx = -1;
  char* sql = "insert into tbl_member ("
    "username,"
    "password," 
    "address1," 
    "address2," 
    "email1," 
    "email2," 
    "city," 
    "pin," 
    "profession,"
    "ipaddr," 
    "phone1) values("
    ":username,"
    ":password,"
    ":address1,"
    ":address2,"
    ":email1,"
    ":email2,"
    ":city,"
    ":pin,"
    ":profession,"
    ":ipaddr,"
    ":phone1"
    ")";

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK && rc != SQLITE_STATIC) {
    return -1;
  }
# define SQLBIND(param) \
  idx = sqlite3_bind_parameter_index(stmt, ":" STR(param));\
  sqlite3_bind_text(stmt, idx, member.param, -1, SQLITE_STATIC)

  SQLBIND(username);
  SQLBIND(password);
  SQLBIND(address1);
  SQLBIND(address2);
  SQLBIND(email1);
  SQLBIND(email2);
  SQLBIND(city);
  SQLBIND(pin);
  SQLBIND(profession);
  SQLBIND(ipaddr);
  SQLBIND(phone1);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    return -1;
  }

  sqlite3_finalize(stmt);
  stmt = NULL;

  return 0;
}

int dao_add_friend(friend_info *friend) {
  sqlite3_stmt *stmt = NULL;
  int rc = 0, idx = -1;
  char *sql = "insert into tbl_friend(memberid, friendid) values(:mid, :fid)";

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  if (rc != SQLITE_OK && rc != SQLITE_STATIC) {
    return -1;
  }

  idx = sqlite3_bind_parameter_index(stmt, ":mid");
  sqlite3_bind_text(stmt, idx, friend.memberid, -1, SQLITE_STATIC);

  idx = sqlite3_bind_parameter_index(stmt, ":fid");
  sqlite3_bind_text(stmt, idx, friend.friendid, -1, SQLITE_STATIC);

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
    return -1;
  }

  sqlite3_finalize(stmt);
  stmt = NULL;

  return 0;
}
