#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <cstdlib>
#include "BSocket.h"
#include "Defines.h"
#include "TokenManager.h"
#include "InformationHelper.h"

void publish(int);
void subscribe(int);

int main()
{
    int socketFileDescriptor;
    struct sockaddr_in serverAdress;
    char clientMessage[100];
    char userName[100];
    char userPass[100];
    char userRole[100];

    char articleText[100];
    char articleTopic[100];
    char articleTitle[100];
    char articleDetails[300];

    //createConnectSocket(socketFileDescriptor, serverAdress);
    socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socketFileDescriptor == -1)
    {
        perror("[CLIENT]Eroare in creearea socket-ului");
        exit(1);
    }

    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = inet_addr(adresaIP);
    serverAdress.sin_port = htons(PORT);

    if (connect(socketFileDescriptor, (struct sockaddr *)&serverAdress, sizeof(serverAdress)) == -1)
    {
        perror("[CLIENT]Eroare in conectarea catre server");
        exit(1);
    }

    while (true)
    {
        bzero(clientMessage, 100);
        bzero(userName, 100);
        bzero(userPass, 100);
        bzero(articleTopic, 100);
        bzero(articleTitle, 100);
        bzero(articleText, 100);
        bzero(articleDetails, 100);
    introduceComand:
        printf("[CLIENT]Introduceti o comanda: ");
        fflush(stdout);
        read(0, clientMessage, 100);

        clientMessage[strlen(clientMessage) - 1] = '\0';
        int messageReadWriteStatus = write(socketFileDescriptor, clientMessage, 100);

        if (messageReadWriteStatus < 0)
        {
            perror("[CLIENT]Eroare la trimiterea mesajului catre server");
        }
        else if (messageReadWriteStatus == 0)
        {
            perror("[CLIENT]Serverul s-a inchis sau deconectat");
        }
        if (!strcmp(clientMessage, "exit"))
        {
        ext:
            exit(1);
        }

        //login
        if (!strcmp(clientMessage, "login"))
        {
            printf("[CLIENT]Introduceti user-ul cu care sunteti inregistrat: \n");
            fflush(stdout);
            bzero(userName, MAXUSERLENGTH);
            read(0, userName, 100);

            int userReadWriteStatus = write(socketFileDescriptor, userName, sizeof(userName));

            if (userReadWriteStatus < 0)
            {
                perror("[CLENT]Eroare");
            }

            printf("[CLIENT]Introduceti parola cu care sunteti inregistrat: \n");
            fflush(stdout);
            bzero(userPass, 100);
            read(0, userPass, 100);

            int userPassReadWriteStatus = write(socketFileDescriptor, userPass, sizeof(userPass));

            if (userPassReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare");
            }

            bzero(clientMessage, sizeof(clientMessage));
            messageReadWriteStatus = read(socketFileDescriptor, clientMessage, 100);
            if (messageReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare in citirea mesajului de la server");
            }
            if (messageReadWriteStatus == 0)
            {
                perror("[CLIENT]Serverul s-a inchis sau deconectat");
                exit(1);
            }
            else
            {
                printf("[CLIENT]Mesajul primit de la server este: %s\n", clientMessage);
            }
            char status[10];
            char permision[10];
            char aux[100];
            bzero(status, sizeof(status));
            bzero(permision, sizeof(permision));

            loginTokenSeparator(aux, clientMessage, status, permision);

            if (!strcmp(status, "logat"))
            {
                if (!strcmp(permision, "publish"))
                {
                    printf("[CLIENT]Esti logat ca si publisher. Poti publica articole\n");
                    fflush(stdout);
                    publish(socketFileDescriptor);
                }
                else
                {
                    printf("[CLIENT]Esti logat ca si subscriber. Poti da subscribe la topic-uri\n");
                    fflush(stdout);
                    subscribe(socketFileDescriptor);
                }
            }
            else
            {
                goto introduceComand;
            }
        }

        if (!strcmp(clientMessage, "register"))
        {
            printf("[CLIENT]Introduceti user-ul cu care doriti sa va inregistrati: \n");
            fflush(stdout);
            bzero(userName, 100);
            read(0, userName, 100);

            int userReadWriteStatus = write(socketFileDescriptor, userName, sizeof(userName));

            if (userReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare la scrierea user-ului catre server");
            }

            printf("[CLIENT]Introduceti parola cu care doriti sa va inregistrati: \n");
            fflush(stdout);
            bzero(userPass, 100);
            read(0, userPass, 100);

            int userPassReadWriteStatus = write(socketFileDescriptor, userPass, sizeof(userPass));

            if (userPassReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare la scrierea parolei catre server");
            }

            printf("[CLIENT]Introduceti rolul care doriti sa il aveti 1.Publisher sau 2.Subscriber : \n");
            fflush(stdout);
            bzero(userRole, 100);
            read(0, userRole, 100);

            int userRoleReadWriteStatus = write(socketFileDescriptor, userRole, sizeof(userRole));

            if (userRoleReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare la scrierea rolului catre server");
            }
        }

        bzero(clientMessage, 100);
        messageReadWriteStatus = read(socketFileDescriptor, clientMessage, 100);

        if (messageReadWriteStatus < 0)
        {
            perror("[CLIENT]Eroare in citirea mesajului de la server");
        }
        if (messageReadWriteStatus == 0)
        {
            perror("[CLIENT]Serverul s-a inchis sau deconectat");
            exit(1);
        }
        else
        {
            printf("[CLIENT]Mesajul primit de la server este: %s\n", clientMessage);
        }
    }
    close(socketFileDescriptor);
}

