//============================================================================
// CS 280-102, SPRING 2015
// Ernesto Martinez
// Program #3
//============================================================================

#include <iostream>
#include <fstream>
#include "p2lex.h"
#include <typeinfo>
#include <vector>
using namespace std;

class PTree
{

public:
	PTree *left;
	PTree *right;
	Token data;

	PTree()
	{
		left = NULL;
		right = NULL;
	}
	PTree(PTree *left)
	{
		this->left = left;
		right = NULL;
	}
	PTree(PTree *left,PTree *right)
	{
		this->left = left;
		this->right = right;
	}
	void Insert(PTree *insert)
	{
		if(left == NULL)
			left = insert;
		else
			right = insert;
	}
	void SetData(Token &token)
	{
		data = token;
	}
	void GetData()
	{
		cout << data;
	}
	virtual void SetId(string a) {}
	virtual void SetValue(string a) {}
	virtual void SetValue(int b) {}
	virtual void GetId() {}
	virtual void GetValue() {}
	virtual void GetType() {}
};

class PTreeStmtList : public PTree
{
	string type;
public:
	PTreeStmtList() : PTree() {}
	PTreeStmtList(PTree *s1, PTree *s2) : PTree(s1, s2) {}
	void GetType()
	{
		cout << "Statement List\n";
	}
};

class PTreeStmt : public PTree
{
public:
	PTreeStmt() : PTree(){}
	PTreeStmt(PTree *expr) : PTree(expr){}
	void GetType()
	{
		cout << "Statement\n";
	}
};

class PTreeExpr : public PTree
{
public:
	PTreeExpr() : PTree(){}
	PTreeExpr(PTree *t1, PTree *t2) : PTree(t1, t2) {}
	void GetType()
	{
		cout << "Expression\n";
	}
};

class PTreeTerm : public PTree
{
public:
	PTreeTerm() : PTree(){}
	PTreeTerm(PTree *t1, PTree *t2) : PTree(t1, t2) {}
	void GetType()
	{
		cout << "Term\n";
	}
};

class PTreePrimary : public PTree
{

public:

	string qstring;
	int num;

	PTreePrimary() : PTree(){}
	void SetValue(string a)
	{
		qstring = a;
	}
	void SetValue(int b)
	{
		num = b;
	}
	void GetValue()
	{
		if(qstring.empty())
			cout << num;
		else
			cout << qstring;
	}
	void GetType()
	{
		cout << "Primary\n";
	}
};

class PTreeSet : public PTree
{

public:
	string id;
	PTreeSet() : PTree(){}
	void SetId(string a)
	{
		id = a;
	}
	void GetId()
	{
		cout << id;
	}
	void GetType()
	{
		cout << "Set\n";
	}
};

class PTreePrint : public PTree
{

public:
	string id;
	PTreePrint() : PTree(){}
	void GetType()
	{
		cout << "Print\n";
	}
};

PTree *Program(istream *br, Token &myToken, string &lexeme);
PTree *StmtList(istream *br, Token &myToken, string &lexeme);
PTreeStmt *Stmt(istream *br, Token &myToken, string &lexeme);
PTreeExpr *Expr(istream *br, Token &myToken, string &lexeme);
PTreeTerm *Term(istream *br, Token &myToken, string &lexeme);
PTreePrimary *Primary(istream *br, Token &myToken, string &lexeme);
Token id(ID), integer(INT), quotedString(STRING), plusSign(PLUS), minusSign(MINUS), star(STAR), slash(SLASH), setting(SET), print(PRINT), sc(SC), done(DONE);
PTree *programTree = new PTree;
PTree *program = new PTree;
int counter = 0;
int status = 3;
vector<string> setIds;
vector<string> printIds;
vector<string> idErrors;

//Program ::= StmtList
PTree *Program(istream *br, Token &myToken, string &lexeme)
{
	return StmtList(br, myToken, lexeme);
}


//StmtList ::= Stmt | Stmt StmtList
PTree *StmtList(istream *br, Token &myToken, string &lexeme)
{
	PTreeStmt *stmt = new PTreeStmt;
	stmt = 0;

	stmt = Stmt(br, myToken, lexeme);

	if(stmt && myToken != 10)
	{
		counter++;
		return new PTreeStmtList(stmt, StmtList(br, myToken, lexeme));
	}

	return 0;
}

