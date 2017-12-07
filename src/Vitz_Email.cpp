/*
 * Vitz_Email.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

#include "Vitz_Email.h"

VEmail::VEmail() {
	EmailSocket = NULL;
}

VEmail::~VEmail() {

}

int VEmail::send_email(std::string from_email, std::string password, std::string _hostname, std::vector<std::string> to_email, std::string msg) {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	long int zoneint;
	std::string zonestr;
	std::stringstream convertzone;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	zoneint = timeinfo->tm_gmtoff;
	zoneint = ((zoneint / 60) / 60) * 100;
	convertzone << zoneint;
	convertzone >> zonestr;
	if (zonestr == "0") {
		zonestr = "+0000";
	} else if (zonestr[0] == '-') {
		if (zonestr.size() < 5)
			zonestr.insert(1, 1, '0');
	} else {
		zonestr.insert(0, 1, '+');
		if (zonestr.size() < 5)
			zonestr.insert(1, 1, '0');
	}

	strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", timeinfo);
	std::string time(buffer);

	std::string hostname = _hostname;
	std::string from_id = from_email;
	std::string headers = "From: Vitz Soporte <" + from_id + ">\r\n";
	for(unsigned int i=0; i < to_email.size(); i++){
		headers += "To: <" + to_email.at(i) + ">\r\n";
	}
	headers += "Date: " + time + " " + zonestr + "\r\n"
			"Subject: CTY\r\n";

	headers += msg;

	std::string headersfile;
	std::ifstream fileheader(VE_PathHeadersEmail, std::ios::in | std::ios::binary);
	if (fileheader) {
		fileheader.seekg(0, std::ios::end);
		headersfile.resize(fileheader.tellg());
		fileheader.seekg(0, std::ios::beg);
		fileheader.read(&headersfile[0], headersfile.size());
		fileheader.close();
	}

	std::string temp_header;
	std::istringstream f(headersfile);
	std::string line;
	while (std::getline(f, line)) {
		line = Insert_RN(line);
		temp_header += line;
	}

	headers += temp_header + "\r\n" + msg;

	std::string firma_vitz1;
	std::ifstream firmfile1(VE_PathFirm1Email, std::ios::in | std::ios::binary);
	if (firmfile1) {
		firmfile1.seekg(0, std::ios::end);
		firma_vitz1.resize(firmfile1.tellg());
		firmfile1.seekg(0, std::ios::beg);
		firmfile1.read(&firma_vitz1[0], firma_vitz1.size());
		firmfile1.close();
	}

	std::string temp_firma1;
	std::istringstream f1(firma_vitz1);
	std::string line1;
	while (std::getline(f1, line1)) {
		line1 = Insert_RN(line1);
		temp_firma1 += line1;
	}

	headers += temp_firma1 + msg + "\r\n";

	std::string firma_vitz2;
	std::ifstream firmfile2(VE_PathFirm2Email, std::ios::in | std::ios::binary);
	if (firmfile2) {
		firmfile2.seekg(0, std::ios::end);
		firma_vitz2.resize(firmfile2.tellg());
		firmfile2.seekg(0, std::ios::beg);
		firmfile2.read(&firma_vitz2[0], firma_vitz2.size());
		firmfile2.close();
	}

	std::string temp_firma2;
	std::istringstream f2(firma_vitz2);
	std::string line2;
	while (std::getline(f2, line2)) {
		line2 = Insert_RN(line2);
		temp_firma2 += line2;
	}

	headers += temp_firma2;

	EmailSocket = new VSocket(hostname, 465, false, true);
	VPackchar* paquetesrec = NULL;

	if (!EmailSocket->get_status()) {
		delete EmailSocket;
		delete paquetesrec;
		EmailSocket = NULL;
		paquetesrec = NULL;

		return -1;
	} else {
		paquetesrec = EmailSocket->read();
		if (paquetesrec->size == 0) {
			delete EmailSocket;
			delete paquetesrec;
			EmailSocket = NULL;
			paquetesrec = NULL;

			return -1;
		}
		delete paquetesrec;

		if (send_pack(VE_HELO) == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack(VE_AUTH_LOGIN) == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack(base64(from_email)) == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack(base64(password)) == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack("MAIL FROM: " + from_id + "\r\n") == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		for(unsigned int i=0; i < to_email.size(); i++){
			if (send_pack("RCPT TO: <" + to_email.at(i) + ">\r\n") == -1) {
				delete EmailSocket;
				EmailSocket = NULL;
				return -1;
			}
		}
		if (send_pack("DATA\r\n") == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack(headers + "\r\n.\r\n") == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
		if (send_pack(VE_QUIT) == -1) {
			delete EmailSocket;
			EmailSocket = NULL;
			return -1;
		}
	}

	delete EmailSocket;
	EmailSocket = NULL;

	return 0;
}

int VEmail::send_pack(std::string msg) {
	VPackchar* paquetes = new VPackchar();
	VPackchar* paquetesrec;

	paquetes->init_withString(msg);
	EmailSocket->send(paquetes);
	paquetesrec = EmailSocket->read();
	if (paquetesrec->size == 0) {
		delete paquetesrec;
		delete paquetes;
		paquetesrec = NULL;
		paquetes = NULL;
		return -1;
	}

	std::cout << paquetesrec->Data;
	delete paquetesrec;
	delete paquetes;
	paquetesrec = NULL;
	paquetes = NULL;
	return 0;
}

std::string VEmail::base64(std::string input){
	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, input.c_str(), input.size());
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	std::string stringb64(bptr->data, bptr->length);
	stringb64 = Insert_RN(stringb64);

	BIO_free_all(b64);

	return stringb64;
}

std::string VEmail::Insert_RN(std::string str){
	std::size_t found = str.find("\r\n");
	if (found == std::string::npos){
		std::size_t foundr = str.find("\r");
		std::size_t foundn = str.find("\n");
		if ((foundr == std::string::npos) && (foundn == std::string::npos)){
			if(str.size() > VE_Max_Line_Protocol){
				return "\r\n";
			}else{
				str += "\r\n";
			}
		}else if(foundr != std::string::npos){
			str.insert(foundr+1, "\n");
		}else if(foundn != std::string::npos){
			str.insert(foundn, "\r");
		}
	}

	return str;
}
