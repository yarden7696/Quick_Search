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
        sNew=sNew+ " " +w;
   } 

//for(int i=0; i<sNew.size(); i++) {


if (sNew.substr(i,i+1)=="OR") { 
    before="OR";
    int position= sNew.find(" ",i+2);
    left=sNew.substr(i+2,position-1);
    right=sNew.substr(position+1,sNew.size()-1);
        return std::shared_ptr<QueryBase>(new OrQuery(left,right));
}

else if(sNew.substr(i,i+1)=="AD") {
    before="AD";
    int position= sNew.find(" ",i+2);
    left=sNew.substr(i+2,position-1);
    right=sNew.substr(position+1,sNew.size()-1);
        return std::shared_ptr<QueryBase>(new AdjacentQuery(left,right));
}  

else if(sNew.substr(i,i+2)=="AND") { 
    //before="AND";
    int position= sNew.find(" ",i+2);
    left=sNew.substr(i+2,position-1);
    right=sNew.substr(position+1,sNew.size()-1);
        return std::shared_ptr<QueryBase>(new AndQuery(left,right));
}

//else one word without or /and/ ad
//else - error "Unrecognized search" check when to write this...
  




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