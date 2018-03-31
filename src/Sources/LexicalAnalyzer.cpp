/**************************************************************************
Author: Stefan Rapeanu-Andreescu
Creation date: 21.02.2018
**************************************************************************/

#include "../Headers/LexicalAnalyzer.hpp"

#include <iostream>
#include <algorithm>

LexicalAnalyzer::LexicalAnalyzer()
	: m_dfa(LexicalAnalyzer::NUMBER_OF_STATES, 256)
	, m_keywords({"auto", "break", "case", "char", "const", "continue",
		"default", "do", "double", "else", "enum", "extern", "float", "for",
		"goto", "static", "int", "long", "register", "return", "short", "signed",
		"sizeof", "switch", "typedef", "union", "unsigned", "void", "volatile"})
{
	RegisterLexemeParsing();
}

bool LexicalAnalyzer::Analyze(std::string text)
{
	while (!text.empty())
	{
		Lexeme lexeme;
		bool status = m_dfa.ParseLexeme(text, text, lexeme);
		if (!status)
		{
			std::cout << lexeme << '\n';
			return false;
		}
		if (m_dfa.GetCurrentState() == WHITESPACE_END)
		{
			m_dfa.ResetState();
			continue;
		}
		AddLexemeToDictionary(lexeme);

		m_dfa.ResetState();
	}
	return true;
}

bool LexicalAnalyzer::AnalyzeFile(std::string fileName)
{
	std::ifstream inputFile(fileName.c_str());
	if (!inputFile.is_open())
	{
		return false;
	}
	while (!inputFile.eof())
	{
		Lexeme lexeme;
		bool status = m_dfa.ParseLexeme(inputFile, lexeme);
		if (!status)
		{
			return false;
		}
		if (m_dfa.GetCurrentState() == WHITESPACE_END)
		{
			m_dfa.ResetState();
			continue;
		}
		AddLexemeToDictionary(lexeme);

		m_dfa.ResetState();
	}
	return true;
}

void LexicalAnalyzer::AddLexemeToDictionary(const std::string& lexeme)
{
	Lexemes::iterator lexemeIt = m_lexemeDictionary.find(lexeme);
	if (lexemeIt == m_lexemeDictionary.end())
	{
		Lexemes::iterator it = m_lexemeDictionary.insert(
			std::make_pair(
				lexeme,
				std::make_pair(
					GetLexemeTypeForState(m_dfa.GetCurrentState(), lexeme),
					m_lexemeDictionary.size()
					)
				)
			).first;
		m_lexemes.push_back(it);
	}
	else
	{
		m_lexemes.push_back(lexemeIt);
	}
}

void LexicalAnalyzer::RegisterLexemeParsing()
{
	RegisterWhitespaces();
	RegisterDelimiter();
	RegisterOperators();
	RegisterLineComment();
	RegisterBlockComment();
	RegisterIdentifier();
	RegisterStringLiteral();
	RegisterCharLiteral();
	RegisterIntegerLiteral();
	RegisterFloatingLiteral();
	RegisterParentheses();
}

void LexicalAnalyzer::RegisterWhitespaces()
{
	m_dfa.SetTransition(INITIAL_STATE, WHITESPACE_BODY, ' ');
	m_dfa.SetTransition(INITIAL_STATE, WHITESPACE_BODY, '\n');
	m_dfa.SetTransition(INITIAL_STATE, WHITESPACE_BODY, '\t');
	m_dfa.SetTransition(INITIAL_STATE, WHITESPACE_BODY, '\r');
	m_dfa.SetTransition(INITIAL_STATE, WHITESPACE_BODY, '\b');

	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_END, i);
	}

	m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_BODY, ' ');
	m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_BODY, '\n');
	m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_BODY, '\t');
	m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_BODY, '\r');
	m_dfa.SetTransition(WHITESPACE_BODY, WHITESPACE_BODY, '\b');

	m_dfa.SetAcceptingState(WHITESPACE_END);
}

void LexicalAnalyzer::RegisterDelimiter()
{
	m_dfa.SetTransition(INITIAL_STATE, DELIMITER_BODY, ';');
	m_dfa.SetTransition(INITIAL_STATE, DELIMITER_BODY, ':');
	m_dfa.SetTransition(INITIAL_STATE, DELIMITER_BODY, ',');
	m_dfa.SetTransition(INITIAL_STATE, DELIMITER_BODY, '.');
	for (int i = 0; i < 255; ++i)
	{
		m_dfa.SetTransition(DELIMITER_BODY, DELIMITER_END, i);
	}
	m_dfa.SetTransition(DELIMITER_BODY, DELIMITER_BODY, ';');

	m_dfa.SetAcceptingState(DELIMITER_END);
}

void LexicalAnalyzer::RegisterOperators()
{
	RegisterAssignmentOperator();
	RegisterArithmeticOperators();
	RegisterComparisonOperators();
	RegisterBitwiseOperators();
	RegisterLogicalOperators();
	RegisterCompoundAssignmentOperators();
}

