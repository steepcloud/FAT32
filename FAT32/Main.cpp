#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <sstream>
#include <bitset>
#include <Windows.h>

#pragma warning(disable:4996)

using namespace std;
//FAT32

// Setting ROOT table which holds 340 files
// [0, 255] values for FAT table; [256, 595] ROOT table; [596, 4095] HDD

constexpr auto Nr_UA = 4096; // allocation units number
constexpr auto UA = 32; // alloc. units size
constexpr auto nrFisROOT = 340; // Setting 340 files for ROOT table
unsigned int Dim_HDD, Dim_FAT; // 0 to 4,294,967,295 values
unsigned short int FAT[Nr_UA]; // declaring FAT table
unsigned short int stRootHDx = 0, stRootHDy = 0, stHDx = 0, stHDy = 0; // start positions of root from HD's FAT zone and HD's
std::vector<std::vector<char>>HDD(Nr_UA, std::vector<char>(UA)); // delcaring HDD matrix with 32 UA

// TODO: add an output file where you save and update all the content of the hard drive

/*
* ROOT:
* Name - 17 bytes
* Extension - 4 bytes
* Size - 2 bytes
* First UA - 2 bytes
* ATTR - 1 byte
* Add date - 3 bytes
* Modify date - 3 bytes
*/

int firstUA() { // function that traverses the FAT table and returns the position of first UA available on HD, if not found, return -1
	for (int pos = nrFisROOT; pos < Nr_UA; pos++) {
		if (!FAT[pos])
			return pos;
	}
	return -1; // no available space on HD
}

int getFirstROOTEntry() { // function which returns first position where a file can be stored in ROOT table
	unsigned short int startROOT = Dim_FAT / UA;

	for (unsigned short int i = startROOT; i < startROOT + nrFisROOT; i++) {
		if (HDD[i][0] == '\0')
			return i;
	}
	return -1;
}

int getFirstUA(unsigned int pos) { // function which returns first UA stored in ROOT file from position @pos
	char highByte = HDD[pos][23];
	char lowByte = HDD[pos][24];

	unsigned short int value = (static_cast<unsigned char>(highByte) << 8) | static_cast<unsigned char>(lowByte);
	return value;
}

bool checkAvailableSpace(int filesize) { // function that returns if there is space available for a file to be created
	int neededUA = filesize % UA ? filesize / UA + 1 : filesize / UA,
		countUA = 0;
	
	int lenFAT = Dim_FAT / UA;

	for (int index = nrFisROOT + lenFAT; index < Nr_UA; index++) {
		if (!FAT[index])
			countUA++;
	}
	return neededUA <= countUA;
}

bool checkAvailableSpaceROOT() { // function that check if there's space in ROOT zone
	unsigned short int startROOT = Dim_FAT / UA;

	for (unsigned short int i = startROOT; i < startROOT + nrFisROOT; i++) {
		if (HDD[i][0] == '\0') 
			return true;
	}
	return false;
}

string getFileName(const string& fileName) { // function that extracts from a string of the form file.extension, the file name
	size_t dotIndex = fileName.find('.');

	return fileName.substr(0, dotIndex);
}

void populateHDD(int primaUA, int neededUA, string option, int bytes) { // the function for embedding the content of the files on the hard-disk
	vector<char> hexDigits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	int num_mod;
	char c;

	if (option == "alfa") { // 'a' + index % 26
		c = 'a';
		num_mod = 26;
	}
	else if (option == "num") { // '0' + index % 10
		c = '0';
		num_mod = 10;
	}
	int count = 0;
	for (int i = 0; i < neededUA; i++) {
		int choose = (bytes < UA) ? bytes : UA;
		
		for (int j = 0; j < choose; j++) {
			if (option == "hex") {
				HDD[primaUA][j] = hexDigits[j % hexDigits.size()];
			}
			else {
				HDD[primaUA][j] = c + count % num_mod;
				count++;
			}
		}
		if (bytes < UA) {
			FAT[primaUA] = 3;
		}
		else {
			int auxUA = primaUA;
			FAT[auxUA] = 4;
			primaUA = firstUA();
			FAT[auxUA] = primaUA;
		}
		bytes -= UA;
	}
}

bool checkFileName(string fileName) { // method that checks if a file name already exists (false - already is)
	unsigned short int startROOT = Dim_FAT / UA; // 256
	string name;

	for (int i = startROOT; i < startROOT + nrFisROOT; i++) {
		name = "";
		for (int j = 0; j < 17; j++) {
			name += HDD[i][j];
		}

		if (fileName == name) {
			return false;
		}
	}
	return true;
}

