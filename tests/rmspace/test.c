/*
 * Copyright 2005-2024 Gentoo Foundation
 * Distributed under the terms of the GNU General Public License v2
 *
 * Copyright 2005-2008 Ned Ludd        - <solar@gentoo.org>
 * Copyright 2005-2014 Mike Frysinger  - <vapier@gentoo.org>
 */

#include "main.h"
#include "rmspace.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

const char *argv0;
FILE *warnout;

void test_remove_extra_space(void)
{
    char test1[100], test2[100], test3[100], test4[100], test5[100];
    char test6[100], test7[100], test8[200];
    
    strcpy(test1, "   hello world");
    remove_extra_space(test1);
    assert(strcmp(test1, "hello world") == 0);
    printf("Test 1: Leading whitespace removed\n");
    
    strcpy(test2, "hello world   ");
    remove_extra_space(test2);
    assert(strcmp(test2, "hello world") == 0);
    printf("Test 2: Trailing whitespace removed\n");
    
    strcpy(test3, "hello    world");
    remove_extra_space(test3);
    assert(strcmp(test3, "hello world") == 0);
    printf("Test 3: Multiple spaces collapsed\n");
    
    strcpy(test4, "/etc/init.d\n\n\t/usr/lib/debug");
    remove_extra_space(test4);
    assert(strcmp(test4, "/etc/init.d /usr/lib/debug") == 0);
    printf("Test 4: Newlines/tabs converted to space\n");
    
    strcpy(test5, "  \t\n  hello  \n\t  world  \r\n  ");
    remove_extra_space(test5);
    assert(strcmp(test5, "hello world") == 0);
    printf("Test 5: Mixed whitespace normalized\n");
    
    strcpy(test6, "");
    remove_extra_space(test6);
    assert(strcmp(test6, "") == 0);
    printf("Test 6: Empty string handled\n");
    
    strcpy(test7, "   \t\n\r   ");
    remove_extra_space(test7);
    assert(strcmp(test7, "") == 0);
    printf("Test 7: Whitespace-only string becomes empty\n");
    
    strcpy(test8, "/etc/init.d\n              /etc/runlevels\r              /lib/systemd/system/ldconfig.service\t /usr/lib/debug");
    remove_extra_space(test8);
    assert(strcmp(test8, "/etc/init.d /etc/runlevels /lib/systemd/system/ldconfig.service /usr/lib/debug") == 0);
    
    printf("Test 8 - Standalone elements:\n");
    {
        char *token;
        char temp[200];
        strcpy(temp, test8);
        token = strtok(temp, " ");
        while (token != NULL) {
            printf("  %s\n", token);
            token = strtok(NULL, " ");
        }
    }
    
    /* Ensure "/usr" or "/usr/" don't appear as standalone elements */
    assert(strstr(test8, " /usr ") == NULL);
    assert(strstr(test8, " /usr/") == NULL || strstr(test8, "/usr/lib") != NULL);
    printf("Test 8: Multi-line INSTALL_MASK normalized (no standalone /usr)\n");
    
    assert(remove_extra_space(NULL) == NULL);
    printf("Test 9: NULL returns NULL\n");
    
    printf("\nAll tests passed!\n");
}

int main(int argc, char *argv[])
{
	int i;
	char *s;
	size_t len;

	argv0 = argv[0];
	warnout = stderr;

	if (argc <= 1)
		return 1;

	for (i = 1; i < argc; ++i) {
		s = rmspace(argv[i]);
		len = strlen(s);
		if (isspace(s[0]) || (len && isspace(s[len - 1]))) {
			fprintf(stderr, "FAIL {%s}\n", s);
			return 1;
		}
	}

	test_remove_extra_space();

	return 0;
}
