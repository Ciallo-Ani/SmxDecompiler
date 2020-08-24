#include "smx-disasm.h"

#include <sstream>

static SmxInstr err_instr = { "err", 0, {} };

static SmxInstr instrs[] =
{
	{ "none", 0, {} },
	{ "load.pri", 1, {} },
	{ "load.alt", 1, {} },
	{ "load.s.pri", 1, {} },
	{ "load.s.alt", 1, {} },
	{ "lref.pri", -1, {} },
	{ "lref.alt", -1, {} },
	{ "lref.s.pri", 1, {} },
	{ "lref.s.alt", 1, {} },
	{ "load.i", 0, {} },
	{ "lodb.i", 1, {} },
	{ "const.pri", 1, {} },
	{ "const.alt", 1, {} },
	{ "addr.pri", 1, {} },
	{ "addr.alt", 1, {} },
	{ "stor.pri", 1, {} },
	{ "stor.alt", 1, {} },
	{ "stor.s.pri", 1, {} },
	{ "stor.s.alt", 1, {} },
	{ "sref.pri", -1, {} },
	{ "sref.alt", -1, {} },
	{ "sref.s.pri", 1, {} },
	{ "sref.s.alt", 1, {} },
	{ "stor.i", 0, {} },
	{ "strb.i", 1, {} },
	{ "lidx", 0, {} },
	{ "lidx.b", 1, {} },
	{ "idxaddr", 0, {} },
	{ "idxaddr.b", 1, {} },
	{ "align.pri", -1, {} },
	{ "align.alt", -1, {} },
	{ "lctrl", -1, {} },
	{ "sctrl", -1, {} },
	{ "move.pri", 0, {} },
	{ "move.alt", 0, {} },
	{ "xchg", 0, {} },
	{ "push.pri", 0, {} },
	{ "push.alt", 0, {} },
	{ "push.r", -1, {} },
	{ "push.c", 1, {} },
	{ "push", 1, {SmxParam::ADDRESS} },
	{ "push.s", 1, {} },
	{ "pop.pri", 0, {} },
	{ "pop.alt", 0, {} },
	{ "stack", 1, {SmxParam::CONSTANT} },
	{ "heap", 1, {SmxParam::CONSTANT} },
	{ "proc", 0, {} },
	{ "ret", -1, {} },
	{ "retn", 0, {} },
	{ "call", 1, {SmxParam::FUNCTION} },
	{ "call.pri", -1, {} },
	{ "jump", 1, {SmxParam::JUMP} },
	{ "jrel", -1, {} },
	{ "jzer", 1, {SmxParam::JUMP} },
	{ "jnz", 1, {SmxParam::JUMP} },
	{ "jeq", 1, {SmxParam::JUMP} },
	{ "jneq", 1, {SmxParam::JUMP} },
	{ "jless", -1, {} },
	{ "jleq", -1, {} },
	{ "jgrtr", -1, {} },
	{ "jgeq", -1, {} },
	{ "jsless", 1, {SmxParam::JUMP} },
	{ "jsleq", 1, {SmxParam::JUMP} },
	{ "jsgrtr", 1, {SmxParam::JUMP} },
	{ "jsgeq", 1, {SmxParam::JUMP} },
	{ "shl", 0, {} },
	{ "shr", 0, {} },
	{ "sshr", 0, {} },
	{ "shl.c.pri", 1, {} },
	{ "shl.c.alt", 1, {} },
	{ "shr.c.pri", 1, {} },
	{ "shr.c.alt", 1, {} },
	{ "smul", 0, {} },
	{ "sdiv", 0, {} },
	{ "sdiv.alt", 0, {} },
	{ "umul", -1, {} },
	{ "udiv", -1, {} },
	{ "udiv.alt", -1, {} },
	{ "add", 0, {} },
	{ "sub", 0, {} },
	{ "sub.alt", 0, {} },
	{ "and", 0, {} },
	{ "or", 0, {} },
	{ "xor", 0, {} },
	{ "not", 0, {} },
	{ "neg", 0, {} },
	{ "invert", 0, {} },
	{ "add.c", 1, {} },
	{ "smul.c", 1, {} },
	{ "zero.pri", 0, {} },
	{ "zero.alt", 0, {} },
	{ "zero", 1, {SmxParam::ADDRESS} },
	{ "zero.s", 1, {} },
	{ "sign.pri", -1, {} },
	{ "sign.alt", -1, {} },
	{ "eq", 0, {} },
	{ "neq", 0, {} },
	{ "less", -1, {} },
	{ "leq", -1, {} },
	{ "grtr", -1, {} },
	{ "geq", -1, {} },
	{ "sless", 0, {} },
	{ "sleq", 0, {} },
	{ "sgrtr", 0, {} },
	{ "sgeq", 0, {} },
	{ "eq.c.pri", 1, {} },
	{ "eq.c.alt", 1, {} },
	{ "inc.pri", 0, {} },
	{ "inc.alt", 0, {} },
	{ "inc", 1, {SmxParam::ADDRESS} },
	{ "inc.s", 1, {} },
	{ "inc.i", 0, {} },
	{ "dec.pri", 0, {} },
	{ "dec.alt", 0, {} },
	{ "dec", 1, {SmxParam::ADDRESS} },
	{ "dec.s", 1, {} },
	{ "dec.i", 0, {} },
	{ "movs", 1, {SmxParam::CONSTANT} },
	{ "cmps", -1, {} },
	{ "fill", 1, {SmxParam::CONSTANT} },
	{ "halt", 1, {SmxParam::CONSTANT} },
	{ "bounds", 1, {SmxParam::CONSTANT} },
	{ "sysreq.pri", -1, {} },
	{ "sysreq.c", 1, {} },
	{ "file", -1, {} },
	{ "line", -1, {} },
	{ "symbol", -1, {} },
	{ "srange", -1, {} },
	{ "jump.pri", -1, {} },
	{ "switch", 1, {SmxParam::JUMP} },
	{ "casetbl", 2, {SmxParam::CONSTANT, SmxParam::JUMP} },
	{ "swap.pri", 0, {} },
	{ "swap.alt", 0, {} },
	{ "push.adr", 1, {} },
	{ "nop", 0, {} },
	{ "sysreq.n", 2, {} },
	{ "symtag", -1, {} },
	{ "break", 0, {} },
	{ "push2.c", 2, {} },
	{ "push2", 2, {SmxParam::ADDRESS, SmxParam::ADDRESS} },
	{ "push2.s", 2, {} },
	{ "push2.adr", 2, {} },
	{ "push3.c", 3, {} },
	{ "push3", 3, {SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS} },
	{ "push3.s", 3, {} },
	{ "push3.adr", 3, {} },
	{ "push4.c", 4, {} },
	{ "push4", 4, {SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS} },
	{ "push4.s", 4, {} },
	{ "push4.adr", 4, {} },
	{ "push5.c", 5, {} },
	{ "push5", 5, {SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS, SmxParam::ADDRESS} },
	{ "push5.s", 5, {} },
	{ "push5.adr", 5, {} },
	{ "load.both", 2, {} },
	{ "load.s.both", 2, {} },
	{ "const", 2, {SmxParam::ADDRESS, SmxParam::CONSTANT} },
	{ "const.s", 2, {} },
	{ "sysreq.d", -1, {} },
	{ "sysreq.nd", -1, {} },
	{ "trk.push.c", 1, {} },
	{ "trk.pop", 0, {} },
	{ "genarray", 1, {SmxParam::CONSTANT} },
	{ "genarray.z", 1, {} },
	{ "stradjust.pri", 0, {} },
	{ "stackadjust", -1, {} },
	{ "endproc", 0, {} },
	{ "ldgfn.pri", -1, {} },
	{ "rebase", 3, {SmxParam::ADDRESS, SmxParam::CONSTANT, SmxParam::CONSTANT} },
	{ "firstfake", -1, {} },
	{ "fabs", 0, {} },
	{ "float", 0, {} },
	{ "float.add", 0, {} },
	{ "float.sub", 0, {} },
	{ "float.mul", 0, {} },
	{ "float.div", 0, {} },
	{ "round", 0, {} },
	{ "floor", 0, {} },
	{ "ceil", 0, {} },
	{ "rndtozero", 0, {} },
	{ "float.cmp", 0, {} },
	{ "float.gt", 0, {} },
	{ "float.ge", 0, {} },
	{ "float.lt", 0, {} },
	{ "float.le", 0, {} },
	{ "float.ne", 0, {} },
	{ "float.eq", 0, {} },
	{ "float.not", 0, {} },
};

