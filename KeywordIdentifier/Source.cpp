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
	int frequency;
	int globalFreq;
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

map<string, int> globalDictionary; 
map<string, Word> documentDictionary;
vector<Word> words;

string globalFile = "wordFrequency.txt"; // global word frequencies
string localFile;
const int freqBound = 124000;
int summarySize = 40;

void populateGlobalWords(string);
void populateDocumentWords(string);

int main(int argc, char** argv) {

	// get document to summarize from user
	//istringstream(argv[1]) >> localFile;
	//localFile = argv[1];
	localFile = "alice29.txt";

	// populate global words using global file
	populateGlobalWords(globalFile);

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
		if (word.second.globalFreq < freqBound) {
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

	cout << "Word" << " [document frequency, global frequency]" << endl;
	cout << "------------------------------------" << endl << endl;
	if (words.size() < summarySize) {
		summarySize = words.size();
	}
	for (int i = 0; i < summarySize; i++) {
		cout << words[i].word << " [" << words[i].frequency << ", " << words[i].globalFreq << "]" << endl;
	}
	cout << endl;

	// sort words for summary
	


	
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

			// deliminate by space to separate words ////////////////////////////
			stringstream ss(line);
			string item;

			int count = 1;

			string word = "";
			int frequency = 0;

			while (ss >> item) {

				// if word != "" --> 
				if (item.compare("") != 0) {
					// If item is first, it is the word
					if (count == 1) {
						transform(item.begin(), item.end(), item.begin(), ::tolower);
						int countChar = 0;
						for (char c : item) {
							if (c != ' ' && c != -96) {
								if ((c >= 'a' && c <= 'z') || c == '\'') {
									if (countChar == 0 || countChar == item.size()) {
										if (c != '\'') {
											word += c;
										}
									}
									else {
										word += c;
									}
								}
							}
							countChar++;
						}
						count++;
					}
					// If item is second, it is the frequency
					else if (count == 2) {
						frequency = stoi(item);
						count == 1;

						// add word to globalWords vector
						globalDictionary[word] = frequency;

						word = "";
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
				if (item.compare("") != 0) {
					int count = 0;
					for (char c : item) {
						if (c != ' ' && c != -96) {
							if ((c >= 'a' && c <= 'z') || c == '\'') {
								if (count == 0 || count == item.size()) {
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
}

//void populateDocumentWords(string file) {
//	ifstream infile(file);
//	int countFreq;
//	// Populate globalWords vector with words and their corresponding frequencies
//	if (infile.is_open()) {
//		string line;
//		for (;;) {
//			if (!getline(infile, line)) {
//				break;
//			}
//
//			// deliminate by space to separate words
//			stringstream ss(line);
//			Word w;
//
//			ss >> w.word;
//
//			if (ss.peek() == '\t') {
//				ss.ignore();
//			}
//
//			ss >> w.frequency;
//
//			// add word to globalWords vector
//			globalWords.push_back(w);
//		}
//	}
//	else {
//		cout << "Cannot open file" << endl;
//	}
//
//	// close file
//	infile.close();
//}