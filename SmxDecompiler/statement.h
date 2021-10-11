#pragma once

#include <vector>
#include "il.h"

enum class StatementType
{
	BASIC,
	SEQUENCE,
	IF,
	WHILE,
	DO_WHILE,
	SWITCH,
	GOTO
};

class BasicStatement;
class SequenceStatement;
class IfStatement;
class WhileStatement;
class SwitchStatement;
class GotoStatement;

class StatementVisitor
{
public:
	virtual void VisitBasicStatement( BasicStatement* stmt ) = 0;
	virtual void VisitSequenceStatement( SequenceStatement* stmt ) = 0;
	virtual void VisitIfStatement( IfStatement* stmt ) = 0;
	virtual void VisitWhileStatement( WhileStatement* stmt ) = 0;
	virtual void VisitSwitchStatement( SwitchStatement* stmt ) = 0;
	virtual void VisitGotoStatement( GotoStatement* stmt ) = 0;
};

class Statement
{
public:
	Statement( StatementType type ) : type_( type ) {}
	virtual ~Statement() = default;

	virtual void Accept( StatementVisitor* visitor ) = 0;

	void CreateLabel( cell_t pc ) { label_ = "label_" + std::to_string( pc ); }
	const char* label() const { return label_.empty() ? nullptr : label_.c_str(); }

	StatementType type() const { return type_; }
private:
	StatementType type_;
	std::string label_;
};

class BasicStatement : public Statement
{
public:
	BasicStatement( std::vector<ILNode*> nodes ) :
		Statement( StatementType::BASIC ),
		nodes_( std::move( nodes ) )
	{}
	BasicStatement( ILBlock* block ) :
		Statement( StatementType::BASIC )
	{
		size_t end = block->num_nodes();

		// If this is a jump then we don't want to include it, but if it is a fallthrough then keep it
		if( block->num_out_edges() > 1 ||
			dynamic_cast<ILJump*>(block->Last()) ||
			dynamic_cast<ILSwitch*>(block->Last()))
		{
			end -= 1;
		}

		for( size_t i = 0; i < end; i++ )
			nodes_.push_back( block->node( i ) );
	}

	size_t num_nodes() const { return nodes_.size(); }
	ILNode* node( size_t index ) { return nodes_[index]; }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitBasicStatement( this ); }
private:
	std::vector<ILNode*> nodes_;
};

class SequenceStatement : public Statement
{
public:
	SequenceStatement( std::vector<Statement*> statements ) :
		Statement( StatementType::SEQUENCE ),
		statements_( std::move( statements ) )
	{}

	Statement* statement( size_t index ) { return statements_[index]; }
	size_t num_statements() const { return statements_.size(); }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitSequenceStatement( this ); }
private:
	std::vector<Statement*> statements_;
};

class IfStatement : public Statement
{
public:
	IfStatement( ILNode* condition, Statement* then_branch, Statement* else_branch ) :
		Statement( StatementType::IF ),
		condition_( condition ),
		then_branch_( then_branch ),
		else_branch_( else_branch )
	{}

	ILNode* condition() { return condition_; }
	Statement* then_branch() { return then_branch_; }
	Statement* else_branch() { return else_branch_; }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitIfStatement( this ); }
private:
	ILNode* condition_;
	Statement* then_branch_;
	Statement* else_branch_;
};

class WhileStatement : public Statement
{
public:
	WhileStatement( ILNode* condition, Statement* body ) :
		Statement( StatementType::WHILE ),
		condition_( condition ),
		body_( body )
	{}

	ILNode* condition() { return condition_; }
	Statement* body() { return body_; }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitWhileStatement( this ); }
private:
	ILNode* condition_;
	Statement* body_;
};

struct CaseStatement
{
	cell_t value;
	Statement* body;
};

class SwitchStatement : public Statement
{
public:
	SwitchStatement( ILNode* value, Statement* default_case, std::vector<CaseStatement> cases ) :
		Statement( StatementType::WHILE ),
		value_( value ),
		default_case_( default_case ),
		cases_( std::move( cases ) )
	{}

	ILNode* value() { return value_; }
	Statement* default_case() { return default_case_; }
	size_t num_cases() const { return cases_.size(); }
	const CaseStatement& case_entry( size_t index ) const { return cases_[index]; }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitSwitchStatement( this ); }
private:
	ILNode* value_;
	Statement* default_case_;
	std::vector<CaseStatement> cases_;
};

class GotoStatement : public Statement
{
public:
	GotoStatement( Statement* target ) :
		Statement( StatementType::GOTO ),
		target_( target )
	{}

	Statement* target() { return target_; }

	virtual void Accept( StatementVisitor* visitor ) override { visitor->VisitGotoStatement( this ); }
private:
	Statement* target_;
};