/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "student.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void student_t::student_to_str(char *buffer)
{
  snprintf(buffer, 512, "%i: %s %s in section %s, born on the %d/%d/%d", this->id, this->fname, this->lname, this->section, 
    this->birthdate.tm_mday,
    this->birthdate.tm_mon+1,
    this->birthdate.tm_year+1900
    );
}

int student_t::student_equals(student_t *s2)
{
  // comparing every attribute to see if both students are equal
  return (this->id == s2->id);
}