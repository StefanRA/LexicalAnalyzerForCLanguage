/**************************************************************************
Author: Stefan Rapeanu-Andreescu
Creation date: 21.02.2018
**************************************************************************/

#ifndef LEXICALANALYZER_HPP_
#define LEXICALANALYZER_HPP_

#include "DFA.hpp"

#include <string>
#include <map>
#include <vector>
#include <fstream>

class LexicalAnalyzer
{
public:
	enum State {
		INITIAL_STATE,
		END_OF_INPUT,
		WHITESPACE_BODY,
		WHITESPACE_END,
		DELIMITER_BODY,
		DELIMITER_END,
		ASSIGNMENT_OPERATOR_BODY,
		ASSIGNMENT_OPERATOR_END,
		EQUALITY_OPERATOR_BODY,
		EQUALITY_OPERATOR_END,
		COMMENT_OR_DIVISION_OPERATOR,
		DIVISION_OPERATOR_END,
		LINE_COMMENT_BODY,
		LINE_COMMENT_END,
		BLOCK_COMMENT_BODY,
		BLOCK_COMMENT_POSSIBLE_END,
		BLOCK_COMMENT_NEAR_END,
		BLOCK_COMMENT_END,
		IDENTIFIER_BODY,
		IDENTIFIER_END,
		STRING_LITERAL_BODY,
		STRING_LITERAL_BACKSLASH,
		STRING_LITERAL_CLOSE,
		STRING_LITERAL_END,
		CHAR_LITERAL_BEGIN,
		CHAR_LITERAL_BODY,
		CHAR_LITERAL_BACKSLASH,
		CHAR_LITERAL_CLOSE,
		CHAR_LITERAL_END,
		NUMBER_LITERAL,
		INTEGER_LITERAL_END,
		FLOATING_LITERAL_FRACTIONAL_PART,
		FLOATING_LITERAL_SCIENTIFIC_NOTATION_BEGIN,
		FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY,
		FLOATING_LITERAL_END,
		LEFT_PARENTHESIS_BODY,
		LEFT_PARENTHESIS_END,
		RIGHT_PARENTHESIS_BODY,
		RIGHT_PARENTHESIS_END,
		LEFT_BRACKET_BODY,
		LEFT_BRACKET_END,
		RIGHT_BRACKET_BODY,
		RIGHT_BRACKET_END,
		LEFT_ACCOLADE_BODY,
		LEFT_ACCOLADE_END,
		RIGHT_ACCOLADE_BODY,
		RIGHT_ACCOLADE_END,
		ADDITION_OPERATOR_BODY,
		ADDITION_OPERATOR_END,
		INCREMENT_OPERATOR_BODY,
		INCREMENT_OPERATOR_END,
		SUBTRACTION_OPERATOR_BODY,
		SUBTRACTION_OPERATOR_END,
		DECREMENT_OPERATOR_BODY,
		DECREMENT_OPERATOR_END,
		MULTIPLICATION_OPERATOR_BODY,
		MULTIPLICATION_OPERATOR_END,
		DIVISION_ASSIGNMENT_OPERATOR_BODY,
		DIVISION_ASSIGNMENT_OPERATOR_END,
		MULTIPLICATION_ASSIGNMENT_OPERATOR_BODY,
		MULTIPLICATION_ASSIGNMENT_OPERATOR_END,
		ADDITION_ASSIGNMENT_OPERATOR_BODY,
		ADDITION_ASSIGNMENT_OPERATOR_END,
		SUBTRACTION_ASSIGNMENT_OPERATOR_BODY,
		SUBTRACTION_ASSIGNMENT_OPERATOR_END,
		MODULO_ASSIGNMENT_OPERATOR_BODY,
		MODULO_ASSIGNMENT_OPERATOR_END,
		BITWISE_AND_ASSIGNMENT_OPERATOR_BODY,
		BITWISE_AND_ASSIGNMENT_OPERATOR_END,
		BITWISE_OR_ASSIGNMENT_OPERATOR_BODY,
		BITWISE_OR_ASSIGNMENT_OPERATOR_END,
		BITWISE_XOR_ASSIGNMENT_OPERATOR_BODY,
		BITWISE_XOR_ASSIGNMENT_OPERATOR_END,
		BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_BODY,
		BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_END,
		BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_BODY,
		BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_END,
		MODULO_OPERATOR_BODY,
		MODULO_OPERATOR_END,
		NOT_EQUAL_TO_OPERATOR_BODY,
		NOT_EQUAL_TO_OPERATOR_END,
		LESS_THAN_OPERATOR_BODY,
		LESS_THAN_OPERATOR_END,
		GREATER_THAN_OPERATOR_BODY,
		GREATER_THAN_OPERATOR_END,
		GREATER_THAN_OR_EQUAL_TO_OPERATOR_BODY,
		GREATER_THAN_OR_EQUAL_TO_OPERATOR_END,
		LESS_THAN_OR_EQUAL_TO_OPERATOR_BODY,
		LESS_THAN_OR_EQUAL_TO_OPERATOR_END,
		BITWISE_AND_OPERATOR_BODY,
		BITWISE_AND_OPERATOR_END,
		LOGICAL_AND_OPERATOR_BODY,
		LOGICAL_AND_OPERATOR_END,
		BITWISE_OR_OPERATOR_BODY,
		BITWISE_OR_OPERATOR_END,
		LOGICAL_OR_OPERATOR_BODY,
		LOGICAL_OR_OPERATOR_END,
		LOGICAL_NOT_OPERATOR_BODY,
		LOGICAL_NOT_OPERATOR_END,
		BITWISE_NOT_OPERATOR_BODY,
		BITWISE_NOT_OPERATOR_END,
		BITWISE_XOR_OPERATOR_BODY,
		BITWISE_XOR_OPERATOR_END,
		BITWISE_LEFT_SHIFT_OPERATOR_BODY,
		BITWISE_LEFT_SHIFT_OPERATOR_END,
		BITWISE_RIGHT_SHIFT_OPERATOR_BODY,
		BITWISE_RIGHT_SHIFT_OPERATOR_END,
		NUMBER_OF_STATES
	};