//publish
void publish(int socketFileDescriptor)
{
    char clientMessage[100];
    char userName[100];
    char userPass[100];
    char userRole[100];

    char articleText[100];
    char articleTopic[100];
    char articleTitle[100];
    char articleDetails[300];

    while (1)
    {
        bzero(clientMessage, 100);
        printf("[CLIENT]Introduceti o comanda:");
        fflush(stdout);
        read(0, clientMessage, 100);
        clientMessage[strlen(clientMessage) - 1] = '\0';
        int messageReadWriteStatus = write(socketFileDescriptor, clientMessage, 100);
        if (messageReadWriteStatus < 0)
        {
            perror("[CLIENT]Eroare la trimiterea mesajului catre server");
        }
        else if (messageReadWriteStatus == 0)
        {
            perror("[CLIENT]Serverul s-a inchis sau deconectat");
        }
        if (!strcmp(clientMessage, "publish"))
        {
            bzero(articleDetails, 100);
            showPublishInformation();
            fflush(stdout);
            bzero(articleTopic, 100);
            read(0, articleTopic, 100);

            printf("[CLIENT]Introduceti titlul articolului pe care doriti sa il postati \n");
            fflush(stdout);
            bzero(articleTitle, 100);
            read(0, articleTitle, 100);

            printf("[CLIENT]Introduceti textul articolului pe care doriti sa il postati \n");
            fflush(stdout);
            bzero(articleText, 100);
            read(0, articleText, 100);

            addArticleTokenSeparator(articleTopic, articleTitle, articleText, articleDetails);

            fflush(stdout);

            int articleReadWriteStatus = write(socketFileDescriptor, articleDetails, sizeof(articleDetails));

            if (articleReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare la scrierea articolului catre server");
            }
            bzero(clientMessage, 100);
            messageReadWriteStatus = read(socketFileDescriptor, clientMessage, 100);

            if (messageReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare in citirea mesajului de la server");
            }
            if (messageReadWriteStatus == 0)
            {
                perror("[CLIENT]Serverul s-a inchis sau deconectat");
                exit(1);
            }
            else
            {
                printf("[CLIENT]Mesajul primit de la server este: %s\n", clientMessage);
            }
        }
        else if (!strcmp(clientMessage, "logout"))
        {
            break;
        }
        else if (!strcmp(clientMessage, "subscribe"))
        {
            printf("[CLIENT]Nu ai rolul de subcriber ca sa poti da subcribe la topicuri \n");
        }
        else
        {
            printf("[CLIENT]Ai introdus o comanda gresita \n");
        }
    }
}
void subscribe(int socketFileDescriptor)
{
    char clientMessage[100];
    char userName[100];
    char userPass[100];
    char userRole[100];

    char articleText[100];
    char articleTopic[100];
    char articleTitle[100];
    char articleDetails[300];
    while (1)
    {
        bzero(clientMessage, 100);
        printf("[CLIENT]Introduceti o comanda:");
        fflush(stdout);
        read(0, clientMessage, 100);
        clientMessage[strlen(clientMessage) - 1] = '\0';
        int messageReadWriteStatus = write(socketFileDescriptor, clientMessage, 100);
        if (messageReadWriteStatus < 0)
        {
            perror("[CLIENT]Eroare la trimiterea mesajului catre server");
        }
        else if (messageReadWriteStatus == 0)
        {
            perror("[CLIENT]Serverul s-a inchis sau deconectat");
        }
        if (!strcmp(clientMessage, "subscribe"))
        {
            showSubscriberInformation();
            fflush(stdout);
            bzero(articleTopic, 100);
            read(0, articleTopic, 100);

            int articleTopicReadWrite = write(socketFileDescriptor, articleTopic, sizeof(articleTopic));

            if (articleTopicReadWrite < 0)
            {
                perror("[CLIIENT]Eroare");
            }
            bzero(clientMessage, 100);
            messageReadWriteStatus = read(socketFileDescriptor, clientMessage, 100);

            if (messageReadWriteStatus < 0)
            {
                perror("[CLIENT]Eroare in citirea mesajului de la server");
            }
            if (messageReadWriteStatus == 0)
            {
                perror("[CLIENT]Serverul s-a inchis sau deconectat");
                exit(1);
            }
            else
            {
                printf("[CLIENT]Mesajul primit de la server este: %s\n", clientMessage);
            }
        }
        else if (!strcmp(clientMessage, "logout"))
        {
            break;
        }
        else if (!strcmp(clientMessage, "publish"))
        {
            printf("[CLIENT]Nu ai rolul de publisher ca sa poti publica articole \n");
        }
        else
        {
            printf("[CLIENT]Ai introdus o comanda gresita \n");
        }
    }
}
