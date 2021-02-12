/*
 * MIT License
 *
 * Copyright (c) 2021 Michael Georgoulopoulos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// This may save a question or two, but will keep on popping the same candidate.
// It's kind of boring and repetitive.
//#define OPTIMIZE

// Each element to be sorted gets associated with this struct, holding the
// elements that are known to be greater and less than it.
struct Knowns {
	set<string> knownLess;
	set<string> knownGreater;

	// Returns number of knowns
	int count() { return (int)(knownLess.size() + knownGreater.size()); }

	bool isKnown(const string &other) {
		return knownLess.find(other) != knownLess.end() ||
			knownGreater.find(other) != knownGreater.end();
	}
};
static map<string, Knowns> knowns;

// This function updates our data with the added knowledge that "big" > "small"
static void updateKnowns(const string &big, const string &small) {

	Knowns &knownsOfBig = knowns[big];
	Knowns &knownsOfSmall = knowns[small];

	// If we already knew that, stop here.
	if (knownsOfBig.knownLess.find(small) != knownsOfBig.knownLess.end()) {
		return;
	}

	// Insert the obvious: big > small and small < big
	knownsOfBig.knownLess.insert(small);
	knownsOfSmall.knownGreater.insert(big);

	// Recurse to indirect relationships: smaller than small is smaller and
	// bigger than big is bigger.
	for (const string &smallerThanSmall : knownsOfSmall.knownLess) {
		updateKnowns(big, smallerThanSmall);
	}
	for (const string &biggerThanBig : knownsOfBig.knownGreater) {
		updateKnowns(biggerThanBig, small);
	}
}

// Keep track of how many questions the user had to answer.
static int questions = 0;

// Ask the user if a > b.
static bool askUser(const string &a, const string &b) {
	questions++;
	while (true) {
		cout << "(" << questions << ") Which is better?" << endl;
		cout << "\t1: " << a << endl;
		cout << "\t2: " << b << endl;
		cout << "Answer: ";
		string answer;
		cin >> answer;
		if (answer == "1") {
			updateKnowns(a, b);
			return true;
		}
		else if (answer == "2") {
			updateKnowns(b, a);
			return false;
		}
		cout << "Error: Please enter 1 or 2" << endl;
	}
	return false;
}

// Calls askUser and appends the result to "cache.txt"
const char GE_SEPARATOR = '>';
const char *CACHE_FILENAME = "cache.txt";
static bool askUserAndCache(const string &a, const string &b) {
	const bool result = askUser(a, b);

	const string &greater = result ? a : b;
	const string &smaller = result ? b : a;

	if (greater.find(GE_SEPARATOR) != string::npos) {
		// Ignore caching. Next time we'll need to ask again.
		return result;
	}

	ofstream outputFile;
	outputFile.open(CACHE_FILENAME, ios::app);
	if (!outputFile.is_open()) {
		// Again, silently fail
		return result;
	}

	// Append
	outputFile << greater << GE_SEPARATOR << smaller << endl;
	outputFile.close();

	return result;
}

// This is the function that compares two elements. If we don't know the element
// order, we ask the user.
static bool opinionCompare(const string &a, const string &b) {
	if (knowns[a].knownLess.find(b) != knowns[a].knownLess.end()) {
		return true;
	}

	if (knowns[a].knownGreater.find(b) != knowns[a].knownGreater.end()) {
		return false;
	}

	// We don't know yet - ask the user.
	return askUserAndCache(a, b);
}

// Ask the optimal questions before we start sorting
static void preprocess(vector<string> list) {
	if (list.size() <= 1) {
		// Nothing to ask here
		return;
	}
#ifdef OPTIMIZE
	// Ask n-1 questions, always choosing the two elements we know the least
	// about
	for (int i = 0; i < (int)list.size() - 1; i++) {
#else
	// This will ask questions indefinitely until we know everything.
	while (true) {
#endif // OPTIMIZE
		auto lessKnowns = [](const string &a, const string &b) {
			return knowns[a].count() < knowns[b].count();
		};
		sort(list.begin(), list.end(), lessKnowns);

		if (knowns[list.front()].count() >= (int)(list.size() - 1)) {
			// The element for which we know the least knows about everything.
			// Our work here is done.
			break;
		}

		// Do not ask the same question twice: first two elements (of which we
		// know the least) may have been already asked. So loop until we find
		// something we don't already know.
		for (int j = 1; j < (int)list.size(); j++) {
			const string &a = list[0];
			const string &b = list[j];

			if (knowns[a].isKnown(b)) {
				continue;
			}

			askUserAndCache(a, b);
			break;
		}
	} // end for (n-1 questions)
}

// Loads lines from file ignoring empty lines and duplicates
static bool loadLines(const string &filename, vector<string> &lines) {
	// Open input file
	ifstream file(filename);
	if (file.fail()) {
		return false;
	}

	// Load all lines.
	set<string> alreadyLoaded;
	string currentLine;
	while (getline(file, currentLine)) {
		// Skup empty lines
		if (currentLine.empty())
			continue;
		if (alreadyLoaded.find(currentLine) != alreadyLoaded.end())
			continue;
		alreadyLoaded.insert(currentLine);
		lines.push_back(currentLine);
	}

	file.close();

	return true;
}

// Loads the cache.txt file if present and prepares the cache.
static void loadAndApplyCache(const vector<string> &elements) {
	// Convert it to a set for easy searches
	set<string> elementSet;
	for (const string &element : elements) {
		elementSet.insert(element);
	}

	vector<string> responses;
	if (!loadLines(CACHE_FILENAME, responses)) {
		return;
	}

	for (const string &r : responses) {
		const string greater = r.substr(0, r.find(GE_SEPARATOR));
		const string smaller = r.substr(greater.size() + 1);

		if (elementSet.find(greater) == elementSet.end() ||
			elementSet.find(smaller) == elementSet.end()) {
			// Ignore this response - it is irrelevant
			continue;
		}

		cout << "Loading cached: " << greater << " > " << smaller << endl;
		updateKnowns(greater, smaller);
		questions++;
	}
}

int main(int argc, char *argv[]) {

	const string inputFilename = argc == 2 ? argv[1] : "TestList.txt";

	// Load all lines as elements. Skip duplicated elements.
	vector<string> elements;
	if (!loadLines(inputFilename, elements)) {
		cout << "Failed to open input file " << inputFilename << endl;
		return 1;
	}

	loadAndApplyCache(elements);
	if (questions > 0) {
		cout << questions << " questions automatically retrieved from cache"
			 << endl
			 << "Delete cache.txt in order to start over" << endl;
	}

	// Ask a balanced set of questions first
	preprocess(elements);

	cout << "Now we start sort -----------------\n";

	// Sort using our interactive comparison function
	sort(elements.begin(), elements.end(), opinionCompare);

	cout << "\nResults:\n\n";
	for (const string &s : elements) {
		cout << "\t * " << s << endl;
	}

	cout << "\nTotal questions: " << questions << endl;

	// Write to file for future reference
	const string outputFilename = inputFilename + ".sorted.txt";
	ofstream outputFile(outputFilename);
	if (outputFile.is_open()) {
		for (const string &s : elements) {
			outputFile << s << endl;
		}
	}
	outputFile.close();


#ifdef WIN32
	// Show the file to user
	system((string("start ") + outputFilename).c_str());

	// Wait for keypress
	system("pause");
#endif

	return 0;
}