#include <stdio.h>
#include <string.h>
#include "vec.h"
#include "utils.h"

enum {
	J_ROSTER,
	J_PRESENCE,
	J_MESSAGE,
	J_REGISTER,
	J_ADDGATEWAY
};

static char* strtolower(const char *s) {

}

static char* create_roster_xml(Vec_string *v) {
	int i;
	int flag;
	int counter = 0;
	char *t;
	char *s = strdup("<ROSTER>");
	char *txt;
	char *delimiter = "~";
	char *scratch;
	char buf[1024];

	for (i = 0; i < vec_len(v); i++) {
		t = vec_get(v, i);
		memcpy(buf, t, sizeof(buf));

		while( (txt = strtok_r(!counter? buf: NULL, delimiter, &scratch))) {
			counter ++;
		}

		flag = counter < 4 ? 0: 1;
		counter = 0;
		memcpy(buf, t, sizeof(buf));

		while ( (txt = strtok_r(!counter? buf: NULL, delimiter, &scratch) ) && flag) {
			if (strcmp(txt, "jid") == 0) {
				txt = strtok_r(NULL, delimiter, &scratch);
				asprintf(&s, "%s<FriendID>%s</FriendID>", s, txt);
			} else if (strcmp(txt, "subscription") == 0) {
				txt = strtok_r(NULL, delimiter, &scratch);
				asprintf(&s, "%s<Subscription>%s</Subscription>", s, txt);
			}
			counter++;
		}
	}
	asprintf(&s, "%s</ROSTER>", s);

	return s;	
}

static char* create_presence_xml(Vec_string *v) {
	char *scratch, *delimiter = "~";
	char *s = strdup("<NotifyFriends>");
	char *t = vec_get(v, 0);
	short type = 0;
	short done = 0;
	int counter = 0;
	char *txt;
	char *ch;
	char buf[1024];
	char temp[1024];

	memcpy(buf, t, sizeof(buf));

	while( (txt = strtok_r(!counter? buf: NULL, delimiter, &scratch))) {
		if (strcmp(txt, "from") == 0 && !done) {
			txt = strtok_r(NULL, delimiter, &scratch);

			if (strcmp("jabber.org", txt) != 0) {
				ch = strchr(txt, '@');
				if (ch) {
					strncpy(temp, txt, ch - txt);
					ch = temp;
				}

				asprintf(&s, "%s<UserName>%s</UserName>", s, ch);
				done = 1;
			}
		} else if (strcmp(txt, "type") == 0) {
			txt = strtok_r(NULL, delimiter, &scratch);

			if (strcmp(txt, "unavailable") == 0)
				asprintf(&s, "%s<Status>Off-Line</Status>", s);

			if (strcmp(txt, "subscribe") == 0)
				asprintf(&s, "%s<Status>Subscribe</Status>", s);

			if (strcmp(txt, "unsubscribe") == 0)
				asprintf(&s, "%s<Status>UnSubscribe</Status>", s);

			if (strcmp(txt, "unsubscribed") == 0)
				asprintf(&s, "%s<Status>UnSubscribed</Status>", s);

			if (strcmp(txt, "Subscribed") == 0)
				asprintf(&s, "%s<Status>Subscribed</Status>", s);

			type = 1;
		}
		counter++;
	}

	if (!type)
		asprintf(&s, "%s<Status>On-Line</Status>", s);

	asprintf(&s, "%s</NotifyFriends>", s);
	return s;
}

static char* create_message_xml(Vec_string *v) {
	char *s = strdup("<MSG>");

	asprintf(&s, "%s<Target>%s</Target>", s, vec_get(v, 0));
	asprintf(&s, "%s<Source>%s</Source>", s, vec_get(v, 1));
	asprintf(&s, "%s<Text>%s</Text>", s, vec_get(v, 2));
	asprintf(&s, "%s</MSG>", s);

	return s;
}

static char* create_registeration_xml(Vec_string *v) {
	char *s = strdup("<Register>");
	char *t = vec_get(v, 0);

	if (strcmp(t, "0") == 0) {
		asprintf(&s, "%s<int>-1</int>", s);
	} else if (strcmp(t, "1") == 0) {
		asprintf(&s, "%s<int>1</int>", s);
	}
	asprintf(&s, "%s</Register>", s);

	return s;
}

static char* create_add_gateway_xml(Vec_string *v) {
	char *s = strdup("<AddGateWay>");
	char *t = vec_get(v, 0);

	if (strcmp("0", t) == 0) {
		asprintf(&s, "%s<code>-1</code>", s);
	}
	asprintf(&s, "%s</AddGateWay>", s);

	return s;
}

char* create_xml(int type, Vec_string *v) {
	char *t;
	char *s = strdup("<?xml version=\'1.0\' encoding=\'utf-8\'?><InstantMessenger>");

	switch (type) {
		case J_ROSTER:
			t = create_roster_xml(v);
			break;

		case J_PRESENCE:
			t = create_presence_xml(v);
			break;

		case J_MESSAGE:
			t = create_message_xml(v);
			break;

		case J_REGISTER:
			t = create_roster_xml(v);
			break;

		case J_ADDGATEWAY:
			t = create_add_gateway_xml(v);
			break;

		default:
			return NULL;
	}

	asprintf(&s, "%s%s</InstantMessenger>", s, t);
	Free(t);

	return s;
}

void deinit_xml(char *s) {
	Free(s);
}

#ifdef __MAIN__

int main(void)
{
	Vec_string s;
	vec_init(&s);
	vec_push(&s, "Hello world!");
	vec_push(&s, "Andy");
	vec_push(&s, "Marey");

	char *xml = create_message_xml(&s);
	printf ("%s\n", xml);
	deinit_xml(xml);

	vec_clear(&s);
	vec_push(&s, "0");
	xml = create_add_gateway_xml(&s);
	printf("%s\n", xml);
	deinit_xml(xml);

	vec_clear(&s);
	vec_push(&s, "0");
	xml = create_registeration_xml(&s);
	printf("%s\n", xml);
	deinit_xml(xml);

	vec_clear(&s);
	vec_push(&s, "from~lucy@msn.jabber.org~type~unsubscribed");
	xml = create_presence_xml(&s);
	printf("%s\n", xml);
	deinit_xml(xml);

	vec_clear(&s);
	vec_push(&s, "jid~lucy@msn.jabber.org~subscription~YES~HEI");
	xml = create_roster_xml(&s);
	printf("%s\n", xml);
	deinit_xml(xml);

	vec_clear(&s);
	vec_push(&s, "jid~lucy@msn.jabber.org~subscription~YES~HEI");
	xml = create_xml(J_ROSTER, &s);
	printf("%s\n", xml);
	deinit_xml(xml);

	vec_deinit(&s);
	return 0;
}

#endif
