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
#include <cmath>
using namespace std;

shared_ptr<QueryBase> QueryBase::factory(const string& s) { // s is the search string 

int i=0,j=0;
std::string left,right;

std::istringstream iss(s);
std::string w,sNew;
   while (iss>>w) {
        j++;
        sNew=sNew+w;
        sNew+=" ";
   } 
if (sNew.at(0)=='O' && sNew.at(1)=='R' && j==3) { 
    int position= sNew.find(" ",i+3);
    left=sNew.substr(3,position-3);
    int l=position+2;
    l=sNew.size()-l;
    right=sNew.substr(position+1,l);
        return std::shared_ptr<QueryBase>(new OrQuery(left,right));
}
else if(sNew.at(0)=='A' && sNew.at(1)=='D' && j==3) {
    int position= sNew.find(" ",i+3);
    left=sNew.substr(3,position-3);
    int l=position+2;
    l=sNew.size()-l;
    right=sNew.substr(position+1,l);
        return std::shared_ptr<QueryBase>(new AdjacentQuery(left,right));
}  
else if(sNew.at(0)=='A' && sNew.at(1)=='N' && sNew.at(2)=='D'&& j==3) { 
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
else throw invalid_argument("Unrecognized search\n"); 
}


QueryResult AndQuery::eval (const TextQuery& text) const {
    QueryResult left_result = text.query(left_query);
       QueryResult right_result = text.query(right_query);
           auto ret_lines = make_shared<set<line_no>>();
                set_intersection(left_result.begin(), left_result.end(),
                     right_result.begin(), right_result.end(), 
                          inserter(*ret_lines, ret_lines->begin()));
                            return QueryResult(rep(), ret_lines, left_result.get_file());
}

QueryResult OrQuery::eval(const TextQuery &text) const {
    QueryResult left_result = text.query(left_query);
       QueryResult right_result = text.query(right_query);
          auto ret_lines = make_shared<set<line_no>>(left_result.begin(), left_result.end());
             ret_lines->insert(right_result.begin(), right_result.end());
                return QueryResult(rep(), ret_lines, left_result.get_file());
}


QueryResult AdjacentQuery::eval (const TextQuery& text) const {
    QueryResult left_result = text.query(left_query);
       QueryResult right_result = text.query(right_query);
            auto ret_lines = make_shared<set<line_no>>();
   
    for(auto itL=left_result.begin(); itL!=left_result.end(); itL++) {
         for(auto itR=right_result.begin() ;itR!=right_result.end() ;itR++) {
            if((*itL-*itR==1)|| (*itR-*itL==1)) {
                 ret_lines->insert(*itL);
                    ret_lines->insert(*itR);
        }
     }
}
    return QueryResult(rep(), ret_lines, left_result.get_file());
}


std::ostream &print(std::ostream &os, const QueryResult &qr) {
int i=0, n2;
        os << "\"" << qr.sought << "\"" << " occurs " << 
            (qr.lines->size()+1)/2 << " times:" <<std::endl;
   
            for (auto num : *qr.lines) {
                if(qr.sought.substr(0,2)=="AD" && i!=0 && i%2==0)
                        os<<std::endl;
                if(qr.sought.substr(0,2)=="AD" && (i+1)%2==1 && n2+1==num)  
                    os << "\t(line " << num << ") " << *(qr.file->begin()+ num-1) << std::endl;
               
                os << "\t(line " << num + 1 << ") " <<
                    *(qr.file->begin() + num) << std::endl;
            i++;
            n2=num;
            }
            return os;
        }
