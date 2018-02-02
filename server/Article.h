#ifndef ARTICLE_H_
#define ARTICLE_H_
#include <my_global.h>
#include <mysql.h>

int publishArticle(char articleTopic[100], char articleTitle[100], char articleText[100], char loginRespone[100], MYSQL *con);

int subscribeArticle(char topic[100], char loginRespone[100], MYSQL *con);

#endif