#ifndef _STUDENT_H
#define _STUDENT_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * Student structure type.
 **/
typedef struct
{
    unsigned id;         /** Unique ID **/
    char fname[64];      /** Firstname **/
    char lname[64];      /** Lastname **/
    char section[64];    /** Section **/
    struct tm birthdate; /** Birth date **/
    student_t* next_student;
    student_t* precedent_student;
    bool operator==(student_t *other_student){
        return (this->id == other_student->id && 
        this->fname == other_student->fname &&
        this->lname == other_student->lname &&
        this->section == other_student->section &&
        (this->birthdate.tm_year == other_student->birthdate.tm_year));
    }
} student_t;

/**
 * @brief Convert a student to a human-readlable string.
 **/
void student_to_str(char *buffer, student_t *s);

/**
 * @brief Return whether two students are equal or not.
 **/
int student_equals(student_t *s1, student_t *s2);


#endif