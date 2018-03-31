/**************************************************************************
Author: Stefan Rapeanu-Andreescu
Creation date: 21.02.2018
**************************************************************************/

#include <cstddef>
#include <iostream>

#include "../Headers/DFA.hpp"

DFA::DFA()
	: m_numberOfStates(0)
	, m_numberOfTransitionSymbols(0)
	, m_transitionTable(NULL)
	, m_acceptingStates(NULL)
	, m_currentState(0)
{
}

DFA::DFA(int numberOfStates, int alphabetLength)
	: m_transitionTable(NULL)
	, m_acceptingStates(NULL)
{
	Initialize(numberOfStates, alphabetLength);
}

DFA::~DFA()
{
	Reset();
}

void DFA::Initialize(int numberOfStates, int alphabetLength)
{
	Reset();

	m_numberOfStates = numberOfStates;
	m_numberOfTransitionSymbols = alphabetLength;

	m_transitionTable = new int*[m_numberOfStates];
	for (int i = 0; i < m_numberOfStates; ++i)
	{
		m_transitionTable[i] = new int[m_numberOfTransitionSymbols];
		for (int j = 0; j < m_numberOfTransitionSymbols; ++j)
		{
			m_transitionTable[i][j] = -1;
		}
	}
	m_acceptingStates = new bool[m_numberOfStates];
}

void DFA::Reset()
{
	if (m_transitionTable != NULL)
	{
		for (int i = 0; i < m_numberOfStates; ++i)
		{
			delete[] m_transitionTable[i];
		}
		delete[] m_transitionTable;
		m_transitionTable = NULL;
	}
	if (m_acceptingStates != NULL)
	{
		delete[] m_acceptingStates;
	}
	m_numberOfStates = 0;
	m_numberOfTransitionSymbols = 0;
	m_currentState = 0;
}

bool DFA::IsValidState(int state)
{
	return state >= 0 && state < m_numberOfStates;
}

void DFA::ResetState()
{
	m_currentState = 0;
}

int DFA::GetNumberOfStates() const
{
	return m_numberOfStates;
}

void DFA::Transition(int transitionSymbol)
{
	m_currentState = m_transitionTable[m_currentState][transitionSymbol];
}

bool DFA::ParseLexeme(
	std::string initialText,
	std::string& remainingText,
	std::string& lexeme
	)
{
	for (unsigned int i = 0; i <= initialText.length(); ++i)
	{
		Transition(initialText[i]);
		if (m_currentState == -1)
		{
			return false;
		}
		if (IsAccepting())
		{
			lexeme = initialText.substr(0, i);
			remainingText = initialText.substr(i, initialText.length() - 1);
			return true;
		}
	}
	return true;
}

bool DFA::ParseLexeme(
	std::ifstream& inputFile,
	std::string& lexeme
	)
{
	char currentCharacter;
	while (inputFile >> std::noskipws >> currentCharacter)
	{
		Transition(currentCharacter);
		if (m_currentState == -1)
		{
			//std::cout << lexeme << '\n';
			return false;
		}
		if (IsAccepting())
		{
			inputFile.unget();
			//std::cout << lexeme << '\n';
			return true;
		}
		lexeme.push_back(currentCharacter);
	}
	Transition('\0');
	if (m_currentState == -1)
	{
		return false;
	}
	if (IsAccepting())
	{
		return true;
	}
}

int DFA::GetNumberOfTransitionSymbols() const
{
	return m_numberOfTransitionSymbols;
}

int DFA::GetTransition(int sourceState, int transitionSymbol)
{
	return m_transitionTable[sourceState][transitionSymbol];
}

bool DFA::SetTransition(int sourceState, int destinationState, int transitionSymbol)
{
	if (!IsValidState(sourceState) || !IsValidState(destinationState))
	{
		return false;
	}

	m_transitionTable[sourceState][transitionSymbol] = destinationState;
	return true;
}

bool DFA::IsAccepting()
{
	return m_currentState != -1 && m_acceptingStates[m_currentState];
}

void DFA::SetAcceptingState(int state)
{
	m_acceptingStates[state] = true;
}

int DFA::GetCurrentState() const
{
	return m_currentState;
}
