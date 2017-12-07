/*
 * Vitz_Email.h
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

#ifndef INCLUDE_VITZ_EMAIL_H_
#define INCLUDE_VITZ_EMAIL_H_

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <vector>
#include "Vitz_Socket.h"
#include "Config.h"

#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


class VEmail{
	public:
		//Constructor
		VEmail();

		//Destructor
		virtual ~VEmail();

		//Enviar email
		int send_email(std::string from_email, std::string password, std::string _hostname, std::vector<std::string> to_email, std::string msg);

	protected:

	private:
		#define VE_HELO "EHLO vitz.com.mx\r\n"
		#define VE_AUTH_LOGIN "AUTH LOGIN\r\n"
		#define VE_DATA "DATA\r\n"
		#define VE_QUIT "QUIT\r\n"
		//Path to firm email
		#define VE_PathHeadersEmail CTY_PathProject "/Resources/Files/headers_email.txt"
		#define VE_PathFirm1Email CTY_PathProject "/Resources/Files/firm_email1.txt"
		#define VE_PathFirm2Email CTY_PathProject "/Resources/Files/firm_email2.txt"

		#define VE_Max_Line_Protocol 999

		VSocket* EmailSocket;

		int send_pack(std::string msg);

		std::string base64(std::string input);

		std::string Insert_RN(std::string str);
};



#endif /* INCLUDE_VITZ_EMAIL_H_ */
