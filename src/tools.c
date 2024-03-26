#include "tools.h"

char* multiple_strcat(char* s, ...)
{
  va_list ap;
  char* t = s;
  size_t len = 0;

  va_start(ap, s);
  do
    len += strlen(t);
  while ((t = va_arg(ap, char*)));
  va_end(ap);

  char* ret;
  CALLOC(ret, len + 1);

  t = s;
  va_start(ap, s);
  do
    strcat(ret, t);
  while ((t = va_arg(ap, char*)));
  va_end(ap);

  return ret;
}

void print_top_line(uint length, FILE* out)
{
  fprintf(out, "┌");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "─");
  }
  fprintf(out, "┐\n");
}

void print_mid_line(uint length, FILE* out)
{
  fprintf(out, "├");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "─");
  }
  fprintf(out, "┤\n");
}

void print_bot_line(uint length, FILE* out)
{
  fprintf(out, "└");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "─");
  }
  fprintf(out, "┘\n");
}

void print_sep_line(uint length, FILE* out)
{
  print_top_line(length, out);
  print_bot_line(length, out);
}

void print_dtop_line(uint length, FILE* out)
{
  fprintf(out, "╔");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "═");
  }
  fprintf(out, "╗\n");
}

void print_dmid_line(uint length, FILE* out)
{
  fprintf(out, "╠");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "═");
  }
  fprintf(out, "╣\n");
}

void print_dbot_line(uint length, FILE* out)
{
  fprintf(out, "╚");
  for (uint i = 0; i < length; i++)
  {
    fprintf(out, "═");
  }
  fprintf(out, "╝\n");
}

void print_spaces(uint number, FILE* out)
{
  for (uint i = 0; i < number; i++)
  {
    fprintf(out, " ");
  }
}

void print_char(uint number, char* c, FILE* out)
{
  for (uint i = 0; i < number; i++)
  {
    fprintf(out, "%s", c);
  }
}

uint count_utf8_code_points(const char* s)
{
  uint count = 0;
  while (*s)
  {
    count += (*s++ & 0xC0) != 0x80;
  }
  return count;
}



// Affichage d'un titre dans une boite.
// La taille minimale autorisée est 100 (le max de length et 100 est utilisé)
void print_title_box(uint length, bool closed, FILE* out, uint nlines, ...)
{
  // La taille minimale est 100
  length = max(length, 100);

  // Récupération des lignes à écrire
  va_list list;
  va_start(list, nlines);
  char* input[nlines];
  for (uint i = 0; i < nlines; i++)
  {
    input[i] = va_arg(list, char*);
    if (count_utf8_code_points(input[i]) > length)
    {
      printf("Printing error, the title is too long for the chosen length\n");
      return;
    }
    if (strchr(input[i], '\n') != NULL)
    {
      printf("Printing error, the title should not contain \"newline\"\n");
      return;
    }
  }
  // Si tout s'est bien passé, on passe à la phase d'écriture
  print_top_line(length, out);

  for (uint i = 0; i < nlines; i++)
  {
    uint titlelen = count_utf8_code_points(input[i]);
    fprintf(out, "│");
    uint pad = length - titlelen;
    print_spaces(pad / 2, out);
    fprintf(out, "%s", input[i]);
    print_spaces((pad / 2) + (pad % 2), out);
    fprintf(out, "│\n");
  }

  if (closed)
  {
    print_bot_line(length, out);
  }
  else
  {
    print_mid_line(length, out);
  }
}


void print_title_box_shift(uint length, uint shiftl, uint shiftr, bool closed, FILE* out, uint nlines, ...) {
  // La taille minimale est 100
  length = max(length, 100);

  // Récupération des lignes à écrire
  va_list list;
  va_start(list, nlines);
  char* input[nlines];
  for (uint i = 0; i < nlines; i++)
  {
    input[i] = va_arg(list, char*);
    if (count_utf8_code_points(input[i]) > length)
    {
      printf("Printing error, the title is too long for the chosen length\n");
      return;
    }
    if (strchr(input[i], '\n') != NULL)
    {
      printf("Printing error, the title should not contain \"newline\"\n");
      return;
    }
  }
  // Si tout s'est bien passé, on passe à la phase d'écriture
  print_top_line(length, out);

  for (uint i = 0; i < nlines; i++)
  {
    uint titlelen = count_utf8_code_points(input[i]);
    fprintf(out, "│");
    uint pad = length - titlelen;
    print_spaces(pad / 2 - shiftl, out);
    fprintf(out, "%s", input[i]);
    print_spaces((pad / 2) + (pad % 2) - shiftr, out);
    fprintf(out, "│\n");
  }

  if (closed)
  {
    print_bot_line(length, out);
  }
  else
  {
    print_mid_line(length, out);
  }

}