bool checkFileNameAndExtension(string fileName) { // method that checks if a file name and extension already exists (false - already is)
	unsigned short int startROOT = Dim_FAT / UA; // 256
	string name;

	for (int i = startROOT; i < startROOT + nrFisROOT; i++) {
		name = "";
		
		for (int j = 0; j < 21; j++) {
			if (HDD[i][j] != '\0')
				name += HDD[i][j];
			
			if (j == 16)
				name += ".";
		}

		if (fileName == name) {
			return false;
		}
	}

	return true;
}

string getFileNameAndExtension(int pos) { // method that returns the file name and extension of the element at position @pos
	string name = "";
	
	if (HDD[pos][0] == '?' || HDD[pos][0] == '\0')
		return name;
	else {
		for (int j = 0; j < 21; j++) {
			if (HDD[pos][j] != '\0')
				name += HDD[pos][j];
			
			if (j == 16) // must be of the form file_name.extension <------ check if necessary or just remove
				name += ".";
		}
	return name;
	}
}

string getDates(int pos) { // method that returns a string containing the creation and modification date of the file at position @pos
	string dateCreation = "";
	
	for (int j = 26; j < UA; j++) {
		dateCreation += HDD[pos][j];
	}
	
	return dateCreation;
}

string toUpper(string fileName) { // method that converts a string to uppercase
	string fileNametoUpper = fileName;

	transform(fileNametoUpper.begin(), fileNametoUpper.end(), fileNametoUpper.begin(), [](unsigned char c) {
		return toupper(c);
	});

	return fileNametoUpper;
}

int searchFileNameAndExtension(string NameExt) { // method that searches in the ROOT table for the file with the name and extension @NameExt (returns the position if it exists)
	unsigned short int startROOT = Dim_FAT / UA;
	string name;

	for (int i = startROOT; i < startROOT + nrFisROOT; i++) {
		name = "";
		for (int j = 0; j < 21; j++) {
			if (HDD[i][j] != '\0')
				name += HDD[i][j];

			if (j == 16) // file_name.extension
				name += ".";
		}

		if (NameExt == name) {
			return i;
		}
	}
	return -1;
}

int getLength(int pos) {
	int length = 0;

	for (int i = 0; i < UA; i++) {
		if (HDD[pos][i])
			length++;
	}
	return length;
}

void clearHD(int pos) {
	for (int i = 0; i < getLength(pos); i++) {
		HDD[pos][i] = NULL;
	}
}

void clearFAT(int pos) { // recursive method that clears the FAT zone when a file is deleted
	if (FAT[pos] == 3) {
		FAT[pos] = 0;
		clearHD(pos);
	}
	else {
		unsigned int valFAT = FAT[pos];
		FAT[pos] = 0;
		clearHD(pos);
		clearFAT(valFAT);
	}
}

void transferFAT(unsigned short int& posHD) {
	int index = 0;
	for (int i = 0; i < Nr_UA / 16; i++) {
		for (int j = 0; j < UA / 2; j++) {
			unsigned short int value = FAT[index]; // FAT value
			// Value transfer on 2 bytes in HDD matrix
			HDD[posHD + i][2 * j] = static_cast<char>((value >> 8) & 0xFF);
			HDD[posHD + i][2 * j + 1] = static_cast<char>(value & 0xFF);
			index++;
		}
	}
	posHD += Nr_UA / 8;
}

void checkTransfer() {
	std::vector<unsigned short int> extractedValues;
	unsigned short int posHD = 0;

	for (int i = 0; i < Nr_UA / 16; i++) {
		for (int j = 0; j < UA / 2; j++) {
			char highByte = HDD[posHD + i][2 * j];
			char lowByte = HDD[posHD + i][2 * j + 1];

			unsigned short int value = (static_cast<unsigned char>(highByte) << 8) | static_cast<unsigned char>(lowByte);
			
			if (value == 2 && !stRootHDx && !stRootHDy) { // Extracting start position of root from FAT zone of HD
				stRootHDx = posHD + i;
				stRootHDy = 2 * j;
			}
			
			if (value == 0 && !stHDx && !stHDy) { // Extracting start position of HD from FAT zone of HD
				stHDx = posHD + i;
				stHDy = 2 * j;
			}

			extractedValues.push_back(value);
		}
	}

	for (int i = 0; i < Nr_UA; i++)
		cout << i << ' ' << extractedValues[i] << endl;
}

