#ifndef __DAO_H__
#define __DAO_H__

typedef struct {
  int memberid;
  char username[50];
  char password[32];
  char address1[128];
  char address2[128];
  char email1[80];
  char email2[80];
  char city[30];
  char pin[30];
  char profession[60];
  char ipaddr[15];
  char phone1[30];
} member_info;

typedef struct {
  int memberid;
  int friendid;
} friend_info;

typedef struct {
  char username_a[50];
  char ipaddr[15];
} friend_list;

void dao_init();
void dao_deinit();

int dao_add_member(member_info *member);
int dao_add_friend(friend_info *friend);
int dao_get_members(member_info **member, int limit);
int dao_get_friends();


#endif