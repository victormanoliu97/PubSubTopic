#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"TokenManager.h"

void loginTokenSeparator(char aux[100], char clientMessage[100], char status[10], char permision[10])
{
    if (!strcmp(clientMessage, "logat_publish") || !strcmp(clientMessage, "logat_subscriber"))
    {
        int i;
        strcpy(aux, clientMessage);
        for (i = 0; i < strlen(aux); i++)
        {

            fflush(stdout);
            if (aux[i] == '_')
            {
                break;
            }
        }
        strncpy(status, aux, i);

        int j;
        for (j = i + 1; j < strlen(aux); j++)
        {
            if (aux[j] == '_')
            {
                break;
            }
        }
        strncpy(permision, aux + i + 1, j - i - 1);
    }
}

void articleTokenSeparator(char articleTopic[100], char articleTitle[100], char articleText[100], char articleDetails[300])
{
    int i;
    for (i = 0; i < strlen(articleDetails); i++)
    {
        if (articleDetails[i] == '_')
        {
            break;
        }
    }
    strncpy(articleTopic, articleDetails, i - 1);

    int j;
    for (j = i + 1; j < strlen(articleDetails); j++)
    {
        if (articleDetails[j] == '_')
        {
            break;
        }
    }
    strncpy(articleTitle, articleDetails + i + 1, j - i - 2);

    int k;
    for (k = j + 1; k < strlen(articleDetails); k++)
    {
        if (articleDetails[k] == '_')
        {
            break;
        }
    }
    strncpy(articleText, articleDetails + j + 1, k - 1);
}

void addArticleTokenSeparator(char articleTopic[100], char articleTitle[100], char articleText[100], char articleDetails[300])
{
    strcat(articleDetails, articleTopic);
    char *separator = "_";
    strcat(articleDetails, separator);
    strcat(articleDetails, articleTitle);
    strcat(articleDetails, separator);
    strcat(articleDetails, articleText);
}