void LexicalAnalyzer::RegisterDivisionOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, COMMENT_OR_DIVISION_OPERATOR, '/');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(COMMENT_OR_DIVISION_OPERATOR, DIVISION_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(DIVISION_OPERATOR_END);
}

void LexicalAnalyzer::RegisterDivisionAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, COMMENT_OR_DIVISION_OPERATOR, '/');
	m_dfa.SetTransition(COMMENT_OR_DIVISION_OPERATOR, DIVISION_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(DIVISION_ASSIGNMENT_OPERATOR_BODY, DIVISION_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(DIVISION_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterMultiplicationOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, MULTIPLICATION_OPERATOR_BODY, '*');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(MULTIPLICATION_OPERATOR_BODY, MULTIPLICATION_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(MULTIPLICATION_OPERATOR_END);
}

void LexicalAnalyzer::RegisterMultiplicationAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, MULTIPLICATION_OPERATOR_BODY, '*');
	m_dfa.SetTransition(MULTIPLICATION_OPERATOR_BODY, MULTIPLICATION_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(MULTIPLICATION_ASSIGNMENT_OPERATOR_BODY, MULTIPLICATION_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(MULTIPLICATION_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, ASSIGNMENT_OPERATOR_BODY, '=');
	m_dfa.SetTransition(ASSIGNMENT_OPERATOR_BODY, EQUALITY_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '=') continue;
		m_dfa.SetTransition(ASSIGNMENT_OPERATOR_BODY, ASSIGNMENT_OPERATOR_END, i);
		m_dfa.SetTransition(EQUALITY_OPERATOR_BODY, EQUALITY_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(ASSIGNMENT_OPERATOR_END);
	m_dfa.SetAcceptingState(EQUALITY_OPERATOR_END);
}

void LexicalAnalyzer::RegisterAdditionAndIncrementOperators()
{
	m_dfa.SetTransition(INITIAL_STATE, ADDITION_OPERATOR_BODY, '+');
	m_dfa.SetTransition(ADDITION_OPERATOR_BODY, INCREMENT_OPERATOR_BODY, '+');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '+') continue;
		m_dfa.SetTransition(ADDITION_OPERATOR_BODY, ADDITION_OPERATOR_END, i);
		m_dfa.SetTransition(INCREMENT_OPERATOR_BODY, INCREMENT_OPERATOR_END, i);
	}
	m_dfa.SetTransition(INCREMENT_OPERATOR_BODY, INCREMENT_OPERATOR_END, '+');
	m_dfa.SetAcceptingState(ADDITION_OPERATOR_END);
	m_dfa.SetAcceptingState(INCREMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterArithmeticOperators()
{
	RegisterAdditionAndIncrementOperators();
	RegisterSubtractionAndDecrementOperators();
	RegisterDivisionOperator();
	RegisterMultiplicationOperator();
	RegisterModuloOperator();
}

void LexicalAnalyzer::RegisterModuloOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, MODULO_OPERATOR_BODY, '%');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '=') continue;
		m_dfa.SetTransition(MODULO_OPERATOR_BODY, MODULO_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(MODULO_OPERATOR_END);
}

void LexicalAnalyzer::RegisterComparisonOperators()
{
	RegisterNotEqualToOperator();
	RegisterGreaterThanOperator();
	RegisterLessThanOperator();
	RegisterGreaterThanOrEqualToOperator();
	RegisterLessThanOrEqualToOperator();
}

void LexicalAnalyzer::RegisterLessThanOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LESS_THAN_OPERATOR_BODY, '<');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '<' || i == '=') continue;
		m_dfa.SetTransition(LESS_THAN_OPERATOR_BODY, LESS_THAN_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(LESS_THAN_OPERATOR_END);
}

void LexicalAnalyzer::RegisterGreaterThanOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, GREATER_THAN_OPERATOR_BODY, '>');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '>' || i == '=') continue;
		m_dfa.SetTransition(GREATER_THAN_OPERATOR_BODY, GREATER_THAN_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(GREATER_THAN_OPERATOR_END);
}

