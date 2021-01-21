#include <stdio.h>
#include <stdlib.h>

#include "calc.h"

int main()
{
	char val[60];
	int e=0;
	printf("input string for calculation: ");
	scanf("%s", val);
	printf("%f", Calculate(val, &e));

	return (0);
}
