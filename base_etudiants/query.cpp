#include "query.hpp"
#include "parsing.hpp"
#include <iostream>
#include <time.h>
#include <string.h>

void query_result_init(query_result_t *result, const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->psize = sizeof(student_t)*2;
  result->lsize = 0;
  result->students = (student_t *)malloc(sizeof(student_t));
  struct timespec after;
  clock_gettime(CLOCK_REALTIME, &after);
  result->end_ns = after.tv_nsec + 1e9 * after.tv_sec;

}


void query_result_add(query_result_t *result, student_t s){
  result->lsize += 1;
  query_list_upsize(result);
  result->students[result->lsize] = s;
  result->status = QUERY_SUCCESS;
}

void query_list_upsize(query_result_t *result){

  if (result->lsize >= ((result->psize)/sizeof(student_t))){
    student_t *old_data = result->students;
    size_t old_psize = result->psize;
    result->psize *= 2;
    result->students = (student_t*)malloc(result->psize);
    memcpy(result->students, old_data, old_psize);
    free(old_data);
  }

}