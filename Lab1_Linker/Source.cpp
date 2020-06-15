#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <unordered_map> 
#include <iomanip>
#include <map>

using namespace std;

const char DELIMITER1 = ' ';
const char DELIMITER2 = '\t';

void __parseerror(int errcode, int linenum, int lineoffset) {
	const char* errstr[] = {
		"NUM_EXPECTED", // Number expect
		"SYM_EXPECTED", // Symbol Expected
		"ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
		"SYM_TOO_LONG", // Symbol Name is too long
		"TOO_MANY_DEF_IN_MODULE", // > 16
		"TOO_MANY_USE_IN_MODULE", // > 16
		"TOO_MANY_INSTR", // total num_instr exceeds memory size (512)
	};
	printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode]);
	// cout << "Parse Error line %d offset %d: %s\n" << linenum << lineoffset << errstr[errcode] << endl;
	cin.get();
}

struct error_info {
	int errorCode;
	string token;
};

struct warning_info {
	//int errorCode;
	string token;
	unsigned int module;
	int origin;
	int shouldbe;
};

struct symbol_info {
	unsigned int definedmodule;
	bool used;
	int absaddress;
};

map<int, string> RULECODEMAP = {
	{2, "Error: This variable is multiple times defined; first value used"},
	{3, "Error: %s is not defined; zero used"},
	{4, "Warning: Module %d: %s was defined but never used"},
	{5, "Warning: Module %d1: %s too big %d2 (max=%d3) assume zero relative"},
	{6, "Error: External address exceeds length of uselist; treated as immediate"},
	{7, "Warning: Module %d: %s appeared in the uselist but was not actually used"},
	{8, "Error: Absolute address exceeds machine size; zero used"},
	{9, "Error: Relative address exceeds module size; zero used"},
	{10, "Error: Illegal immediate value; treated as 9999"},
	{11, "Error: Illegal opcode; treated as 9999"}
};

string getError(int ruleCode) {
	auto it = RULECODEMAP.find(ruleCode);
	if (it != RULECODEMAP.end())
		return it->second;
	return "";
}

