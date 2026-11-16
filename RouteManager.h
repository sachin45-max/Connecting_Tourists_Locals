#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H
#include "Place.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <queue>
#include <functional>
#include <utility>
using namespace std;

 class RouteManager{

     vector<Place*>& places;

     vector<pair<int, pair<int, double> > > edges;
     // source ID, (destination ID, distance)

     void loadRoutes(){
        edges.clear();
        ifstream f("routes.txt");
        string line;
        while(getline(f, line)){
            stringstream ss(line);
            string a, b, d;
            if(getline(ss, a, '|')&&getline(ss, b, '|')&&getline(ss, d)){
                try{
                    int x=stoi(a), y=stoi(b);
                    double w=stod(d);
                    if(x!=y&&w>0){
                        edges.push_back(make_pair(x, make_pair(y, w)));
                        edges.push_back(make_pair(y, make_pair(x, w)));
                    }
                }
                catch(...){
                    cout<<"Invalid route record skipped.\n";
                }
            }
        }
    }

     void saveRoutes() const {
        ofstream f("routes.txt");
        for(size_t i=0; i<edges.size(); ++i)if(edges[i].first<edges[i].second.first)f<<edges[i].first<<'|'<<edges[i].second.first<<'|'<<edges[i].second.second<<'\n';
    }

     Place* getPlace(int id) const {
        for(size_t i=0; i<places.size(); ++i)if(places[i]->getId()==id)return places[i];
        return NULL;
    }

     public:
     RouteManager(vector<Place*>& p):places(p){
        loadRoutes();
    }

     void addRoute(){
        if(places.size()<2){
            cout<<"At least two places are required to add a route.\n";
            return;
        }
        cout<<"Available places:\n";
        for(size_t i=0; i<places.size(); ++i)cout<<places[i]->getId()<<" - "<<places[i]->getName()<<" ("<<places[i]->getLocation()<<")\n";
        int a, b;
        double d;
        cout<<"Enter source place ID: ";
        if(!(cin>>a)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid ID.\n";
            return;
        }
        cout<<"Enter destination place ID: ";
        if(!(cin>>b)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid ID.\n";
            return;
        }
        cout<<"Enter distance (km): ";
        if(!(cin>>d)||d<=0){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Distance must be positive.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(a==b||!getPlace(a)||!getPlace(b)){
            cout<<"Choose two different valid place IDs.\n";
            return;
        }
        for(size_t i=0; i<edges.size(); ++i)if(edges[i].first==a&&edges[i].second.first==b){
            cout<<"Route already exists.\n";
            return;
        }
        edges.push_back(make_pair(a, make_pair(b, d)));
        edges.push_back(make_pair(b, make_pair(a, d)));
        saveRoutes();
        cout<<"Route added and saved.\n";
    }

     void displayGraph() const {
        if(edges.empty()){
            cout<<"No routes have been added yet.\n";
            return;
        }
        cout<<"--- Route Graph (undirected) ---\n";
        for(size_t i=0; i<edges.size(); ++i)if(edges[i].first<edges[i].second.first){
            Place*a=getPlace(edges[i].first);
            Place*b=getPlace(edges[i].second.first);
            cout<<(a?a->getName():"Unknown")<<" <-> "<<(b?b->getName():"Unknown")<<" : "<<edges[i].second.second<<" km\n";
        }
    }

     void shortestPath(){
        if(places.empty()){
            cout<<"No places available.\n";
            return;
        }
        cout<<"Available places:\n";
        for(size_t i=0; i<places.size(); ++i)cout<<places[i]->getId()<<" - "<<places[i]->getName()<<'\n';
        int source, target;
        cout<<"Source place ID: ";
        if(!(cin>>source)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid ID.\n";
            return;
        }
        cout<<"Destination place ID: ";
        if(!(cin>>target)){
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Invalid ID.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if(!getPlace(source)||!getPlace(target)){
            cout<<"Invalid place ID.\n";
            return;
        }
        vector<int> ids;
        for(size_t i=0; i<places.size(); ++i)ids.push_back(places[i]->getId());
        vector<double> dist(ids.size(), 1e18);
        vector<int> prev(ids.size(), -1);
        int si=-1, ti=-1;
        for(size_t i=0; i<ids.size(); ++i){
            if(ids[i]==source)si=(int)i;
            if(ids[i]==target)ti=(int)i;
        }
        typedef pair<double, int> Node;
        priority_queue<Node, vector<Node>, greater<Node> > pq;
        dist[si]=0;
        pq.push(make_pair(0.0, si));
        while(!pq.empty()){
            Node cur=pq.top();
            pq.pop();
            double cd=cur.first;
            int u=cur.second;
            if(cd!=dist[u])continue;
            for(size_t e=0; e<edges.size(); ++e)if(edges[e].first==ids[u]){
                int vi=-1;
                for(size_t k=0; k<ids.size(); ++k)if(ids[k]==edges[e].second.first)vi=(int)k;
                if(vi<0)continue;
                double nd=cd+edges[e].second.second;
                if(nd<dist[vi]){
                    dist[vi]=nd;
                    prev[vi]=u;
                    pq.push(make_pair(nd, vi));
                }
            }
        }
        if(dist[ti]>=1e18){
            cout<<"No route connects these places. Add connecting routes first.\n";
            return;
        }
        vector<int> path;
        for(int at=ti; at!=-1; at=prev[at])path.push_back(ids[at]);
        cout<<"Shortest distance: "<<dist[ti]<<" km\nPath: ";
        for(int i=(int)path.size()-1; i>=0; --i){
            Place*p=getPlace(path[i]);
            cout<<(p?p->getName():"Unknown");
            if(i)cout<<" -> ";
        }
        cout<<'\n';
    }

}
;

 #endif
