#ifndef BES_H
#define BES_H

#include <string>

class BES {
public:
	BES(std::string& p);
	void new_encrypt(std::string& file_path);
	void encrypt(std::string& file_path);
	void decrypt(std::string& file_path);
	int get_hash_value() const {return hash_value;}
private:
	std::string password;
	int hash_value = 0;

	// character range (inclusive)
	int range_min = 33;
	int range_max = 126;
};

#endif