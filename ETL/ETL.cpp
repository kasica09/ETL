// ETL.cpp : Defines the entry point for the console application.
//
//parsowanie stringa to dwie funkcje:
//1 find (szuka danego ciagu w stringu) np. najpierw "review-score-count"
//2 substr zwraca string z pozycji ktora podasz w argumencie o dlugosci ktory podasz w drugim argumencie



#include "stdafx.h"
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <locale.h>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <cctype>
#include <algorithm>
#include <iterator>
#include <sstream>

/*#include <boost/filesystem.hpp>
using boost::filesystem;
*/
using std::cout;
using std::endl;
using std::cin;

//global
std::fstream plik;
std::string productCode;
int globalCounter = 0;

//function prototype

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
void clearBuffer(std::string & _mainBuffer, const std::string & _delimiter, const std::string & _delimiter2);
std::string parser(const std::string & _mainBuffer, const std::string & _delimiter, const std::string & _delimiter2, int & _pos, int & _pos2);
void onceDownloadElem(std::string & _mainBuffer, std::string & _delimiter, std::string & _delimiter2, int & _pos, int & _pos2);
void multiplyDownloadElem(std::string & _mainBuffer, std::string & _delimiter, std::string & _delimiter2, int & _pos, int & _pos2);
void removeNewLine(std::vector<std::string> & str);
bool createFileToWrite();
void openFileToWrite();
void closeFile();
void proccedExtractOnce(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus);
bool proccedExtractMultiply(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
void correctAuthor(std::vector<std::string> & _author);
bool saveToFile(std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote, int ifUpdate, bool ifCvs);
std::string createFirstlink();
void downloadFirstSite(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
void downloadRestSite(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
std::string createRestLinks(int & _counter);
void updateFile(std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
bool loadFromFile(std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
std::string ifLineExist(std::string & _line);
bool is_number(const std::string& s);
void openFileWithCodes(void);

//opineo
void extractOpineo(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
bool findProductInSearchResults(std::string _originalLink, std::string & _newLink, std::string product);
bool saveBufferToFile(std::string filename, std::string & buffer);
void downloadProductSiteOpineo(std::string & _mainBuffer, std::string linker, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
int extractOpinionsOpineo(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote);
//text matching
template<typename T, typename C>
size_t seq_distance(const T& seq1, const T& seq2, const C& cost, const typename T::value_type& empty);
size_t letter_distance(char letter1, char letter2);
size_t word_distance(const std::string& word1, const std::string& word2);
size_t sentence_distance(const std::string& sentence1, const std::string& sentence2);

int _tmain(int argc, _TCHAR* argv[])
{
	//setlocale(LC_ALL, "");

	//const char * link = "http://www.ceneo.pl/35379075#tab=spec";

	//linki
	//glowny: http://www.ceneo.pl/XXX#tab=reviews_scroll  w miejsce XXX kod produktu
	//podstrony http://www.ceneo.pl/XXX/opinie-2
	// itd: http://www.ceneo.pl/XXX/opinie-3

	std::string subPageBuffer;

	//once
	std::vector<std::string> appliance;
	std::vector<std::string> producerModel;
	std::vector<std::string> feature;
	std::vector<std::string> howManyPlus;
	std::vector<std::string> howManyMinus;

	//multiply
	std::vector<std::string> author;
	std::vector<std::string> ifRecommend;
	std::vector<std::string> noOfStars;
	std::vector<std::string> date;
	std::vector<std::string> opinion;
	std::vector<std::string> vote;
	std::vector<std::string> secondVote;

	//productCode = "39580136"; //39580136  32065635
	// link = "http://www.ceneo.pl/39580136#tab=reviews_scroll";
	setlocale(LC_ALL, "");

	cout << "Aplikacja umo¿liwiaj¹ca przeprowadzenie kompletnego procesu ETL" << endl;
	cout << "Wybierz jedn¹ z poni¿szych mo¿liwoœci i wcisnij enter.." << endl;
	int choose = 1;
	while (choose != 0)
	{
		cout << "1. Kompletny proces ETL" << endl;
		cout << "2. Proces ETL w kolejnych fazach zaczynaj¹c od Extract" << endl;
		cout << "3. Wyœwietlenie informacji z bazy danych o danym produkcie" << endl;
		cout << "4. Usuwanie produktu z bazy danych" << endl;
		cout << "0. Wyjscie z programu" << endl;

		std::string mainBuffer;
		std::cin >> choose;
		std::cin.clear();

		if (choose == 0)
			break;
		switch (choose)
		{
		case 1:
		{
			//extract
			downloadFirstSite(mainBuffer, appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
			downloadRestSite(mainBuffer, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
			cout << "Proces ekstrakcji zakonczyl sie sukcesem! Pobrano " << globalCounter << " rekordow z ceneo." << endl;
			globalCounter = 0;
			extractOpineo(mainBuffer, appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
			cout << "Proces ekstrakcji zakonczyl sie sukcesem! Pobrano " << globalCounter << " rekordow z Opineo." << endl;
			globalCounter = 0;
			//transform
			correctAuthor(author);
			removeNewLine(opinion);

			//load					
			if (!createFileToWrite()) //if file exist only update is possible
			{
				std::string decision;
				cout << "Plik juz istnieje, mozliwy tylko update!" << endl;
				cout << "Czy chcesz go przeprowadzic? " << endl;
				cin >> decision;
				if (decision == "tak")
				{
					updateFile(author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
					saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 0, 0);
					cout << "Update zakonczyl sie sukcesem! Do bazy zaladowano " << globalCounter << " nowych rekordow." << endl;
					globalCounter = 0;
					cout << "Czy chcesz wykonac import danych do pliku cvs?" << endl;
					std::string wantCvs;
					cin >> wantCvs;
					if (wantCvs == "tak")
					{
						saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 0, 1);
					}
					globalCounter = 0;
					break;
				}
				break;
			}
			else
			{
				openFileWithCodes(); //create file with product codes - neccessery to display info about the same products
				saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 1, 0);
				cout << "Ladowanie danych do bazy zakonczylo sie sukcesem! Zaladowano " << globalCounter << " nowych rekordow." << endl;
				globalCounter = 0;
				cout << "Czy chcesz wykonac import danych do pliku cvs?" << endl;
				std::string wantCvs;
				cin >> wantCvs;
				if (wantCvs == "tak")
				{
					saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 0, 1);
				}
				globalCounter = 0;
				break;
			}
		}
		case 2:
		{
			std::string decision;
			cout << "Czy wykonac proces ekstrakcji?" << endl;
			cin >> decision;
			if (decision == "tak")
			{
				downloadFirstSite(mainBuffer, appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
				downloadRestSite(mainBuffer, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
				cout << "Proces ekstrakcji zakonczyl sie sukcesem! Pobrano " << globalCounter << " rekordow z ceneo." << endl;
				globalCounter = 0;
				extractOpineo(mainBuffer, appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
				cout << "Proces ekstrakcji zakonczyl sie sukcesem! Pobrano " << globalCounter << " rekordow z Opineo." << endl;
				globalCounter = 0;
			}
			else
				break;

			cout << "Czy wykonac proces transformacji?" << endl;
			cin >> decision;
			if (decision == "tak")
			{
				correctAuthor(author);
				removeNewLine(opinion);
			}
			else
				break;

			cout << "Czy wykonac proces ladowania? " << endl;
			cin >> decision;
			if (decision == "tak")
			{
				if (!createFileToWrite()) //if file exist only update is possible
				{
					std::string decision;
					cout << "Plik juz istnieje, mozliwy tylko update!" << endl;
					cout << "Czy chcesz go przeprowadzic? " << endl;
					cin >> decision;
					if (decision == "tak")
					{
						updateFile(author, ifRecommend, noOfStars, date, opinion, vote, secondVote);
						saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 0, 0);
						cout << "Update zakonczyl sie sukcesem! Do bazy zaladowano " << globalCounter << " nowych rekordow." << endl;
						globalCounter = 0;
						break;
					}
					break;
				}
				else
				{
					openFileWithCodes(); //create file with product codes - necessery to display info about the same products
					saveToFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote, 1, 0);
					cout << "Ladowanie danych do bazy zakonczylo sie sukcesem! Zaladowano " << globalCounter << " nowych rekordow." << endl;
					globalCounter = 0;
					break;
				}
			}
			break;
		}
		case 3:
		{
			/*cout << "Podaj numer produktu i wcisnij enter.." << endl;
			cin >> productCode;
			//productCode = "32065635";
			if (!loadFromFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote))
			break;
			cout << "Opinie dla danego produktu to: "<<endl;
			std::vector<std::string>::iterator it;
			int i = 0;
			for (it = author.begin(); it < author.end(); it++, i++)
			{
			cout<< opinion[i] << endl<< endl;
			}
			break;*/
			cout << "Podaj nazwe produktu o ktorym chcesz wyswietlic informacje" << endl;
			std::string productName;
			cin >> productName;
			std::vector<std::string> productsCode;
			std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
			plik.open(fileName, std::fstream::in | std::fstream::app);
			if (plik.is_open() == false)
			{
				cout << "Brak wpisow w bazie danych!" << endl;
			}

			int loopCounter = 0;
			std::string line;

			while (getline(plik, line))
			{
				productsCode.push_back(line);
				loopCounter++;
			}
			//cout << "KONIEC" << endl;
			closeFile();

			//serch fraze in files
			while (loopCounter != 0)
			{
				fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
				fileName += productsCode[loopCounter - 1];
				fileName += ".txt";
				//cout << "FILE NAME=" << fileName << endl;
				productCode = productsCode[loopCounter - 1];
				plik.open(fileName, std::fstream::in /*| std::fstream::app*/);
				if (plik.is_open() == false)
				{
					cout << "Problem z plikiem: " << fileName << endl;
				}

				getline(plik, line);
				getline(plik, line);
				//cout << "line = " << line << endl;
				if (line.find(productName) != std::string::npos)
				{
					if (!loadFromFile(appliance, producerModel, feature, howManyPlus, howManyMinus, author, ifRecommend, noOfStars, date, opinion, vote, secondVote))
						break;
				}

				loopCounter--;
				closeFile();
			}
			//print all existing opinion

			std::vector<std::string>::iterator it;
			int i = 0;
			for (it = opinion.begin(); it < opinion.end(); it++, i++)
			{
				cout << opinion[i] << endl << endl;
			}

			break;
		}
		case 4:
		{
			cout << "Podaj numer produktu i wcisnij enter.." << endl;
			cin >> productCode;
			//productCode = "39580136";
			//delete productCode from productCodes.txt
			std::ifstream kody("E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\productsCode.txt");
			std::ofstream temp("E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\temp.txt"); // temp file for input of every student except the one user wants to delete

			//plik.open(fileName, std::fstream::out | std::fstream::app);
			std::string oneLine;
			while (std::getline(kody, oneLine))
			{
				//cout << oneLine << endl;
				if (oneLine != productCode)
				{
					temp << oneLine << endl;
				}
			}
			kody.clear(); // clear eof and fail bits
			kody.seekg(0, std::ios::beg);
			kody.close();
			temp.close();
			remove("E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\productsCode.txt");
			rename("E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\temp.txt", "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\productsCode.txt");

			//productCode = "32065635";
			//only temporary - change it as a function used by few other functions
			std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
			fileName += productCode;
			fileName += ".txt";
			const char * path = fileName.c_str();
			// createFileToRead();
			if (remove(path) == 0)
			{
				fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
				fileName += productCode;
				fileName += "nr.txt";
				const char * path2 = fileName.c_str();
				if (remove(path2) == 0)
				{
					fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
					fileName += productCode;
					fileName += ".cvs";
					const char * path3 = fileName.c_str();
					if (remove(path3) == 0)
					{
						cout << "Usunieto podany produkt!" << endl;
					}
					else
						cout << "Dla podanego produktu nie istnial odpowiedni plik .cvs!" << endl;
				}
				else
					cout << "Podany produkt nie istnial w bazie(numery)!" << endl;
			}

			else
				cout << "Podany produkt nie istnial w bazie!" << endl;
			break;

		}
		default:
			cout << "Nie ma takiej opcji! Wybierz ponownie lub wyjdŸ!" << endl;
			break;
		}
		//clear existing strings to be sure that they do not support trashes files
		appliance.clear();
		producerModel.clear();
		feature.clear();
		howManyPlus.clear();
		howManyMinus.clear();

		//multiply
		author.clear();
		ifRecommend.clear();
		noOfStars.clear();
		date.clear();
		opinion.clear();
		vote.clear();
		secondVote.clear();
	}
	system("PAUSE");
	return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string parser(const std::string & _mainBuffer, const std::string & _delimiter, const std::string & _delimiter2, int & _pos, int & _pos2)
{
	std::string token;
	int /*pos, pos2,*/ diff;// = mainBuffer.find(delimiter);

	_pos = _mainBuffer.find(_delimiter);
	_pos += _delimiter.length(); //update pos to check after delimiter which was found
	_pos2 = _pos;
	_pos2 = _mainBuffer.find(_delimiter2, _pos2);
	diff = _pos2 - _pos; //diff beetwen position of second and first string
	_pos2 += _delimiter2.length(); //update pos2 to check after delimiter which was found
	token = _mainBuffer.substr(_pos, diff); //grade

	return token;
}

void clearBuffer(std::string & _mainBuffer, const std::string & _delimiter, const std::string & _delimiter2)
{
	_mainBuffer.erase(0, _mainBuffer.find(_delimiter) + _delimiter.length());
	_mainBuffer.erase(0, _mainBuffer.find(_delimiter2) + _delimiter2.length());
}

void removeNewLine(std::vector<std::string> & str)
{
	int start = 0;
	int occurs = 0;
	int i = 0;
	std::vector<std::string>::iterator it;

	for (it = str.begin(); it < str.end(); it++, i++)
	{
		start = 0;
		occurs = str[i].find("<br/>", start);
		while (occurs != std::string::npos)//delete all new line and <br/> selectors
		{
			str[i].erase(occurs, 6); //+ because erase <br> and also new line
			start = occurs;
			occurs = str[i].find("<br/>", start);
		}
	}

	i = 0;
	for (it = str.begin(); it < str.end(); it++, i++)
	{
		start = 0;
		occurs = str[i].find("\n", start);
		while (occurs != std::string::npos)//delete all new line and \n selectors
		{
			str[i].erase(occurs, 3); //+ because erase \n and also new line
			start = occurs;
			occurs = str[i].find("\n", start);
		}
	}
}

bool createFileToWrite(void)
{
	//this method should run only when file doesn't exist!

	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += productCode;
	fileName += ".txt";

	struct stat info;
	const char * path = fileName.c_str();

	if (stat(path, &info) != 0)
	{
		return true;
	}
	else if (info.st_mode & S_IFMT)
	{
		//only update
		return false;
	}

	return true;
}
void openFileToWrite(void)
{
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += productCode;
	fileName += ".txt";
	plik.open(fileName, std::fstream::out | std::fstream::app);
	if (plik.is_open() == false)
	{

		plik.open(fileName, std::fstream::trunc | std::fstream::in | std::fstream::out);
		plik.close();
		// re-open with original flags
		plik.open(fileName, std::fstream::out | std::fstream::app);
		cout << "Nie udalo sie utworzyc pliku do zapisu!" << endl;
	}
}
void openCvsFileToWrite(void)
{
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += productCode;
	fileName += ".cvs";
	plik.open(fileName, std::fstream::out | std::fstream::app);
	if (plik.is_open() == false)
	{

		plik.open(fileName, std::fstream::trunc | std::fstream::in | std::fstream::out);
		plik.close();
		// re-open with original flags
		plik.open(fileName, std::fstream::out | std::fstream::app);
		cout << "Nie udalo sie utworzyc pliku do zapisu!" << endl;
	}
}
void closeFile(void)
{
	plik.close();
}
void proccedExtractOnce(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus)
{
	std::string delimiter;
	std::string delimiter2;
	int pos, pos2;
	std::string tmp;

	int countFeatures = 0;
	std::fstream fileX;
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\"; //create file with number of features
	fileName += productCode;
	fileName += "nr.txt";
	fileX.open(fileName, std::fstream::out | std::fstream::trunc);
	if (fileX.is_open() == false)
	{

		//fileX.open(fileName, std::fstream::trunc | std::fstream::in | std::fstream::out);
		//fileX.close();
		// re-open with original flags
		//fileX.open(fileName, std::fstream::out | std::fstream::trunc); //trunc discard previous information
		//cout << "Nie udalo sie utworzyc pliku do numerow feauterow!" << endl;
	}

	delimiter = "catmin: '";
	delimiter2 = "',";
	tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
	_appliance.push_back(tmp);

	//producer and model
	delimiter = "description\" content=\"";
	delimiter2 = " - ";
	tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
	_producerModel.push_back(tmp);

	delimiter = "feature-vote-btn vote-pro js_vote-product-feature-pro\" data-val=\"";
	while ((pos = _mainBuffer.find(delimiter)) != std::string::npos)
	{
		countFeatures++;
		//feature
		delimiter2 = "\" data";
		pos2 = pos;
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_feature.push_back(tmp);

		//how many in plus
		delimiter = "js_vote-label-pro\">";
		delimiter2 = "</strong>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_howManyPlus.push_back(tmp);

		//how many in minus
		delimiter = "js_vote-label-con\">";
		delimiter2 = "</strong>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_howManyMinus.push_back(tmp);

		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "feature-vote-btn vote-pro js_vote-product-feature-pro\" data-val=\"";
		globalCounter += 3;
	}
	globalCounter += 2;
	fileX << countFeatures;
	fileX.close();
}
bool proccedExtractMultiply(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	std::string delimiter;
	std::string delimiter2;
	int pos, pos2;
	std::string tmp;

	clearBuffer(_mainBuffer, delimiter, delimiter2);

	delimiter = "product-reviewer\">";
	while ((pos = _mainBuffer.find(delimiter)) != std::string::npos)
	{
		//author name
		delimiter2 = "</div>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_author.push_back(tmp);

		//recommend/do not recommend
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "em class=\"product-";
		delimiter2 = "rec"; //trick -not recommend and  recommend has a the same value
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "\">";
		delimiter2 = "</em>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_ifRecommend.push_back(tmp);

		//number of stars
		delimiter = "review-score-count\">";
		delimiter2 = "</span>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_noOfStars.push_back(tmp);

		//date
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "time datetime=\"";
		delimiter2 = "\">";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_date.push_back(tmp);

		//opinion
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "product-review-body\">";// opinion
		delimiter2 = "</p>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_opinion.push_back(tmp);

		//vote
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "votes-yes-percent";
		delimiter2 = "\">";
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "votes-";
		delimiter2 = "es-";
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "\">"; //special condition -number of vote is changing dynamic 
		delimiter2 = "</span>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_vote.push_back(tmp);

		//second vote
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "vot";
		delimiter2 = "s-";
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "\">"; //special condition -number of vote is changing dynamic 
		delimiter2 = "</span>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_secondVote.push_back(tmp);


		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "product-reviewer\">";
		globalCounter += 7;
	}
	return true;
}

void correctAuthor(std::vector<std::string> & _author)
{
	std::vector<std::string>::iterator it;
	int i = 0;
	for (it = _author.begin(); it < _author.end(); it++, i++)
	{
		int count = _author[i].size();
		if (count > 30)
		{
			std::string extractCount = _author[i].substr(count - 17, 6); //check if it is "Anonim" it is always 17 char from end of string
			if (extractCount == "Anonim")
			{
				_author[i] = "anonim";
			}
		}
	}
}
bool saveToFile(std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus,
	std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date,
	std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote, int ifUpdate, bool ifCvs)
{
	std::vector<std::string>::iterator it;
	int i = 0;
	if (ifCvs)
	{
		openCvsFileToWrite();
	}
	else
	{
		openFileToWrite();
	}
	if (ifUpdate != 0)
	{
		//plik << productCode << std::endl;
		plik << _appliance[0] << std::endl;
		plik << _producerModel[0] << std::endl;
		globalCounter += 2;
		for (it = _feature.begin(); it < _feature.end(); it++, i++)
		{
			plik << _feature[i] << std::endl;
			plik << _howManyPlus[i] << std::endl;
			plik << _howManyMinus[i] << std::endl;
			globalCounter += 3;
		}
	}
	i = 0;
	for (it = _author.begin(); it < _author.end(); it++, i++)
	{
		if (_author[i] == "")
			continue;
		plik << _author[i] << std::endl;
		plik << _ifRecommend[i] << std::endl;
		plik << _noOfStars[i] << std::endl;
		plik << _date[i] << std::endl;
		plik << _opinion[i] << std::endl;
		plik << _vote[i] << std::endl;
		plik << _secondVote[i] << std::endl;
		globalCounter += 7;
	}

	closeFile();
	return true;
}

std::string createFirstlink()
{
	cout << "Podaj kod produktu i wcisnij enter: ";
	cin >> productCode;

	//productCode = "32065635";//"28280564"//"39580136";
	std::string link = "http://www.ceneo.pl/";
	link += productCode;
	link += "#tab=reviews_scroll";

	//cout << "zwracam taki link: " << link << endl;
	//cout << "link = " << link << endl;
	//const char * link2 = link.c_str();
	return link;
}

void openFileWithCodes(void)
{
	std::fstream fileWithCodes;
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\productsCode.txt";
	fileWithCodes.open(fileName, std::fstream::out | std::fstream::app);
	if (fileWithCodes.is_open() == false)
	{
		//fileWithCodes.open(fileName, std::fstream::trunc | std::fstream::in | std::fstream::out);
		//fileWithCodes.close();
		// re-open with original flags
		//fileWithCodes.open(fileName, std::fstream::out | std::fstream::app);
		cout << "Nie udalo sie utworzyc pliku productCodes!" << endl;
	}
	fileWithCodes << productCode << endl;
	fileWithCodes.close();
}

void downloadFirstSite(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		std::string linker = createFirstlink();
		const char * link2 = linker.c_str(); //link.c_str();
		//cout << "LINK 1 strony: " << link2 << endl; //temporary 4 tests
		curl_easy_setopt(curl, CURLOPT_URL, link2);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_mainBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		proccedExtractOnce(_mainBuffer, _appliance, _producerModel, _feature, _howManyPlus, _howManyMinus);

		//std::cout << _appliance[0] << endl;
		//std::cout << _producerModel[0] << endl;
		//std::vector<std::string>::iterator it;

		int count = 0;
		proccedExtractMultiply(_mainBuffer, _author, _ifRecommend, _noOfStars, _date, _opinion, _vote, _secondVote);//only main page so counter 0
	}
}
void downloadRestSite(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	CURL *curl;
	CURLcode res;

	int counter = 2;
	std::string delimiter;
	int pos = 0;
	while (1)
	{
		curl = curl_easy_init();
		if (curl)
		{
			delimiter = "<li class=\"page-arrow arrow-next\">"; //Does exist another subpage?
			if ((pos = _mainBuffer.find(delimiter)) == std::string::npos)//(!(pos = mainBuffer.find(delimiter)))
			{
				break;
			}


			std::string linker = createRestLinks(counter);
			const char * link2 = linker.c_str();
			curl_easy_setopt(curl, CURLOPT_URL, link2);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_mainBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			proccedExtractMultiply(_mainBuffer, _author, _ifRecommend, _noOfStars, _date, _opinion, _vote, _secondVote);
			counter++;
		}
		else
		{
			cout << "Curl not loaded!" << endl;
		}
	}
}
std::string createRestLinks(int & _counter)
{
	std::string link = "http://www.ceneo.pl/"; // XXX / opinie - ";

	link += productCode;
	link += "/opinie-";
	std::string _str = std::to_string(_counter);
	link += _str;

	return link;
}

void createFileToRead(void)
{
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += productCode;
	fileName += ".txt";
	plik.open(fileName, std::fstream::in /*| std::fstream::app*/);
	if (plik.is_open() == false)
	{
		cout << "Podany plik: " << fileName << "  nie istnieje!" << endl;
	}

}
void updateFile(std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	createFileToRead();
	int i = 0;
	std::string line;
	while (getline(plik, line))
	{
		std::vector<std::string>::iterator it;
		i = 0;
		for (it = _opinion.begin(); it < _opinion.end(); it++, i++)
		{
			correctAuthor(_author);
			removeNewLine(_opinion);
			if (line == _opinion[i])
			{
				_opinion[i].erase();
				_author[i].erase();
				_ifRecommend[i].erase();
				_noOfStars[i].erase();
				_date[i].erase();
				_vote[i].erase();
				_secondVote[i].erase();
			}
		}
	}
	//i = 0;
	//std::vector<std::string>::iterator it;

	closeFile();

}
bool loadFromFile(std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	closeFile();
	std::string fileName = "E:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += productCode;
	fileName += "nr.txt";
	std::fstream file;
	file.open(fileName, std::fstream::in /*| std::fstream::app*/);
	if (file.is_open() == false)
	{
		cout << "Brak wpisow w bazie danych dla podanego kodu!" << endl;
		file.close();
		return false;
	}
	int featureCounter = 0;
	std::string featureCount;
	std::getline(file, featureCount);
	file.close();
	featureCounter = atoi(featureCount.c_str());
	//cout << "featureCounter" <<featureCounter<< endl;
	createFileToRead(); //only open but the same method is used

	std::string line;
	getline(plik, line);
	_appliance.push_back(line);
	getline(plik, line);
	_producerModel.push_back(line);

	for (int j = 0; j < featureCounter; j++)
	{
		getline(plik, line);
		_feature.push_back(line);
		getline(plik, line);
		_howManyPlus.push_back(line);
		getline(plik, line);
		_howManyMinus.push_back(line);
	}
	int i = 0;
	std::string test = "test";
	while (1)
	{
		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_author.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_ifRecommend.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_noOfStars.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_date.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_opinion.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_vote.push_back(line);

		line = ifLineExist(test);
		if (line == "koniecPliku")
			break;
		_secondVote.push_back(line);
		i++;
	}
	closeFile();
	return true;
}
std::string ifLineExist(std::string & _line)
{
	getline(plik, _line);

	if (plik.eof())
		return "koniecPliku";
	if (_line.length() == 0) //empty line
		ifLineExist(_line);
	if (_line.length() == 1)
	{
		if (is_number(_line))
			return _line;
		ifLineExist(_line);
		//if (_line == "\n" )
		//ifLineExist(_line);


	}

	return _line;
}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}


// ------------- OPINEO ------------
void extractOpineo(std::string & _mainBuffer, std::vector<std::string> & _appliance, std::vector<std::string> & _producerModel, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	std::string product = _producerModel[0]; // zmienna wyswietlana po wpisaniu kodu produktu zawierajaca nazwe produktu  
	if (product.empty())
		return;
	cout << "Wyszukuje " << product << " w Opineo.pl" << endl;
	std::replace(product.begin(), product.end(), ' ', '+');
	std::transform(product.begin(), product.end(), product.begin(), ::tolower);
	product.erase(product.find_last_not_of("+ \n\r\t") + 1);
	std::string link = "http://www.opineo.pl/?szukaj=";
	link += product;
	link += "&s=2";
	//cout << "Link wyszukiwania: " << link << endl; // DEBUG
	std::string productLink;
	if (!findProductInSearchResults(link, productLink, _producerModel[0]))
	{
		cout << "Nie znaleziono produktu w Opineo.pl" << endl;
		return;
	}
	//cout << "Link do produktu na Opineo.pl: " << productLink << endl; //DEBUG

	downloadProductSiteOpineo(_mainBuffer, productLink, _feature, _howManyPlus, _howManyMinus, _author, _ifRecommend, _noOfStars, _date, _opinion, _vote, _secondVote);
}

bool findProductInSearchResults(std::string _originalLink, std::string & _newLink, std::string product)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	std::string buffer = "pusty";
	if (curl)
	{
		const char * cLink = _originalLink.c_str(); //link.c_str();
		curl_easy_setopt(curl, CURLOPT_URL, cLink);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (buffer == "pusty")
		{
			cout << "Nie udalo sie pobrac strony wyszukiwania" << endl;
			return false;
		}
		//cout << "Przetwarzanie strony wyszukiwania" << endl; // DEBUG
		// przerobienie html na male litery
		std::transform(buffer.begin(), buffer.end(), buffer.begin(), ::tolower);
		std::transform(product.begin(), product.end(), product.begin(), ::tolower);
		int nawias;
		if ((nawias = product.find("(")) != std::string::npos)
		{
			product.erase(nawias);
			//cout << "Usunieto (" << endl; // DEBUG
		}

		std::string delimiter = "divollo' title='"; // "<a class = \"divollo\" title = \"";
		std::string delimiter2 = "' ";
		int pos, pos2, count = 0, curDist;// , dist1 = 1500, dist2 = 1500;
		std::string tmp;
		//saveBufferToFile("search", buffer);
		std::string delimLink1 = "href='";
		std::string delimLink2 = "'>";
		//std::string primarySolution = "", secondarySolution = "";
		std::string curLink = "";// , secondaryLink = "";
		// kazde rozwiazanie: nazwa link i odleglosc 
		std::vector<std::string> foundSolutions(5, "");  //"" pusty wektor pod wyszukane 5 produktow
		std::vector<std::string> foundLinks(5, "");
		std::vector<int> distances(5, 1500); 
		while ((pos = buffer.find(delimiter)) != std::string::npos)
		{
			tmp = parser(buffer, delimiter, delimiter2, pos, pos2);
			clearBuffer(buffer, delimiter, delimiter2);
			curDist = sentence_distance(product, tmp);
			curLink = "http://www.opineo.pl" + parser(buffer, delimLink1, delimLink2, pos, pos2);

			// dokladnie dopasowuje nam znalezione wyniki wyszukiwania
			// np. znaleziony 3 produkt jest lepszy od 2 wiec zamieniaja sie miejscami itd
			for (int i = 0; i < foundSolutions.size(); i++)
			{
				if (curDist < distances[i])
				{
					foundSolutions.erase(foundSolutions.end() - 1);
					foundLinks.erase(foundLinks.end() - 1);
					distances.erase(distances.end() - 1);

					foundSolutions.insert(foundSolutions.begin() + i, tmp);
					foundLinks.insert(foundLinks.begin() + i, curLink);
					distances.insert(distances.begin() + i, curDist);
					if (curDist == 0)
					{
						cout << "Znaleziono dokladne dopasowanie: " << tmp << endl; // DEBUG
						_newLink = curLink;// nasza zmienna z gotowym linkiem produktu "" 
						return true;
					}
					count++;
					break;
				}
			}
			/*cout << "Aktualna postac wektorow:" << endl;
			for (int i = 0; i < foundSolutions.size(); i++)
			cout << i + 1 << ". : " << foundSolutions[i] << " dist=" << distances[i] << endl;
			system("PAUSE");*/
		}
		// tu sie znajdziemy jak nie udalo sie dokladne dopasowanie
		if (count == 0)
		{
			cout << "Nie znaleziono zadnych produktow dla frazy" << product << "." << endl;
			return false;
		}

		// tu sie znajdziemy jak znalezlismy cokolwiek ale nie dokladnie to czego szukalismy - struktura wyswietlania znalezionch produktow
		if (count > foundSolutions.size())
			count = foundSolutions.size();
		cout << "Propozycja " << count << " mozliwych dopasowan" << endl;
		for (int i = 0; i < count; i++)
			cout << i + 1 << ". : " << foundSolutions[i] << endl;

		for (int i = 0; i < count; i++)
		{
			cout << "Czy akceptujesz dopasowanie: " << foundSolutions[i] << "?";
			std::string decision;
			cin >> decision;
			if (decision == "tak" || decision == "t")
			{
				_newLink = foundLinks[i];
				return true;
			}
		}
	}
	else
	{
		cout << "Nie udalo sie nawiazac polaczenia." << endl;
	}
	// Wyszukiwanie na opineo.pl nie dalo rezultatow
	return false;
}

bool saveBufferToFile(std::string filename, std::string & buffer)
{
	std::fstream oFile;
	std::string fileName = "e:\\magister\\uek\\hurtowanie_danych\\zad2\\files\\";
	fileName += filename;
	fileName += ".html";
	oFile.open(fileName, std::fstream::out | std::fstream::app);
	if (oFile.is_open() == false)
	{

		oFile.open(fileName, std::fstream::trunc | std::fstream::in | std::fstream::out);
		oFile.close();
		// re-open with original flags
		oFile.open(fileName, std::fstream::out | std::fstream::app);
		if (oFile.is_open())
		{
			cout << "Za pierwszym razem nie udalo sie utworzyc pliku do zapisu!" << endl;
		}
		else
		{
			cout << "Ponownie nie udalo sie utworzyc pliku do zapisu!" << endl;
			return false;
		}
	}
	// plik jest otwarty
	oFile << buffer;
	return true;
}

void downloadProductSiteOpineo(std::string & _mainBuffer, std::string linker, std::vector<std::string> & _feature, std::vector<std::string> & _howManyPlus, std::vector<std::string> & _howManyMinus, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	bool nextPageExists = true;
	int countOpinions = 0;
	CURL *curl;
	CURLcode res;
	std::string opineoPrefix = "http://www.opineo.pl/";

	while (nextPageExists)
	{

		curl = curl_easy_init();
		if (curl)
		{
			_mainBuffer = "";
			const char * link2 = linker.c_str();
			//cout << "Pobieranie strony z opiniami: " << link2 << endl; //temporary 4 tests
			curl_easy_setopt(curl, CURLOPT_URL, link2);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_mainBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
							// wywolanie funkcji 
			countOpinions += extractOpinionsOpineo(_mainBuffer, _author, _ifRecommend, _noOfStars, _date, _opinion, _vote, _secondVote);
			if (countOpinions == 0)
			{
				cout << "Brak opinii dla tego produktu" << endl;
				return;
			}

			// szukamy czy sa nastepne paginacje
			//std::string nextPageLink = "";
			std::string nextPageDel = "strona' rel='next'"; // "title = 'Nastêpna strona'";
			std::string delimiter = "href = '";
			std::string delimiter2 = "' alt";
			int pos, pos2;
			if ((pos = _mainBuffer.find(nextPageDel)) != std::string::npos)
			{
				clearBuffer(_mainBuffer, "", nextPageDel);
				linker = opineoPrefix;
				linker += parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
				clearBuffer(_mainBuffer, delimiter, delimiter2);
				//cout << "Istnieje kolejna strona do sprawdzenia: " << linker << endl; // DEBUG
			}
			else
			{
				nextPageExists = false;
				//cout << "Nie ma wiecej stron z opiniami" << endl; // DEBUG
			}
		}
	}
	cout << "Pobrano " << countOpinions << " opinii z Opineo.pl" << endl; // DEBUG
}

int extractOpinionsOpineo(std::string & _mainBuffer, std::vector<std::string> & _author, std::vector<std::string> & _ifRecommend, std::vector<std::string> & _noOfStars, std::vector<std::string> & _date, std::vector<std::string> & _opinion, std::vector<std::string> & _vote, std::vector<std::string> & _secondVote)
{
	std::string delimiter;
	std::string delimiter2;
	int pos, pos2, opinionCounter = 0;
	std::string tmp;

	clearBuffer(_mainBuffer, delimiter, delimiter2);
	//sprawdzmy czy sa jakies opinie
	delimiter = "sh_rnote' itemprop='ratingValue'>";
	delimiter2 = "</span>";
	if ((pos = _mainBuffer.find(delimiter)) != std::string::npos)
	{
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		//cout << "Liczba opinii: " << tmp << endl; // DEBUG 
		int opinionCount = ::atoi(tmp.c_str());
		if (opinionCount <= 0)
			return 0;
	}
	else
	{
		//cout << "extractOpinionsOpineo - brak znacznika z liczba opinii" << endl; // DEBUG
		return 0;
	}
	//cout << "Przegladanie opinii w toku.." << endl; // DEBUG
	delimiter = "revz_container";
	while ((pos = _mainBuffer.find(delimiter)) != std::string::npos)
	{
		//number of stars
		delimiter2 = "review_badge pos";
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "<strong>";
		delimiter2 = "</strong>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		//cout << "Ocena: " << tmp; // DEBUG 
		_noOfStars.push_back(tmp);
		float noOfStars = ::atof(tmp.c_str());
		if (noOfStars < 3.0)
		{
			//cout << " Nie polecam ";
			_ifRecommend.push_back("Nie polecam");
		}
		else if (noOfStars == 3.0)
		{
			//cout << " Nie mam zdania ";
			_ifRecommend.push_back("Nie mam zdania");
		}
		else
		{
			//cout << " Polecam ";
			_ifRecommend.push_back("Polecam");
		}


		//author
		delimiter = "itemprop='author'>";
		delimiter2 = "</span>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		//cout << " autor: " << tmp; // DEBUG 
		_author.push_back(tmp);

		//date
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "itemprop='datePublished' content='";
		delimiter2 = "'/>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		//cout << " date: " << tmp << endl; // DEBUG  
		_date.push_back(tmp);

		//opinion
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "itemprop='description'>";// opinion
		delimiter2 = "</span>";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		//cout << tmp; // DEBUG  
		_opinion.push_back(tmp);

		//vote
		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "revz_points'>";
		delimiter2 = "pkt";
		tmp = parser(_mainBuffer, delimiter, delimiter2, pos, pos2);
		_vote.push_back(tmp);
		//second vote
		_secondVote.push_back(tmp + '\n');
		//cout << endl << "Vote: " << tmp << "/" << tmp << endl; // DEBUG

		clearBuffer(_mainBuffer, delimiter, delimiter2);
		delimiter = "revz_container";
		globalCounter += 7;
		opinionCounter++;
	}
	return opinionCounter;
}

// ==== TEXT MATCHING funcrions ======
// source: http://ideone.com/93IhH2
//algorytm levebstuna napotrzeby 
template<typename T, typename C>
size_t
seq_distance(const T& seq1, const T& seq2, const C& cost,
const typename T::value_type& empty = typename T::value_type()) {
	const size_t size1 = seq1.size();
	const size_t size2 = seq2.size();

	std::vector<size_t> curr_col(size2 + 1);
	std::vector<size_t> prev_col(size2 + 1);

	prev_col[0] = 0;
	for (size_t idx2 = 0; idx2 < size2; ++idx2) {
		prev_col[idx2 + 1] = prev_col[idx2] + cost(empty, seq2[idx2]);
	}

	for (size_t idx1 = 0; idx1 < size1; ++idx1) {
		curr_col[0] = curr_col[0] + cost(seq1[idx1], empty);

		for (size_t idx2 = 0; idx2 < size2; ++idx2) {
			curr_col[idx2 + 1] = min(min(
				curr_col[idx2] + cost(empty, seq2[idx2]),
				prev_col[idx2 + 1] + cost(seq1[idx1], empty)),
				prev_col[idx2] + cost(seq1[idx1], seq2[idx2]));
		}

		curr_col.swap(prev_col);
		curr_col[0] = prev_col[0];
	}

	return prev_col[size2];
}

size_t
letter_distance(char letter1, char letter2) {
	return letter1 != letter2 ? 1 : 0;
}

size_t
word_distance(const std::string& word1, const std::string& word2) {
	return seq_distance(word1, word2, &letter_distance);
}

size_t
sentence_distance(const std::string& sentence1, const std::string& sentence2) {
	std::vector<std::string> words1;
	std::vector<std::string> words2;
	std::istringstream iss1(sentence1);
	std::istringstream iss2(sentence2);
	for (std::istream_iterator<std::string> it(iss1), end;;) {
		words1.push_back(*it);
		if (++it == end)
			break;
		words1.push_back(" ");
	}
	for (std::istream_iterator<std::string> it(iss2), end;;) {
		words2.push_back(*it);
		if (++it == end)
			break;
		words2.push_back(" ");
	}
	return seq_distance(words1, words2, &word_distance);
}
// ======================================