void LexicalAnalyzer::RegisterNotEqualToOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LOGICAL_NOT_OPERATOR_BODY, '!');
	m_dfa.SetTransition(LOGICAL_NOT_OPERATOR_BODY, NOT_EQUAL_TO_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(NOT_EQUAL_TO_OPERATOR_BODY, NOT_EQUAL_TO_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(NOT_EQUAL_TO_OPERATOR_END);
}

void LexicalAnalyzer::RegisterGreaterThanOrEqualToOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, GREATER_THAN_OPERATOR_BODY, '>');
	m_dfa.SetTransition(GREATER_THAN_OPERATOR_BODY, GREATER_THAN_OR_EQUAL_TO_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(GREATER_THAN_OR_EQUAL_TO_OPERATOR_BODY, GREATER_THAN_OR_EQUAL_TO_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(GREATER_THAN_OR_EQUAL_TO_OPERATOR_END);
}

void LexicalAnalyzer::RegisterLessThanOrEqualToOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LESS_THAN_OPERATOR_BODY, '<');
	m_dfa.SetTransition(LESS_THAN_OPERATOR_BODY, LESS_THAN_OR_EQUAL_TO_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(LESS_THAN_OR_EQUAL_TO_OPERATOR_BODY, LESS_THAN_OR_EQUAL_TO_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(LESS_THAN_OR_EQUAL_TO_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseOperators()
{
	RegisterBitwiseAndOperator();
	RegisterBitwiseOrOperator();
	RegisterBitwiseNotOperator();
	RegisterBitwiseXorOperator();
	RegisterBitwiseLeftShiftOperator();
	RegisterBitwiseRightShiftOperator();
}

void LexicalAnalyzer::RegisterBitwiseAndOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_AND_OPERATOR_BODY, '&');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '&') continue;
		m_dfa.SetTransition(BITWISE_AND_OPERATOR_BODY, BITWISE_AND_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_AND_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseOrOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_OR_OPERATOR_BODY, '|');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '|') continue;
		m_dfa.SetTransition(BITWISE_OR_OPERATOR_BODY, BITWISE_OR_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_OR_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseNotOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_NOT_OPERATOR_BODY, '~');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_NOT_OPERATOR_BODY, BITWISE_NOT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_NOT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseXorOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_XOR_OPERATOR_BODY, '^');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_XOR_OPERATOR_BODY, BITWISE_XOR_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_XOR_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseLeftShiftOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LESS_THAN_OPERATOR_BODY, '<');
	m_dfa.SetTransition(LESS_THAN_OPERATOR_BODY, BITWISE_LEFT_SHIFT_OPERATOR_BODY, '<');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '=') continue;
		m_dfa.SetTransition(BITWISE_LEFT_SHIFT_OPERATOR_BODY, BITWISE_LEFT_SHIFT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_LEFT_SHIFT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseRightShiftOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, GREATER_THAN_OPERATOR_BODY, '>');
	m_dfa.SetTransition(GREATER_THAN_OPERATOR_BODY, BITWISE_RIGHT_SHIFT_OPERATOR_BODY, '>');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_RIGHT_SHIFT_OPERATOR_BODY, BITWISE_RIGHT_SHIFT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_RIGHT_SHIFT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterLogicalOperators()
{
	RegisterLogicalAndOperator();
	RegisterLogicalOrOperator();
	RegisterLogicalNotOperator();
}

void LexicalAnalyzer::RegisterLogicalAndOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_AND_OPERATOR_BODY, '&');
	m_dfa.SetTransition(BITWISE_AND_OPERATOR_BODY, LOGICAL_AND_OPERATOR_BODY, '&');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(LOGICAL_AND_OPERATOR_BODY, LOGICAL_AND_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(LOGICAL_AND_OPERATOR_END);
}

void LexicalAnalyzer::RegisterLogicalOrOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_OR_OPERATOR_BODY, '|');
	m_dfa.SetTransition(BITWISE_OR_OPERATOR_BODY, LOGICAL_OR_OPERATOR_BODY, '|');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(LOGICAL_OR_OPERATOR_BODY, LOGICAL_OR_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(LOGICAL_OR_OPERATOR_END);
}

