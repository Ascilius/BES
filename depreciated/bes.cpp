#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <utility>

#include "bes.h"

BES::BES(std::string& p) {
	password = p;
	
}

/*
33 - 126 (inclusive)
scrambles even with same characters (positional hashing)
get min/max hashed values and scale

// cipher
 - most basic
 - least secure
 - chance of just not encrypting at all
*/

// encrypting file for first time
void BES::new_encrypt(std::string& file_path) {
	
}

// 
void BES::encrypt(std::string& file_path) {
}

void BES::decrypt(std::string& file_path) {

}