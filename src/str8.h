#ifndef STR8_H
#define STR8_H

#include <stdbool.h>
#include <stddef.h>

#include "darr.h"

typedef struct str8 {
	char* data;
	size_t length;
} str8;

darr_def(str8);

void str8_free_pool();
str8 new_str8(const char* data, size_t length, bool inplace);
str8 str8_cpy(const str8 src);

bool str8_contains(str8 s, int c);
bool str8_cmp(str8 left, str8 right);

str8 str8_concat(str8 left, str8 right);
str8 str8_list_concat(str8_list list);
str8_list str8_split(str8 s, int delim, bool inplace);
str8* str8_cut(str8 s, int sep, bool inplace);
str8 str8_trim_space(str8 s, bool inplace);
str8 str8_trim(str8 s, str8 cutset, bool inplace);

#endif

#define STR8_IMPLEMENTATION
#ifdef STR8_IMPLEMENTATION

#include <ctype.h>

static str8_list pool = {};

void str8_free_pool() {
	if (pool.data && pool.capacity) {
		for (int i = 0; i < pool.length; i++) {
			free(pool.data[i].data);
		}
	}
	free(pool.data);
	pool = (str8_list){};
}

str8 new_str8(const char* data, size_t length, bool inplace) {
	if (length == 0) {
		return (str8){};
	}
	if (inplace) {
		return (str8){
			.data = (char*)data,
			.length = length
		};
	}

	str8 result = {
		.data = (char*)malloc(length+1),
		.length = length
	};
	if (data != 0 && data[0] != '\0') {
		memcpy(result.data, data, length);
	}
	result.data[result.length] = '\0';

	darr_push(pool, result);

	return result;
}

str8 str8_cpy(const str8 src) {
	str8 result = new_str8(src.data, src.length, false);
	return result;
}

bool str8_contains(str8 s, int c) {
	for (int i = 0; i < s.length; i++) {
		if (s.data[i] == c) {
			return true;
		}
	}

	return false;
}

bool str8_cmp(str8 left, str8 right) {
	if (left.length != right.length) {
		return false;
	}

	for (int i = 0; i < left.length; i++) {
		if (left.data[i] != right.data[i]) {
			return false;
		}
	}

	return true;
}

str8 str8_concat(str8 left, str8 right) {
	if (left.length == 0 || left.data[0] == '\0') {
		return new_str8(right.data, right.length, false);
	}
	if (right.length == 0 || right.data[0] == '\0') {
		return new_str8(left.data, left.length, false);
	}

	str8 result = new_str8(0, left.length+right.length, false);
	memcpy(result.data, left.data, left.length);
	memcpy(result.data+left.length, right.data, right.length);

	return result;
}

str8 str8_list_concat(str8_list list) {
	str8 result = {};
	size_t len = 0;
	for (int i = 0; i < list.length; i++) {
		len += list.data[i].length;
	}
	if (len) {
		result = new_str8("", len, false);
		size_t head = 0;
		for (int i = 0; i < list.length; i++) {
			if (list.data[i].length == 0) continue;
			
			memcpy(result.data+head, list.data[i].data, list.data[i].length);	
			head += list.data[i].length;
		}
	}

	return result;
}

str8_list str8_split(str8 s, int delim, bool inplace) {
	str8_list result = {};

	int start = 0, c = 0;
	for (; c < s.length; c++) {
		if (s.data[c] == delim) {
			str8 substr = {.data=s.data+start, .length=c-start};
			darr_push(result, new_str8(substr.data, substr.length, inplace));
			start = c+1;
		}
	}

	if (result.length == 0) {
		darr_push(result, s);
	} else if (start < c) {
		str8 substr = {.data=s.data+start, .length=c-start};
		darr_push(result, new_str8(substr.data, substr.length, inplace));
	}

	return result;
}

str8* str8_cut(str8 s, int sep, bool inplace) {
	// NOTE: Could result simply be a str8 result[2]?
	str8* result = 0;

	for (int i = 0; i < s.length; i++) {
		if (s.data[i] == sep) {
			result = calloc(sizeof(str8), 2);
			result[0] = new_str8(s.data, i, inplace);
			result[1] = new_str8(s.data+i+1, s.length-i-1, inplace);
			break;
		};
	}

	return result;
}

str8 str8_trim_space(str8 s, bool inplace) {
	int start = 0, end = s.length-1;

	while(isspace(s.data[start]) && start < end) {
		start++;
	}

	while(isspace(s.data[end]) && end > start) {
		end--;
	}

	str8 result = {};
	if (start == 0 && end == s.length-1) {
		result = new_str8(s.data, s.length, inplace);
	} else if (start < end) {
		result = new_str8(s.data+start, (end-start)+1, inplace);
	}

	return result;
}

str8 str8_trim(str8 s, str8 cutset, bool inplace) {
	int start = 0, end = s.length-1;

	while (str8_contains(cutset, s.data[start]) && start < end) {
		start++;
	}

	while (str8_contains(cutset, s.data[end]) && end > start) {
		end--;
	}

	str8 result = {};
	if (start < end) {
		result = new_str8(s.data+start, (end-start)+1, inplace);
	}

	return result;
}

#endif
