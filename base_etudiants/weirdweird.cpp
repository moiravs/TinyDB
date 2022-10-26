void query_select_and_delete(database_t *db, query_result_t *queryResult, char *query, char *saveptr, const char *queryKey)
{
    student_t *s = new student_t;
    char *fieldFilter = new char[64](), *valueFilter = new char[64](), *fieldToUpdate = new char[64](), *updateValue = new char[64](), *value = new char[64];
    char value_str[64] = "0", date_str[64] = "0";
    parse_selectors(saveptr, fieldFilter, value);
    for (size_t i = 0; i < db->lsize; i++)
    {

        *s = db->data[i];
        if (strcmp(fieldFilter, "id") == 0)
        {
            sprintf(value_str, "%u", s->id); // convertir le id (unsigned) à un char* pour la comparaison
            if (strcmp(value_str, value) == 0)
            {
                query_result_add(queryResult, *s);
                if (strcmp(queryKey, "delete"))
                    db_delete(db, i);
            }
        }

        else if (strcmp(fieldFilter, "fname") == 0)
        {
            if (strcmp(s->fname, value) == 0)
            {
                query_result_add(queryResult, *s);
                if (strcmp(queryKey, "delete"))
                    db_delete(db, i);
            }
        }
        else if (strcmp(fieldFilter, "lname") == 0)
        {
            if (strcmp(s->lname, value) == 0)
            {
                query_result_add(queryResult, *s);
                if (strcmp(queryKey, "delete"))
                    db_delete(db, i);
            }
        }
        else if (strcmp(fieldFilter, "section") == 0)
        {
            if (strcmp(s->section, value) == 0)
            {
                query_result_add(queryResult, *s);
                if (strcmp(queryKey, "delete"))
                    db_delete(db, i);
            }
        }
        else if (strcmp(fieldFilter, "birthdate") == 0)
        {
            strftime(date_str, 32, "%d/%B/%Y", &s->birthdate);
            if (strcmp(date_str, value) == 0)
            {
                query_result_add(queryResult, *s);
                if (strcmp(queryKey, "delete"))
                    db_delete(db, i);
            }
        }
        else
        {
            std::cout << "An error has occurred during the select query : bad filter." << std::endl;
            // break;
        }
    }
    log_query(queryResult);
    delete s;
    delete queryResult;
}

close(fd2[1]);
read(fd2[0], query, 256);
if (strcmp(query, "01") != 0)
{
    query_result_t *queryResult = new query_result_t();
    query_result_init(queryResult, query);

    char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
    memcpy(querymod, query, 256);
    char *saveptr;
    const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
    queryKey = strtok_r(querymod, " ", &saveptr);

    if (strcmp(queryKey, "select") == 0)
    {
        query_result_t *queryResult = new query_result_t();
        query_result_init(queryResult, query);
        query_select_and_delete(db, queryResult, query, saveptr, "select");
    }
    memcpy(query, "01", 256);
}