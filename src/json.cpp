#include "fi/app/json.h"

#include <algorithm>
#include <list>
#include <iostream>

// -- utils --

std::string opString(JSONOp o)
{
	std::string out;
	switch(o)
	{
		case JSON_OP_PAIR:
			out = "close pair ',' or '}'";
			break;
		case JSON_OP_ARRAY:
			out = "close array ']'";
			break;
		case JSON_OP_OBJECT:
			out = "close object '}'";
			break;
		case JSON_OP_STRING:
			out = "close string '\"'";
			break;
	}
	return out;
}

std::string indent(size_t level)
{
	std::string ind;

	for(size_t i=0; i<=level; i++)
	{
		ind += "  ";
	}

	return ind;
}

// -- JSON Value --

void JSONValue::set(std::string str)
{
	std::string *s = new std::string(str);
	data.push_back(std::make_pair(JSON_TYPE_STRING, s));
}

void JSONValue::set(JSONObject obj)
{
	JSONObject *o = new JSONObject(obj);
	data.push_back(std::make_pair(JSON_TYPE_OBJECT, o));
}

// -- JSONParse --

JSONParse::JSONParse(const std::string jsonStr):
	m_scopeLevel(0),
	m_bScopeBegan(false),
	m_jsonString(jsonStr)
{
	strip(m_jsonString);
	parseSubstring(m_jsonString);
}

JSONParse::~JSONParse()
{
}

void JSONParse::printTree()
{
	m_scopeLevel=0;
	printObject(m_jsonTree);
}

void JSONParse::strip(std::string &str)
{
	std::string out;
	out.reserve(str.length());
	bool isQuote = false;
	
	for(size_t i=0; i<str.length(); i++)
	{
		if(str[i]=='\"')
		{
			if(isQuote)
			{
				isQuote = false;
			}
			else
			{
				isQuote = true;
			}
		}
		
		if(isQuote)
		{
			out += str[i];
		}
		else
		{
			if(
			   str[i]==' ' ||
			   str[i]=='\t' ||
			   str[i]=='\r' ||
			   str[i]=='\n')
			{
				continue;
			}
			else
			{
				out += str[i];
			}
		}
	}
	str = out;
}

