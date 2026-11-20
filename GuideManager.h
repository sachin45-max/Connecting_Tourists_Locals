#ifndef GUIDE_MANAGER_H
#define GUIDE_MANAGER_H
#include "User.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <limits>
using namespace std;

struct GuideRequest { int id; string tourist, guide, destination, status; int priority; };
class GuideManager {
    vector<Local*> locals;
    queue<GuideRequest> requests; // FIFO pending requests
    stack<string> history;        // most recent processed request first
    int nextRequestId=1;
    void saveRequests() const {
        ofstream out("requests.txt");
        queue<GuideRequest> copy=requests;
        while(!copy.empty()){auto r=copy.front();copy.pop();out<<r.id<<'|'<<r.tourist<<'|'<<r.guide<<'|'<<r.destination<<'|'<<r.status<<'|'<<r.priority<<'\n';}
    }
public:
    void addLocal(Local* local) {
        if(!local)return;
        for(auto* x:locals)if(x->getId()==local->getId())return;
        locals.push_back(local);
    }
    void loadRequests() {
        ifstream f("requests.txt"); string line;
        while(getline(f,line)){stringstream ss(line);string a,b,c,d,e,g;
            if(getline(ss,a,'|')&&getline(ss,b,'|')&&getline(ss,c,'|')){
                int id=nextRequestId++; string destination, status="Pending"; int priority=0;
                // Legacy format: tourist|guide|destination
                if(getline(ss,d,'|')) { try{id=stoi(a);}catch(...){id=nextRequestId++; d=c; c=b; b=a;}
                    destination=d; if(getline(ss,e,'|'))status=e; if(getline(ss,g))try{priority=stoi(g);}catch(...){}
                } else destination=c;
                if(status=="Pending")requests.push({id,b,c,destination,status,priority});
                else history.push("Request "+to_string(id)+" "+status+": "+b+" -> "+c);
                nextRequestId=max(nextRequestId,id+1);
            }
        }
        ifstream hf("requests_history.txt"); string hline;
        while(getline(hf,hline)) {
            stringstream hs(hline); string rid,tourist,guide,dest,status;
            if(getline(hs,rid,'|') && getline(hs,tourist,'|') &&
               getline(hs,guide,'|') && getline(hs,dest,'|') && getline(hs,status))
                history.push("Request "+rid+" "+status+": "+tourist+" -> "+guide+" ("+dest+")");
        }
    }
    void displayRequests() const {
        if(requests.empty()){cout<<"\nNo pending guide requests.\n";return;}
        queue<GuideRequest> q=requests;
        while(!q.empty()){auto r=q.front();q.pop();cout<<"\nRequest ID: "<<r.id<<"\nTourist: "<<r.tourist<<"\nGuide: "<<r.guide<<"\nDestination: "<<r.destination<<"\nStatus: "<<r.status<<"\nPriority: "<<r.priority<<'\n';}
    }
    void displayGuides() const {if(locals.empty()){cout<<"No Local/Guides registered yet.\n";return;}for(auto* l:locals)displayOne(l);}
    void displayGuidesForDestination(const string& d) const {
        bool found=false;for(auto* l:locals)if(l->getServiceLocation()==d||l->getLocation()==d){displayOne(l);found=true;}
        if(!found)cout<<"No Local/Guides available for "<<d<<".\n";
    }
    void sendRequest(const string& tourist) {
        string dest;cout<<"Enter destination: ";getline(cin,dest);
        if(dest.empty()){cout<<"Destination cannot be empty.\n";return;}
        displayGuidesForDestination(dest);bool found=false;
        for(auto* l:locals)if(l->getServiceLocation()==dest||l->getLocation()==dest)found=true;
        if(!found)return;
        int id;cout<<"Enter Local/Guide ID: ";
        if(!(cin>>id)){cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');cout<<"Invalid ID.\n";return;}
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        string guide;for(auto* l:locals)if(l->getId()==id&&(l->getServiceLocation()==dest||l->getLocation()==dest)){guide=l->getName();break;}
        if(guide.empty()){cout<<"Guide not found for destination.\n";return;}
        GuideRequest r{nextRequestId++,tourist,guide,dest,"Pending",0};requests.push(r);saveRequests();
        cout<<"Request sent. Request ID: "<<r.id<<" (Pending, FIFO order).\n";
    }
    void processNextRequest() {
        if(requests.empty()){cout<<"No pending requests.\n";return;}
        auto r=requests.front();cout<<"Request "<<r.id<<" from "<<r.tourist<<" for "<<r.guide<<" at "<<r.destination<<"\n1. Accept  2. Reject: ";
        int c;if(!(cin>>c)){cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');cout<<"Invalid choice.\n";return;}cin.ignore(numeric_limits<streamsize>::max(),'\n');
        if(c!=1&&c!=2){cout<<"Invalid choice.\n";return;}
        requests.pop();r.status=(c==1?"Accepted":"Rejected");history.push("Request "+to_string(r.id)+" "+r.status+": "+r.tourist+" -> "+r.guide);
        // Persist processed status along with still-pending queue.
        ofstream out("requests_history.txt",ios::app);out<<r.id<<'|'<<r.tourist<<'|'<<r.guide<<'|'<<r.destination<<'|'<<r.status<<'\n';
        saveRequests();cout<<"Request "<<r.status<<".\n";
    }
    void displayHistory() const {if(history.empty()){cout<<"No request history.\n";return;}stack<string>s=history;while(!s.empty()){cout<<s.top()<<'\n';s.pop();}}
    // Priority queue example for urgent requests; higher priority is served first.
    void demonstratePriorityQueue() const {
        priority_queue<pair<int,string>> pq;pq.push({1,"Normal request"});pq.push({3,"Emergency request"});pq.push({2,"Urgent request"});
        cout<<"Priority order: ";while(!pq.empty()){cout<<pq.top().second<<" ";pq.pop();}cout<<'\n';
    }
private:
    static void displayOne(const Local* l){cout<<"\n--- Local / Guide ---\nID: "<<l->getId()<<"\nName: "<<l->getName()<<"\nLocation: "<<l->getServiceLocation()<<"\nLanguage: "<<l->getLanguage()<<"\nExpertise: "<<(l->getExpertise().empty()?"Not specified":l->getExpertise())<<"\nRating: "<<l->getGuideRating()<<"/5\n";}
};
#endif
