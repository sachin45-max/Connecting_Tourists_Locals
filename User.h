#ifndef USER_H
#define USER_H

 #include <iostream>
#include <string>
#include <cctype>
using namespace std;


 class User {

     protected:
     int id;

     string name, username, password, location;

     public:
     User(int i, const string& n, const string& u, const string& p, const string& l)
     : id(i), name(n), username(u), password(p), location(l) {
    }

     virtual ~User() {
    }

     virtual string getRole() const = 0;

     bool login(const string& u, const string& p) const {
         return username == u && password == p;
    }

     int getId() const {
         return id;
    }

     string getName() const {
         return name;
    }

     string getUsername() const {
         return username;
    }

     string getPasswordForStorage() const {
         return password;
    }

     string getLocation() const {
         return location;
    }


     static bool isValidName(const string& s) {

         if (s.empty()) return false;

         for (size_t i=0; i<s.size(); ++i)
         if (!(isalpha((unsigned char)s[i]) || s[i]==' ' || s[i]=='-' || s[i]=='.')) return false;

         return true;

    }

     static bool isValidUsername(const string& s) {

         if (s.size()<4) return false;

         for (size_t i=0; i<s.size(); ++i)
         if (!(isalnum((unsigned char)s[i]) || s[i]=='_' || s[i]=='.')) return false;

         return true;

    }

     static bool isValidPassword(const string& s) {

         if (s.size()<8) return false;

         bool upper=false, lower=false, digit=false, special=false;

         for (size_t i=0; i<s.size(); ++i) {

             unsigned char c=(unsigned char)s[i];

             upper |= isupper(c)!=0;
             lower |= islower(c)!=0;

             digit |= isdigit(c)!=0;
             special |= !(isalnum(c)!=0);

        }

         return upper && lower && digit && special;

    }

     static bool isValidLocation(const string& s) {
         return !s.empty();
    }

     virtual void displayProfile() const {

         cout << "\nName: " << name << "\nUsername: " << username
         << "\nLocation: " << location << "\nRole: " << getRole() << '\n';

    }

}
;


 class Tourist : public User {

     public:
     Tourist(int i, const string& n, const string& u, const string& p, const string& l):User(i, n, u, p, l){
    }

     string getRole() const {
         return "Tourist";
    }

}
;


 class Local : public User {

     private:
     string language, expertise, serviceLocation;

     bool guideEnabled;

     double guideRating;

     public:
     Local(int i, const string& n, const string& u, const string& p, const string& l, const string& lang="Hindi", bool enabled=true, const string& exp="", const string& serviceLoc="", double rating=0.0)
     :User(i, n, u, p, l), language(lang), expertise(exp), serviceLocation(serviceLoc),
     guideEnabled(enabled), guideRating(rating){
    }

     string getRole() const {
         return "Local";
    }

     bool isGuide() const {
         return true;
    }
     // Every Local is also a Guide.

     string getLanguage() const {
         return language;
    }

     string getExpertise() const {
         return expertise;
    }

     string getServiceLocation() const {
         return serviceLocation.empty()?location:serviceLocation;
    }

     double getGuideRating() const {
         return guideRating;
    }

     void setGuideRating(double r) {
         if(r>=0 && r<=5) guideRating=r;
    }

     void createOrUpdateGuideProfile() {

         string lang, exp, service;

         cout << "Language: ";
         getline(cin, lang);

         cout << "Expertise (e.g. Trekking, Sightseeing): ";
         getline(cin, exp);

         cout << "Service location: ";
         getline(cin, service);

         if(lang.empty() || exp.empty() || service.empty()) {
             cout << "All fields are required. Profile unchanged.\n";
             return;
        }

         language=lang;
         expertise=exp;
         serviceLocation=service;
         guideEnabled=true;

         cout << "Local/Guide profile updated.\n";

    }

     void displayProfile() const {

         User::displayProfile();

         cout << "Guide role: Local / Guide\nLanguage: " << language
         << "\nService location: " << getServiceLocation()
         << "\nExpertise: " << (expertise.empty()?"Not specified":expertise)
         << "\nGuide rating: " << guideRating << "/5\n";

    }

}
;

 #endif
