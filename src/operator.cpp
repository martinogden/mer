#include "operator.hpp"


OpType getOpType(BinOp op) {
	switch (op) {
		case BinOp::ADD:
		case BinOp::SUB:
		case BinOp::MUL:
		case BinOp::DIV:
		case BinOp::MOD:
			return OpType::ARITH;
		case BinOp::LT:
		case BinOp::LT_EQL:
		case BinOp::GT:
		case BinOp::GT_EQL:
			return OpType::REL;
		case BinOp::EQL:
		case BinOp::NOT_EQL:
			return OpType::EQL;
		case BinOp::LOG_AND:
		case BinOp::LOG_OR:
			return OpType::LOGICAL;
		case BinOp::BIT_AND:
		case BinOp::BIT_OR:
		case BinOp::XOR:
			return OpType::BITWISE;
	}
}
