all:
	g++ BSocket.cpp Article.cpp User.cpp TokenManager.cpp server.cpp -o server -I/usr/include/mysql -lmysqlclient -g
	g++ BSocket.cpp TokenManager.cpp InformationHelper.cpp client.cpp -o client -I/usr/include/mysql -lmysqlclient -g
