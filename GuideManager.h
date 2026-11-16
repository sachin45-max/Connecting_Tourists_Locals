#ifndef GUIDE_MANAGER_H
#define GUIDE_MANAGER_H
#include "User.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cctype>
using namespace std;

 struct GuideRequest{
    string tourist;
    int guideId;
    string guide, destination;
    int priority;
    string status;
}
;

 class GuideManager{

     vector<Local*> locals;

     vector<GuideRequest> requests;

     vector<pair<int, bool> > availability;

     static string lower(string s){
        for(size_t i=0; i<s.size(); ++i)s[i]=(char)tolower((unsigned char)s[i]);
        return s;
    }

     static bool containsIgnoreCase(const string&a, const string&b){
        return lower(a).find(lower(b))!=string::npos;
    }

     bool isAvailable(int id) const {
        for(size_t i=0; i<availability.size(); ++i)if(availability[i].first==id)return availability[i].second;
        return true;
    }

     void setAvailable(int id, bool value){
        bool found=false;
        for(size_t i=0; i<availability.size(); ++i)if(availability[i].first==id){
            availability[i].second=value;
            found=true;
        }
        if(!found)availability.push_back(make_pair(id, value));
        saveAvailability();
    }

     void saveAvailability() const {
        ofstream f("availability.txt");
        for(size_t i=0; i<availability.size(); ++i)f<<availability[i].first<<'|'<<(availability[i].second?1:0)<<'\n';
    }

     void saveRequests() const {
        ofstream f("requests.txt");
        for(size_t i=0; i<requests.size(); ++i)f<<requests[i].tourist<<'|'<<requests[i].guideId<<'|'<<requests[i].guide<<'|'<<requests[i].destination<<'|'<<requests[i].priority<<'|'<<requests[i].status<<'\n';
    }

     Local* findLocal(int id) const {
        for(size_t i=0; i<locals.size(); ++i)if(locals[i]->getId()==id)return locals[i];
        return NULL;
    }

     public:
     void loadAvailability(){
        availability.clear();
        ifstream f("availability.txt");
        string line;
        while(getline(f, line)){
            stringstream ss(line);
            string id, v;
            if(getline(ss, id, '|')&&getline(ss, v)){
                try{
                    availability.push_back(make_pair(stoi(id), v=="1"));
                }
                catch(...){
                }
            }
        }
    }

     void loadRequests(){

         requests.clear();

         ifstream f("requests.txt");

         string line;

         while(getline(f, line)){

             if(line.empty()) continue;

             vector<string> fields;

             string part;

             stringstream ss(line);

             while(getline(ss, part, '|')) fields.push_back(part);

             try {

                 if(fields.size() >= 6) {

                     requests.push_back(GuideRequest{
                        fields[0], stoi(fields[1]), fields[2], fields[3], stoi(fields[4]), fields[5]
                    }
                    );

                }
                 else if(fields.size() == 3) {

                     // Backward compatibility with original: tourist|guideName|destination

                     requests.push_back(GuideRequest{
                        fields[0], 0, fields[1], fields[2], 1, "Pending"
                    }
                    );

                }

            }
             catch(...) {
                 cout << "Invalid request record skipped.\n";
            }

        }

    }

     void addLocal(Local* l){
        if(!l)return;
        for(size_t i=0; i<locals.size(); ++i)if(locals[i]->getId()==l->getId())return;
        locals.push_back(l);
        for(size_t r=0; r<requests.size(); ++r)if(requests[r].guideId==0&&requests[r].guide==l->getName())requests[r].guideId=l->getId();
        bool exists=false;
        for(size_t i=0; i<availability.size(); ++i)if(availability[i].first==l->getId())exists=true;
        if(!exists)availability.push_back(make_pair(l->getId(), true));
        saveAvailability();
    }

     void displayGuides() const {
        bool any=false;
        for(size_t i=0; i<locals.size(); ++i){
            Local*l=locals[i];
            cout<<"\n--- Local / Guide ---\nID: "<<l->getId()<<"\nName: "<<l->getName()<<"\nLocation: "<<l->getServiceLocation()<<"\nLanguage: "<<l->getLanguage()<<"\nExpertise: "<<l->getExpertise()<<"\nRating: "<<l->getGuideRating()<<"/5\nAvailability: "<<(isAvailable(l->getId())?"Available":"Unavailable")<<'\n';
            any=true;
        }
        if(!any)cout<<"No registered Local/Guides found. Locals can update their guide profiles after registration.\n";
    }

     void sendRequest(const string& tourist){
        string dest;
        cout<<"Enter destination: ";
        getline(cin, dest);
        if(dest.empty()){
            cout<<"Destination cannot be empty.\n";
            return;
        }
        vector<Local*> matches;
        for(size_t i=0; i<locals.size(); ++i)if((containsIgnoreCase(locals[i]->getServiceLocation(), dest)||containsIgnoreCase(locals[i]->getLocation(), dest))&&isAvailable(locals[i]->getId()))matches.push_back(locals[i]);
        if(matches.empty()){
            cout<<"No available Local/Guide found for this destination.\n";
            return;
        }
        cout<<"Available Local/Guides:\n";
        for(size_t i=0; i<matches.size(); ++i)cout<<"ID: "<<matches[i]->getId()<<" | "<<matches[i]->getName()<<" | "<<matches[i]->getServiceLocation()<<" | "<<matches[i]->getExpertise()<<'\n';
        int id, priority;
        cout<<"Enter guide ID: ";
        if(!(cin>>id)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid ID.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        Local*l=NULL;
        for(size_t i=0; i<matches.size(); ++i)if(matches[i]->getId()==id)l=matches[i];
        if(!l){
            cout<<"Please select an ID from the list.\n";
            return;
        }
        cout<<"Priority (1=Normal, 2=High, 3=Urgent): ";
        if(!(cin>>priority)||priority<1||priority>3){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid priority.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        requests.push_back(GuideRequest{
            tourist, id, l->getName(), dest, priority, "Pending"
        }
        );
        saveRequests();
        cout<<"Guide request sent successfully.\n";
    }

     void displayRequestsFor(const string& guideName="") const {
        vector<GuideRequest> sorted=requests;
        stable_sort(sorted.begin(), sorted.end(), [](const GuideRequest&a, const GuideRequest&b){
            return a.priority>b.priority;
        }
        );
        bool any=false;
        for(size_t i=0; i<sorted.size(); ++i)if(guideName.empty()||sorted[i].guide==guideName){
            cout<<"\nTourist: "<<sorted[i].tourist<<"\nGuide: "<<sorted[i].guide<<"\nDestination: "<<sorted[i].destination<<"\nPriority: "<<sorted[i].priority<<"\nStatus: "<<sorted[i].status<<'\n';
            any=true;
        }
        if(!any)cout<<"No requests found.\n";
    }

     void displayRequests() const {
        displayRequestsFor();
    }

     void manageRequests(int guideId){

         vector<size_t> pending;

         for(size_t i=0; i<requests.size(); ++i)
         if(requests[i].guideId==guideId && requests[i].status=="Pending") pending.push_back(i);

         stable_sort(pending.begin(), pending.end(), [this](size_t a, size_t b){
            return requests[a].priority>requests[b].priority;
        }
        );

         if(pending.empty()){
            cout<<"No pending requests.\n";
            return;
        }

         cout<<"--- Pending Requests (highest priority first) ---\n";

         for(size_t i=0; i<pending.size(); ++i){
            const GuideRequest&r=requests[pending[i]];
            cout<<i+1<<". Tourist: "<<r.tourist<<" | Destination: "<<r.destination<<" | Priority: "<<r.priority<<"\n";
        }

         int n;
        cout<<"Select request number (0=cancel): ";

         if(!(cin>>n)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid choice.\n";
            return;
        }

         cin.ignore(numeric_limits<streamsize>::max(), '\n');

         if(n==0)return;

         if(n<1||n>(int)pending.size()){
            cout<<"Invalid request number.\n";
            return;
        }

         int action;
        cout<<"1. Accept  2. Reject: ";

         if(!(cin>>action)||(action!=1&&action!=2)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid action.\n";
            return;
        }

         cin.ignore(numeric_limits<streamsize>::max(), '\n');

         GuideRequest&r=requests[pending[n-1]];
        r.status=(action==1?"Accepted":"Rejected");

         if(action==1 && r.guideId>0)setAvailable(r.guideId, false);

         saveRequests();
        cout<<"Request "<<r.status<<".\n";

    }

     void updateAvailability(int guideId){
        Local*l=findLocal(guideId);
        if(!l){
            cout<<"Guide profile not found.\n";
            return;
        }
        cout<<"Current availability: "<<(isAvailable(l->getId())?"Available":"Unavailable")<<"\n1. Available\n2. Unavailable\nChoice: ";
        int c;
        if(!(cin>>c)||(c!=1&&c!=2)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid choice.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        setAvailable(l->getId(), c==1);
        cout<<"Availability updated.\n";
    }

     void requestHistory(int guideId) const {
        cout<<"--- Request History ---\n";
        bool any=false;
        for(size_t i=0; i<requests.size(); ++i)if(requests[i].guideId==guideId){
            cout<<"Tourist: "<<requests[i].tourist<<" | Destination: "<<requests[i].destination<<" | Priority: "<<requests[i].priority<<" | Status: "<<requests[i].status<<'\n';
            any=true;
        }
        if(!any)cout<<"No request history.\n";
    }

}
;

 #endif
