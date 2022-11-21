/*
Projet 2 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Milan
Description du projet *SmallDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "student.hpp"
#include <iostream>

void student_to_str(student_t *s, char *buffer)
{
  snprintf(buffer, 512, "%i: %s %s in section %s, born on the %d/%d/%d", s->id, s->fname, s->lname, s->section,
           s->birthdate.tm_mday,
           s->birthdate.tm_mon + 1,
           s->birthdate.tm_year + 1900);
}