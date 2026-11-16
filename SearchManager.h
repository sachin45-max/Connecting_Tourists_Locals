#ifndef SEARCH_MANAGER_H
#define SEARCH_MANAGER_H
#include "Place.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <cctype>
using namespace std;

 class SearchManager {

     vector<Place*>& places;

     static string lower(string s){
        for(size_t i=0; i<s.size(); ++i)s[i]=(char)tolower((unsigned char)s[i]);
        return s;
    }

     void showMatches(const vector<Place*>& found) const {
        if(found.empty()){
            cout<<"No matching places found.\n";
            return;
        }
        for(size_t i=0; i<found.size(); ++i)found[i]->display();
    }

     public:
     SearchManager(vector<Place*>& p):places(p){
    }

     void displayPlaces() const {
        if(places.empty()){
            cout<<"No places available.\n";
            return;
        }
        for(size_t i=0; i<places.size(); ++i)places[i]->display();
    }

     void searchByName(){
        string q;
        cout<<"Enter place name or location: ";
        getline(cin, q);
        if(q.empty()){
            cout<<"Search text cannot be empty.\n";
            return;
        }
        vector<Place*> f;
        string key=lower(q);
        for(size_t i=0; i<places.size(); ++i)if(lower(places[i]->getName()).find(key)!=string::npos||lower(places[i]->getLocation()).find(key)!=string::npos)f.push_back(places[i]);
        showMatches(f);
    }

     void searchByCategory(){
        string q;
        cout<<"Enter category (Tourist Place / Hotel / Food Place): ";
        getline(cin, q);
        if(q.empty()){
            cout<<"Category cannot be empty.\n";
            return;
        }
        vector<Place*> f;
        string key=lower(q);
        for(size_t i=0; i<places.size(); ++i)if(lower(places[i]->getCategory()).find(key)!=string::npos)f.push_back(places[i]);
        showMatches(f);
    }

     void sortByRating(){
        sort(places.begin(), places.end(), [](const Place*a, const Place*b){
            return a->getRating()>b->getRating();
        }
        );
        displayPlaces();
    }

     void sortByPrice(){
        sort(places.begin(), places.end(), [](const Place*a, const Place*b){
            return a->getPrice()<b->getPrice();
        }
        );
        displayPlaces();
    }

     void buildHashTable(unordered_map<string, vector<Place*> >& table) const {
        table.clear();
        for(size_t i=0; i<places.size(); ++i)table[lower(places[i]->getName())].push_back(places[i]);
    }

     void searchByHashTable() const {
        unordered_map<string, vector<Place*> > table;
        buildHashTable(table);
        string q;
        cout<<"Enter exact place name for hash-table lookup: ";
        getline(cin, q);
        unordered_map<string, vector<Place*> >::const_iterator it=table.find(lower(q));
        if(it==table.end())cout<<"Place not found.\n";
        else showMatches(it->second);
    }

}
;

 #endif
