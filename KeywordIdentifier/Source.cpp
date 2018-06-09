// Keyword Identifier

// Use a source that contains word frequencies to determine the score of each word based on global frequency

// For each word in a document, add the global frequency score 

// Sort words in document by frequency score (from lowest to highest)

// Choose a number of words (with less global frequency) to use for the summary (10%?)

// Display the unique words in the summary

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include <cstdlib>

using namespace std;

struct Word {
	string word;
	long long frequency;
	long long globalFreq;
};

struct lessFreq {
	inline bool operator() (const Word& word1, const Word& word2) {
		return (word1.globalFreq < word2.globalFreq);
	}
};

struct greaterFreq {
	inline bool operator() (const Word& word1, const Word& word2) {
		return (word1.frequency > word2.frequency);
	}
};

double docWordsCount = 0;

struct orderForSummary {
	inline bool operator() (const Word& word1, const Word& word2) {
		double first = ((double)word1.frequency / (double)docWordsCount) / (double)log((double)word1.globalFreq + 2);
		double second = ((double)word2.frequency / (double)docWordsCount) / (double)log((double)word2.globalFreq + 2);
		return (first > second);
	}
};

map<string, long long> globalDictionary;
map<string, Word> documentDictionary;
map<string, vector<string>> lemmaDictionary; // base word, vector<alternate words>
vector<Word> words;

string globalFile = "wordFreq.txt"; // global word frequencies
string localFile = "";
string lemmaFile = "e_lemma.txt";

const int freqBound = 124000;
int summarySize = 30;

void populateGlobalOverall();
void populateGlobalWords(string);
void populateDocumentWords(string);
void populateLemmaMap(string);
void summarizeText();

int main(int argc, char** argv) {

	// get document to summarize from user
	//istringstream(argv[1]) >> localFile;
	//localFile = argv[1];

	vector<string> texts = { "alice29.txt", "aiWiki.txt", "goldenretrieverWiki.txt", "cogPsych.txt" };
	//localFile = "alice29.txt";
	//localFile = "testing.txt";
	//localFile = "aiWiki.txt";
	//localFile = "goldenretrieverWiki.txt";

	populateGlobalOverall();

	for (int i = 0; i < texts.size(); i++) {
		localFile = texts[i];
		cout << "Summarize " << localFile << endl;
		summarizeText();

		documentDictionary.clear();
		words.clear();
	}

}

void populateGlobalOverall() {
	// populate global words using global file
	populateGlobalWords(globalFile);

	// populate lemma map
	populateLemmaMap(lemmaFile);
}

void summarizeText() {

	// populate document words using file given by user
	populateDocumentWords(localFile);

	// assign global frequencies to document word
	for (auto& word : documentDictionary) {
		word.second.globalFreq = globalDictionary[word.first];
	}

	// sort document words by 
	//sort(documentDictionary.begin(), documentDictionary.end(), greaterFreq());

	// check if it meets a criteria
	// remove words with high global frequencies
	for (auto& word : documentDictionary) {
		Word w;
		if (word.second.globalFreq < freqBound && word.first.size() != 0) {
			w.word = word.first;
			w.globalFreq = word.second.globalFreq;
			w.frequency = word.second.frequency;
			words.push_back(w);
		}
	}

	// sort by lower frequency globally
	sort(words.begin(), words.end(), lessFreq());

	// sort by greater frequency in document
	sort(words.begin(), words.end(), greaterFreq());

	sort(words.begin(), words.end(), orderForSummary());

	// only keep words for summary
	vector<Word> summary;
	if (words.size() < summarySize) {
		summarySize = words.size();
	}
	for (int i = 0; i < summarySize; i++) {
		summary.push_back(words[i]);
	}

	// sort by lower frequency globally
	sort(summary.begin(), summary.end(), lessFreq());

	cout << "Word" << " [document frequency, global frequency]" << endl;
	cout << "------------------------------------" << endl << endl;

	for (int i = 0; i < summarySize; i++) {
		cout << summary[i].word << " [" << summary[i].frequency << ", " << summary[i].globalFreq << "]" << endl;
	}
	cout << endl;
}

