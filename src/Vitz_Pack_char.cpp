/*
 * Vitz_Pack_char.cpp
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

#include "Vitz_Pack_char.h"

VPackchar::VPackchar(){
	size = 0;
	Data = NULL;
}

VPackchar::~VPackchar(){
	size = 0;
	delete []Data;
	Data = NULL;
}

void VPackchar::init_withString(std::string s){
	size = 0;
	delete []Data;
	Data = NULL;

	size = s.size();
	const char *temp = s.c_str();
	Data = new unsigned char[size + 1];
	for(int i = 0; i < size; i++){
		Data[i] = temp[i];
	}
}



