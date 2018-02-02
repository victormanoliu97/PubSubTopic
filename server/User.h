#ifndef USER_H_
#define USER_H_
#include"Defines.h"
#include <my_global.h>
#include <mysql.h>

int registerUser(char userName[MAXUSERLENGTH], char userPass[100], char userRole[100], char loginResponse[100], MYSQL *con);

int loginUser(char userName[MAXUSERLENGTH], char userPass[100], char loginResponse[100], MYSQL *con);

#endif