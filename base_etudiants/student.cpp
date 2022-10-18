#include "student.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void student_to_str(char *buffer, student_t *s)
{
  strftime(buffer, sizeof(buffer), "%Y-%m-%d", &s->birthdate);
  printf("%i, %s, %s,%s, %s\n", s->id, s->fname, s->lname, s->section, buffer);
}


int student_equals(student_t *s1, student_t *s2)
{
  return (s1->id == s2->id && (strcmp(s1->fname, s2->fname) == 0) && 
    (strcmp(s1->lname, s2->lname) == 0) && (strcmp(s1->section, s2->section) == 0) &&
    s1->birthdate.tm_year == s2->birthdate.tm_year &&
    s1->birthdate.tm_mon == s2->birthdate.tm_mon && 
    s1->birthdate.tm_mday == s2->birthdate.tm_mday);
}