/*
 * Vitz_Pack.h
 *
 *  Created on: Nov 10, 2016
 *      Author: root
 */

#ifndef INCLUDE_VITZ_PACK_CHAR_H_
#define INCLUDE_VITZ_PACK_CHAR_H_

#include <string>

class VPackchar{
	public:
		//Size of buffer
		int size;

		//Buffer
		unsigned char *Data;

		//Constructor
		VPackchar();

		//Destructor
		virtual ~VPackchar();

		//Set data
		void init_withString(std::string s);

	protected:

	private:

};



#endif /* INCLUDE_VITZ_PACK_CHAR_H_ */