	typedef std::string Lexeme;

	typedef int LexemeId;

	enum LexemeType {
		IDENTIFIER,
		KEYWORD,
		LINE_COMMENT,
		BLOCK_COMMENT,
		DELIMITER,
		STRING_LITERAL,
		CHAR_LITERAL,
		INTEGER_LITERAL,
		FLOATING_LITERAL,
		LEFT_PARENTHESIS,
		RIGHT_PARENTHESIS,
		LEFT_BRACKET,
		RIGHT_BRACKET,
		LEFT_ACCOLADE,
		RIGHT_ACCOLADE,
		ASSIGNMENT_OPERATOR,
		EQUALITY_OPERATOR,
		ADDITION_OPERATOR,
		INCREMENT_OPERATOR,
		SUBTRACTION_OPERATOR,
		DECREMENT_OPERATOR,
		DIVISION_OPERATOR,
		DIVISION_ASSIGNMENT_OPERATOR,
		MULTIPLICATION_OPERATOR,
		MULTIPLICATION_ASSIGNMENT_OPERATOR,
		ADDITION_ASSIGNMENT_OPERATOR,
		SUBTRACTION_ASSIGNMENT_OPERATOR,
		MODULO_ASSIGNMENT_OPERATOR,
		BITWISE_AND_ASSIGNMENT_OPERATOR,
		BITWISE_OR_ASSIGNMENT_OPERATOR,
		BITWISE_XOR_ASSIGNMENT_OPERATOR,
		BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR,
		BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR,
		MODULO_OPERATOR,
		NOT_EQUAL_TO_OPERATOR,
		LESS_THAN_OPERATOR,
		GREATER_THAN_OPERATOR,
		GREATER_THAN_OR_EQUAL_TO_OPERATOR,
		LESS_THAN_OR_EQUAL_TO_OPERATOR,
		LOGICAL_AND_OPERATOR,
		LOGICAL_OR_OPERATOR,
		LOGICAL_NOT_OPERATOR,
		BITWISE_AND_OPERATOR,
		BITWISE_OR_OPERATOR,
		BITWISE_NOT_OPERATOR,
		BITWISE_XOR_OPERATOR,
		BITWISE_LEFT_SHIFT_OPERATOR,
		BITWISE_RIGHT_SHIFT_OPERATOR
	};

	typedef std::map<Lexeme, std::pair<LexemeType, LexemeId> > Lexemes;

public:
	LexicalAnalyzer();

	bool Analyze(std::string text);
	bool AnalyzeFile(std::string fileName);
	void DisplayLexemes();
	void DisplayLexemeDictionary();
	Lexemes GetLexemes();

private:
	LexemeType GetLexemeTypeForState(int state, const std::string& lexeme);
	LexemeType GetIdentifierType(const std::string& lexeme);
	std::string StringForLexemeType(int lexemeType);

	void AddLexemeToDictionary(const std::string& lexeme);

	void RegisterLexemeParsing();

	void RegisterWhitespaces();
	void RegisterDelimiter();
	void RegisterOperators();
	void RegisterDivisionOperator();
	void RegisterMultiplicationOperator();
	void RegisterAssignmentOperator();
	void RegisterAdditionAndIncrementOperators();
	void RegisterSubtractionAndDecrementOperators();
	void RegisterLineComment();
	void RegisterBlockComment();
	void RegisterIdentifier();
	void RegisterStringLiteral();
	void RegisterCharLiteral();
	void RegisterIntegerLiteral();
	void RegisterFloatingLiteral();
	void RegisterParentheses();

	void RegisterArithmeticOperators();
	void RegisterModuloOperator();

	void RegisterComparisonOperators();
	void RegisterLessThanOperator();
	void RegisterGreaterThanOperator();
	void RegisterNotEqualToOperator();
	void RegisterGreaterThanOrEqualToOperator();
	void RegisterLessThanOrEqualToOperator();

	void RegisterBitwiseOperators();
	void RegisterBitwiseAndOperator();
	void RegisterBitwiseOrOperator();
	void RegisterBitwiseNotOperator();
	void RegisterBitwiseXorOperator();
	void RegisterBitwiseLeftShiftOperator();
	void RegisterBitwiseRightShiftOperator();

	void RegisterLogicalOperators();
	void RegisterLogicalAndOperator();
	void RegisterLogicalOrOperator();
	void RegisterLogicalNotOperator();

	void RegisterCompoundAssignmentOperators();
	void RegisterAdditionAssignmentOperator();
	void RegisterSubtractionAssignmentOperator();
	void RegisterMultiplicationAssignmentOperator();
	void RegisterDivisionAssignmentOperator();
	void RegisterModuloAssignmentOperator();
	void RegisterBitwiseAndAssignmentOperator();
	void RegisterBitwiseOrAssignmentOperator();
	void RegisterBitwiseXorAssignmentOperator();
	void RegisterBitwiseLeftShiftAssignmentOperator();
	void RegisterBitwiseRightShiftAssignmentOperator();

private:
	DFA m_dfa;
	Lexemes m_lexemeDictionary;
	std::vector<Lexemes::iterator> m_lexemes;
	std::vector<std::string> m_keywords;
};

#endif /* LEXICALANALYZER_HPP_ */