void JSONParse::parseSubstring(std::string str)
{
	if(m_bScopeBegan && !m_scopeLevel)
	{
		return;
	}

	std::string::size_type openBracePos = std::string::npos;
	std::string::size_type closeBracePos = std::string::npos;
	std::string::size_type openBracketPos = std::string::npos;
	std::string::size_type closeBracketPos = std::string::npos;
	std::string::size_type colonPos = std::string::npos;
	std::string::size_type commaPos = std::string::npos;

	std::string::size_type quotePos = str.find_first_of("\"");

	if(m_opStack.empty() || m_opStack.top() != JSON_OP_STRING)
	{
		openBracePos = str.find_first_of("{");
		closeBracePos = str.find_first_of("}");
		openBracketPos = str.find_first_of("[");
		closeBracketPos = str.find_first_of("]");
		colonPos = str.find_first_of(":");
		commaPos = str.find_first_of(",");
	}

	std::list<std::string::size_type> list;
	list.push_back(openBracePos);
	list.push_back(closeBracePos);
	list.push_back(openBracketPos);
	list.push_back(closeBracketPos);
	list.push_back(colonPos);
	list.push_back(commaPos);
	list.push_back(quotePos);

	list.sort();

	//test
	/*
	 std::cout<<"stckSz["<<m_nodeStack.size();
	 std::cout<<"] token["<<str.substr(list.front(), 1);
	 std::cout<<"] \""<<str<<"\""<<std::endl;
	 */

	if(list.front() == std::string::npos)
	{
		return;
	}

	if(list.front() == openBracePos)
	{
		m_bScopeBegan=true;
		m_scopeLevel++;
		JSONObject n;
		m_objectStack.push(n);
		m_opStack.push(JSON_OP_OBJECT);
		parseSubstring(str.substr(openBracePos+1));
	}
	else if(list.front() == colonPos)
	{
		JSONPair p;
		p.name = m_currentStringVal;
		m_currentStringVal="";
		m_pairStack.push(p);
		m_opStack.push(JSON_OP_PAIR);
		parseSubstring(str.substr(colonPos+1));
	}
	else if(list.front() == commaPos)
	{
		std::string s = m_currentStringVal.length() ? m_currentStringVal : str.substr(0,commaPos);
		m_currentStringVal="";
		
		if(m_opStack.top() == JSON_OP_PAIR)
		{
			m_opStack.pop();

			if(s.length())
			{
				m_pairStack.top().value.set(s);
				
				JSONPair p = m_pairStack.top();
				m_pairStack.pop();
				m_objectStack.top().pairs.push_back(p);
			}
		}
		else if(m_opStack.top() == JSON_OP_ARRAY)
		{
			if(s.length())
			{
				m_pairStack.top().value.set(s);
			}
		}
		else if(m_opStack.top() == JSON_OP_OBJECT)
		{
			if(s.length())
			{
				std::cout << "syntax error: expected " << opString(m_opStack.top()) << std::endl;
				std::cout << "rememainder of JSON string: " << str << std::endl;
				exit(1);
			}
		}
		else
		{
			std::cout << "syntax error: expected " << opString(m_opStack.top()) << std::endl;
			std::cout << "rememainder of JSON string: " << str << std::endl;
			exit(1);
		}
		
		parseSubstring(str.substr(commaPos+1));
	}
	else if(list.front() == openBracketPos)
	{
		m_opStack.push(JSON_OP_ARRAY);
		parseSubstring(str.substr(openBracketPos+1));
	}
	else if(list.front() == closeBracketPos)
	{
		if(m_opStack.top() == JSON_OP_ARRAY)
		{
			std::string s = m_currentStringVal.length() ? m_currentStringVal : str.substr(0,closeBracketPos);
			m_currentStringVal="";
			if(s.length())
			{
				m_pairStack.top().value.set(s);
			}
			
			m_opStack.pop();

			JSONPair p = m_pairStack.top();
			m_pairStack.pop();
			m_objectStack.top().pairs.push_back(p);
			parseSubstring(str.substr(closeBracketPos+1));
		}
		else
		{
			std::cout << "syntax error: expected " << opString(m_opStack.top()) << std::endl;
			std::cout << "rememainder of JSON string: " << str << std::endl;
			exit(1);
		}
	}
	else if(list.front() == closeBracePos)
	{
		std::string s = m_currentStringVal.length() ? m_currentStringVal : str.substr(0,closeBracePos);
		m_currentStringVal="";

		if(s.length() && m_opStack.top() == JSON_OP_PAIR)
		{
			m_opStack.pop();

			m_pairStack.top().value.set(s);

			JSONPair p = m_pairStack.top();
			m_pairStack.pop();
			m_objectStack.top().pairs.push_back(p);
		}

		JSONObject o = m_objectStack.top();
		m_objectStack.pop();

		if(!m_objectStack.empty())
		{
			switch(m_opStack.top())
			{
				case JSON_OP_PAIR:
					m_opStack.pop();
					break;
				case JSON_OP_OBJECT:
					m_opStack.pop();
					break;
				default:
					std::cout << "syntax error: expected " << opString(m_opStack.top()) << std::endl;
					std::cout << "rememainder of JSON string: " << str << std::endl;
					exit(1);
			}

			JSONPair p;
			p.name = m_pairStack.top().name;
			p.value.set(o);

			if(m_opStack.top() == JSON_OP_ARRAY)
			{
				m_pairStack.top().value.set(o);
			}
			else
			{
				m_objectStack.top().pairs.push_back(p);
				m_pairStack.pop();
			}
		}
		else
		{
			std::cout << "assigning tree" << std::endl;
			m_jsonTree = o;
		}

		m_scopeLevel--;
		parseSubstring(str.substr(closeBracePos+1));
	}
	else if(list.front() == quotePos)
	{
		if(m_opStack.top() == JSON_OP_STRING)
		{
			m_opStack.pop();
			m_currentStringVal = str.substr(0,quotePos);
		}
		else
		{
			m_opStack.push(JSON_OP_STRING);
		}

		parseSubstring(str.substr(quotePos+1));
	}
}

void JSONParse::printObject(JSONObject obj)
{
	for(size_t i=0; i<obj.pairs.size(); i++)
	{
		std::cout << indent(m_scopeLevel) + obj.pairs[i].name << "=";
		for(size_t j=0; j<obj.pairs[i].value.data.size(); j++)
		{
			switch(obj.pairs[i].value.data[j].first)
			{
				case JSON_TYPE_STRING:
					if(j>0){std::cout << ", ";}
					std::cout << "\"" + *(std::string*)obj.pairs[i].value.data[j].second << "\"";
					if(j == obj.pairs[i].value.data.size()-1){std::cout << std::endl;}
					break;
				case JSON_TYPE_OBJECT:
					m_scopeLevel++;
					std::cout << std::endl;
					printObject(*(JSONObject*)obj.pairs[i].value.data[j].second);
					m_scopeLevel--;
					break;
				default:
					break;
			}
		}
	}
}

