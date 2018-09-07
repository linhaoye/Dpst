#ifndef __XML_H__
#define __XML_H__

typedef Vec(char*) Vec_string;

enum {
	J_ROSTER,
	J_PRESENCE,
	J_MESSAGE,
	J_REGISTER,
	J_ADDGATEWAY
};

struct Msg {
	char *target;
}

char* create_xml(int type, Vec_string *v);
void deinit_xml(char *s);

#endif