void populateGlobalWords(string file) {
	ifstream infile(file);

	// Populate globalWords vector with words and their corresponding frequencies
	if (infile.is_open()) {
		string line;
		for (;;) {
			if (!getline(infile, line)) {
				break;
			}

			transform(line.begin(), line.end(), line.begin(), ::tolower);

			// deliminate by space to separate words ////////////////////////////
			stringstream ss(line);

			string word;
			string frequency;
			
			ss >> word;
			ss >> frequency;
			
			long long f = stoll(frequency);
			globalDictionary[word] = f;

			//string item;

			//int count = 1;

			//string word = "";
			//int frequency = 0;

			//while (ss >> item) {

			//	// if word != "" --> 
			//	if (item.compare("") != 0) {
			//		// If item is first, it is the word
			//		if (count == 1) {
			//			transform(item.begin(), item.end(), item.begin(), ::tolower);
			//			int countChar = 0;
			//			for (char c : item) {
			//				if (c != ' ' && c != -96) {
			//					if ((c >= 'a' && c <= 'z') || c == '\'') {
			//						if (countChar == 0 || countChar == item.size()) {
			//							if (c != '\'') {
			//								word += c;
			//							}
			//						}
			//						else {
			//							word += c;
			//						}
			//					}
			//				}
			//				countChar++;
			//			}
			//			count++;
			//		}
			//		// If item is second, it is the frequency
			//		else if (count == 2) {
			//			frequency = stoi(item);
			//			count == 1;

			//			// add word to globalWords vector
			//			globalDictionary[word] = frequency;

			//			word = "";
			//		}
			//	}
			//}
		}
	}
	else {
		cout << "Cannot open file" << endl;
	}

	// close file
	infile.close();
}

// if base word is found for the given alternate word, return frequency of base global word

// map alternate words to their corresponding base words
void populateLemmaMap(string file) {
	ifstream infile(file);

	// Populate alternate words with words and their corresponding lemma/base words
	if (infile.is_open()) {
		string line;
		for (;;) {
			if (!getline(infile, line)) {
				break;
			}

			transform(line.begin(), line.end(), line.begin(), ::tolower);
			// deliminate by space to separate words ////////////////////////////
			stringstream ss(line);
			string item;
			bool alternateWords = false;
			vector<string> altWords;
			string lemma = "";

			while (ss >> item) {

				// delimiter
				if (item.compare("->") == 0) {
					alternateWords = true;
				}

				// lemma
				else if (item.compare("") != 0 && alternateWords == false) {
					int count = 0;
					for (char c : item) {
						if (c != ' ' && c != -96) {
							if ((c >= 'a' && c <= 'z') || c == '\'') {
								if (count == 0 || count == item.size()) {
									if (c != '\'') {
										lemma += c;
									}
								}
								else {
									lemma += c;
								}
							}
						}
						count++;
					}
				}

				// alternate words
				else {
					int count = 0;
					string altw = "";
					for (char c : item) {
						// delimiter reached
						if (c == ',') {
							altWords.push_back(altw);
							altw = "";
						}
						else if (c != ' ' && c != -96) {
							altw += c;
						}
					}
					if (altw != "") {
						altWords.push_back(altw);
					}
				}
			}

			// populate lemma map
			lemmaDictionary[lemma] = altWords;
		}
	}
	else {
		cout << "Cannot open file" << endl;
	}

	// close file
	infile.close();

	// if lemma is found in global dictionary, add alternate words to global dictionary with corresponding frequencies
	for (auto& item : lemmaDictionary) {
		if (item.first == "do" || item.first == "don't") {
			int x = 0;
		}
		// if lemma found in global dictionary
		if (globalDictionary.find(item.first) != globalDictionary.end()) {
			// for each alternate word, add alt words to global
			for (string word : item.second) {

				if (globalDictionary.find(word) != globalDictionary.end()) {
					//globalDictionary.emplace(make_pair(word, globalDictionary[item.first]));
					string testWord = word;
					long long testFreq = globalDictionary.find(word)->second; //globalDictionary.at(item.first);
					globalDictionary.emplace(make_pair(testWord, (long long)5));
					//globalDictionary[word] = globalDictionary[updateItem];
				}
			}
		}
	}
}

