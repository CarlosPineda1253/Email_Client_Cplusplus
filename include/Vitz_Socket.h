/*
 * Vitz_Socket.h
 *
 *  Created on: Nov 12, 2016
 *      Author: root
 */

#ifndef INCLUDE_VITZ_SOCKET_H_
#define INCLUDE_VITZ_SOCKET_H_

#include <iostream>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "Vitz_Pack_char.h"
#include "Config.h"

class VSocket{
	public:
		//Constructor
		VSocket(std::string hostname, int port, bool is_Server, bool is_Secure);

		//Destructor
		virtual ~VSocket();

		//Send bytes
		int send(VPackchar*& msg);

		//Read bytes
		VPackchar* read();

		//Conect with host SSL/TLS
		int connect_secure(const char *hostname, int port);

		//Connect with host
		int connect_insecure(const char *hostname, int port);

		//Disconnect with host SSL/TLS
		void disconnect(SSL *ssl, SSL_CTX *ctx, int fd_socket);

		//Disconnect with host
		void disconnect(int fd_socket);

		//Get status of socket
		bool get_status();

		//Get SSL object
		SSL* get_SSL();

	protected:
		//Socket file descriptor
		int fd_socket;

		//Context for SSL
		SSL_CTX *ctx;

		//Server or client
		bool is_Server;

		//Socket SSL/TLS or insecure
		bool is_Secure;

		//Socket create and connect
		bool is_Connect;

	private:
		//Number of attempts
		#define Num_attempts 10

		//Hostname
		std::string hostname;

		//Port
		int port;

		//SSL socket
		SSL *ssl;

		//Init context SSL/TSL
		SSL_CTX* initctx();
};



#endif /* INCLUDE_VITZ_SOCKET_H_ */
