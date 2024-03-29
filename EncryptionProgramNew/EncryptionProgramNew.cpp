// EncryptionProgramNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <list>
#include <set>

using namespace std;
#define WRITE 0
#define DEBUG 1
#define RANDOM_LIMIT ((1000 + 1 - 100) + 100)
#define NUM_SUBSTITUTE 3 //The number of corresponding numbers to any given character. If 3, a = 123. If 4, a = 1234, etc 

fstream FilePathStream; //stream to the file we are working on

vector<unsigned char> FileData; //the data in the file to be encrypted

void GetPathToFile();

string EncryptOrDecrypt();

void FillFileData();

bool Encrypt(); //returns true or false for whether the file was encrypted correctly

bool DecryptNew(); //returns true or false for successful or unsuccessful decryption

string GetPassword(); //prompt user to enter password and returns it

int main(int argc, char *argv[])
{
	srand(time(NULL)); //set random seed every time different

	bool programPassed; //did the program work right? 

	if (argc < 3)
	{
		GetPathToFile(); //Get the path to the file to open
	}
	else
	{
		ifstream pathStream(argv[2]);
		if (!pathStream)
		{
			cout << "Please enter a valid file path to the file you wish to work with in the second argument." << endl;
		}
		else
		{
			FilePathStream.open(argv[2]);
		}
	}

	string encOrDec;

	if (argc < 2)
	{

		encOrDec = EncryptOrDecrypt(); //find out if encrypting or decrypting

	}
	else
	{
		
		encOrDec = argv[1];

		std::transform(encOrDec.begin(), encOrDec.end(), encOrDec.begin(), ::tolower);

		if (encOrDec != "encrypt" && encOrDec != "decrypt")
		{
			cout << "The proper form for the first argument is ""encrypt"" or ""decrypt"", and nothing else." << endl;
			return 1;
		}

	}
	if (encOrDec == "encrypt")
	{
		programPassed = Encrypt();
	}
	else
	{
		programPassed = DecryptNew();
	}

#ifdef _WIN32
	cout << endl;
	system("pause");
#elif __linux__
	cout << endl << "Press any key to continue..." << endl;
	system("read -s -N 1");
#endif

}

int recursiveFindNewRand(int num, set<int> setOfNums)//Finds a new random number by recursive methods and comparing against a set
{
	if (setOfNums.find(num) == setOfNums.end())
	{
		return num;
	}

	return recursiveFindNewRand(rand() % RANDOM_LIMIT, setOfNums);
}

bool Encrypt()
{
	map<char, int> contentMap; //The map for reference of characters; a=111, for example. Then you can find it with find('a')

	map<char, int>::iterator iterator; //for iteration over contentMap

	set<int> setOfRandomNumbers;

	string encryptedDataString;

	string password; //What is used to decrypt the file

	FillFileData();

	cout << "File contents read. Encrypting file..." << endl << endl;

	int randomNum = 0;

	for (int i = 0; i < FileData.size(); i++)
	{


		while ((randomNum < 101 || randomNum > 999) || setOfRandomNumbers.find(randomNum) != setOfRandomNumbers.end())
		{
			randomNum = rand() % RANDOM_LIMIT;
			if (setOfRandomNumbers.find(randomNum) != setOfRandomNumbers.end())
			{
				setOfRandomNumbers.insert(randomNum);
			}
		}

		setOfRandomNumbers.insert(randomNum);

		iterator = contentMap.find(FileData[i]);
		
		if (iterator == contentMap.end())
		{
			if (FileData[i] != '\n')
			{
				contentMap[FileData[i]] = randomNum; //limits 100 to 999, but allows us to store the chars
				if (DEBUG)
				{
					cout << FileData[i] << " = " << randomNum << endl;
				}
				password += FileData[i];
			}
		}
		else
		{
			randomNum = iterator->second;
		}

		if (FileData[i] == '\n') //If it's a new line, set it to write end of line chars instead of the regular ones
		{
			encryptedDataString += "100";
		}
		else
		{
			encryptedDataString += to_string(randomNum);
		}
		randomNum = 0;
	}
	if (WRITE == 0)
	{
		cout << endl << "The encrypted data (copy and paste into where you want to store it for decryption): " << endl;
		cout << encryptedDataString << endl << endl;
	}
	cout << "The file was successfully encrypted. Copy the next line of characters and paste it somewhere safe. (The next line is the password which will be asked upon decryption)" << endl << endl << password;

	return true;

}

