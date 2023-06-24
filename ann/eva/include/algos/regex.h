#ifndef EVA_REGEX_H_
#define EVA_REGEX_H_

// return 1 if match; 0 otherwise.
//
// This is basically the regexp matcher from Kernighan & Pike, "The Practice of
// Programming", Chapter 9.
//
// Only supports meta chars: ^ $ . *
int regex_match(const char *regexp, const char *text);

#endif  // EVA_REGEX_H_
