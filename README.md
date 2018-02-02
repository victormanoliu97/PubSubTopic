# About 
The main goal of this project is to implement a simple system of publish&subscribe between a client and a server. The client can be either a publisher and publish articles on predefined topics or a subscriber and it can subscribe to the same predefined topics.

# Arhitecture
The application is formed of 2 components: the server and a client with his 2 possible roles. The communication between them is made through BSD Sockets, the server being a concurrent one, his concurrency being provided through the fork() primitive. The articles, topics and other user information required to run this application are stored in a MySQL Database.

# Installing
In order to run this application, you will need to have a Unix Operating System and MySql Client installed.
After you made sure you met those 2 requirements: 

1) Clone the repo on your personal computer using:
`git clone`

2) Go in the folder that you copied the repo using:
`cd FolderName`

3) Run the Makefile using:
`make`

4) Run the server and client executables using:
`./server`  `./client`
