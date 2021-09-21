#include <mysql/mysql.h>

#include <cstring>
#include <iostream>
#include <thread>

bool stop = false;
const char* host = "127.0.0.1";
const char* user = "root";
const char* pass = "";
const char* db = "test";

void ensure(bool expected_true, const char* what, const char* funcname, const char* filename, int line)
{
    if (!expected_true)
    {
        std::cerr << "Check failed in " << funcname << " at " << filename << ":" << line << std::endl;
        std::cerr << what << std::endl;
        exit(1);
    }
}

#define ENSURE(x) ensure(x, #x, __func__, __FILE__, __LINE__)

void modifier()
{
    MYSQL *mysql;
    ENSURE(mysql_thread_init() == 0);
    ENSURE((mysql = mysql_init(nullptr)) != nullptr);
    ENSURE(mysql_real_connect(mysql, host, user, pass, db, 0, 0, 0) != nullptr);

    int round = 0;
    while (!stop)
    {
        char q[256];
        const char* qt1 = "insert into part_test (id1, id2, id3) values (%d, %d, %d)";
        int t = (int)(time(0) % 10000);
        int p = t / 5 % 4;
        sprintf(q, qt1, p, t, round++);
        ENSURE(mysql_real_query(mysql, q, strlen(q)) == 0);
        p = (p + 2) % 4;
        const char* qt2 = "ALTER TABLE `part_test` TRUNCATE PARTITION part%d";
        sprintf(q, qt2, p + 1);
        ENSURE(mysql_real_query(mysql, q, strlen(q)) == 0);
    }

    mysql_close(mysql);
    mysql_thread_end();
}


void selector()
{
    MYSQL *mysql;
    ENSURE(mysql_thread_init() == 0);
    ENSURE((mysql = mysql_init(nullptr)) != nullptr);
    ENSURE(mysql_real_connect(mysql, host, user, pass, db, 0, 0, 0) != nullptr);

    size_t counter = 0;
    while (!stop)
    {
        std::cout << "************************** #" << (counter++) << std::endl;
        const char* q = "select id1, id2, id3 from part_test";
        ENSURE(mysql_real_query(mysql, q, strlen(q)) == 0);
        MYSQL_RES* res;
        ENSURE((res = mysql_store_result(mysql)) != nullptr);

        ENSURE(mysql_num_fields(res) == 3);
        MYSQL_FIELD* field;
        ENSURE((field = mysql_fetch_field(res)) != nullptr);
        std::cout << field->name << "\t";
        ENSURE((field = mysql_fetch_field(res)) != nullptr);
        std::cout << field->name << "\t";
        ENSURE((field = mysql_fetch_field(res)) != nullptr);
        std::cout << field->name << std::endl;
        ENSURE((field = mysql_fetch_field(res)) == nullptr);

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
            std::cout << row[0] << "\t" << row[1] << "\t" << row[2] << std::endl;
    }

    mysql_close(mysql);
    mysql_thread_end();
}

int main(int, const char**)
{
    ENSURE (mysql_library_init(0, nullptr, nullptr) == 0);

    std::thread modifier_thread(modifier);
    std::thread selector_thread(selector);

    std::string str;
    std::cin >> str;

    stop = true;

    selector_thread.join();
    modifier_thread.join();

    mysql_library_end();
}
