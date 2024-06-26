/*
 * Token.h
 *
 *  Created on: 12.04.2021
 *      Author: harald
 */

#ifndef FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_
#define FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_

#include <stdint.h>
#include <string>

namespace cLine {

class Token {
public:
	enum TokenType {Command, String, UInt64, SInt32, Float, None };

	Token(): _type { None } {};
	Token(TokenType type): _type { type } {};
	virtual ~Token() {};

	TokenType getType(void) { return _type; };

protected:
	TokenType _type;
};

template <class TOKEN_CLASS, Token::TokenType type>
class TToken: public Token {
public:
	TToken(): Token(None), _value { 0 } {};
	TToken(TOKEN_CLASS value): Token(type), _value { value } {};
	virtual ~TToken() {};

	TOKEN_CLASS getVal(void) { return _value; };

private:
	TOKEN_CLASS _value;
};

typedef TToken<uint32_t, Token::TokenType::Command> CmdToken;
typedef TToken<uint64_t, Token::TokenType::UInt64> UInt64Token;
//typedef TToken<uint64_t, Token::TokenType::UInt64> UInt64Token;
typedef TToken<int32_t, Token::TokenType::SInt32> SInt32Token;
typedef TToken<float, Token::TokenType::Float> FltToken;
typedef TToken<char*, Token::TokenType::String> ChrToken;

} /* namespace cLine */

#endif /* FRAMEWORK_SYSTEM_COMMANDLINE_TOKEN_H_ */
