/*
 *  web/login.c
 *
 *  (C) 2024  Chen Zichao
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#define DATABASE "account.db"
#define N        64
	
int main(int argc, const char *argv[])
{
    char name[N] = {0};
    char passwd[N] = {0};
    char sqlCommand[N] = {};
    sqlite3* db;
    char *errmsg;
    char **resultp;
	int nrow;
	int ncolumn;

	printf("Content-type: text/html;charset=utf-8\n\n");
    printf("<html>\n");
    printf("<head><title>登录提示</title></head>\n");

	// 从环境变量中获取CONTENT_LENGTH
    char *content_length_str = getenv("CONTENT_LENGTH");
    if (content_length_str == NULL) {
        printf("<p>Content length not specified.</p>");
        return 1;
    }
    int content_length = atoi(content_length_str);

    // 分配足够的内存来存储POST数据
    char *post_data = (char *)malloc(content_length + 1);
    if (post_data == NULL) {
        printf("<p>Memory allocation failed.</p>");
        return 1;
    }

    // 读取POST数据
    if (fread(post_data, 1, content_length, stdin) != content_length) {
        printf("<p>Failed to read POST data.</p>");
        free(post_data);
        return 1;
    }
    post_data[content_length] = '\0'; // 添加字符串结束符
	
	char *username_ptr = strstr(post_data, "username=");
    char *password_ptr = strstr(post_data, "password=");
    char *login_ptr = strstr(post_data, "login=");
    char *register_ptr = strstr(post_data, "register=");

	// 偏移量为字符串长度，跳过 "username=" 和 "password=" 部分
    strcpy(name, username_ptr + strlen("username="));
    strcpy(passwd, password_ptr + strlen("password="));
    // 在 '&' 之前添加字符串结束符号
    *strchr(name, '&') = '\0';
    *strchr(passwd, '&') = '\0';
    // printf("<h1>name = %s, passwd = %s</h1>", name, passwd);

    // open sqlite
    if (sqlite3_open(DATABASE, &db) != SQLITE_OK) {
        printf("%s\n", sqlite3_errmsg(db));
        return -1;
    }

    // create table
    if (sqlite3_exec(db, "create table usr(name text primary key, passwd text);",
        NULL, NULL, &errmsg) != SQLITE_OK) {
        // printf("%s\n", errmsg);
    } else {
        // printf("Create or open table success.\n");
    }

    if (login_ptr != NULL) { // login
        sprintf(sqlCommand, "select * from usr where name = '%s' and passwd = '%s';", name, passwd);
        if (sqlite3_get_table(db, sqlCommand, &resultp, &nrow, &ncolumn, &errmsg) != SQLITE_OK) {
            // printf("%s\n", errmsg);
            return -1;
        } else {
            // printf("query done.\n");
        }

        if (nrow == 1) { // query success
            printf("<h1>Login Success!</h1>");
		    printf("<h1>Welcome %s!</h1>", name);	
		    printf("<meta http-equiv=\"refresh\" content=\"2;url=/monitor.html\">");
        } else { // query fail
            printf("<h1>Login Error!</h1>");
            printf("<p>please check your password!</p>");
        }
    }

    if (register_ptr != NULL) { // register
        sprintf(sqlCommand, "insert into usr values('%s', '%s');", name, passwd);
        if (sqlite3_exec(db, sqlCommand, NULL, NULL, &errmsg) != SQLITE_OK) {
            printf("%s\n", errmsg);
        } else {
            printf("<h1>Register Success!</h1>");
        }
    }

	/*if ((strcmp(name, "czc") == 0) && (strcmp(passwd, "123") == 0)) {
		printf("<h1>Login Success!</h1>");
		printf("<h1>Welcome %s!</h1>", name);	
		printf("<meta http-equiv=\"refresh\" content=\"2;url=/monitor.html\">");
	} else {
		printf("<h1>Login Error!</h1>");
        printf("<p>please check your password!</p>");
	}*/

	printf("</html>\n");

	return 0;
}