/**************************************************************************
Author: Stefan Rapeanu-Andreescu
Creation date: 21.02.2018
**************************************************************************/

#include "Headers/LexicalAnalyzer.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv)
{
	LexicalAnalyzer lex;
	std::string fileName;

	std::cout << "Enter the name of the file to be parsed: ";
	std::getline(std::cin, fileName);

	bool status = lex.AnalyzeFile(fileName);
	if (!status)
	{
		std::cout << "Lexical analysis failed!\n";
		return 0;
	}
	lex.DisplayLexemes();

	return 0;
}