bool DecryptNew() //Perfect
{

	//Parse data in 3's and get each number, assign each unique new number to a 
	//given letter, and then type that with enter being the same each time (100)

	//107108109110111107
	//Password: qrL k
	//107 = q
	//108 = r
	//109 = L
	//110 =  
	//111 = k
	//Decrypted: qrL kq

	FillFileData();

	string pw = GetPassword();

	string fileData(FileData.begin(), FileData.end());

	vector<int> listDataAsData; //List of the characters, ints pushed back in this for easier runthrough
							  //and side-by-side parsing of passwords passed in

	map<int, char> numbersAndLetters; //The map which holds the data of what numbers go to what letters.

	int pwIndex = 0; //The index of the password character being checked

	for (int i = 0; i < fileData.length(); i++) //Cycle through the fileData to record each number
											  //into the list of ints storing it
	{
		if ((i + 1) % 3 == 0 && i >= 2) //If i is divisible by 3 when one is added to it, add the past three to the list
		{
			ostringstream temp1, temp2, temp3;
			temp1 << fileData[(unsigned long long)i - 2];
			temp2 << fileData[(unsigned long long)i - 1];
			temp3 << fileData[i];
			string temp = temp1.str() + temp2.str() + temp3.str();

			if (DEBUG)
			{
				cout << "i: " << i << endl;
				cout << "Parts: " << temp1.str() << ", " << temp2.str() << ", " << temp3.str() << ";" << endl;
				cout << "Pushing " << temp << endl;
			}

			listDataAsData.push_back(stoi(temp));
		}
	}

	set<int> listOfUsedInts;
	for (int i = 0; i < listDataAsData.size(); i++) //For each element in the listDataAsData list
	{
		if (listOfUsedInts.find(listDataAsData.at(i)) == listOfUsedInts.end() && listDataAsData.at(i) != 100) 
			//If the current listDataAsData is not found in the list of ints stored (meaning it is being 
			//encountered for the first time) and it isn't a new line
		{
			char ch = NULL;
			listOfUsedInts.insert(listDataAsData.at(i)); //Insert the listDataAsData element into the set
			try 
			{
				ch = pw.at(pwIndex);
				if (DEBUG)
				{
					cout << "Inserting " << pw.at(pwIndex) << " for the integer " << listDataAsData.at(i) << endl;
				}
			}
			catch (out_of_range e)
			{
				if (DEBUG)
				{
					cout << endl << "Password index did not have enough characters. Possible end of line detected." << endl;
				}
			}
			numbersAndLetters.insert({ listDataAsData.at(i), ch }); //Insert into the map the int for the character
																	//in the password
			pwIndex++; 
		}
	} 

	cout << endl << "The decrypted data (based upon the password): " << endl;

	for (int i = 0; i < listDataAsData.size(); i++) //Now we actually output it
	{
		if (listDataAsData.at(i) == 100) //If it's a new line
		{
			cout << endl;
		}
		else //otherwise
		{
			cout << numbersAndLetters.at(listDataAsData.at(i)); //print the right data
		}
	}

	cout << endl;

	return true;

}

