#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "vec.h"

typedef struct Foo {
	int ace;
} Foo;

typedef Vec(Foo) vector_string;

void func(vector_string *cq) {

}

int main(void)
{
	vector_string foo;

	vec_init(&foo);
	vec_deinit(&foo);
	return 0;
}