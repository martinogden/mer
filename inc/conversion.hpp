#pragma once
#include <string>
#include "token.hpp"
#include "operator.hpp"
#include "type.hpp"
#include "operand.hpp"


// tokentype -> operators
UnOp toUnOp(TokenType type);
BinOp toBinOp(TokenType type);
BinOp asnOpToBinOp(TokenType type);
BinOp postOpToBinOp(TokenType type);


// tokens -> types
Type toType(Token token);


// operators -> opcodes
Inst::OpCode toOpCode(BinOp op);