void LexicalAnalyzer::RegisterLogicalNotOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LOGICAL_NOT_OPERATOR_BODY, '!');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '=') continue;
		m_dfa.SetTransition(LOGICAL_NOT_OPERATOR_BODY, LOGICAL_NOT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(LOGICAL_NOT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterCompoundAssignmentOperators()
{
	RegisterAdditionAssignmentOperator();
	RegisterSubtractionAssignmentOperator();
	RegisterDivisionAssignmentOperator();
	RegisterMultiplicationAssignmentOperator();
	RegisterModuloAssignmentOperator();
	RegisterBitwiseAndAssignmentOperator();
	RegisterBitwiseOrAssignmentOperator();
	RegisterBitwiseXorAssignmentOperator();
	RegisterBitwiseLeftShiftAssignmentOperator();
	RegisterBitwiseRightShiftAssignmentOperator();
}

void LexicalAnalyzer::RegisterModuloAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, MODULO_OPERATOR_BODY, '%');
	m_dfa.SetTransition(MODULO_OPERATOR_BODY, MODULO_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(MODULO_ASSIGNMENT_OPERATOR_BODY, MODULO_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(MODULO_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseAndAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_AND_OPERATOR_BODY, '&');
	m_dfa.SetTransition(BITWISE_AND_OPERATOR_BODY, BITWISE_AND_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_AND_ASSIGNMENT_OPERATOR_BODY, BITWISE_AND_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_AND_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseOrAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_OR_OPERATOR_BODY, '|');
	m_dfa.SetTransition(BITWISE_OR_OPERATOR_BODY, BITWISE_OR_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_OR_ASSIGNMENT_OPERATOR_BODY, BITWISE_OR_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_OR_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseXorAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, BITWISE_XOR_OPERATOR_BODY, '^');
	m_dfa.SetTransition(BITWISE_XOR_OPERATOR_BODY, BITWISE_XOR_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_XOR_ASSIGNMENT_OPERATOR_BODY, BITWISE_XOR_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_XOR_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseLeftShiftAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, LESS_THAN_OPERATOR_BODY, '<');
	m_dfa.SetTransition(LESS_THAN_OPERATOR_BODY, BITWISE_LEFT_SHIFT_OPERATOR_BODY, '<');
	m_dfa.SetTransition(BITWISE_LEFT_SHIFT_OPERATOR_BODY, BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_BODY, BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterBitwiseRightShiftAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, GREATER_THAN_OPERATOR_BODY, '>');
	m_dfa.SetTransition(GREATER_THAN_OPERATOR_BODY, BITWISE_RIGHT_SHIFT_OPERATOR_BODY, '>');
	m_dfa.SetTransition(BITWISE_RIGHT_SHIFT_OPERATOR_BODY, BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_BODY, BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterAdditionAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, ADDITION_OPERATOR_BODY, '+');
	m_dfa.SetTransition(ADDITION_OPERATOR_BODY, ADDITION_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(ADDITION_ASSIGNMENT_OPERATOR_BODY, ADDITION_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(ADDITION_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterSubtractionAndDecrementOperators()
{
	m_dfa.SetTransition(INITIAL_STATE, SUBTRACTION_OPERATOR_BODY, '-');
	m_dfa.SetTransition(SUBTRACTION_OPERATOR_BODY, DECREMENT_OPERATOR_BODY, '-');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '-') continue;
		m_dfa.SetTransition(SUBTRACTION_OPERATOR_BODY, SUBTRACTION_OPERATOR_END, i);
		m_dfa.SetTransition(DECREMENT_OPERATOR_BODY, DECREMENT_OPERATOR_END, i);
	}
	m_dfa.SetAcceptingState(SUBTRACTION_OPERATOR_END);
	m_dfa.SetAcceptingState(DECREMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterSubtractionAssignmentOperator()
{
	m_dfa.SetTransition(INITIAL_STATE, SUBTRACTION_OPERATOR_BODY, '-');
	m_dfa.SetTransition(SUBTRACTION_OPERATOR_BODY, SUBTRACTION_ASSIGNMENT_OPERATOR_BODY, '=');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(SUBTRACTION_ASSIGNMENT_OPERATOR_BODY, SUBTRACTION_ASSIGNMENT_OPERATOR_END, i);
	}

	m_dfa.SetAcceptingState(SUBTRACTION_ASSIGNMENT_OPERATOR_END);
}

void LexicalAnalyzer::RegisterLineComment()
{
	m_dfa.SetTransition(INITIAL_STATE, COMMENT_OR_DIVISION_OPERATOR, '/');
	m_dfa.SetTransition(COMMENT_OR_DIVISION_OPERATOR, LINE_COMMENT_BODY, '/');
	for (int i = 1; i < 255; ++i)
	{
		m_dfa.SetTransition(LINE_COMMENT_BODY, LINE_COMMENT_BODY, i);
	}
	m_dfa.SetTransition(LINE_COMMENT_BODY, LINE_COMMENT_END, '\n');
	m_dfa.SetTransition(LINE_COMMENT_BODY, LINE_COMMENT_END, '\0');

	m_dfa.SetAcceptingState(LINE_COMMENT_END);
}

void LexicalAnalyzer::RegisterBlockComment()
{
	m_dfa.SetTransition(INITIAL_STATE, COMMENT_OR_DIVISION_OPERATOR, '/');

	m_dfa.SetTransition(COMMENT_OR_DIVISION_OPERATOR, BLOCK_COMMENT_BODY, '*');
	for (int i = 1; i < 255; ++i)
	{
		m_dfa.SetTransition(BLOCK_COMMENT_BODY, BLOCK_COMMENT_BODY, i);
	}
	m_dfa.SetTransition(BLOCK_COMMENT_BODY, BLOCK_COMMENT_POSSIBLE_END, '*');
	for (int i = 1; i < 255; ++i)
	{
		m_dfa.SetTransition(BLOCK_COMMENT_POSSIBLE_END, BLOCK_COMMENT_BODY, i);
	}
	m_dfa.SetTransition(BLOCK_COMMENT_POSSIBLE_END, BLOCK_COMMENT_POSSIBLE_END, '*');
	m_dfa.SetTransition(BLOCK_COMMENT_POSSIBLE_END, BLOCK_COMMENT_NEAR_END, '/');

	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(BLOCK_COMMENT_NEAR_END, BLOCK_COMMENT_END, i);
	}

	m_dfa.SetAcceptingState(BLOCK_COMMENT_END);
}

void LexicalAnalyzer::RegisterIdentifier()
{
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(IDENTIFIER_BODY, IDENTIFIER_END, i);
	}

	m_dfa.SetTransition(INITIAL_STATE, IDENTIFIER_BODY, '_');
	m_dfa.SetTransition(IDENTIFIER_BODY, IDENTIFIER_BODY, '_');
	for (int i = 'A'; i <= 'Z'; ++i)
	{
		m_dfa.SetTransition(INITIAL_STATE, IDENTIFIER_BODY, i);
		m_dfa.SetTransition(INITIAL_STATE, IDENTIFIER_BODY, i + 'a' - 'A');

		m_dfa.SetTransition(IDENTIFIER_BODY, IDENTIFIER_BODY, i);
		m_dfa.SetTransition(IDENTIFIER_BODY, IDENTIFIER_BODY, i + 'a' - 'A');
	}

	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(IDENTIFIER_BODY, IDENTIFIER_BODY, i);
	}

	m_dfa.SetAcceptingState(IDENTIFIER_END);
}

void LexicalAnalyzer::RegisterStringLiteral()
{
	m_dfa.SetTransition(INITIAL_STATE, STRING_LITERAL_BODY, '"');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '\n' || i == '\0') continue;
		m_dfa.SetTransition(STRING_LITERAL_BODY, STRING_LITERAL_BODY, i);
	}
	m_dfa.SetTransition(STRING_LITERAL_BODY, STRING_LITERAL_BACKSLASH, '\\');
	for (int i = 0; i < 256; ++i)
	{
		if (i == '\0') continue;
		m_dfa.SetTransition(STRING_LITERAL_BACKSLASH, STRING_LITERAL_BODY, i);
	}
	m_dfa.SetTransition(STRING_LITERAL_BACKSLASH, STRING_LITERAL_BACKSLASH, '\r');
	m_dfa.SetTransition(STRING_LITERAL_BODY, STRING_LITERAL_CLOSE, '"');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(STRING_LITERAL_CLOSE, STRING_LITERAL_END, i);
	}

	m_dfa.SetAcceptingState(STRING_LITERAL_END);
}

