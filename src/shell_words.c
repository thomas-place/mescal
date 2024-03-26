#include "shell_words.h"
#include <ctype.h>

uint alphabet_size(char *word)
{
  uint size = 0;
  for (uint i = 0; i < strlen(word); i++) {
    if (!islower(word[i]))
      FATAL("Word %s contains non-lowercase characters", word);
    size = max(size, (uint)abs(word[i] - 'a' + 1));
  }
  return size;
}

uint *make_word(char *word)
{
  uint len = strlen(word);
  uint *w;
  MALLOC(w, len + 1);
  for (uint i = 0; i < len; i++)
    w[i] = word[i] - 'a';
  w[len] = 0;
  return w;
}
