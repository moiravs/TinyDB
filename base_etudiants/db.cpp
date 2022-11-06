/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include <time.h>
#include <unistd.h>
#include <iostream>
#include <sys/mman.h>
#include <cstring>
#include <fcntl.h>

#include "db.hpp"
#include "student.hpp"

student_t *local_data_map = 0;

void database_t::db_init()
{
    this->lsize = 0;
    this->psize = 2 * sizeof(student_t);

    int smfd = shm_open("/dbtest", O_RDWR | O_CREAT, 0600);
    ftruncate(smfd, this->psize);
    this->data = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE,
                                   MAP_SHARED, smfd, 0);

    if (this->data == MAP_FAILED)
    {
        std::cout << "Mapped failed\n";
        exit(-1);
    }
}

size_t database_t::get_lsize()
{
    return this->lsize;
}
void database_t::db_map_memory()
{
    if (this->data != local_data_map)
    {
        int smfd = shm_open("/dbtest", O_RDWR | O_CREAT, 0600);
        if (mmap(NULL, this->psize, PROT_READ | PROT_WRITE,
                 MAP_SHARED, smfd, 0) == MAP_FAILED)
        {
            std::cout << "Re Mapped failed\n";
            exit(-1);
        }
        local_data_map = this->data;
    }
}

student_t *database_t::get_record(int i)
{
    this->db_map_memory();
    return &this->data[i];
}

void database_t::db_upsize()
{

    if (this->lsize > (this->psize / sizeof(student_t))) // if we reached the end of the allocated size for this
    {
        size_t old_psize = this->psize;
        this->psize *= 2;
        int smfd = shm_open("/dbtest", O_RDWR | O_CREAT, 0600);
        ftruncate(smfd, this->psize);
        student_t *new_student = (student_t *)mremap(this->data, old_psize, this->psize, MREMAP_MAYMOVE);

        if (new_student == MAP_FAILED)
        {
            std::cout << "Mapped failed when upsize\n";
            exit(-1);
        }
        this->data = new_student;
        msync(this, sizeof(this), MS_SYNC);
    }
}

void database_t::db_load(const char *path)
{

    std::cout << "Loading your tiny tiny database..." << std::endl;
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
    std::cout << "Done !" << std::endl;
}

void database_t::db_save(const char *path)
{
    this->db_map_memory();

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

bool database_t::db_add(student_t student)
{
    this->db_map_memory();

    signed int position = this->lsize - 1;
    while ((position >= 0) && (student.id < this->data[position].id))
    {
        position--;
    }
    if ((position >= 0) && (student.id == this->data[position].id))
    {
        std::cout << "ID already in the database." << std::endl;
        return false;
    }
    this->lsize++;
    this->db_upsize();

    if ((this->lsize - position - 2) > 0) // move all the students after the new student we inserted
    {
        memmove(&this->data[position + 2], &this->data[position + 1], (this->lsize - position - 2) * sizeof(student_t));
    }
    memcpy(&this->data[position + 1], &student, 256); // insert at end of db if there is no student after new student inserted
    return true;
}

void database_t::db_delete(size_t indice)
{
    this->db_map_memory();

    if (indice >= this->lsize)
    {
        perror("db_delete()");
    }
    this->lsize--;
    memmove(&this->data[indice], &this->data[indice + 1], (sizeof(student_t) * (this->lsize - indice))); // overriding a student by moving what's after it to the deleted student's location in memory
}