bool replace(string& str, const string& from, const string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

bool has_only_digits(const string& s) {
	return s.find_first_not_of("0123456789") == string::npos;
}

int readInt(const string& s, int lineCount, size_t tokenStart) {
	/* parse error 0 */
	if (!has_only_digits(s)) {
		__parseerror(0, lineCount, tokenStart + 1);
		exit(EXIT_FAILURE);
	}
	return stoi(s);
}


void pass1(const char* inputpath, unordered_map<string, struct symbol_info>& SymbolTable, vector<int>& modelBase) {
	/* open the file */
	ifstream InputFile(inputpath);

	/* cache data structure initialization */
	modelBase.push_back(0);
	unordered_map<string, int> symbolErrorMap;
	unordered_map<int, struct warning_info> warningTable;
	vector<string> SymbolTableWrap;

	/* file record intialization */
	int lineCount = 1;
	string str;
	size_t lastLineOffset;
	int defcount = -1, usecount = -1, instcount = -1;
	int curdefcount = 0, curusecount = 0, curinstcount = 0;
	int instruction;
	bool phase1Done = false, phase2Done = false, phase3Done = false, skip = false;
	unordered_map<string, int> localSymbolTable;
	string token, pretoken;
	int intread;

	/* now we start to process file */
	while (getline(InputFile, str)) {
		stringstream line(str);

		/* within the line, we read token one by one */
		/* line record intialization */
		size_t start, tokenStart, tokenEnd, tokenLength;
		start = str.find_first_not_of(DELIMITER1);  // Skip all occurences of the delimiter to find new start
		start = str.find_first_not_of(DELIMITER2, start);
		size_t end = start;
		size_t endLine = str.find_last_of("\n");

		/* now we start to process line */
		while (start != string::npos) {
			end = min(str.find(DELIMITER1, start), str.find(DELIMITER2, start)); // Find next occurence of delimiter
			if (end == endLine) {
				token = str.substr(start, str.length() - start);
				tokenStart = start;
				tokenEnd = str.length();
				tokenLength = str.length() - start;
			}
			else {
				token = str.substr(start, end - start);
				tokenStart = start;
				tokenEnd = end;
				tokenLength = end - start;
			}
			//cout << "Token: " << token << " start: " << tokenStart << ", end: " << tokenEnd << ", length:" << tokenLength << endl;
			/* post process line pointer */
			start = str.find_first_not_of(DELIMITER1, end);  // Skip all occurences of the delimiter to find new start
			start = str.find_first_not_of(DELIMITER2, start);

			/* process token */
			if (!phase1Done) {
				if (defcount == -1) {
					defcount = readInt(token, lineCount, tokenStart);
					/* parse error 4 */
					if (defcount > 16) {
						__parseerror(4, lineCount, tokenStart + 1);
						exit(EXIT_FAILURE);
					}
					if (defcount == 0) {
						phase1Done = true;
					}
					continue;
				}
				else {
					if (curdefcount % 2 == 0) {
						/* check error message 2 */
						if (SymbolTable.find(token) != SymbolTable.end()) {
							symbolErrorMap[token] = 2;
							skip = true;

						}
						/* parse error 1 */
						if (!isalpha(token[0])) {
							__parseerror(1, lineCount, tokenStart + 1);
							exit(EXIT_FAILURE);
						}
						pretoken = token;  // read symbol
						/* parse error 3 */
						if (token.size() > 16) {
							__parseerror(3, lineCount, tokenStart + 1);
							exit(EXIT_FAILURE);
						}
					}
					else {
						// read int
						if (skip) {
							skip = false;
						}
						else {
							intread = readInt(token, lineCount, tokenStart);
							SymbolTableWrap.push_back(pretoken);
							SymbolTable[pretoken] = { static_cast<unsigned int>(modelBase.size()), false, intread + modelBase[modelBase.size() - 1] };  // read int
							localSymbolTable[pretoken] = intread;
						}
					}
					curdefcount++;
					if (curdefcount % 2 == 0 && curdefcount / 2 == defcount) {
						phase1Done = true;
					}
				}
			}
			else if (!phase2Done) {
				if (usecount == -1) {
					usecount = readInt(token, lineCount, tokenStart);
					/* parse error 5 */
					if (usecount > 16) {
						__parseerror(5, lineCount, tokenStart + 1);
						exit(EXIT_FAILURE);
					}
					if (usecount == 0) {
						phase2Done = true;
					}
					continue;
				}
				else {
					/* parse error 1 */
					if (!isalpha(token[0])) {
						__parseerror(1, lineCount, tokenStart + 1);
						exit(EXIT_FAILURE);
					}
					curusecount++;
					if (curusecount == usecount) {
						phase2Done = true;
					}
				}
			}
			else {
				if (instcount == -1) {
					instcount = readInt(token, lineCount, tokenStart);
					/* parse error 6 */
					if (instcount + modelBase[modelBase.size()-1] > 512) {
						__parseerror(6, lineCount, tokenStart + 1);
						exit(EXIT_FAILURE);
					}
					/* check error message 5 */
					for (auto x : localSymbolTable) {
						if (x.second > instcount - 1) {
							warningTable[5] = {x.first, static_cast<unsigned int>(modelBase.size()), x.second, instcount - 1};
							SymbolTable.at(x.first).absaddress = modelBase[modelBase.size() - 1];
						}
					}
					modelBase.push_back(instcount + modelBase[modelBase.size() - 1]);
					continue;
				}
				else {
					if (curinstcount % 2 == 0) {
						pretoken = token;  // read char
						if (token != "I" && token != "A" && token != "E" && token != "R") {
							__parseerror(2, lineCount, tokenStart + 1);
						}
					}
					else {
						instruction = readInt(token, lineCount, tokenStart); 
					}
					curinstcount++;
					if (curinstcount % 2 == 0 && curinstcount / 2 == instcount) {
						phase3Done = true;
					}
				}

				// at the end of pase three, reset
				if (phase3Done) {
					/* cout warning message 5 */
					for (auto it = warningTable.cbegin(); it != warningTable.cend(); ++it) {
						string warningstr = getError((*it).first);
						replace(warningstr, "%d1", to_string((*it).second.module));
						replace(warningstr, "%s", (*it).second.token);
						replace(warningstr, "%d2", to_string((*it).second.origin));
						replace(warningstr, "%d3", to_string((*it).second.shouldbe));
						std::cout << warningstr << endl;
					}
					/* reset now */
					defcount = -1, usecount = -1, instcount = -1;
					phase1Done = false, phase2Done = false, phase3Done = false, skip = false;
					curdefcount = 0, curusecount = 0, curinstcount = 0;
					localSymbolTable.clear();
					warningTable.clear();
				}
			}
		} 
		/* post process line */
		lastLineOffset = str.length();
		//cout << "After Process line " << lineCount << " lastLineOffset: " << lastLineOffset << endl << endl;
		//while (line >> token) {
		//	cout << "Line: " << lineCount << " Token: " << token << endl;
		//}
		lineCount++;
	}
	/* final position record */
	// cout << "Final Position: Line " << --lineCount << " Line Offset " << lastLineOffset << endl;

	/* parse error 012 */
	if (curdefcount < defcount) {
		if (curdefcount % 2) {
			__parseerror(0, --lineCount, lastLineOffset + 1);
		}
		else {
			__parseerror(1, --lineCount, lastLineOffset + 1);
		}
		exit(EXIT_FAILURE);
	}	
	if (curusecount < usecount) {
		__parseerror(1, --lineCount, lastLineOffset + 1);
		exit(EXIT_FAILURE);
	}
	if (curinstcount < 2 * instcount) {
		if (curinstcount % 2) {
			__parseerror(0, --lineCount, lastLineOffset + 1);
		}
		else {
			__parseerror(2, --lineCount, lastLineOffset + 1);
		}
		exit(EXIT_FAILURE);
	}

	/* free memory space */
	InputFile.close();

	/* cout symbol table */
	cout << "Symbol Table";
	string sym;
	for (int i = 0; i < SymbolTableWrap.size(); i++) {
		sym = SymbolTableWrap[i];
		cout << endl << sym << "=" << SymbolTable.at(sym).absaddress;  
		if (symbolErrorMap.find(sym) != symbolErrorMap.end()) {
			cout << " " << getError(symbolErrorMap.at(sym));
		}
	}
}

void pass2(const char* inputpath, unordered_map<string, struct symbol_info>& SymbolTable, const vector<int>& modelBase, vector<int>& MemoryMap) {
	/* open the file */
	ifstream InputFile(inputpath);

	/* cache data structure initialization */
	unordered_map <int, struct error_info> memoryErrorMap;  // line: error_info
	unordered_map<int, struct warning_info> warningTable;

	/* file record intialization */
	int lineCount = 1;
	string str;
	size_t lastLineOffset;
	unsigned int curobj = 0;
	int defcount = -1, usecount = -1, instcount = -1;
	int curdefcount = 0, curusecount = 0, curinstcount = 0;
	int instruction, opcode, operand;
	bool phase1Done = false, phase2Done = false, phase3Done = false;
	unordered_map<string, bool> localUsedTable;
	vector<string> uselist;
	vector<bool> symbolused;
	string token, pretoken;

	/* now we start to process file */
	while (getline(InputFile, str)) {
		stringstream line(str);
		/* within the line, we read token one by one */
		/* line record intialization */
		size_t start, tokenStart, tokenEnd, tokenLength;
		start = str.find_first_not_of(DELIMITER1);  // Skip all occurences of the delimiter to find new start
		start = str.find_first_not_of(DELIMITER2, start);
		size_t end = start;
		size_t endLine = str.find_last_of("\n");

		/* now we start to process line */
		while (start != string::npos) {
			end = min(str.find(DELIMITER1, start), str.find(DELIMITER2, start)); // Find next occurence of delimiter
			if (end == endLine) {
				token = str.substr(start, str.length() - start);
				tokenStart = start;
				tokenEnd = str.length();
				tokenLength = str.length() - start;
			}
			else {
				token = str.substr(start, end - start);
				tokenStart = start;
				tokenEnd = end;
				tokenLength = end - start;
			}
			/* post process line pointer */
			start = str.find_first_not_of(DELIMITER1, end);  // Skip all occurences of the delimiter to find new start
			start = str.find_first_not_of(DELIMITER2, start);

			/* process token */
			if (!phase1Done) {
				if (defcount == -1) {
					defcount = stoi(token);
					if (defcount == 0) {
						phase1Done = true;
					}
					continue;
				}
				else {
					curdefcount++;
					if (curdefcount % 2 == 0 && curdefcount / 2 == defcount) {
						phase1Done = true;
					}
				}
			}
			else if (!phase2Done) {
				if (usecount == -1) {
					usecount = stoi(token);
					if (usecount == 0) {
						phase2Done = true;
					}
					continue;
				}
				else {
					uselist.push_back(token);
					symbolused.push_back(false);
					if (SymbolTable.find(token) != SymbolTable.end()) {
						SymbolTable.at(token).used = true;
					}
					curusecount++;
					if (curusecount == usecount) {
						phase2Done = true;
					}
				}
			}
			else {
				if (instcount == -1) {
					instcount = stoi(token);
					continue;
				}
				else {
					if (curinstcount % 2 == 0) {
						pretoken = token;  // read char
					}
					else {
						instruction = stoi(token);  // read int
						opcode = instruction / 1000;
						/* check error message 10 */
						if (pretoken == "I" && instruction >= 10000) {
							memoryErrorMap[MemoryMap.size()] = { 10, "" };
							MemoryMap.push_back(9999);
						}
						/* check error message 11 */
						else if (opcode >= 10) {
							memoryErrorMap[MemoryMap.size()] = { 11, "" };
							MemoryMap.push_back(9999);
						}
						else {
							operand = instruction % 1000;
							if (pretoken == "R") {
								/* check error message 9 */
								if (operand > modelBase[curobj + 1] - modelBase[curobj]) {
									memoryErrorMap[MemoryMap.size()] = { 9, "" };
									MemoryMap.push_back(opcode * 1000 + modelBase.at(curobj));
								}
								else {
									MemoryMap.push_back(instruction + modelBase.at(curobj));
								}
							}
							else if (pretoken == "E") {
								if (operand > uselist.size() - 1) {
									/* check error message 6 */
									memoryErrorMap[MemoryMap.size()] = { 6, "" };
									MemoryMap.push_back(instruction);
								}
								else if (SymbolTable.find(uselist.at(operand)) == SymbolTable.end()) {
									/* check error message 3 */
									memoryErrorMap[MemoryMap.size()] = { 3, uselist.at(operand) };
									symbolused.at(operand) = true;
									MemoryMap.push_back(opcode * 1000);
								}
								else {
									symbolused.at(operand) = true;
									MemoryMap.push_back(opcode * 1000 + SymbolTable.at(uselist.at(operand)).absaddress);
								}
							}
							else if (pretoken == "I") {
								MemoryMap.push_back(instruction);
							}
							else if (pretoken == "A") {
								/* check error message 8 */
								if (operand >= 512) {
									memoryErrorMap[MemoryMap.size()] = { 8, "" };
									MemoryMap.push_back(1000 * opcode);
								}
								else {
									MemoryMap.push_back(instruction);
								}
							}
						}
					}
					curinstcount++;
					if (curinstcount % 2 == 0 && curinstcount / 2 == instcount) {
						phase3Done = true;
					}
				}

				// at the end of pase three, reset
				if (phase3Done) {
					defcount = -1, usecount = -1, instcount = -1;
					phase1Done = false, phase2Done = false, phase3Done = false;
					curdefcount = 0, curusecount = 0, curinstcount = 0;
					curobj++;
					//for (vector<string>::iterator it = uselist.begin(); it != uselist.end(); ++it) {
					//	cout << "uselist:" << *it << endl;
					//}
					/* check error message 7 */
					for (int i = 0; i < uselist.size(); i++) {
						if (!symbolused[i]) {
							warningTable[MemoryMap.size()-1] = { uselist[i], curobj, 0, 0 };
						}
					}
					uselist.clear();
					symbolused.clear();
				}
			}
		}
		/* post process line */
		lastLineOffset = str.length();
		lineCount++;
	}

	/* free memory space */
	InputFile.close();

	/* cont memory map */
	//cout << endl << endl << "Memory Error Map" << endl;
	//for (auto x : memoryErrorMap) {
	//	cout << endl << x.first << "=" << x.second;
	//}
	cout << endl << endl << "Memory Map";
	int errorcode;
	for (int i = 0; i < MemoryMap.size(); i++) {
		cout << endl << setw(3) << setfill('0') << i << ": ";
		cout << setw(4) << setfill('0') << MemoryMap.at(i);
		if (memoryErrorMap.find(i) != memoryErrorMap.end()) {
			errorcode = memoryErrorMap.at(i).errorCode;
			if (errorcode == 3) {
				string errorstr = getError(memoryErrorMap.at(i).errorCode);
				replace(errorstr, "%s", memoryErrorMap.at(i).token);
				cout << " " << errorstr;
			}
			else {
				cout << " " << getError(memoryErrorMap.at(i).errorCode);
			}
		}

		/* warning message 7 */
		if (warningTable.find(i) != warningTable.end()) {
			string warningstr = getError(7);
			replace(warningstr, "%d", to_string(warningTable.at(i).module));
			replace(warningstr, "%s", warningTable.at(i).token);
			cout << endl << warningstr;
		}
	}
	/* warning message 4 */
	for (auto x : SymbolTable) {
		if (!x.second.used) {
			string errorstr = getError(4);
			replace(errorstr, "%d", to_string(x.second.definedmodule));
			replace(errorstr, "%s", x.first);
			cout << endl << errorstr;
		}
	}
}


int main(int argc, char *argv[]) {
	//const char* inputpath = "input-24";
	const char* inputpath = argv[1];
	unordered_map<string, struct symbol_info> SymbolTable;
	vector<int> modelBase;
	vector<int> MemoryMap;

	pass1(inputpath, SymbolTable, modelBase);

	/*
	cout << "modelBase" << endl;
	for (vector<int>::iterator it = modelBase.begin(); it != modelBase.end(); ++it) {
		cout << "modelBase:" << *it << endl;
	}

	for (auto it = SymbolTable.cbegin(); it != SymbolTable.cend(); ++it) {
		std::cout << (*it).first << ": " << (*it).second << endl;
	}
	*/

	pass2(inputpath, SymbolTable, modelBase, MemoryMap);
	cin.get();
}