void addFileDates(unsigned short int indexRoot) { // Add creation / modification dates in ROOT zone at position index
	time_t now = time(0);
	tm* ltm = localtime(&now);

	// Adding creation date
	HDD[indexRoot][UA - 6] = static_cast<char>(ltm->tm_mday);
	HDD[indexRoot][UA - 5] = static_cast<char>(1 + ltm->tm_mon);
	HDD[indexRoot][UA - 4] = static_cast<char>(ltm->tm_year % 100);

	// Adding modification date
	HDD[indexRoot][UA - 3] = static_cast<char>(ltm->tm_mday);
	HDD[indexRoot][UA - 2] = static_cast<char>(1 + ltm->tm_mon);
	HDD[indexRoot][UA - 1] = static_cast<char>(ltm->tm_year % 100);
}

void updateModificationDate(unsigned short int indexRoot) { // update modification date of a file
	time_t now = time(0);
	tm* ltm = localtime(&now);

	HDD[indexRoot][UA - 3] = static_cast<char>(ltm->tm_mday);
	HDD[indexRoot][UA - 2] = static_cast<char>(1 + ltm->tm_mon);
	HDD[indexRoot][UA - 1] = static_cast<char>(ltm->tm_year % 100);
}

void splitDateDMY(const string& input, string& firstDate, string& secondDate) {
	int dateLength = input.length() / 2;

	firstDate = input.substr(0, dateLength);

	secondDate = input.substr(dateLength, dateLength);
}

void populateROOT(int pos, string fileName, unsigned short int marime, unsigned short int primaUA) { // method that completes the name of the file in the ROOT zone at position @pos
	string nmFis = getFileName(fileName), extension = fileName.substr(getFileName(fileName).length() + 1);
	int attr = 7; // 111, rwx
	
	// adding file name
	for (unsigned int i = 0; i < nmFis.length(); i++) {
		HDD[pos][i] = nmFis[i];
	}
	// adding file extension
	int ex = 0;
	for (unsigned int i = 17; i < 17 + extension.length(); i++) {
		HDD[pos][i] = extension[ex++];
	}

	// adding file size on 2 bytes
	HDD[pos][21] = static_cast<char>((marime >> 8) & 0xFF);
	HDD[pos][22] = static_cast<char>(marime & 0xFF);

	// adding firstUA of the file on 2 bytes
	HDD[pos][23] = static_cast<char>((primaUA >> 8) & 0xFF);
	HDD[pos][24] = static_cast<char>(primaUA & 0xFF);

	// adding attribute values on 1 byte
	HDD[pos][25] = static_cast<char>(attr & 0xFF);

	// adding creation and modification dates of the file
	addFileDates(pos);
}