void print_dtitle_box(uint length, bool closed, FILE* out, uint nlines, ...)
{
  // La taille minimale est 100
  length = max(length, 100);

  // Récupération des lignes à écrire
  va_list list;
  va_start(list, nlines);
  char* input[nlines];
  for (uint i = 0; i < nlines; i++)
  {
    input[i] = va_arg(list, char*);
    if (count_utf8_code_points(input[i]) > length)
    {
      printf("Printing error, the title is too long for the chosen length\n");
      return;
    }
    if (strchr(input[i], '\n') != NULL)
    {
      printf("Printing error, the title should not contain \"newline\"\n");
      return;
    }
  }

  // Si tout s'est bien passé, on passe à la phase d'écriture
  print_dtop_line(length, out);

  for (uint i = 0; i < nlines; i++)
  {
    uint titlelen = count_utf8_code_points(input[i]);
    fprintf(out, "║");
    uint pad = length - titlelen;
    print_spaces(pad / 2, out);
    fprintf(out, "%s", input[i]);
    print_spaces((pad / 2) + (pad % 2), out);
    fprintf(out, "║\n");
  }

  if (closed)
  {
    print_dbot_line(length, out);
  }
  else
  {
    print_dmid_line(length, out);
  }
}

void print_line_box(uint length, FILE* out, char* s)
{
  // La taille minimale est 100
  length = max(length, 100);
  uint linelen = count_utf8_code_points(s);
  // Récupération des lignes à écrire
  if (linelen > length)
  {
    printf("Printing error, the line is too long for the chosen length\n");
    return;
  }
  if (strchr(s, '\n') != NULL)
  {
    printf("Printing error, the line should not contain \"newline\"\n");
    return;
  }

  // Si tout s'est bien passé, on passe à la phase d'écriture
  fprintf(out, "│");
  uint pad = length - linelen;
  fprintf(out, "%s", s);
  print_spaces(pad, out);
  fprintf(out, "│\n");
}
void print_dline_box(uint length, FILE* out, char* s)
{
  // La taille minimale est 100
  length = max(length, 100);
  uint linelen = count_utf8_code_points(s);
  // Récupération des lignes à écrire
  if (linelen > length)
  {
    printf("Printing error, the line is too long for the chosen length\n");
    return;
  }
  if (strchr(s, '\n') != NULL)
  {
    printf("Printing error, the line should not contain \"newline\"\n");
    return;
  }

  // Si tout s'est bien passé, on passe à la phase d'écriture
  fprintf(out, "║");
  uint pad = length - linelen;
  fprintf(out, "%s", s);
  print_spaces(pad, out);
  fprintf(out, "║\n");
}

void print_booltab_alph(bool* alph_array, uint alph_size, FILE* out)
{

  uint a = 0;
  while (a < alph_size && !alph_array[a])
  {
    a++;
  }
  if (a == alph_size)
  {
    fprintf(out, "∅.\n");
  }
  else
  {
    fprintf(out, "{%c", a + 'a');
    a++;
    while (a < alph_size)
    {
      if (alph_array[a])
      {
        fprintf(out, ",%c", a + 'a');
      }

      a++;
    }
    fprintf(out, "}.\n");
  }
}


void append_power(uint n, char* name) {
  if (n != 0) {
    uint d = n % 10;
    append_power(n / 10, name);
    switch (d) {
    case 0:
      strcat(name, "⁰");
      break;
    case 1:
      strcat(name, "¹");
      break;
    case 2:
      strcat(name, "²");
      break;
    case 3:
      strcat(name, "³");
      break;
    case 4:
      strcat(name, "⁴");
      break;
    case 5:
      strcat(name, "⁵");
      break;
    case 6:
      strcat(name, "⁶");
      break;
    case 7:
      strcat(name, "⁷");
      break;
    case 8:
      strcat(name, "⁸");
      break;
    case 9:
      strcat(name, "⁹");
      break;
    default:
      break;
    }
  }
}


void print_power(uint n, FILE* out) {
  if (n != 0) {
    uint d = n % 10;
    print_power(n / 10, out);
    switch (d) {
    case 0:
      fprintf(out, "⁰");
      break;
    case 1:
      fprintf(out, "¹");
      break;
    case 2:
      fprintf(out, "²");
      break;
    case 3:
      fprintf(out, "³");
      break;
    case 4:
      fprintf(out, "⁴");
      break;
    case 5:
      fprintf(out, "⁵");
      break;
    case 6:
      fprintf(out, "⁶");
      break;
    case 7:
      fprintf(out, "⁷");
      break;
    case 8:
      fprintf(out, "⁸");
      break;
    case 9:
      fprintf(out, "⁹");
      break;
    default:
      break;
    }
  }
}



void print_facto_word(char* word, FILE* out) {
  uint len = strlen(word);
  if (len == 0) {
    printf("ε");
  }
  else {
    uint n = 1;
    fprintf(out, "%c", word[0]);
    for (uint i = 1; i < len;i++) {
      if (word[i] != word[i - 1]) {
        if (n > 1) {
          print_power(n, out);
        }
        fprintf(out, "%c", word[i]);
        n = 1;
      }
      else {
        n++;
      }
    }
    if (n > 1) {
      print_power(n, out);
    }

  }
}