bool Decrypt() //Error likely related to the spaces 
			   //Fine with one new line, screws up with two
{

	map<int, unsigned char> valueMap; //the vector which assigns values to characters so they can be decrypted

	cout << "Decrypting file." << endl;

	FillFileData(); //Fill data in file into FileData variable

	string pw = GetPassword(); //Insert password to decrypt

	int pass_increment = 0, numOfSpaces = 0; //increment the number so that each 3 numbers that is UNIQUE is taken into account

	string originalFileData(FileData.begin(), FileData.end()); //Store original file data

	string newFileData = originalFileData; //New file data will be worked upon by decryption alg

	string newFileDataSpaces = originalFileData; //New file data spaces is just spaces to transpose the decrypted string onto

	cout << "Before: " << newFileDataSpaces << endl;
	cout << "Length: " << newFileDataSpaces.length() << endl;

	for (int i = 0; i < newFileData.length(); i++) //Making newFileDataSpaces be entirely made of spaces
	{
		newFileDataSpaces.at(i) = ' ';
	}
	cout << "After: " << newFileDataSpaces << endl;
	cout << "Length: " << newFileDataSpaces.length() << endl;


	cout << "Num to go to: " << (originalFileData.length() / NUM_SUBSTITUTE) + 1 << endl;

	bool lastValueWasNewLine = false; //Useful for detecting if the last char was a new line
	int numOfNewLineCounts = 0;

	for (int i = 0; i <= (originalFileData.length() / NUM_SUBSTITUTE); i++)
	{
		int val; //temporary value variable

			try
			{
				if (originalFileData.substr((unsigned long long)i*NUM_SUBSTITUTE, NUM_SUBSTITUTE) != "100") //If it wasn't an end line...
				{
					val = stoi(originalFileData.substr((unsigned long long)i*NUM_SUBSTITUTE, NUM_SUBSTITUTE)); //Take the numbers and sub in
																						   //the int values for them
					lastValueWasNewLine = false; 
				}
				else //If it was a end line...
				{
					val = 100; //Set the value to the new line value
					if (lastValueWasNewLine == false) //If this variable was not set to true (if there was not multiple
													  //end lines or if there was no new line before this)...
					{
						pass_increment--; //decrement the password (fixes it for some reason...)
						numOfNewLineCounts++; //add to the number of new lines accounted for
						
						//Below worked for only one password combo.

						//if (numOfNewLineCounts == 2)
						//{
						//	pass_increment += numOfNewLineCounts - 1;
						//}
						//if (numOfNewLineCounts == 3)
						//{
						//	pass_increment -= numOfNewLineCounts -4;
						//}
						//if (numOfNewLineCounts == 4)
						//{
						//	pass_increment += numOfNewLineCounts - 7;
						//}

						lastValueWasNewLine = true; //set this so multiple new lines don't increment the pass over and over
					}
				}
				map<int, unsigned char>::iterator mapIt = valueMap.find(val); //Scan the value map for the value associated
																			  //with val
				if (mapIt == valueMap.end()) //check if it has the key already...
				{
					if (pw.length() > pass_increment)
					{
						valueMap[val] = pw.at(pass_increment); //Find the character and assign it to the map
					}
					pass_increment++; //increase the pass increment.
				}
			}
			catch (invalid_argument e)
			{
				cout << endl << "Password is too long for the file. The decrypted file may be partially incorrect." << endl;
			}
			catch (out_of_range e)
			{
				cout << endl << "An error occurred - out_of_range exception, error code 0x01. Please report this." << endl;
			}

			string charAtPoint = "";
			stringstream ss;

			map<int, unsigned char>::iterator mapIt2 = valueMap.find(val); //Finding all occurences of val
			if (valueMap.count(val))
			{
				char c = valueMap.at(val);
				string s;
				try {
					ss << c; //Character goes into string stream
					ss >> s; //string stream is assigned to string 
					charAtPoint = string() + s; //Character at point is s as a string
				}
				catch (out_of_range e)
				{
					cout << endl << "An error occurred - out_of_range exception, error code 0x02. Please report this." << endl;
				}
				try {
					//cout << "charAtPoint: " << charAtPoint << "; valueMap at val: " << valueMap.at(val) << endl;
					if (charAtPoint == " ")
					{
						numOfSpaces++;
						//Possible source of errors? 
						newFileDataSpaces.append(" ");
						//
					}
					newFileDataSpaces = newFileDataSpaces.replace(i, NUM_SUBSTITUTE, charAtPoint);
					//cout << "Iterator: " << i << "; Iterator char: " << newFileDataSpaces.at(i) << endl;
				}
				catch (out_of_range e)
				{
					cout << endl << "i:" << i << "; i val: " << newFileDataSpaces[(unsigned long long)i - 2] << endl;
					cout << "Decrypted string: " << newFileDataSpaces << endl;
					return true;
					cout << endl << "An error occurred - out_of_range exception, error code 0x03. Please report this." << endl;
					cout << endl << "Size of newFileDataSpaces: " << newFileDataSpaces.size() << endl;
					cout << endl << "i:" << i << "; i val: " << newFileDataSpaces[(unsigned long long)i-1]<< endl;
					cout << endl << "Full newFileDataSpaces: " << newFileDataSpaces << endl;
				}
			}
			if (val == 100)
			{
				newFileDataSpaces = newFileDataSpaces.replace(i, NUM_SUBSTITUTE, "\n");
			}
	}
	cout << "Decrypted string: " << newFileDataSpaces << endl;

	return true;

}

string GetPassword()
{
	char password[1024];

	string passwordString;

	while (passwordString.length() == 0)
	{
		cout << "Please enter the password for this file given to you upon encryption." << endl;

		cin.getline(password, sizeof(password));

		passwordString = password;
	}

	return passwordString;
}

void FillFileData() //Fills FileData variable with the data inside the file
{
	if (FilePathStream.is_open() == false) //If it's false, there is no defined path
	{
		cerr << "The file path is invalid. Please restart and try again." << endl;
		return;
	}

	while (!FilePathStream.eof()) //get the file data and assign it to fileData
	{
		FileData.push_back(FilePathStream.get());
	}
}

string EncryptOrDecrypt()
{
	string in, inLower;

	while (in != "encrypt" && in != "decrypt")
	{
		cout << "Would you like to encrypt or decrypt this file? Type \"quit\" to quit." << endl;

		cin >> in;

		inLower = in;

		for (int i = 0; i <= in.length(); i++) //convert to lowercase and store in the string - this is to 
										  //make it case-insensitive
		{
			inLower[i] = tolower(in[i]);
		}

		if (inLower == "quit")
		{
			exit(0);
		}
	}

	return in;
}

void GetPathToFile() //Gets the path to the file to encrypt
{
	string path, lowerPath; //the path inputted from the user and the path converted to a lowercase one

	while (FilePathStream.is_open() == false) //while the stream isn't open (aka we don't have a file path)
	{
		cout << "Please enter the complete file path to the file which you would like to encrypt or decrypt." <<
			"Type \"quit\" to quit." << endl;

		cin >> path;

		lowerPath = path; //assign to make it same length as path

		for (int i = 0; i <= path.length(); i++) //convert to lowercase and store in the string - this is to 
											  //make it case-insensitive
		{

			lowerPath[i] = tolower(path[i]);
		}

		if (lowerPath == "quit")
		{
			exit(0);
		}

		ifstream pathStream(path);

		if (!pathStream)
		{
			cerr << "Unable to open the file provided." << endl;
		}
		else
		{
			FilePathStream.open(path.c_str()); //Open the file and exit the loop
		}
	}

}
