#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <my_global.h>
#include <mysql.h>

#include "BSocket.h"
#include "Article.h"
#include "User.h"
#include "Defines.h"
#include"TokenManager.h"

int main()
{
    MYSQL *con = NULL;
    struct sockaddr_in serverAdress;
    struct sockaddr_in clientAdress;
    char clientMessage[100];
    char clientResponse[100] = " ";
    char userName[100];
    char userPass[100];
    char userRole[100];
    int socketFileDescriptor;


    char articleTopic[100];
    char articleTitle[100];
    char articleText[100];
    char articleDetails[300];

    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFileDescriptor == -1)
    {
        perror("[SERVER]Eroare la creearea socket-ului");
        exit(1);
    }

    bzero(&serverAdress, sizeof(serverAdress));
    bzero(&clientAdress, sizeof(clientAdress));

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAdress.sin_port = htons(PORT);

    if (bind(socketFileDescriptor, (struct sockaddr *)&serverAdress, sizeof(struct sockaddr)) == -1)
    {
        perror("[SERVER]Eroare la bind");
        exit(1);
    }

    if (listen(socketFileDescriptor, 5) == -1)
    {
        perror("[SERVER]Eroare la ascultarea clientilor");
        exit(1);
    }

    printf("%s\n", "[SERVER]Ma conectez la baza de date");

    con = mysql_init(NULL);
    if (mysql_real_connect(con, "fenrir.info.uaic.ro", "RCPR", "aCidoti4LV", "RCPR", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    else
    {
        printf("%s\n", "[SERVER]Conexiunea cu baza de date a reusit");
    }

    while (true)
    {
        int clientAcceptStatus;
        socklen_t clientAdressLength = sizeof(clientAdress);

        printf("[SERVER]Asteptam la portul %d... \n", PORT);
        fflush(stdout);

        clientAcceptStatus = accept(socketFileDescriptor, (struct sockaddr *)&clientAdress, &clientAdressLength);

        printf("[SERVER]S-a conectat clientul cu descriptorul %d \n", clientAcceptStatus);

        int childPid = fork();

        if (childPid < 0)
        {
            perror("[SERVER]Eroare la creearea unui procesc copil");
        }
        else if (childPid > 0)
        {
            continue;
        }
        else if (childPid == 0)
        {

            if (clientAcceptStatus < 0)
            {
                perror("[SERVER]Eroare la acceptarea unui client");
                continue;
            }
            int userPermissions = 0;
            int userRolePermissions = 0;    
            while (true)
            {
                bzero(clientMessage, 100);
                bzero(clientResponse, 100);
                printf("[SERVER]Astept mesaje de la clienti \n");
                fflush(stdout);

                int messageReadWriteStatus = read(clientAcceptStatus, clientMessage, 100);

                if (messageReadWriteStatus < 0)
                {
                    perror("[SERVER]Eroare in citirea mesajului de la client");
                    break;
                }
                else if (messageReadWriteStatus == 0)
                {
                    printf("[SERVER]Client deconectat in conditii fortate ce avea descriptorul %d \n", clientAcceptStatus);
                    close(clientAcceptStatus);
                    break;
                }

                if (!strcmp(clientMessage, "quit"))
                {
                    printf("[SERVER]Client deconectat in conditii normale");
                }
                if (!strcmp(clientMessage, "logout"))
                {
                    strcpy(clientResponse,"Te-ai delogat cu suces");
                    userRolePermissions=0;
                    userPermissions=0;
                }

                printf("[SERVER]Urmatorul mesaj a fost primit: %s\n", clientMessage);

                if (!strcmp(clientMessage, "login"))
                {
                    if (userPermissions != 0)
                    {
                        strcat(clientResponse, "Esti deja logat");
                    }
                    else
                    {
                        bzero(userName, 100);
                        bzero(userPass, 100);
                        fflush(stdout);

                        int userReadWriteStatus = read(clientAcceptStatus, userName, sizeof(userName));

                        if (userReadWriteStatus < 0)
                        {
                            perror("[SERVER]Eroare la citirea user-ului");
                        }

                        int userPassReadWriteStatus = read(clientAcceptStatus, userPass, sizeof(userPass));

                        if (userPassReadWriteStatus < 0)
                        {
                            perror("[CLIENT]Eroare la citirea parolei");
                        }

                        userRolePermissions = loginUser(userName, userPass, clientResponse, con);

                        
                        if (userRolePermissions != -1)
                        {
                            userPermissions = 1;
                            if(userRolePermissions == 1)
                            {
                                strcpy(clientResponse, "logat_publish");
                            }
                            if(userRolePermissions == 2)
                            {
                                strcpy(clientResponse, "logat_subscriber");
                            }
                        }
                    }
                }

                if (!strcmp(clientMessage, "register"))
                {
                    if (userPermissions != 0)
                    {
                        strcat(clientResponse, "Esti deja logat");
                    }
                    bzero(userName, 100);
                    bzero(userPass, 100);
                    bzero(userRole, 100);
                    fflush(stdout);

                    int userReadWriteStatus = read(clientAcceptStatus, userName, sizeof(userName));
                    printf("%s\n", userName);
                    int userPassReadWriteStatus = read(clientAcceptStatus, userPass, sizeof(userPass));
                    printf("%s\n", userPass);
                    int userRoleReadWriteStatus = read(clientAcceptStatus, userRole, sizeof(userRole));
                    printf("%s\n", userRole);

                    if (userReadWriteStatus < 0 || userPassReadWriteStatus < 0 || userRoleReadWriteStatus < 0)
                    {
                        perror("[SERVER]Eroare la citirea user-ului sau parolei sau rolului necesare pentru inregistrare");
                    }

                    registerUser(userName, userPass, userRole, clientResponse, con);
                }

                if (!strcmp(clientMessage, "publish"))
                {
                    if (userPermissions != 1)
                    {
                        strcpy(clientResponse, "[SERVER]Nu esti logat, nu poti sa publici articole");
                        goto errors;
                    }
                    if (userRolePermissions == 2)
                    {
                        strcpy(clientResponse, "[SERVER]Nu ai rolul de publish pentru a putea publica articole");
                        goto errors;
                    }
                    bzero(articleTopic, 100);
                    bzero(articleTitle, 100);
                    bzero(articleText, 100);
                    bzero(articleDetails, 100);

                    int articleReadWriteStatus = read(clientAcceptStatus, articleDetails, sizeof(articleDetails));

                    if (articleReadWriteStatus < 0)
                    {
                        perror("[SERVER]Eroare la citirea articolului");
                    }
                    articleTokenSeparator(articleTopic, articleTitle, articleText, articleDetails);

                    publishArticle(articleTopic, articleTitle, articleText, clientResponse, con);
                }

                if (!strcmp(clientMessage, "subscribe"))
                {
                    if (userPermissions != 1)
                    {
                        strcat(clientResponse, "[SERVER]Nu esti logat, nu poti sa dai subscribe la articole");
                        goto errors;
                    }
                    if (userRolePermissions == 1)
                    {
                        strcat(clientResponse, "[SERVER]Nu esti subscriber ca sa poti da subscribe la articole");
                        goto errors;
                    }
                    bzero(articleTopic, 100);

                    int articleReadWriteStatus = read(clientAcceptStatus, articleTopic, sizeof(articleTopic));

                    if (articleReadWriteStatus < 0)
                    {
                        perror("[SERVER]Eroare la citirea topicului de la client");
                    }

                    subscribeArticle(articleTopic, clientResponse, con);
                }

                printf("[SERVER]Trimit inapoi urmatorul mesaj %s\n", clientResponse);
                fflush(stdout);

                errors:messageReadWriteStatus = write(clientAcceptStatus, clientResponse, 100);

                if (messageReadWriteStatus < 0)
                {
                    perror("[SERVER]Eroare la trimiterea mesajului catre client");
                }
                else if (messageReadWriteStatus == 0)
                {
                    printf("[SERVER]Client deconectat in conditii fortate \n");
                    close(clientAcceptStatus);
                    break;
                }
                else
                {
                    printf("[SERVER]Mesajul a fost trimis cu succes catre client \n");
                }
            }
            close(clientAcceptStatus);
        }
    }
}