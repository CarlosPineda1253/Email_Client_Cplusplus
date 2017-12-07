/*
 * Vitz_Socket.cpp
 *
 *  Created on: Nov 12, 2016
 *      Author: root
 */

#include <Vitz_Socket.h>

VSocket::VSocket(std::string hostname, int port, bool is_Server, bool is_Secure){
	VSocket::hostname = hostname;
	VSocket::port = port;
	ssl = NULL;
	fd_socket = 0;
	ctx = NULL;
	VSocket::is_Server = is_Server;
	VSocket::is_Secure = is_Secure;

	if(VSocket::is_Secure){
		SSL_library_init();
		ctx = initctx();
		fd_socket = connect_secure(VSocket::hostname.c_str(), VSocket::port);
		if(fd_socket == -1){
			SSL_free(ssl);
			close(fd_socket);
			SSL_CTX_free(ctx);
			is_Connect = false;
			return;
		}
		is_Connect = true;
	}else{
		fd_socket = connect_insecure(VSocket::hostname.c_str(), VSocket::port);
		if(fd_socket == -1){
			is_Connect = false;
		}
		is_Connect = true;
	}
}

VSocket::~VSocket(){
	if(is_Secure){
		disconnect(ssl, ctx, fd_socket);
	}else{
		disconnect(fd_socket);
	}
}

int VSocket::send(VPackchar*& msg){
	int ret;
	if(is_Secure){
		ret = SSL_write(ssl, msg->Data, msg->size);
	}else{
		ret = write(fd_socket, msg->Data, msg->size);
	}
	return ret;
}

VPackchar* VSocket::read(){
	int attempts = 0;
	VPackchar* msg = new VPackchar();
	if(is_Secure){
		while(msg->size == 0){
			SSL_read(ssl, NULL, 0);
			msg->size = SSL_pending(ssl);
		}
		msg->Data = new unsigned char[msg->size];
		int ret = SSL_read(ssl, msg->Data, msg->size);
		if(ret <= 0)
			msg->size = -1;
	}else{
		while(msg->size == 0){
			ioctl(fd_socket,FIONREAD,&msg->size);
			if(attempts > Num_attempts)
				return msg;
		}
		msg->Data = new unsigned char[msg->size];
		int ret = recv(fd_socket, msg->Data, msg->size , 0);
		if(ret <= 0)
			msg->size = -1;
	}

	return msg;
}

int VSocket::connect_secure(const char *hostname, int port){
	int sd;
	struct hostent *host;
	struct sockaddr_in addr;

	if ( (host = gethostbyname(hostname)) == NULL )
	{
		perror(hostname);
		return -1;
	}

	sd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);

	if(is_Server){
		if (sd < 0) {
			close(sd);
			perror("Unable to create socket");
			return -1;
		}

		if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			close(sd);
			perror("Unable to bind");
			return -1;
		}

		if (listen(sd, 1) < 0) {
			close(sd);
			perror("Unable to listen");
			return -1;
		}
	}else{
		if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ){
			close(sd);
			perror(hostname);
			return -1;
		}
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl, sd);
		if ( SSL_connect(ssl) == -1 ){   /* perform the connection */
			close(sd);
			ERR_print_errors_fp(stderr);
			return -1;
		}
	}
	return sd;
}

int VSocket::connect_insecure(const char *hostname, int port){
	int sd;
	struct hostent *host;
	struct sockaddr_in addr;

	if ( (host = gethostbyname(hostname)) == NULL )
	{
		perror(hostname);
	}
	sd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*)(host->h_addr);

	if(is_Server){
		if (sd < 0) {
			close(sd);
			perror("Unable to create socket");
			return -1;
		}

		if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			close(sd);
			perror("Unable to bind");
			return -1;
		}

		if (listen(sd, 1) < 0) {
			close(sd);
			perror("Unable to listen");
			return -1;
		}
	}else{
		if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ){
			close(sd);
			perror(hostname);
			return -1;
		}
	}

	return sd;
}

void VSocket::disconnect(SSL *ssl, SSL_CTX *ctx, int fd_socket){
	SSL_free(ssl);
	close(fd_socket);
	SSL_CTX_free(ctx);
}

void VSocket::disconnect(int fd_socket){
	close(fd_socket);
}

SSL_CTX* VSocket::initctx(){
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
	SSL_load_error_strings();   /* Bring in and register error messages */
	if(is_Server){
		method = TLSv1_2_server_method();
	}else{
		method = TLSv1_2_client_method();  /* Create new client-method instance */
	}
	ctx = SSL_CTX_new(method);   /* Create new context */
	if ( ctx == NULL )
	{
		ERR_print_errors_fp(stderr);
	}

	if(is_Server){
		//SSL_CTX_set_ecdh_auto(ctx, 1);

		/* set the local certificate from CertFile */
		if ( SSL_CTX_use_certificate_file(ctx, CTY_Cert_PIC_Server, SSL_FILETYPE_PEM) <= 0 )
		{
			ERR_print_errors_fp(stderr);
			abort();
		}
		/* set the private key from KeyFile (may be the same as CertFile) */
		if ( SSL_CTX_use_PrivateKey_file(ctx, CTY_Key_PIC_Server, SSL_FILETYPE_PEM) <= 0 )
		{
			ERR_print_errors_fp(stderr);
			abort();
		}
		/* verify private key */
		if ( !SSL_CTX_check_private_key(ctx) )
		{
			fprintf(stderr, "Private key does not match the public certificate\n");
			abort();
		}
	}

	return ctx;
}

bool VSocket::get_status(){
	return is_Connect;
}

SSL* VSocket::get_SSL(){
	return ssl;
}
