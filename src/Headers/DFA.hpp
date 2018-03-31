/**************************************************************************
Author: Stefan Rapeanu-Andreescu
Creation date: 21.02.2018
**************************************************************************/

#ifndef DFA_HPP_
#define DFA_HPP_

#include <map>
#include <string>
#include <fstream>

class DFA
{
public:
	DFA();
	DFA(int numberOfStates, int alphabetLength);
	virtual ~DFA();

	void Initialize(int numberOfStates, int alphabetLength);
	void Reset();

	bool IsValidState(int state);

	int GetNumberOfStates() const;
	int GetNumberOfTransitionSymbols() const;

	int GetCurrentState() const;

	int GetTransition(int sourceState, int transitionSymbol);
	bool SetTransition(int sourceState, int destinationState, int transitionSymbol);

	bool IsAccepting();
	void SetAcceptingState(int state);
	void ResetState();

	bool ParseLexeme(
		std::string initialText,
		std::string& remainingText,
		std::string& lexeme
		);

	bool ParseLexeme(
		std::ifstream& initialText,
		std::string& lexeme
		);

protected:
	void Transition(int transitionSymbol);

protected:
	int		m_numberOfStates;
	int		m_currentState;
	int		m_numberOfTransitionSymbols;
	int**	m_transitionTable;
	bool*	m_acceptingStates;
};


#endif /* DFA_HPP_ */
