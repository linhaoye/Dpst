#include <stdio.h>
#include "vec.h"
#include "xml.h"

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