void LexicalAnalyzer::RegisterCharLiteral()
{
	m_dfa.SetTransition(INITIAL_STATE, CHAR_LITERAL_BEGIN, '\'');
	for (int i = 'A'; i <= 'Z'; ++i)
	{
		m_dfa.SetTransition(CHAR_LITERAL_BEGIN, CHAR_LITERAL_BODY, i);
		m_dfa.SetTransition(CHAR_LITERAL_BEGIN, CHAR_LITERAL_BODY, i + 'a' - 'A');
	}
	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(CHAR_LITERAL_BEGIN, CHAR_LITERAL_BODY, i);
	}
	m_dfa.SetTransition(CHAR_LITERAL_BEGIN, CHAR_LITERAL_BODY, ' ');

	m_dfa.SetTransition(CHAR_LITERAL_BEGIN, CHAR_LITERAL_BACKSLASH, '\\');
	for (int i = 'A'; i <= 'Z'; ++i)
	{
		m_dfa.SetTransition(CHAR_LITERAL_BACKSLASH, CHAR_LITERAL_BODY, i);
		m_dfa.SetTransition(CHAR_LITERAL_BACKSLASH, CHAR_LITERAL_BODY, i + 'a' - 'A');
	}
	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(CHAR_LITERAL_BACKSLASH, CHAR_LITERAL_BODY, i);
	}
	m_dfa.SetTransition(CHAR_LITERAL_BACKSLASH, CHAR_LITERAL_BODY, '\'');
	m_dfa.SetTransition(CHAR_LITERAL_BODY, CHAR_LITERAL_CLOSE, '\'');

	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(CHAR_LITERAL_CLOSE, CHAR_LITERAL_END, i);
	}

	m_dfa.SetAcceptingState(CHAR_LITERAL_END);
}

void LexicalAnalyzer::RegisterIntegerLiteral()
{
	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(INITIAL_STATE, NUMBER_LITERAL, i);
		m_dfa.SetTransition(NUMBER_LITERAL, NUMBER_LITERAL, i);
	}
	for (int i = 0; i < 256; ++i)
	{
		if (i < '0' || i > '9')
		{
			m_dfa.SetTransition(NUMBER_LITERAL, INTEGER_LITERAL_END, i);
		}
	}

	m_dfa.SetAcceptingState(INTEGER_LITERAL_END);
}

