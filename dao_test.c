#include <stdio.h>
#include "dao.h"
#include "unit.h"

char *dao_add_member_tests() {
  dao_init();
  member_info mb = {
    -1,
    "hello,world!",
    "81dc9bdb52d04dc20036dbd8313ed055",
    "U.S.A",
    "EA",
    "71423823382@qq.com",
    "12483438@qq.com",
    "Laurence ST D.A",
    "2974",
    "Doctor",
    "255.255.255.255",
    "183293283828"
  };
  dao_add_member(&mb);
  dao_deinit();
  return NULL;
}

char *dao_login_tests() {
  dao_init();
  char username[] = "hello,world";
  char password[] = "81dc9bdb52d04dc20036dbd8313ed055";
  char ipaddr[] = "182.168.1.219";

  dao_login(username, password, ipaddr);
  dao_deinit();
  return NULL;
}

char *dao_logout_tests() {
  dao_init();
  char username[] = "hello,world";
  char ipaddr[] = "";

  dao_logout(username, ipaddr);
  dao_deinit();
  return NULL;
}

char *dao_get_friends_test() {
  dao_init();
  char username[] = "hello,world";
  friend_list **list = NULL;
  int i,sz = 0;

  dao_get_friends(username, &list, &sz);
  if (sz > 0) {
    printf("total friend num: %d\n", sz);

    for(i = 0; i < sz; i++) {
      printf("\tThe 1th: name=%s, ip=%s\n", 
        list[i]->username_a, list[i]->ipaddr);
    }
  }
    for (i = 0; i < sz; i++) {
    if (list[i]) {
      free(list[i]);
    }
  }
  free(list);
  dao_deinit();
  return NULL;
}

char *all_tests() {
  ph_suite_start();
  ph_run_test(dao_add_member_tests);
  ph_run_test(dao_login_tests);
  ph_run_test(dao_logout_tests);
  ph_run_test(dao_get_friends_test);

  return NULL;
}

PH_RUN_TESTS(all_tests);
