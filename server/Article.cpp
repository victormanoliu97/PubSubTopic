#include "Article.h"
#include "Defines.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <my_global.h>
#include <mysql.h>

int publishArticle(char articleTopic[100], char articleTitle[100], char articleText[100], char loginResponse[100], MYSQL *con)
{
    char maxId[500];
    char topicId[500];

    strcpy(maxId, "SELECT MAX(articol_id) FROM articole;");

    if (mysql_query(con, maxId))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    MYSQL_RES *result = mysql_store_result(con);
    int num_rows = mysql_num_rows(result);
    MYSQL_ROW row = mysql_fetch_row(result);

    bzero(maxId, 100);
    strcpy(maxId, row[0]);
    int auxId = atoi(maxId);
    auxId = auxId + 1;
    sprintf(maxId, "%d", auxId);
    maxId[strlen(maxId) + 1] = '/0';

    mysql_free_result(result);

    char articleAux[500];
    strcpy(articleAux, "INSERT INTO articole(articol_id, topic_id, titlu_articol, text_articol) VALUES (");
    strcat(articleAux, maxId);
    strcat(articleAux, ",'");
    strcat(articleAux, articleTopic);
    strcat(articleAux, "','");
    strcat(articleAux, articleTitle);
    strcat(articleAux, "','");
    strcat(articleAux, articleText);
    strcat(articleAux, "');");

    if (mysql_query(con, articleAux))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    else
    {
        strcpy(loginResponse, "Ai publicat cu succes");
    }
}

int subscribeArticle(char topic[500], char loginResponse[500], MYSQL *con)
{
    char query[500];

    strcpy(loginResponse, "");
    strcpy(query, "SELECT titlu_articol, text_articol FROM articole WHERE topic_id=");
    strcat(query, topic);
    strcat(query, ";");

    if (mysql_query(con, query))
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if (result == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    strcat(loginResponse, "\n");

    while ((row = mysql_fetch_row(result)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            strcat(loginResponse, row[i]);
            strcat(loginResponse, "\n");
        }
        strcat(loginResponse, "\n");
        printf("\n");
    }

    if(strlen(loginResponse) == 0)
    {
        strcpy(loginResponse, "[CLIENT]Pe moment nu exista articole pentru topicul mentionat");
    }
}