void populateDocumentWords(string file) {
	ifstream infile(file);

	// Populate globalWords vector with words and their corresponding frequencies
	if (infile.is_open()) {
		string line;
		for (;;) {
			if (!getline(infile, line)) {
				break;
			}

			transform(line.begin(), line.end(), line.begin(), ::tolower);
			// deliminate by space to separate words ////////////////////////////
			stringstream ss(line);
			string item;

			while (ss >> item) {
				string word = "";
				// if word != "" --> 
				// if (item.compare("") != 0) {
				if (item != "") {
					int count = 0;
					for (char c : item) {
						if (c != ' ' && c != -96) {
							if ((c >= 'a' && c <= 'z') || c == '\'') {
								if (count == 0 || count == item.size() - 1) {
									if (c != '\'') {
										word += c;
									}
								}
								else {
									word += c;
								}
							}
						}
						count++;
					}
					if (documentDictionary.find(word) == documentDictionary.end()) {
						//not found
						documentDictionary[word].frequency = 1;
					}
					else {
						//found
						documentDictionary[word].frequency++;
					}
				}
			}
		}
	}
	else {
		cout << "Cannot open file" << endl;
	}

	// close file
	infile.close();

	docWordsCount = documentDictionary.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Keyword Identifier

// Use a source that contains word frequencies to determine the score of each word based on global frequency

// For each word in a document, add the global frequency score 

// Sort words in document by frequency score (from lowest to highest)

// Choose a number of words (with less global frequency) to use for the summary (10%?)

// Display the unique words in the summary
//
//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <sstream>
//#include <algorithm>
//#include <map>
//#include <stdio.h>
//#include <stdlib.h>
//#include <locale>
//#include <cstdlib>
//
//using namespace std;
//
//struct Word {
//	string word;
//	long long frequency; // word freq in document
//	long long globalFreq; // word frequency globally
//};
//
//struct lessFreq {
//	inline bool operator() (const Word& word1, const Word& word2) {
//		return (word1.globalFreq < word2.globalFreq);
//	}
//};
//
//struct greaterFreq {
//	inline bool operator() (const Word& word1, const Word& word2) {
//		return (word1.frequency > word2.frequency);
//	}
//};
//
//long long docWordsCount = 0;
//
//struct orderForSummary {
//	inline bool operator() (const Word& word1, const Word& word2) {
//		double first = ((double)word1.frequency / (double)docWordsCount) / (double)log((double)word1.globalFreq + 2);
//		double second = ((double)word2.frequency / (double)docWordsCount) / (double)log((double)word2.globalFreq + 2);
//		return (first > second);
//	}
//};
//
//map<string, long long> globalDictionary;
//map<string, Word> documentDictionary;
//map<string, vector<string>> lemmaDictionary; // base word, vector<alternate words>
//vector<Word> words;
//
//string globalFile = "wordFreq.txt"; // global word frequencies
//									//string globalFile = "wordFrequency.txt";
//string localFile = "";
//string lemmaFile = "e_lemma.txt";
//
//const long freqBound = 124000;
//int summarySize = 30;
//
//void populateGlobalOverall();
//void populateGlobalWords(string);
//void populateDocumentWords(string);
//void populateLemmaMap(string);
//void summarizeText();
//
//int main(int argc, char** argv) {
//
//	// get document to summarize from user
//	//istringstream(argv[1]) >> localFile;
//	//localFile = argv[1];
//
//	vector<string> texts = { "alice29.txt", "aiWiki.txt", "goldenretrieverWiki.txt" };
//	//localFile = "alice29.txt";
//	//localFile = "testing.txt";
//	//localFile = "aiWiki.txt";
//	//localFile = "goldenretrieverWiki.txt";
//
//	populateGlobalOverall();
//
//	for (int i = 0; i < texts.size(); i++) {
//		localFile = texts[i];
//		cout << "Summarize " << localFile << endl;
//		summarizeText();
//
//		documentDictionary.clear();
//		words.clear();
//	}
//
//}
//
//void populateGlobalOverall() {
//	// populate global words using global file
//	populateGlobalWords(globalFile);
//
//	// populate lemma map
//	populateLemmaMap(lemmaFile);
//}
//
//void summarizeText() {
//
//	// populate document words using file given by user
//	populateDocumentWords(localFile);
//
//	// assign global frequencies to document word
//	for (auto& word : documentDictionary) {
//		word.second.globalFreq = globalDictionary[word.first];
//	}
//
//	// sort document words by 
//	//sort(documentDictionary.begin(), documentDictionary.end(), greaterFreq());
//
//	// check if it meets a criteria
//	// remove words with high global frequencies
//	for (auto& word : documentDictionary) {
//		Word w;
//		if (word.second.globalFreq < freqBound && word.first.size() != 0) {
//			w.word = word.first;
//			w.globalFreq = word.second.globalFreq;
//			w.frequency = word.second.frequency;
//			words.push_back(w);
//		}
//	}
//
//	// sort by lower frequency globally
//	//sort(words.begin(), words.end(), lessFreq());
//
//	// sort by greater frequency in document
//	//sort(words.begin(), words.end(), greaterFreq());
//
//	// only keep words for summary
//	vector<Word> summary;
//	if (words.size() < summarySize) {
//		summarySize = words.size();
//	}
//	for (int i = 0; i < summarySize; i++) {
//		summary.push_back(words[i]);
//	}
//
//	// sort by lower frequency globally
//	//sort(summary.begin(), summary.end(), lessFreq());
//
//	// sort for summary
//	sort(words.begin(), words.end(), orderForSummary());
//
//	cout << "Word" << " [document frequency, global frequency]" << endl;
//	cout << "------------------------------------" << endl << endl;
//
//	for (int i = 0; i < summarySize; i++) {
//		cout << summary[i].word << " [" << summary[i].frequency << ", " << summary[i].globalFreq << "]" << endl;
//	}
//	cout << endl;
//}
//
//void populateGlobalWords(string file) {
//	ifstream infile(file);
//
//	// Populate globalWords vector with words and their corresponding frequencies
//	if (infile.is_open()) {
//		string line;
//		for (;;) {
//			if (!getline(infile, line)) {
//				break;
//			}
//
//			// deliminate by space to separate words ////////////////////////////
//			stringstream ss(line);
//
//			string word;
//			string frequency;
//
//			ss >> word;
//			ss >> frequency;
//
//			long long f = stoll(frequency);
//			globalDictionary[word] = f;
//		}
//	}
//	else {
//		cout << "Cannot open file" << endl;
//	}
//
//	// close file
//	infile.close();
//}
//
//// if base word is found for the given alternate word, return frequency of base global word
//
//// map alternate words to their corresponding base words
//void populateLemmaMap(string file) {
//	ifstream infile(file);
//
//	// Populate alternate words with words and their corresponding lemma/base words
//	if (infile.is_open()) {
//		string line;
//		for (;;) {
//			if (!getline(infile, line)) {
//				break;
//			}
//
//			transform(line.begin(), line.end(), line.begin(), ::tolower);
//			// deliminate by space to separate words ////////////////////////////
//			stringstream ss(line);
//			string item;
//			bool alternateWords = false;
//			vector<string> altWords;
//			string lemma = "";
//
//			while (ss >> item) {
//
//				// delimiter
//				if (item.compare("->") == 0) {
//					alternateWords = true;
//				}
//
//				// lemma
//				else if (item.compare("") != 0 && alternateWords == false) {
//					int count = 0;
//					for (char c : item) {
//						if (c != ' ' && c != -96) {
//							if ((c >= 'a' && c <= 'z') || c == '\'') {
//								if (count == 0 || count == item.size()) {
//									if (c != '\'') {
//										lemma += c;
//									}
//								}
//								else {
//									lemma += c;
//								}
//							}
//						}
//						count++;
//					}
//				}
//
//				// alternate words
//				else {
//					int count = 0;
//					string altw = "";
//					for (char c : item) {
//						// delimiter reached
//						if (c == ',') {
//							altWords.push_back(altw);
//							altw = "";
//						}
//						else if (c != ' ' && c != -96) {
//							altw += c;
//						}
//					}
//					if (altw != "") {
//						altWords.push_back(altw);
//					}
//				}
//			}
//
//			// populate lemma map
//			lemmaDictionary[lemma] = altWords;
//		}
//	}
//	else {
//		cout << "Cannot open file" << endl;
//	}
//
//	// close file
//	infile.close();
//
//	// if lemma is found in global dictionary, add alternate words to global dictionary with corresponding frequencies
//	for (auto& item : lemmaDictionary) {
//		// if lemma found in global dictionary
//		if (globalDictionary.find(item.first) != globalDictionary.end()) {
//			// for each alternate word, add alt words to global
//			for (string word : item.second) {
//				if (globalDictionary.find(word) != globalDictionary.end()) {
//					globalDictionary[word] = globalDictionary[item.first];
//				}
//			}
//		}
//	}
//}
//
//void populateDocumentWords(string file) {
//	ifstream infile(file);
//
//	// Populate globalWords vector with words and their corresponding frequencies
//	if (infile.is_open()) {
//		string line;
//		for (;;) {
//			if (!getline(infile, line)) {
//				break;
//			}
//
//			transform(line.begin(), line.end(), line.begin(), ::tolower);
//			// deliminate by space to separate words ////////////////////////////
//			stringstream ss(line);
//			string item;
//
//			while (ss >> item) {
//				string word = "";
//				// if word != "" --> 
//				// if (item.compare("") != 0) {
//				if (item != "") {
//					int count = 0;
//					for (char c : item) {
//						if (c != ' ' && c != -96) {
//							if ((c >= 'a' && c <= 'z') || c == '\'') {
//								if (count == 0 || count == item.size() - 1) {
//									if (c != '\'') {
//										word += c;
//									}
//								}
//								else {
//									word += c;
//								}
//							}
//						}
//						count++;
//					}
//					if (documentDictionary.find(word) == documentDictionary.end()) {
//						//not found
//						documentDictionary[word].frequency = (long long)1;
//					}
//					else {
//						//found
//						documentDictionary[word].frequency++;
//					}
//				}
//			}
//		}
//	}
//	else {
//		cout << "Cannot open file" << endl;
//	}
//
//	// close file
//	infile.close();
//
//	docWordsCount = documentDictionary.size();
//}
//