void LexicalAnalyzer::RegisterFloatingLiteral()
{
	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(INITIAL_STATE, NUMBER_LITERAL, i);
		m_dfa.SetTransition(NUMBER_LITERAL, NUMBER_LITERAL, i);
		m_dfa.SetTransition(FLOATING_LITERAL_FRACTIONAL_PART, FLOATING_LITERAL_FRACTIONAL_PART, i);
	}
	m_dfa.SetTransition(NUMBER_LITERAL, FLOATING_LITERAL_FRACTIONAL_PART, '.');
	for (int i = 0; i < 256; ++i)
	{
		if (i < '0' || i > '9')
		{
			m_dfa.SetTransition(FLOATING_LITERAL_FRACTIONAL_PART, FLOATING_LITERAL_END, i);
			m_dfa.SetTransition(FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY, FLOATING_LITERAL_END, i);
		}
	}
	m_dfa.SetTransition(NUMBER_LITERAL, FLOATING_LITERAL_SCIENTIFIC_NOTATION_BEGIN, 'e');
	m_dfa.SetTransition(FLOATING_LITERAL_FRACTIONAL_PART, FLOATING_LITERAL_SCIENTIFIC_NOTATION_BEGIN, 'e');
	m_dfa.SetTransition(FLOATING_LITERAL_SCIENTIFIC_NOTATION_BEGIN, FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY, '-');
	for (int i = '0'; i <= '9'; ++i)
	{
		m_dfa.SetTransition(FLOATING_LITERAL_SCIENTIFIC_NOTATION_BEGIN, FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY, i);
		m_dfa.SetTransition(FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY, FLOATING_LITERAL_SCIENTIFIC_NOTATION_BODY, i);
	}

	m_dfa.SetAcceptingState(FLOATING_LITERAL_END);
}

void LexicalAnalyzer::RegisterParentheses()
{
	m_dfa.SetTransition(INITIAL_STATE, LEFT_PARENTHESIS_BODY, '(');
	m_dfa.SetTransition(INITIAL_STATE, RIGHT_PARENTHESIS_BODY, ')');
	m_dfa.SetTransition(INITIAL_STATE, LEFT_BRACKET_BODY, '[');
	m_dfa.SetTransition(INITIAL_STATE, RIGHT_BRACKET_BODY, ']');
	m_dfa.SetTransition(INITIAL_STATE, LEFT_ACCOLADE_BODY, '{');
	m_dfa.SetTransition(INITIAL_STATE, RIGHT_ACCOLADE_BODY, '}');
	for (int i = 0; i < 256; ++i)
	{
		m_dfa.SetTransition(LEFT_PARENTHESIS_BODY, LEFT_PARENTHESIS_END, i);
		m_dfa.SetTransition(RIGHT_PARENTHESIS_BODY, RIGHT_PARENTHESIS_END, i);
		m_dfa.SetTransition(LEFT_BRACKET_BODY, LEFT_BRACKET_END, i);
		m_dfa.SetTransition(RIGHT_BRACKET_BODY, RIGHT_BRACKET_END, i);
		m_dfa.SetTransition(LEFT_ACCOLADE_BODY, LEFT_ACCOLADE_END, i);
		m_dfa.SetTransition(RIGHT_ACCOLADE_BODY, RIGHT_ACCOLADE_END, i);
	}

	m_dfa.SetAcceptingState(LEFT_PARENTHESIS_END);
	m_dfa.SetAcceptingState(RIGHT_PARENTHESIS_END);
	m_dfa.SetAcceptingState(LEFT_BRACKET_END);
	m_dfa.SetAcceptingState(RIGHT_BRACKET_END);
	m_dfa.SetAcceptingState(LEFT_ACCOLADE_END);
	m_dfa.SetAcceptingState(RIGHT_ACCOLADE_END);
}

LexicalAnalyzer::Lexemes LexicalAnalyzer::GetLexemes()
{
	return m_lexemeDictionary;
}

void LexicalAnalyzer::DisplayLexemes()
{
	for (
		std::vector<Lexemes::iterator>::iterator it = m_lexemes.begin();
		it != m_lexemes.end();
		++it
		)
	{
		std::cout << StringForLexemeType((*it)->second.first) << ": " << (*it)->first <<
				", " << (*it)->second.second << '\n';
	}
}

void LexicalAnalyzer::DisplayLexemeDictionary()
{
	for (Lexemes::iterator it = m_lexemeDictionary.begin(); it != m_lexemeDictionary.end(); ++it)
	{
		std::cout << StringForLexemeType(it->second.first) << ": " << it->first <<
				", " << it->second.second << '\n';
	}
}

