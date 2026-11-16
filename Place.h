#ifndef PLACE_H
#define PLACE_H
#include <iostream>
#include <string>
using namespace std;

 class Place {

     protected:
     int id;
     string name, location, category;
     double rating, price;

     public:
     Place(int i, const string& n, const string& l, const string& c, double r, double p)
     :id(i), name(n), location(l), category(c), rating(r), price(p){
    }

     virtual ~Place(){
    }

     int getId() const{
        return id;
    }
     string getName() const{
        return name;
    }

     string getLocation() const{
        return location;
    }
     string getCategory() const{
        return category;
    }

     double getRating() const{
        return rating;
    }
     double getPrice() const{
        return price;
    }

     void setRating(double r){
        if(r>=0 && r<=5) rating=r;
    }

     virtual void display() const {

         cout << "\nID: " <<id<<"\nName: "<<name<<"\nLocation: "<<location
         <<"\nCategory: "<<category<<"\nRating: "<<rating<<"/5\nPrice/Budget: Rs. "<<price<<'\n';

    }

}
;

 class TouristPlace:public Place{
    public: TouristPlace(int i, const string&n, const string&l, const string&c, double r, double p):Place(i, n, l, c, r, p){
    }
     void display()const{
        cout<<"\n--- Tourist Place ---";
        Place::display();
    }
}
;

 class Hotel:public Place{
    public: Hotel(int i, const string&n, const string&l, const string&c, double r, double p):Place(i, n, l, c, r, p){
    }
     void display()const{
        cout<<"\n--- Hotel ---";
        Place::display();
    }
}
;

 class FoodPlace:public Place{
    public: FoodPlace(int i, const string&n, const string&l, const string&c, double r, double p):Place(i, n, l, c, r, p){
    }
     void display()const{
        cout<<"\n--- Food Place ---";
        Place::display();
    }
}
;

 #endif
