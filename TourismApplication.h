#ifndef TOURISM_APPLICATION_H
#define TOURISM_APPLICATION_H
#include "User.h"
#include "Place.h"
#include "GuideManager.h"
#include "ReviewManager.h"
#include "SearchManager.h"
#include "RouteManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
using namespace std;

 class TourismApplication{

     vector<User*> users;
     vector<Place*> places;

     GuideManager guideManager;
     ReviewManager reviewManager;

     SearchManager searchManager;
     RouteManager routeManager;

     int nextUserId;

     void loadUsers(){
        ifstream f("users.txt");
        string line;
        while(getline(f, line)){
            if(line.empty())continue;
            stringstream ss(line);
            string id, role, name, username, password, location, language="Hindi", enabled="1", expertise, service, rating="0";
            if(!(getline(ss, id, '|')&&getline(ss, role, '|')&&getline(ss, name, '|')&&getline(ss, username, '|')&&getline(ss, password, '|')&&getline(ss, location, '|')))continue;
            getline(ss, language, '|');
            getline(ss, enabled, '|');
            getline(ss, expertise, '|');
            getline(ss, service, '|');
            getline(ss, rating);
            try{
                int uid=stoi(id);
                User*u=NULL;
                if(role=="1")u=new Tourist(uid, name, username, password, location);
                else if(role=="2")u=new Local(uid, name, username, password, location, language.empty()?"Hindi":language, enabled!="0", expertise, service, rating.empty()?0.0:stod(rating));
                if(u){
                    users.push_back(u);
                    nextUserId=max(nextUserId, uid+1);
                }
            }
            catch(...){
                cout<<"Invalid user record skipped.\n";
            }
        }
    }

     void loadPlaces(){
        ifstream f("places.txt");
        string line;
        while(getline(f, line)){
            stringstream ss(line);
            string id, name, loc, cat, rating, price;
            if(!(getline(ss, id, '|')&&getline(ss, name, '|')&&getline(ss, loc, '|')&&getline(ss, cat, '|')&&getline(ss, rating, '|')&&getline(ss, price)))continue;
            try{
                int i=stoi(id);
                double r=stod(rating), p=stod(price);
                if(i<=0||r<0||r>5||p<0||name.empty())continue;
                if(cat=="Hotel")places.push_back(new Hotel(i, name, loc, cat, r, p));
                else if(cat=="Food Place")places.push_back(new FoodPlace(i, name, loc, cat, r, p));
                else places.push_back(new TouristPlace(i, name, loc, cat, r, p));
            }
            catch(...){
                cout<<"Invalid place record skipped.\n";
            }
        }
    }

     void saveUsers(){
        ofstream f("users.txt");
        if(!f){
            cout<<"Unable to save users.\n";
            return;
        }
        for(size_t i=0; i<users.size(); ++i){
            Local*l=dynamic_cast<Local*>(users[i]);
            f<<users[i]->getId()<<'|'<<(users[i]->getRole()=="Tourist"?1:2)<<'|'<<users[i]->getName()<<'|'<<users[i]->getUsername()<<'|'<<users[i]->getPasswordForStorage()<<'|'<<users[i]->getLocation();
            if(l)f<<'|'<<l->getLanguage()<<"|1|"<<l->getExpertise()<<'|'<<l->getServiceLocation()<<'|'<<l->getGuideRating();
            else f<<"|Hindi|0|||0";
            f<<'\n';
        }
    }

     void registerUser(){
        int role;
        string name, username, password, location;
        cout<<"\n1. Tourist\n2. Local (also a Guide)\nChoice: ";
        if(!(cin>>role)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid choice.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(role!=1&&role!=2){
            cout<<"Invalid role.\n";
            return;
        }
        cout<<"Name: ";
        getline(cin, name);
        if(!User::isValidName(name)){
            cout<<"Use letters, spaces, hyphens or dots for name.\n";
            return;
        }
        cout<<"Username (4+ letters/numbers/_/.): ";
        getline(cin, username);
        if(!User::isValidUsername(username)){
            cout<<"Invalid username.\n";
            return;
        }
        for(size_t i=0; i<users.size(); ++i)if(users[i]->getUsername()==username){
            cout<<"Username already exists.\n";
            return;
        }
        cout<<"Password (8+ chars, upper, lower, digit, special): ";
        getline(cin, password);
        if(!User::isValidPassword(password)){
            cout<<"Password does not meet requirements.\n";
            return;
        }
        cout<<"Location: ";
        getline(cin, location);
        if(!User::isValidLocation(location)){
            cout<<"Location cannot be empty.\n";
            return;
        }
        User*u=NULL;
        if(role==1)u=new Tourist(nextUserId, name, username, password, location);
        else u=new Local(nextUserId, name, username, password, location, "Hindi");
        users.push_back(u);
        if(role==2)guideManager.addLocal(static_cast<Local*>(u));
        saveUsers();
        ++nextUserId;
        cout<<"Registration successful.\n";
        if(role==2)cout<<"You are registered as both Local and Guide. Update your guide profile from your dashboard.\n";
    }

     User* loginUser(){
        string u, p;
        cout<<"Username: ";
        getline(cin, u);
        cout<<"Password: ";
        getline(cin, p);
        for(size_t i=0; i<users.size(); ++i)if(users[i]->login(u, p)){
            cout<<"Welcome, "<<users[i]->getName()<<"!\n";
            return users[i];
        }
        cout<<"Invalid username or password.\n";
        return NULL;
    }

     void touristDashboard(User*t){
        int c;
        do{
            cout<<"\n===== TOURIST DASHBOARD =====\n1. Explore places\n2. Search by name/location\n3. Sort by rating\n4. View Local/Guides\n5. Send guide request\n6. Add review\n7. View reviews\n8. View profile\n9. Search by category\n10. Sort by price\n11. Hash-table place search\n12. Add route\n13. Display routes\n14. Find shortest route (Dijkstra)\n0. Logout\nChoice: ";
            if(!(cin>>c)){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout<<"Invalid choice.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch(c){
                case 1:searchManager.displayPlaces();
                break;
                case 2:searchManager.searchByName();
                break;
                case 3:searchManager.sortByRating();
                break;
                case 4:guideManager.displayGuides();
                break;
                case 5:guideManager.sendRequest(t->getName());
                break;
                case 6:reviewManager.addReview(t->getName(), places);
                break;
                case 7:reviewManager.displayReviews();
                break;
                case 8:t->displayProfile();
                break;
                case 9:searchManager.searchByCategory();
                break;
                case 10:searchManager.sortByPrice();
                break;
                case 11:searchManager.searchByHashTable();
                break;
                case 12:routeManager.addRoute();
                break;
                case 13:routeManager.displayGraph();
                break;
                case 14:routeManager.shortestPath();
                break;
                case 0:cout<<"Logged out.\n";
                break;
                default:cout<<"Invalid choice.\n";
            }
        }
        while(c!=0);
    }

     void localDashboard(Local*l){
        int c;
        do{
            cout<<"\n===== LOCAL / GUIDE DASHBOARD =====\n1. View profile\n2. View places\n3. Update guide profile\n4. View incoming requests\n5. Accept/reject request\n6. Update availability\n7. Request history\n0. Logout\nChoice: ";
            if(!(cin>>c)){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout<<"Invalid choice.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch(c){
                case 1:l->displayProfile();
                break;
                case 2:searchManager.displayPlaces();
                break;
                case 3:l->createOrUpdateGuideProfile();
                saveUsers();
                break;
                case 4:guideManager.displayRequests();
                break;
                case 5:guideManager.manageRequests(l->getId());
                break;
                case 6:guideManager.updateAvailability(l->getId());
                break;
                case 7:guideManager.requestHistory(l->getId());
                break;
                case 0:cout<<"Logged out.\n";
                break;
                default:cout<<"Invalid choice.\n";
            }
        }
        while(c!=0);
    }

     public:
     TourismApplication():searchManager(places), routeManager(places), nextUserId(1){
        loadUsers();
        loadPlaces();
        guideManager.loadAvailability();
        guideManager.loadRequests();
        for(size_t i=0; i<users.size(); ++i){
            Local*l=dynamic_cast<Local*>(users[i]);
            if(l)guideManager.addLocal(l);
        }
        reviewManager.loadReviews(places);
    }

     ~TourismApplication(){
        for(size_t i=0; i<users.size(); ++i)delete users[i];
        for(size_t i=0; i<places.size(); ++i)delete places[i];
    }

     void run(){
        int c;
        do{
            cout<<"\n===== CONNECTING TOURISTS & LOCALS =====\n1. Register\n2. Login\n0. Exit\nChoice: ";
            if(!(cin>>c)){
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout<<"Invalid choice.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            switch(c){
                case 1:registerUser();
                break;
                case 2:{
                    User*u=loginUser();
                    if(u){
                        if(u->getRole()=="Tourist")touristDashboard(u);
                        else{
                            Local*l=dynamic_cast<Local*>(u);
                            if(l)localDashboard(l);
                        }
                    }
                    break;
                }
                case 0:cout<<"Thank you for using Connecting Tourists & Locals.\n";
                break;
                default:cout<<"Invalid choice.\n";
            }
        }
        while(c!=0);
    }

}
;

 #endif
