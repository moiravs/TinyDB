#include "student.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void student_to_str(char *buffer, student_t *s)
{
  // snprintf(NULL, 10, "%s,%s,%s,%lf,%lf,%d", id, student.fname, student.lname, section,birthdate);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", &s->birthdate);
  printf("%i, %s, %s,%s, %s\n", s->id, s->fname, s->lname, s->section, buffer);
}


int student_equals(student_t *s1, student_t *s2)
{
  return s1 == s2;
}
