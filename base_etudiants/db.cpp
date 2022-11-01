/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "db.hpp"
#include <time.h>
#include <unistd.h>
#include <iostream>
#include "student.hpp"
#include <sys/mman.h>

void database_t::db_save(const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(this->data, sizeof(student_t), this->lsize, f) < 0)
    {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
}

void database_t::db_load(const char *path)

{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file))
    {
        this->db_add(student);
    }
    fclose(file);
}

void database_t::db_upsize()
{
    if (this->lsize > (this->psize / sizeof(student_t))) // if we reached the end of the allocated size for this
    {
        size_t oldPsize = this->psize;
        this->psize *= 2;
        student_t *newStudent;
        newStudent = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // establishes a mapping between an adress space of a process and a memory object
        memcpy(newStudent, this->data, oldPsize);                                                                     // copy this to newly allocated memory
        munmap(this->data, oldPsize);                                                                                 // deallocate old memory
        this->data = newStudent;
    }
}

void database_t::db_init()
{
    this->lsize = 0;
    this->psize = sizeof(student_t) * 2000000;
    this->data = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE,
                                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void database_t::db_add(student_t student)
{
    signed int position = this->lsize - 1;
    while ((position >= 0) && (student.id < this->data[position].id))
    {
        position--;
    }
    if ((position >= 0) && (student.id == this->data[position].id))
    {
        std::cout << "ID already in the database." << std::endl;
        return;
    }

    // décaler tout après
    this->lsize++;
    this->db_upsize();

    if ((this->lsize - position - 2) > 0)
    {
        memmove(&this->data[position + 2], &this->data[position + 1], (this->lsize - position - 2) * sizeof(student_t));
    }

    memcpy(&this->data[position + 1], &student, 256); // at end of db

    /*
  if (this->lsize > 0){
      bool found = false;
      //std::cout << "found " << found << " and !found " << !found << std::endl;
      size_t i = this->lsize-1;
      //std::cout << this->lsize << std::endl;
      while (i >= 0 && student.id < this->data[i].id && found){
          std::cout << "going through here" << std::endl;
          if (student.id == this->data[i].id){
              std::cout << "ID already in the database." << std::endl;
              found = true;
          }
          i--;
      }

      if (found){  // if didn't find the id
          this->lsize++;
          this->db_upsize();
          for (size_t index = i+1; index < this->lsize-1; index++){
              this->data[index] = this->data[index + 1];
          }
          memcpy(&this->data[i], &student, 256); // at end of db

      }
  }

  else {
      this->lsize++;
      this->db_upsize();
      memcpy(&this->data[this->lsize - 1], &student, 256); // at end of db
  }*/
}

void database_t::db_delete(size_t indice)
{
    if (indice >= this->lsize)
    {
        perror("db_delete()");
    }
    this->lsize--;
    memmove(&this->data[indice], &this->data[indice + 1], (sizeof(student_t) * (this->lsize - indice))); // overriding a student by moving what's after it to the deleted student's location in memory
}
