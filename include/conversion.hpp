#pragma once
#include <string>
#include "parser/token.hpp"
#include "cst/operator.hpp"
#include "type/type.hpp"
#include "inst/operand.hpp"
#include "inst/inst.hpp"
#include "x86/asm.hpp"


// tokentype -> operators
UnOp toUnOp(TokenType type);
BinOp toBinOp(TokenType type);
BinOp asnOpToBinOp(TokenType type);
BinOp postOpToBinOp(TokenType type);


// operators -> ir opcodes
Inst::OpCode toOpCode(BinOp op);


// TODO: better name
// ir opcodes -> asm opcodes
X86Asm::OpCode toOpcode(Inst::OpCode opcode);