//Stmt ::= PRINT Expr SC | SET ID Expr SC
PTreeStmt *Stmt(istream *br, Token &myToken, string &lexeme)
{
	PTreeStmt *stmt = new PTreeStmt;
	PTreeExpr *expr = new PTreeExpr;
	PTreeSet *set = new PTreeSet;
	PTreePrint *printnode = new PTreePrint;
	expr = 0;

	if(myToken == print && myToken)
	{
		status = 1;
		stmt->Insert(printnode);//inserts "print" node as child of stmt
		lexeme.clear();
		myToken = getToken(br, lexeme);//gets new token
		expr = Expr(br, myToken, lexeme);//method call to check for Expr

		if(expr)
		{
			stmt->Insert(expr);
		}
		else
			cout << "Expected an expression\n";

		if(myToken == sc)
		{
			lexeme.clear();
			myToken = getToken(br, lexeme);//gets new token
			return stmt;
		}
		else
			cout << "Missing semicolon\n";
	}
	else if(myToken == setting)
	{
		status = 0;
		stmt->SetData(myToken);//saves "set" token to stmt object
		myToken = getToken(br, lexeme);//gets new token
		if(myToken == id)
		{
			set->SetValue(lexeme);//saves value of the id to "set"
			stmt->Insert(set);//inserts set to stmt
			setIds.push_back(lexeme);
			lexeme.clear();//clears lexeme
			myToken = getToken(br, lexeme);//gets new token
			expr = Expr(br, myToken, lexeme);//method call to check for Expr
		}

		if(expr)
		{
			stmt->Insert(expr);
		}
		else
		{
			cout << "Expected an expression\n";
			return 0;
		}

		if(myToken == sc)
		{
			lexeme.clear();
			myToken = getToken(br, lexeme);//gets new token
			return stmt;
		}
		else
			cout << "Missing semicolon\n";
	}
	else
	{
		cout << "not a valid statement\n";
	}

	return 0;
}

//Expr ::= Expr PLUS Term | Expr MINUS Term | Term
PTreeExpr *Expr(istream *br, Token &myToken, string &lexeme)
{
	PTreeExpr *expr = new PTreeExpr;
	PTreeTerm *term = new PTreeTerm;
	term = 0;

	term = Term(br, myToken, lexeme);//calls for term

	if(term)
	{
		if(myToken == minusSign || myToken == plusSign)
		{
			expr->SetData(myToken);
			myToken = getToken(br, lexeme);//gets new token
			term = 0;
		}
		else
		{
			expr->Insert(term);
			return expr;
		}

		return new PTreeExpr(term, Expr(br, myToken, lexeme));
	}
	else
	{
		return 0;
	}

	return 0;
}

//Term ::= Term STAR Primary | Term SLASH Primary | Primary
PTreeTerm *Term(istream *br, Token &myToken, string &lexeme)
{
	PTreeTerm *term = new PTreeTerm;
	PTreePrimary *primary = new PTreePrimary;
	primary = 0;

	primary = Primary(br, myToken, lexeme);//calls for primary

	if(primary)
	{
		if(myToken == slash || myToken == star)
		{
			term->SetData(myToken);
			myToken = getToken(br, lexeme);//gets new token
			primary = 0;
		}
		else
		{
			term->Insert(primary);
			return term;
		}

		return new PTreeTerm(primary, Term(br, myToken, lexeme));
	}
	else
	{
		cout << "Missing primary\n";
		return 0;
	}
	return 0;
}

//Primary ::= ID | INT | STRING
PTreePrimary *Primary(istream *br, Token &myToken, string &lexeme)
{
	PTreePrimary *primary = new PTreePrimary;

	if(myToken == id)
	{
		primary->SetData(myToken);
		primary->SetValue(lexeme);

		if(status == 1)
		{
			printIds.push_back(lexeme);
		}
		lexeme.clear();
		myToken = getToken(br, lexeme);//gets new token
		return primary;
	}
	else if(myToken == quotedString || myToken == integer)
	{
		primary->SetData(myToken);
		primary->SetValue(lexeme);
		lexeme.clear();
		myToken = getToken(br, lexeme);//gets new token
		return primary;
	}
	else
	{
		//cout << "Missing Primary\n";
	}
	return 0;
}

//Traverses Tree
void traverse(PTree *p)
{
	if (p)
	{
       traverse(p->left);
       p->GetType();
       traverse(p->right);
    }
}
int main(int argc, char*argv[]) {

	istream *br;
	ifstream infile;
	 // check args and open the file
	if( argc == 1 )
		br = &cin;
	else if( argc != 2 )
	{
		return 1; // print an error msg
	}
	else
	{
		infile.open(argv[1]);
		if( infile.is_open() )
			br = &infile;
		else
		{
			cout << argv[1] << " can't be opened\n" << endl;
			return 1;
		}
	}

	string lexeme;
	Token myToken;

	myToken = getToken(br, lexeme);

	if(myToken != done)
		program->Insert(Program(br, myToken, lexeme));

	for(int i = 0; i < printIds.size(); i++)
	{
		int error = 0;
		for(int j = 0; j < setIds.size(); j++)
		{
			if(printIds.at(i) == setIds.at(j))
			{
				error++;
			}
		}
		if(error == 0)
			idErrors.push_back(printIds.at(i));
	}

	if(!printIds.empty())
	{
		for(int i = 0; i < idErrors.size(); i++)
		{
			cout << "Identifier " << idErrors.at(i) << " is used without being set\n";
		}
	}

	return 0;
}
