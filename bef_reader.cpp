// basic encrypted file (BEF) reader
// uses the basic encryption standard (BES)

// v1.0

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

const bool debug = false;

// TOFIX: optimize
const int range_min = 10;
const int range_max = 126;
const int range = range_max - range_min;

// getting file from file_path
std::ifstream get_file(std::string& file_path) {
	// opening file
	std::ifstream in_str(file_path);
	if (!in_str.good()) {
		std::cerr << "ERROR: Unable to open \"" << file_path << "\"!" << std::endl;
		exit(1);
	}

	// returning file
	return in_str; // remember to close!
}

// converting std::string to char* array
char* stoc(std::string str) {
	int size = str.size();
	char* charray = new char[size];
	for (int i = 0; i < size; i++)
		charray[i] = str[i];
	if (debug) {
		std::cout << "Debug: charray: \"";
		for (int i = 0; i < size; i++)
			std::cout << charray[i];
		std::cout << "\"" << std::endl;
	}
	return charray;
} 

// -----------------------------------------------------------------------------------------------------------------------------
// encrypting

// calculating hash value
int get_hash_value(std::string& password) {
	int hash_value = 0;
	int len = password.length();
	for (int i = 0; i < len; i++)
		hash_value += i * password[i]; // TOFIX: improve hash function
	hash_value %= range_max - range_min + 1;
	return hash_value;
}

// encrypting a file
void encrypt(std::string& file_path, std::string& password) {
	// opening file
	std::ifstream in_str(file_path);
	if (!in_str.good()) {
		std::cerr << "ERROR: Unable to open \"" << file_path << "\"!" << std::endl;
		exit(1);
	}
	std::cout << "Encrypting \"" << file_path << "\"..." << std::endl;

	// getting contents
	std::string file_content((std::istreambuf_iterator<char>(in_str)), std::istreambuf_iterator<char>()); // read entire file
	in_str.close();

	// generating encrypted content
	int hash_value = get_hash_value(password); // getting hash value for encryption
	std::string encrypted_content = "";
	int len = file_content.length();
	for (int i = 0; i < len; i++) {
		// simple cipher encryption
		int n = file_content[i];
		n += hash_value;
		if (n > range_max)
			n -= range;
		encrypted_content += char(n);
		if (debug)
			std::cout << "Debug: old: " << file_content[i] << "(" << int(file_content[i]) << "), new: " << char(n) << "(" << n << ")" << std::endl;
	}

	// writing encrypted content
	std::string encrypted_file_path = file_path + ".bef";
	std::ofstream out_str(encrypted_file_path);
	if (!out_str.good()) {
		std::cerr << "ERROR: Unable to open \"" << file_path << "\"!" << std::endl;
		exit(1);
	}
	out_str << encrypted_content;
	std::cout << "\n\"" << file_path << "\" encrypted. See \"" << encrypted_file_path << "\".\n" << std::endl;
	out_str.close();

	// deleting unencrypted file
	try {
		if (debug)
			std::cout << "Debug: Removing \"" << file_path << "\"..." << std::endl;
		std::filesystem::remove(file_path);
	}
	catch (const std::filesystem::filesystem_error& err) {
		std::cerr << "ERROR: Unable to remove \"" << file_path << "\"!: " << err.what() << std::endl;
	}
}

// encrypting a file for the first time
void new_encrypt(std::string& file_path, std::string& password) {
	// opening file
	std::ifstream in_str(file_path);
	if (!in_str.good()) {
		std::cerr << "ERROR: Unable to open \"" << file_path << "\"!" << std::endl;
		exit(1);
	}

	// getting contents
	std::string file_content((std::istreambuf_iterator<char>(in_str)), std::istreambuf_iterator<char>()); // read entire file
	in_str.close();

	// adding password
	std::ofstream out_str(file_path);
	if (!out_str.good()) {
		std::cerr << "ERROR: Unable to open \"" << file_path << "\"!" << std::endl;
		exit(1);
	}
	out_str << password << " (DO NOT EDIT THIS LINE)\n\n";
	out_str << file_content;
	out_str.close();

	// encrypting
	encrypt(file_path, password);
}

// -----------------------------------------------------------------------------------------------------------------------------
// decrypting