void dirCommand(string optionalParameter) { // method for DIR command that lists all content from HDD
	// -a / --help
	int startROOT = Dim_FAT / UA;
	bool display = false;
	if (optionalParameter == "-A") {
		display = true;
	} else {
		if (optionalParameter == "--HELP") {
			cout << "DIR command shows name and extension of the files contained in the ROOT structure\n\n";
			cout << "Optional parameters list: " << endl;
			cout << "-a     : shows the size, creation date and modification date of the file\n" << endl;
			return;
		}
		else if (optionalParameter != "") {
			cout << "Command \'" << optionalParameter << "\' does not exist." << endl;
			return;
		}
	} //TODO: possible bug for the first optional param -a, not checked

	cout << "\nFile list:" << endl;
	int countFiles = 0;
	for (int i = startROOT; i < startROOT + nrFisROOT; i++) {
		string getNameExt = getFileNameAndExtension(i);
		
		if (getNameExt != "" && display) {
			char hb = HDD[i][21];
			char lb = HDD[i][22];
			unsigned short int marime = (static_cast<unsigned char>(hb) << 8) | static_cast<unsigned char>(lb),
							   primaUA = (static_cast<unsigned char>(HDD[i][23]) << 8) | static_cast<unsigned char>(HDD[i][24]),
							   attr = static_cast<unsigned char>(HDD[i][25]);

			bitset<8> binary(attr);
			string binString = binary.to_string();

			string bothDates = getDates(i), dateCreation, dateModification;
			splitDateDMY(bothDates, dateCreation, dateModification);

			if (getNameExt.length() < 8) {
				cout << "-----------" << ++countFiles << "------------" << "| Dim | " << "First UA |  ATTR  |  Creation Date  |  Modification Date  |\n" << getNameExt << "\t\t\t  " << marime << "\t  " << primaUA << "\t     " << binString.substr(binString.length() - 3) << "\t";
				for (unsigned int i = 0; i < dateCreation.length(); i++) {
					if (i == dateCreation.length() - 1) {
						cout << static_cast<int>(dateCreation[i]);
					}
					else {
						cout << static_cast<int>(dateCreation[i]) << "/";
					}
				}
				cout << "\t      ";
				for (unsigned int i = 0; i < dateModification.length(); i++) {
					if (i == dateModification.length() - 1) {
						cout << static_cast<int>(dateModification[i]);
					}
					else {
						cout << static_cast<int>(dateModification[i]) << "/";
					}
				}
				cout << endl;
			}
			else {
				cout << "-----------" << ++countFiles << "------------" << "| Dim | " << "First UA |  ATTR  |  Creation Date  |  Modification Date  |\n" << getNameExt << "\t\t  " << marime << "\t  " << primaUA << "\t     " << binString.substr(binString.length() - 3) << "\t";
				for (unsigned int i = 0; i < dateCreation.length(); i++) {
					if (i == dateCreation.length() - 1) {
						cout << static_cast<int>(dateCreation[i]);
					}
					else {
						cout << static_cast<int>(dateCreation[i]) << "/";
					}
				}
				cout << "\t      ";
				for (unsigned int i = 0; i < dateModification.length(); i++) {
					if (i == dateModification.length() - 1) {
						cout << static_cast<int>(dateModification[i]);
					}
					else {
						cout << static_cast<int>(dateModification[i]) << "/";
					}
				}
				cout << endl;
			}
		} else
			if (getNameExt != "") {
				cout << ++countFiles << ". " << getNameExt << endl;
			}
	}
	if (!countFiles) {
		cout << "\tNo files found." << endl;
		return;
	}

	cout << endl;
}

void createCommand(string fileName, unsigned int bytes, string optionalParameter) { // method for CREATE command that create a file
	int ROOTEntry = getFirstROOTEntry(), primaUA = firstUA(), neededUA = bytes % UA ? bytes / UA + 1 : bytes / UA;
	unsigned short int transferUpdatedFAT = 0;
	
	if (optionalParameter == "--HELP") {
		cout << "CREATE command creates a file with a preset content of a parameter (-ALFA, -NUM, -HEX)" << endl;
		cout << "Also, the file size in bytes must be provided" << endl << endl;
		cout << "Optional parameters list:" << endl;
		cout << "-ALFA      : creates a file in which lowercase letters of the English alphabet will be placed in a row, after the letters are finished, they will be resumed" << endl;
		cout << "-NUM       : creates a file in which the numbers from 0 to 9 will be placed, after the numbers are finished, they will be resumed" << endl;
		cout << "-HEX		: creeaza un fisier in care se vor pune cifrele hexazecimale ([0-9A-F]{16}), dupa terminare se reiau" << endl;
		cout << "-HEX       : creates a file in which hexadecimal digits ([0-9A-F]{16}) will be placed in a row, after the digits are finished, they will be resumed" << endl;
		return;
	}

	// check available space
	if (!checkAvailableSpace(bytes)) {
		cout << "There is not enough space to create the file." << endl;
		return;
	}

	// check available root space
	if (!checkAvailableSpaceROOT()) {
		cout << "There is not enough space to create the file." << endl;
		return;
	}

	// check if file already exists
	if (!checkFileNameAndExtension(fileName)) {
		cout << "File with name \'" << getFileName(fileName) << "\' and extension \'." << (fileName.substr(getFileName(fileName).length() + 1)) << "\' already exists." << endl;
		return;
	}
	// check if the file name has the minimum length of 1 and maximum of 17 characters
	if (getFileName(fileName).length() < 1 || getFileName(fileName).length() > 17) {
		cout << "File name must be between 1-17 characters." << endl;
		return;
	}
	// check if the file extension has the minimum length of 1 and maximum of 4 characters
	if ((fileName.substr(getFileName(fileName).length() + 1)).length() < 1 || (fileName.substr(getFileName(fileName).length() + 1)).length() > 4) {
		cout << "File extension must be between 1-4 characters." << endl;
		return;
	}
	
	if (optionalParameter == "-ALFA") {
		// traverses the free UA zone on the hdd and completes it with the respective content also add dates
		populateROOT(ROOTEntry, fileName, bytes, primaUA);
		populateHDD(primaUA, neededUA, "alfa", bytes);
		transferFAT(transferUpdatedFAT);
		addFileDates(ROOTEntry);
		return;
	} 
		else if (optionalParameter == "-NUM") {
			populateROOT(ROOTEntry, fileName, bytes, primaUA);
			populateHDD(primaUA, neededUA, "num", bytes);
			transferFAT(transferUpdatedFAT);
			addFileDates(ROOTEntry);
			return;
		}
			else if (optionalParameter == "-HEX") {
				populateROOT(ROOTEntry, fileName, bytes, primaUA);
				populateHDD(primaUA, neededUA, "hex", bytes);
				transferFAT(transferUpdatedFAT);
				addFileDates(ROOTEntry);
				return;
			}
				else if (optionalParameter != "") {
					cout << "Command \'" << optionalParameter << "\' does not exist." << endl;
					return;
				}
					else {
						cout << "Missing parameter." << endl;
						return;
					}
}

