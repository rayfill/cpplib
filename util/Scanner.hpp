#ifndef SCANNER_HPP_
#define SCANNER_HPP_

#include <util/FSM.hpp>
#include <string>

template <
	typename CharType,
	typename CharTrait = std::char_traits<CharType> >
class Scanner
{
public:
	typename std::basic_string<CharType> ScannerString;
	typename ScannerString::iterator iterator;

	enum
	{
		NONE,
		LITERAL,
		IF,
		ELSE,
		WHILE,
		FOR,
		CONTINUE,
		BREAK,
	};

private:
	typedef FiniteStateMachine<CharType> fsm_t;
	typedef CharTrait char_trait_t;

	fsm_t literalFsm;
	fsm_t symbolFsm;

	static void literalFsmInit(fsm_t& fsm)
	{
		// literal is "^[_a-zA-Z][_a-zA-Z0-9]*$"(reglar expression reps).
		fsm_t::state_t* state = fsm.findStateForId(0); // get root state.
		fsm_t::state_t* secondLiteral = new fsm_t::state_t();
		
		// small alphabet 'a' to 'z'
		for (CharType smallAlphabet = char_trait_t::to_char_type('a');
			 smallAlphabet <= char_trait_t::to_char_type('z');
			 ++smallAlphabet)
		{
			state->setTransit(smallAlphabet, secondLiteral);
			secondLiteral->setTransit(smallAlphabet, secondLiteral);
		}

		// big alphabet 'A' to 'Z'
		for (CharType bigAlphabet = char_trait_t::to_char_type('A');
			 bigAlphabet <= char_trait_t::to_char_type('Z');
			 ++bigAlphabet)
		{
			state->setTransit(bigAlphabet, secondLiteral);
			secondLiteral->setTransit(bigAlphabet, secondLiteral);
		}

		// number '0' to '9'
		for (CharType number = char_trait_t::to_char_type('0');
			 number <= char_trait_t::to_char_type('9');
			 ++number)
		{
			secondLiteral->setTransit(number, secondLiteral);
		}

		state->setId(LITERAL);
		secondLiteral(LITERAL);
	}

	static void symbolFsmInit(fsm_t& fsm)
	{
		fsm.add("if", IF);
		fsm.add("else", ELSE);
		fsm.add("while", WHILE);
		fsm.add("for", FOR);
		fsm.add("continue", CONTINUE);
		fsm.add("for", BREAK);
	}

public:
	Scanner():
		literalFsm(), symbolFsm()
	{
		literamFsmInit(literalFsm);
		symbolFsmInit(symbolFsm);
	}
};

#endif /* SCANNER_HPP_ */