const SmxInstr& SmxDisassembler::InstrInfo( SmxOpcode op )
{
	if( op >= 0 && op < SMX_OPCODES_TOTAL )
	{
		return instrs[op];
	}
	return err_instr;
}
const SmxInstr& SmxDisassembler::InstrInfo( cell_t op )
{
	if( op >= 0 && op < SMX_OPCODES_TOTAL )
	{
		return instrs[op];
	}
	return err_instr;
}

std::string SmxDisassembler::DisassembleInstr( const cell_t* instr )
{
	const SmxInstr& info = InstrInfo( instr[0] );
	std::ostringstream ss;
	ss << info.mnem;
	if( info.num_params > 0 )
	{
		ss << " ";
		for( int param = 1; param <= info.num_params; param++ )
		{
			ss << std::hex << instr[param];
			if( param != info.num_params )
			{
				ss << ", ";
			}
		}
	}
	return ss.str();
}

std::string SmxDisassembler::DisassembleFunction( const SmxFunction& func )
{
	std::ostringstream ss;
	const cell_t* instr = func.pcode_start;
	while( instr < func.pcode_end )
	{
		const SmxInstr& info = InstrInfo( instr[0] );
		ss << DisassembleInstr( instr ) << "\n";
		instr += 1 + info.num_params;
	}
	return ss.str();
}
