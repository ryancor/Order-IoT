#include "pal.hpp"
#include <string.h>
#include <stdio.h>

extern "C" char read();

bool isPalindrome(char* word) {
	bool ret = true;

	char *p = word;
	int len = strlen(word);
	char *q = &word[len-1];

	for(int i = 0; i < len; ++i, ++p, --q) {
	  if(*p != *q) {
    	ret = false;
	  }
	}
	return ret;
}

unsigned char readKey() {
	char(*read_ptr)() = &read;
	return (*read_ptr)();
}

char *is_repeated(char *name) {
	int i, j, len, len1;

	// calc length
	for(len=0; name[len] != '\0'; len++);

	// assign 0 to len1 - length of removed characters
	len1 = 0;

	for(i=0; i<(len-len1);) {
		// if name['A'] == name['A']
		if(name[i] == name[i+1]) {
			// shift all characters
			for(j=i; j<(len-len1); j++) {
				// replace repeating shifts to just one letter
				name[j] = name[j+1];
			}
			len1++;
		} else {
			i++;
		}
	}

	return name;
}
