#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
using namespace std;
/////////////////////////////////////////////////////////
shared_ptr<QueryBase> QueryBase::factory(const string& s) { // s is the search string 



int i=0,j=0;
std::string before,left,right,oWord;

std::istringstream iss(s);
std::string w,sNew;
   while (iss>>w) {
        j++;
        sNew=sNew+w;
        sNew+=" ";
   } 

//for(int i=0; i<sNew.size(); i++) {


if (sNew.at(0)=='O' && sNew.at(1)=='R' && j==3) { 
    //before="OR"; 
    int position= sNew.find(" ",i+3);
    left=sNew.substr(3,position-3);

    int l=position+2;
    l=sNew.size()-l;

    right=sNew.substr(position+1,l);
        return std::shared_ptr<QueryBase>(new OrQuery(left,right));
}

else if(sNew.at(0)=='A' && sNew.at(1)=='d' && j==3) {
    //before="AD";
    int position= sNew.find(" ",i+3);
    left=sNew.substr(3,position-3);

    int l=position+2;
    l=sNew.size()-l;

    right=sNew.substr(position+1,l);
        return std::shared_ptr<QueryBase>(new AdjacentQuery(left,right));
}  

else if(sNew.at(0)=='A' && sNew.at(1)=='N' && sNew.at(2)=='D'&& j==3) { 
    //before="AND";
    int position= sNew.find(" ",i+4);
    left=sNew.substr(4,position-4);

    int l=position+2;
    l=sNew.size()-l;

    right=sNew.substr(position+1,l);
        return std::shared_ptr<QueryBase>(new AndQuery(left,right));
}

else if(j==1) {
    int size=sNew.size()-1;
    sNew=sNew.substr(0,size);
 return std::shared_ptr<QueryBase>(new WordQuery(sNew));
}

else 
  throw invalid_argument("Unrecognized search"); 



}



// . . .
/////////////////////////////////////////////////////////

QueryResult AndQuery::eval (const TextQuery& text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>();
    set_intersection(left_result.begin(), left_result.end(),
        right_result.begin(), right_result.end(), 
        inserter(*ret_lines, ret_lines->begin()));
   return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const
{
    QueryResult left_result = text.query(left_query);
    QueryResult right_result = text.query(right_query);
    auto ret_lines = make_shared<set<line_no>>(left_result.begin(), left_result.end());
    ret_lines->insert(right_result.begin(), right_result.end());
    return QueryResult(rep(), ret_lines, left_result.get_file());
}
/////////////////////////////////////////////////////////
QueryResult AdjacentQuery::eval (const TextQuery& text) const
{
// . . .
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
std::ostream &print(std::ostream &os, const QueryResult &qr)
{
    os << "\"" << qr.sought << "\"" << " occurs " << 
        qr.lines->size() << " times:" <<std::endl;
    for (auto num : *qr.lines)
    {
        os << "\t(line " << num + 1 << ") " 
            << *(qr.file->begin() + num) << std::endl;
    }
    return os;
}
/////////////////////////////////////////////////////////