LexicalAnalyzer::LexemeType LexicalAnalyzer::GetLexemeTypeForState(int state, const std::string& lexeme)
{
	if (m_dfa.IsValidState(state))
	{
		if (state == LINE_COMMENT_END) return LINE_COMMENT;
		if (state == BLOCK_COMMENT_END) return BLOCK_COMMENT;
		if (state == IDENTIFIER_END) return GetIdentifierType(lexeme);
		if (state == DELIMITER_END) return DELIMITER;
		if (state == STRING_LITERAL_END) return STRING_LITERAL;
		if (state == CHAR_LITERAL_END) return CHAR_LITERAL;
		if (state == INTEGER_LITERAL_END) return INTEGER_LITERAL;
		if (state == FLOATING_LITERAL_END) return FLOATING_LITERAL;
		if (state == LEFT_PARENTHESIS_END) return LEFT_PARENTHESIS;
		if (state == RIGHT_PARENTHESIS_END) return RIGHT_PARENTHESIS;
		if (state == LEFT_BRACKET_END) return LEFT_BRACKET;
		if (state == RIGHT_BRACKET_END) return RIGHT_BRACKET;
		if (state == LEFT_ACCOLADE_END) return LEFT_ACCOLADE;
		if (state == RIGHT_ACCOLADE_END) return RIGHT_ACCOLADE;
		if (state == ADDITION_OPERATOR_END) return ADDITION_OPERATOR;
		if (state == INCREMENT_OPERATOR_END) return INCREMENT_OPERATOR;
		if (state == SUBTRACTION_OPERATOR_END) return SUBTRACTION_OPERATOR;
		if (state == DECREMENT_OPERATOR_END) return DECREMENT_OPERATOR;
		if (state == ASSIGNMENT_OPERATOR_END) return ASSIGNMENT_OPERATOR;
		if (state == EQUALITY_OPERATOR_END) return EQUALITY_OPERATOR;
		if (state == DIVISION_OPERATOR_END) return DIVISION_OPERATOR;
		if (state == MODULO_OPERATOR_END) return MODULO_OPERATOR;
		if (state == DIVISION_ASSIGNMENT_OPERATOR_END) return DIVISION_ASSIGNMENT_OPERATOR;
		if (state == MULTIPLICATION_OPERATOR_END) return MULTIPLICATION_OPERATOR;
		if (state == MULTIPLICATION_ASSIGNMENT_OPERATOR_END) return MULTIPLICATION_ASSIGNMENT_OPERATOR;
		if (state == ADDITION_ASSIGNMENT_OPERATOR_END) return ADDITION_ASSIGNMENT_OPERATOR;
		if (state == SUBTRACTION_ASSIGNMENT_OPERATOR_END) return SUBTRACTION_ASSIGNMENT_OPERATOR;
		if (state == MODULO_ASSIGNMENT_OPERATOR_END) return MODULO_ASSIGNMENT_OPERATOR;
		if (state == BITWISE_AND_ASSIGNMENT_OPERATOR_END) return BITWISE_AND_ASSIGNMENT_OPERATOR;
		if (state == BITWISE_OR_ASSIGNMENT_OPERATOR_END) return BITWISE_OR_ASSIGNMENT_OPERATOR;
		if (state == BITWISE_XOR_ASSIGNMENT_OPERATOR_END) return BITWISE_XOR_ASSIGNMENT_OPERATOR;
		if (state == BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR_END) return BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR;
		if (state == BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR_END) return BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR;
		if (state == NOT_EQUAL_TO_OPERATOR_END) return NOT_EQUAL_TO_OPERATOR;
		if (state == LESS_THAN_OPERATOR_END) return LESS_THAN_OPERATOR;
		if (state == GREATER_THAN_OPERATOR_END) return GREATER_THAN_OPERATOR;
		if (state == GREATER_THAN_OR_EQUAL_TO_OPERATOR_END) return GREATER_THAN_OR_EQUAL_TO_OPERATOR;
		if (state == LESS_THAN_OR_EQUAL_TO_OPERATOR_END) return LESS_THAN_OR_EQUAL_TO_OPERATOR;
		if (state == LOGICAL_AND_OPERATOR_END) return LOGICAL_AND_OPERATOR;
		if (state == LOGICAL_OR_OPERATOR_END) return LOGICAL_OR_OPERATOR;
		if (state == LOGICAL_NOT_OPERATOR_END) return LOGICAL_NOT_OPERATOR;
		if (state == BITWISE_AND_OPERATOR_END) return BITWISE_AND_OPERATOR;
		if (state == BITWISE_OR_OPERATOR_END) return BITWISE_OR_OPERATOR;
		if (state == BITWISE_NOT_OPERATOR_END) return BITWISE_NOT_OPERATOR;
		if (state == BITWISE_XOR_OPERATOR_END) return BITWISE_XOR_OPERATOR;
		if (state == BITWISE_LEFT_SHIFT_OPERATOR_END) return BITWISE_LEFT_SHIFT_OPERATOR;
		if (state == BITWISE_RIGHT_SHIFT_OPERATOR_END) return BITWISE_RIGHT_SHIFT_OPERATOR;
	}
}