void deleteCommand(string fileName, string optionalParameter) { // method for file deletion
	if (optionalParameter == "--HELP") {
		cout << "Command DELETE FILE_NAME.EXTENSION deletes a file." << endl;
		return;
	}
	
	int deletePos;
	if ((deletePos = searchFileNameAndExtension(fileName)) != -1) {
		// we replace the first character of the file name with '?', then we go to the FAT table and hard-disk and empty the memory area used by the file
		unsigned short int value = getFirstUA(deletePos), transferUpdatedFAT = 0;
		HDD[deletePos][0] = '?';
		clearFAT(value);
		transferFAT(transferUpdatedFAT); // now that we updated the FAT table, we need to update it on the hard-disk

		cout << "File \'" << fileName << "\' was successfully deleted." << endl;

		return;
	}
	else {
		cout << "File \'" << fileName << "\' does not exist." << endl;
		return;
	}
}

void renameCommand(string fileName, string fileChange, string optionalParameter) { // method for file renaming
	if (optionalParameter == "--HELP") {
		cout << "RENAME command renames file_name.extesion in file_name_renamed.extension." << endl;
		return;
	}

	// check if there's already a file with the renamed option
	if (!checkFileName(fileName)) {
		cout << "File named \"" << fileName << "\" does not exist." << endl;
		return;
	}
	// check if there's already a file name and extension with the renamed option
	if (checkFileNameAndExtension(fileName)) {
		cout << "File with name \'" << getFileName(fileName) << "\' and extension \'." << (fileName.substr(getFileName(fileName).length() + 1)) << "\' does not exist." << endl;
		return;
	}

	// check if the file name and extension of the file to be renamed already exists
	if (!checkFileNameAndExtension(fileChange)) {
		cout << "File with name \'" << getFileName(fileChange) << "\' and extension \'." << (fileChange.substr(getFileName(fileChange).length() + 1)) << "\' already exists." << endl;
		return;
	}
	
	// check if the file name is between 1-17 characters
	if (getFileName(fileChange).length() < 1 || getFileName(fileChange).length() > 17) {
		cout << "File name does not respect the rule (min. 1 character, max. 17 characters)" << endl;
		return;
	}

	// check if the extensions are the same
	if ((fileName.substr(getFileName(fileName).length() + 1)) != (fileChange.substr(getFileName(fileChange).length() + 1))) {
		cout << "Extensions are not the same." << endl;
		return;
	}

	int strPos = searchFileNameAndExtension(fileName);
	if (strPos != -1) {
		for (unsigned int j = 0; j < 17; j++) {
			if (j < getFileName(fileChange).length()) {
				HDD[strPos][j] = fileChange[j];
			}
			else {
				HDD[strPos][j] = '\0';
			}
		}
		updateModificationDate(strPos);
		return;
	}
}
/*
void copyCommand(string fileName1, string fileName2, string optionalParameter) {
	if (optionalParameter == "--HELP") {
		cout << "COPY command renames file1.extension in file2.extension." << endl;
		return;
	}
}
*/
void showCommand(string fileName) { // method for displaying contents of a file
	// -> search for the file in root
	// -> extracts firstUA
	// -> shows the content from the HDD from that UA
	// -> primaUA = FAT[primaUA]
	int fileROOT = searchFileNameAndExtension(fileName);
	unsigned short int primaUA = (static_cast<unsigned char>(HDD[fileROOT][23]) << 8) | static_cast<unsigned char>(HDD[fileROOT][24]),
					   marime = (static_cast<unsigned char>(HDD[fileROOT][21]) << 8) | static_cast<unsigned char>(HDD[fileROOT][22]);

	unsigned short int neededUA = marime % UA ? marime / UA + 1 : marime / UA;

	if (fileROOT != -1) {
		for (int times = 0; times < neededUA; times++) {
			for (int i = 0; i < UA && HDD[primaUA][i]; i++) {
				cout << HDD[primaUA][i] << ' ';
			}
			primaUA = FAT[primaUA];
		}
	}
	cout << endl;
}

