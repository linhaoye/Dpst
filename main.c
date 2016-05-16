#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "vec.h"

typedef struct Foo {
	int ace;
} Foo;

int main(void)
{
	Vec(Foo) foo;

	vec_init(&foo);
	vec_deinit(&foo);
	return 0;
}