LexicalAnalyzer::LexemeType LexicalAnalyzer::GetIdentifierType(const std::string& lexeme)
{
	if (std::find(m_keywords.begin(), m_keywords.end(), lexeme) != m_keywords.end())
	{
		return KEYWORD;
	}
	return IDENTIFIER;
}

std::string LexicalAnalyzer::StringForLexemeType(int lexemeType)
{
	if (lexemeType == LINE_COMMENT) return "Line comment";
	if (lexemeType == BLOCK_COMMENT) return "Block comment";
	if (lexemeType == IDENTIFIER) return "Identifier";
	if (lexemeType == KEYWORD) return "Keyword";
	if (lexemeType == DELIMITER) return "Delimiter";
	if (lexemeType == STRING_LITERAL) return "String literal";
	if (lexemeType == CHAR_LITERAL) return "Character literal";
	if (lexemeType == INTEGER_LITERAL) return "Integer literal";
	if (lexemeType == FLOATING_LITERAL) return "Floating literal";
	if (lexemeType == LEFT_PARENTHESIS) return "Left parenthesis";
	if (lexemeType == RIGHT_PARENTHESIS) return "Right parenthesis";
	if (lexemeType == LEFT_BRACKET) return "Left bracket";
	if (lexemeType == RIGHT_BRACKET) return "Right bracket";
	if (lexemeType == LEFT_ACCOLADE) return "Left accolade";
	if (lexemeType == RIGHT_ACCOLADE) return "Right accolade";
	if (lexemeType == ADDITION_OPERATOR) return "Addition operator";
	if (lexemeType == INCREMENT_OPERATOR) return "Increment operator";
	if (lexemeType == SUBTRACTION_OPERATOR) return "Subtraction operator";
	if (lexemeType == DECREMENT_OPERATOR) return "Decrement operator";
	if (lexemeType == ASSIGNMENT_OPERATOR) return "Assignment operator";
	if (lexemeType == EQUALITY_OPERATOR) return "Equality operator";
	if (lexemeType == DIVISION_OPERATOR) return "Division operator";
	if (lexemeType == MODULO_OPERATOR) return "Modulo operator";
	if (lexemeType == DIVISION_ASSIGNMENT_OPERATOR) return "Division assignment operator";
	if (lexemeType == MULTIPLICATION_OPERATOR) return "Multiplication operator";
	if (lexemeType == MULTIPLICATION_ASSIGNMENT_OPERATOR) return "Multiplication assignment operator";
	if (lexemeType == ADDITION_ASSIGNMENT_OPERATOR) return "Addition assignment operator";
	if (lexemeType == SUBTRACTION_ASSIGNMENT_OPERATOR) return "Subtraction assignment operator";
	if (lexemeType == MODULO_ASSIGNMENT_OPERATOR) return "Modulo assignment operator";
	if (lexemeType == BITWISE_AND_ASSIGNMENT_OPERATOR) return "Bitwise and assignment operator";
	if (lexemeType == BITWISE_OR_ASSIGNMENT_OPERATOR) return "Bitwise or assignment operator";
	if (lexemeType == BITWISE_XOR_ASSIGNMENT_OPERATOR) return "Bitwise xor assignment operator";
	if (lexemeType == BITWISE_LEFT_SHIFT_ASSIGNMENT_OPERATOR) return "Bitwise left shift assignment operator";
	if (lexemeType == BITWISE_RIGHT_SHIFT_ASSIGNMENT_OPERATOR) return "Bitwise right shift assignment operator";
	if (lexemeType == NOT_EQUAL_TO_OPERATOR) return "Not equal to operator";
	if (lexemeType == GREATER_THAN_OPERATOR) return "Greater than operator";
	if (lexemeType == LESS_THAN_OPERATOR) return "Less than operator";
	if (lexemeType == GREATER_THAN_OR_EQUAL_TO_OPERATOR) return "Greater than or equal to operator";
	if (lexemeType == LESS_THAN_OR_EQUAL_TO_OPERATOR) return "Less than or equal to operator";
	if (lexemeType == LOGICAL_NOT_OPERATOR) return "Logical not operator";
	if (lexemeType == LOGICAL_AND_OPERATOR) return "Logical and operator";
	if (lexemeType == LOGICAL_OR_OPERATOR) return "Logical or operator";
	if (lexemeType == BITWISE_NOT_OPERATOR) return "Bitwise not operator";
	if (lexemeType == BITWISE_AND_OPERATOR) return "Bitwise and operator";
	if (lexemeType == BITWISE_OR_OPERATOR) return "Bitwise or operator";
	if (lexemeType == BITWISE_XOR_OPERATOR) return "Bitwise xor operator";
	if (lexemeType == BITWISE_LEFT_SHIFT_OPERATOR) return "Bitwise left shift operator";
	if (lexemeType == BITWISE_RIGHT_SHIFT_OPERATOR) return "Bitwise right shift operator";
}
