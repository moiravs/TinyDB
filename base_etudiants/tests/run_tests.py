#! /usr/bin/env python3
import os
import time

# Size of a student in bytes
STUDENT_SIZE = 256
# Success and failure constants, used for testing
SUCCESS = 1
FAILURE = 0
# Path to the database used in the tests
TEST_DB = "tests/data/test_db.bin"


def check_log_content(logs, expected_lines):
    res = True
    for line in expected_lines:
        if line not in logs:
            print('"%s" should be in the results but is not!')
            res = False
    return res


def copy_db():
    """
    Copy the database so that there is no risk of corrupting its data.
    """
    check_db()
    filename = "tests/data/" + str(time.time()) + ".bin"
    db = open(TEST_DB, "rb")
    f = open(filename, "wb")
    f.write(db.read())
    db.close()
    f.close()
    return filename


def get_logs(query_type):
    log_files = [
        "logs/" + f for f in os.listdir("logs") if f.endswith(query_type + ".txt")]
    log_files.sort()
    if len(log_files) == 0:
        return []
    f = open(log_files[-1], "r")
    log_content = f.readlines()
    f.close()
    return log_content


def check_db():
    nb_students = compute_nb_students(TEST_DB)
    if nb_students != 9:
        print("Database test_db.bin is corrupted! It should contain exactly 9 entries but there are %d!" % (
            nb_students))
        exit(-1)


def compute_nb_students(db_file):
    f = open(db_file, "rb")
    content = f.read()
    f.close()
    return len(content) / STUDENT_SIZE


def run_test(test_name, db_filename):
    try:
        os.makedirs("logs")
    except:
        pass
    command = "./tinydb " + db_filename + " < tests/queries/" + \
        test_name + ".txt > tests/test_" + test_name + ".output"
    print("Running test " + test_name + ": \"" + command + "\"")
    if os.system(command) != 0:
        print("The command '" + command + "' has issued an error!")
        return FAILURE
    return SUCCESS


def test_update_multiple():
    db_filename = copy_db()
    res = run_test("update_multiple", db_filename)
    if res != FAILURE:
        if compute_nb_students(db_filename) != 9:
            print("The number of students has been changed when performing an update!")
            res = FAILURE
        log_content = get_logs("update")
        if len(log_content) != 3:
            print("This test expects 3 lines in the log file. Found",
                  len(log_content), "lines")
            res = FAILURE
        if not check_log_content(log_content, ['111111: Pharmacien Durandal in section pharma, born on the 12/12/1994\n', '472905: Pharmacien Vancauwelaert in section pharma, born on the 8/8/1998\n']):
            print("The content of the log file is not the expected one!")
            res = FAILURE
    os.remove(db_filename)
    return res


def test_update():
    db_filename = copy_db()
    res = run_test("update_single", db_filename)
    if res != FAILURE:
        if compute_nb_students(db_filename) != 9:
            print("The number of students has been changed when performing an update!")
            res = FAILURE
        log_content = get_logs("update")
        if not check_log_content(log_content, ["864030: Trucmuche Legast in section info, born on the 30/9/1998\n"]):
            print("The content of the log file is not what was expected!")
            res = FAILURE
    os.remove(db_filename)
    return res


def test_delete():
    db_filename = copy_db()
    res = run_test("delete_single", db_filename)
    if res != FAILURE:
        nb_students = compute_nb_students(db_filename)
        if nb_students != 8:
            print("Error: the database is expected to contain 8 students after removal. %d were found!" % (
                nb_students))
            res = FAILURE
    os.remove(db_filename)
    return res


def test_delete_multiple():
    db_filename = copy_db()
    res = run_test("delete_multiple", db_filename)
    if res != FAILURE:
        log_content = get_logs("delete")
        if not check_log_content(log_content, ["985432: Gaspard Francois in section chemistry, born on the 4/4/1995\n", "234556: Corentin Francois in section physics, born on the 12/12/1994\n"]):
            print("The log file does not contain the expected data")
            res = FAILURE
        nb_students = compute_nb_students(db_filename)
        if nb_students != 7:
            print("Error: the database is expected to contain 7 students after removal. %d were found!" % (
                nb_students))
            res = FAILURE
    os.remove(db_filename)
    return res


def test_select_single():
    check_db()
    res = run_test("select_single", TEST_DB)
    if res != FAILURE:
        this_log_content = get_logs("select")
        if len(this_log_content) != 2:
            print("The test failed either because the first line does not contain a summary of the query or because no result have been found (expected 1 result)")
            res = FAILURE
        if not check_log_content(this_log_content, ["864030: Hadrien Legast in section info, born on the 30/9/1998\n"]):
            print(
                "The test failed because the log file does not contain the expected result")
            res = FAILURE
    return res


def test_select_multiple():
    check_db()
    res = run_test("select_multiple", TEST_DB)
    if res != FAILURE:
        this_log_content = get_logs("select")
        if len(this_log_content) != 3:
            print("The test failed either because the first line does not contain a summary of the query or because no result have been found (expected 2 result)")
            res = FAILURE
        if not check_log_content(this_log_content, ["111111: Bastien Durandal in section pharma, born on the 12/12/1994\n", "234556: Corentin Francois in section physics, born on the 12/12/1994\n"]):
            print("The content of the file does not match the expected one.")
            res = FAILURE
    return res


def test_insert_single():
    db_filename = copy_db()
    res = run_test("insert_single", db_filename)
    if res != FAILURE:
        nb_students = compute_nb_students(db_filename)
        if nb_students != 10:
            print(
                "Test failed: there should be 10 students in the database! Found", nb_students)
            res = FAILURE
    os.remove(db_filename)
    return res


def test_insert_multiple():
    db_filename = copy_db()
    res = run_test("insert_multiple", db_filename)
    if res != FAILURE:
        nb_students = compute_nb_students(db_filename)
        if nb_students != 18:
            print(
                "Test failed: there should be 18  students in the database! (Found", nb_students, ")")
            res = FAILURE
    os.remove(db_filename)
    return res


if __name__ == "__main__":
    success = 0
    tests = [
        test_insert_single,
        test_insert_multiple,
        test_select_single,
        test_select_multiple,
        test_delete,
        test_delete_multiple,
        test_update,
        test_update_multiple
    ]
    for test in tests:
        if test() == SUCCESS:
            success += 1
        else:
            print("failure!")
    print(success, "/", len(tests), "tests succeeded")