// verifying password
bool verify(std::string& file_path, std::string& password) {
	// getting file
	std::ifstream in_str = get_file(file_path);

	// getting encrypted password
	int size = password.size();
	char* encrypted_password = new char[size]; // string of encrypted password
	in_str.read(encrypted_password, size);
	in_str.close();

	// decrypting password
	int hash_value = get_hash_value(password); // getting hash value for encryption
	std::string decrypted_password = "";
	for (int i = 0; i < size; i++) {
		int n = encrypted_password[i];
		n -= hash_value;
		if (n < range_min)
			n += range;
		decrypted_password += char(n);
	}
	if (debug)
		std::cout << "Debug: decrypted_password: " << decrypted_password << std::endl;

	// verifying password
	if (password.compare(decrypted_password) == 0)
		return true; // correct password
	else
		return false; // incorrect password
}

// decrypting file
void decrypt(std::string& file_path, std::string& password) {	
	// getting file content
	std::ifstream in_str = get_file(file_path);
	std::string encrypted_content((std::istreambuf_iterator<char>(in_str)), std::istreambuf_iterator<char>()); // read entire file
	in_str.close();

	// unencrypting content
	int hash_value = get_hash_value(password); // getting hash value for encryption
	std::string decrypted_content = "";
	int size = encrypted_content.size();
	for (int i = 0; i < size; i++) {
		int n = encrypted_content[i];
		n -= hash_value;
		if (n < range_min)
			n += range;
		decrypted_content += char(n);
	}

	// creating unencrypted file
	size = file_path.size();
	std::string decrypted_file_path = file_path.substr(0, size - 4);
	/*
	if (debug)
		std::cout << "Debug: decrypted_file_path: " << decrypted_file_path << std::endl;
	*/
	std::ofstream out_str(decrypted_file_path); // remove .bef
	out_str << decrypted_content;
	out_str.close();
	std::cout << "\n\"" << file_path << "\" decrypted. See \"" << decrypted_file_path << "\".\n" << std::endl;

	// deleting encrypted file
	try {
		if (debug)
			std::cout << "Debug: Removing \"" << file_path << "\"..." << std::endl;
		std::filesystem::remove(file_path);
	}
	catch (const std::filesystem::filesystem_error& err) {
		std::cerr << "ERROR: Unable to remove \"" << file_path << "\"!: " << err.what() << std::endl;
	}

	// let user make changes
	while(true) {
		std::cout << "When finished, type \"done\": ";
		std::string done;
		std::cin >> done;
		if (done.compare("done") == 0)
			break;
	}

	// re-encrypting content
	encrypt(decrypted_file_path, password);
}

// -----------------------------------------------------------------------------------------------------------------------------
int main() {
	// getting option
	std::string option;
	std::cout << "Encrypt/Decrypt? (e/d): ";
	std::cin >> option;
	if (debug)
		std::cout << "Debug: option: " << option << std::endl;

	// encrypt file
	if (option.compare("e") == 0) {
		if (debug)
			std::cout << "Debug: Encrypting file..." << std::endl;

		// getting file
		std::string file_path;
		std::cout << "File: ";
		std::cin >> file_path;

		// getting password
		std::string password;
		std::cout << "Set a password: ";
		std::cin >> password; // TOFIX: hide input as user types password
		if (debug) {
			std::cout << "Debug: file_path: " << file_path << std::endl;
			std::cout << "Debug: password: " << password << std::endl;
		}

		// encrypting
		new_encrypt(file_path, password); // encrypting file
	}

	// decrypt file
	else if (option.compare("d") == 0) {
		if (debug)
			std::cout << "Debug: Decrypting file..." << std::endl;

		// getting file
		std::string file_path;
		std::cout << "File: ";
		std::cin >> file_path;

		// getting password
		std::string password;
		std::cout << "Password: ";
		std::cin >> password;

		// verifying password
		std::cout << "Verifying password..." << std::endl;
		if (verify(file_path, password)) {
			std::cout << "Password verified. Decrypting \"" << file_path << "\"..." << std::endl;

			// decrypting
			decrypt(file_path, password);

			// let user make changes

			// re-encrypting
		}
		else
			std::cout << "ERROR: Incorrect password." << std::endl;
	}

	// invalid option
	else {
		std::cerr << "ERROR: Invalid option." << std::endl;
		return 1;
	}

	return 0; // successful execution
}