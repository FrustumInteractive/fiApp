#ifndef _JSON_H
#define _JSON_H

#include <string>
#include <vector>
#include <stack>
#include <deque>

enum JSONType
{
	JSON_TYPE_NULL,
	JSON_TYPE_FALSE,
	JSON_TYPE_TRUE,
	JSON_TYPE_ARRAY,
	JSON_TYPE_OBJECT,
	JSON_TYPE_NUMBER,
	JSON_TYPE_STRING
};

enum JSONOp
{
	JSON_OP_OBJECT,
	JSON_OP_PAIR,
	JSON_OP_ARRAY,
	JSON_OP_STRING
};

// forward decl.
struct JSONObject;

struct JSONValue
{
	void set(std::string str);

	void set(JSONObject obj);

	std::vector<std::pair<JSONType,void*>> data;
};

struct JSONPair
{
	std::string name;
	JSONValue value;
};

struct JSONObject
{
	std::vector<JSONPair> pairs;
};


class JSONParse
{
public:
	JSONParse(const std::string jsonStr);
	~JSONParse();

	void printTree();

private:

	void strip(std::string &str);

	void parseSubstring(std::string str);
	void printObject(JSONObject node);

	JSONObject m_jsonTree;
	std::stack<JSONObject> m_objectStack;
	std::stack<JSONPair> m_pairStack;
	std::string m_currentStringVal;
	std::stack<JSONOp> m_opStack;
	int m_scopeLevel;
	bool m_bScopeBegan;
	std::string m_jsonString;
};


#endif /*_JSON_H*/
