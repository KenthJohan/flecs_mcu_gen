#include "str.h"
#include <ctype.h>
#include <string.h>

void str_copy_gpioaf(char *dst, int size, char const *src)
{
	if (strncmp(src, "GPIO_", 5) != 0) {
		return;
	}
	src += 5;
	while (*src && size > 1) {
		if (*src == '_') {
			*dst = 0;
			break;
		}
		*dst++ = *src++;
	}
	*dst = 0;
}


void str_copy_af(char *dst, char const *src)
{
	while (*src) {
		if (isalnum(*src)) {
			*dst++ = *src;
		} else {
			*dst++ = '_';
		}
		src++;
	}
	*dst = 0;
}