void clsCommand() { // method for console clearing
	system("cls");
}
// ********* MAIN TASKS *********
//         TODO COPY FCT
//		   TODO DEFRAG

/*
* 0 - UA free on HDD
* 1 - UA reserved for FAT
* 2 - UA reserved for ROOT
* 3 - UA that represents end of a file (last UA used to store on HDD a file)
*/
void init_fat(unsigned short int arr[], unsigned short int lenFAT, unsigned short int lenROOT) { // inits FAT table
	// inits FAT table with 1
	for (int i = 0; i < lenFAT; i++) {
		arr[i] = 1;
	}
	
	// inits ROOT zone with 2
	for (int i = lenFAT; i < lenFAT + lenROOT; i++) {
		arr[i] = 2;
	}

	// inits HDD zone with 0
	for (int i = lenFAT + lenROOT; i < Nr_UA; i++) {
		arr[i] = 0;
	}
}

void init_ROOT_HD(unsigned short int lenFAT, unsigned short int lenROOT) { // inits ROOT zone from HD with '\0'
	for (int i = lenFAT; i < lenFAT + lenROOT; i++) {
		for (int j = 0; j < UA; j++) {
			HDD[i][j] = '\0';
		}
	}
}

void FAT_OS() { // method that simulates the OS
	while (true) {
		string command;
		cout << "OS/> ";
		getline(cin, command);

		istringstream iss(command);
		string commandName;
		iss >> commandName;

		if (toUpper(commandName) == "DIR") {
			string arg;
			iss >> arg;

			dirCommand(toUpper(arg));
		}
		else if (toUpper(commandName) == "CREATE") {
			string inputName;
			unsigned int fileSize;
			string optionalParameter;

			iss >> inputName;
			
			if (toUpper(inputName) == "--HELP") {
				createCommand("", 0, toUpper(inputName));
			}
			else {
				iss >> fileSize >> optionalParameter;

				createCommand(inputName, fileSize, toUpper(optionalParameter));
			}

		}
		else if (toUpper(commandName) == "DELETE") {
			string fileName, optionalParameter, inputName;

			iss >> inputName;

			if (toUpper(inputName) == "--HELP") {
				deleteCommand("", toUpper(inputName));
			}
			else {
				iss >> optionalParameter;

				deleteCommand(inputName, optionalParameter);
			}
		}
		else if (toUpper(commandName) == "RENAME") {
			string fileName, fileChange, optionalParameter, inputName;

			iss >> inputName;

			if (toUpper(inputName) == "--HELP") {
				renameCommand("", "", toUpper(inputName));
			}
			else {
				iss >> fileChange;

				renameCommand(inputName, fileChange, "");
			}
		}
		else if (toUpper(commandName) == "SHOW") {
			string fileName;
			
			iss >> fileName;

			showCommand(fileName);
		}
		else if (toUpper(commandName) == "CLS" || toUpper(commandName) == "CLEAR") {
			clsCommand();
		}
		else if (toUpper(command) == "EXIT") {
			break;
		}
		else {
			cout << "Comanda: \'" << commandName << "\' nu exista." << endl;
		}
	}
}

int main() {
	unsigned short int posHDD = 0;

	Dim_HDD = Nr_UA * UA;
	Dim_FAT = Nr_UA * 2; //256 UA for FAT
	
	// initialize FAT table
	init_fat(FAT, Dim_FAT / UA, nrFisROOT);

	// Initialize ROOT zone from HDD
	init_ROOT_HD(Dim_FAT / UA, nrFisROOT);

	//for (int i = 0; i < Nr_UA; i++)
	//	cout << i << ' ' << FAT[i] << endl;

	// Transfer FAT table on Hard-disk
	transferFAT(posHDD);
	//checkTransfer();

	FAT_OS();

	return 0;
}