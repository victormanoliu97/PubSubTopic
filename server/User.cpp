#include "User.h"
#include "Defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

int registerUser(char userName[MAXUSERLENGTH], char userPass[100], char userRole[100], char loginResponse[100], MYSQL *con)
{
    userName[strlen(userName) - 1] = '\0';
    userPass[strlen(userPass) - 1] = '\0';
    userRole[strlen(userRole) - 1] = '\0';

    char userNameAux[500];
    strcpy(userNameAux, "SELECT username FROM user where username ='");
    strcat(userNameAux, userName);
    strcat(userNameAux, "';");
    //printf("Query : %s\n", coi);

    if (mysql_query(con, userNameAux))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);
    mysql_free_result(result);

    if (num_rows == 1)
    {
        strcat(loginResponse, "Deja exista un user in baza de date");
    }
    else
    {
        char maxId[500];
        strcpy(maxId, "SELECT MAX(user_id) FROM user");

        if (mysql_query(con, maxId))
        {
            fprintf(stderr, "%s\n", mysql_error(con));
            mysql_close(con);
            exit(1);
        }
        else
        {
            result = mysql_store_result(con);
            MYSQL_ROW row = mysql_fetch_row(result);

            bzero(maxId, 100);
            strcpy(maxId, row[0]);
            int auxId = atoi(maxId);
            auxId = auxId + 1;
            sprintf(maxId, "%d", auxId);
            maxId[strlen(maxId) + 1] = '/0';

            printf("%s\n", maxId);

            mysql_free_result(result);

            char userPassAux[500];
            strcpy(userPassAux, "INSERT INTO user(user_id, username, parola, role) VALUES (");
            strcat(userPassAux, maxId);
            strcat(userPassAux, ",'");
            strcat(userPassAux, userName);
            strcat(userPassAux, "','");
            strcat(userPassAux, userPass);
            strcat(userPassAux, "',");
            strcat(userPassAux, userRole);
            strcat(userPassAux, ");");

            if (mysql_query(con, userPassAux))
            {
                fprintf(stderr, "%s\n", mysql_error(con));
                mysql_close(con);
                exit(1);
            }
            else
            {
                strcat(loginResponse, "Te-ai inregistrat cu succes");
            }
        }
    }
}

int loginUser(char userName[MAXUSERLENGTH], char userPass[100], char loginResponse[100], MYSQL *con)
{
    int roleDeterminer;
    userName[strlen(userName) - 1] = '\0';
    userPass[strlen(userPass) - 1] = '\0';

    char userNameAux[500];
    strcpy(userNameAux, "SELECT role FROM user where username ='");
    strcat(userNameAux, userName);
    strcat(userNameAux, "' AND parola='");
    strcat(userNameAux, userPass);
    strcat(userNameAux, "';");

    if (mysql_query(con, userNameAux))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);

    if (num_rows < 1)
    {
        strcat(loginResponse, "Ai gresit user-ul sau parola");
        return -1;
    }
    else
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        char auxRole[2];
        strcpy(auxRole, row[0]);
        int auxId = atoi(auxRole);
        printf("%d\n", auxId);
        if (auxId == 1)
        {
            roleDeterminer = 1;
        }
        else
        {
            roleDeterminer = 2;
        }
    }

    mysql_free_result(result);

    return roleDeterminer;
}