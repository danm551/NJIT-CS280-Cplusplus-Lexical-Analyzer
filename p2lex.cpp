/*
o CS 280-102, SPRING 2015
o Ernesto Martinez
o Program #2
*/

#include <iostream>
#include <map>
#include "p2lex.h"
using namespace std;

/*
 * Name: getToken
 *
 * Args: br, lexeme
 *
 * Description: reads from file and returns a lexeme token
 *
 * Returns: returnToken
 */
Token getToken(istream *br, string& lexeme)
{
	Token returnToken;
	string copy;
	map<Token,string> reference;
	Token id(ID), integer(INT), string(STRING), plus(PLUS), minus(MINUS), star(STAR), slash(SLASH), set(SET), print(PRINT), sc(SC), done(DONE);
	int i = 0;
	char c, peek;
	reference[string] = "\"";
	reference[plus] = "+";
	reference[minus] = "-";
	reference[star] = "*";
	reference[slash] = "/";
	reference[set] = "set";
	reference[print] = "print";
	reference[sc] = ";";
	reference[done] = "\0";

	lexeme.clear();

	while(!br->eof()){
		br->get(c);
		copy = c;
		peek = br->peek();

		if(isalpha(c)) //locates identifiers and keywords
		{
			lexeme.insert(i, copy);
			i++;
			if(lexeme.length() == 3)
			{
				if(lexeme == "set")
				{
					return returnToken = set;
				}
			}
			if(lexeme.length() == 5)
			{
				if(lexeme == "print")
				{
					return returnToken = print;
				}
			}
			if(!isalpha(peek))
			{
				return returnToken = id;
			}
		}
		else if(isdigit(c)) //locates numbers
		{
			lexeme.insert(i, copy);
			i++;
			if(!isdigit(peek))
			{
				lexeme.clear();
				return returnToken = integer;
			}

		}
		else if(copy == reference[string]) //locates quoted strings
		{
			while(!br->eof())
			{
					br->get(c);
					copy = c;
					peek = br->peek();
					if(copy != "\"")
					{
						lexeme.insert(i, copy);
						i++;
					}
					if(copy == "\n")
					{
						return returnToken = done;
					}
					if(copy == "\"")
					{
						return returnToken = string;
					}
					else if(br->eof())
						return returnToken = done;
			}
		}
		else if(copy == reference[plus]) //locates +
		{
			return returnToken = plus;
		}
		else if(copy == reference[minus]) //locates -
		{
			return returnToken = minus;
		}
		else if(copy == reference[star]) //locates *
		{
			return returnToken = star;
		}
		else if(copy == reference[slash]) //locates / and //
		{
			if(peek == '/')
			{
				while(c != '\n')
				{
					br->get(c);
					peek = br->peek();
					if(br->eof())
					{
						return returnToken = done;
					}
					if(c == '\n')
					{
						returnToken = getToken(br, lexeme);
						return returnToken;
					}
				}
			}
			return returnToken = slash;
		}
		else if(copy == reference[sc]) //locates semicolon
		{
			return returnToken = sc;
		}
		else if(br->eof()) //locates end of file
		{
			return returnToken = done;
		}
	}

	return returnToken;
}

