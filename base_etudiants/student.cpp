#include "student.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void student_to_str(char *buffer, student_t *s)
{
  size_t len = 0;
  std::string student = std::string(buffer);
  std::string id = student.substr(0, 32); // hexa to decimal ->id de l'étudiant
  std::string fname = student.substr(32, 96);
  std::string lname = student.substr(96, 160);
  std::string section = student.substr(160, 224);
  std::string birthdate = student.substr(224, 264);
  len = snprintf(NULL, len, "%s,%s,%s,%lf,%lf,%d,%s,%s", ap.gpsId, ap.type, ap.name, ap.latitude,
                 ap.longitude, ap.elevationFeet, ap.city, ap.countryAbbrv);
  // Copy two characters of s1 (starting
  // from position 3)

  // prints the result
  cout << "String is: " << r;
  printf("%02X:%02X:%02X:%02X", buf[0], buf[1], buf[2], buf[3]);
}

int student_equals(student_t *s1, student_t *s2)
{
  // Your code here

  // TODO ANDRIUS compare les étudiants override l'opérateur bool operator= 

  return 0;
}
