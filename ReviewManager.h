#ifndef REVIEW_MANAGER_H
#define REVIEW_MANAGER_H

#include "Place.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

struct Review {
    int placeId;
    string tourist, comment;
    double rating;
};

class ReviewManager {
    list<Review> reviews;

    static string lower(string s) {
        for (size_t i = 0; i < s.size(); ++i) {
            s[i] = (char)tolower((unsigned char)s[i]);
        }
        return s;
    }

    void saveReviews() const {
        ofstream f("reviews.txt");
        for (list<Review>::const_iterator i = reviews.begin(); i != reviews.end(); ++i) {
            f << i->placeId << '|' << i->tourist << '|' << i->rating << '|' << i->comment << '\n';
        }
    }

    void updateRating(int id, const vector<Place*>& places) {
        double sum = 0;
        int n = 0;
        for (list<Review>::const_iterator i = reviews.begin(); i != reviews.end(); ++i) {
            if (i->placeId == id) {
                sum += i->rating;
                ++n;
            }
        }
        if (n) {
            for (size_t j = 0; j < places.size(); ++j) {
                if (places[j]->getId() == id) {
                    places[j]->setRating(sum / n);
                    break;
                }
            }
        }
    }

    void savePlaceRatings(const vector<Place*>& places) const {
        ifstream in("places.txt");
        if (!in) return;

        vector<string> lines;
        string line;
        while (getline(in, line)) {
            lines.push_back(line);
        }
        in.close();

        ofstream out("places.txt");
        for (size_t i = 0; i < lines.size(); ++i) {
            stringstream ss(lines[i]);
            string id, name, loc, cat, rating, price;
            if (getline(ss, id, '|') && getline(ss, name, '|') && getline(ss, loc, '|') && getline(ss, cat, '|') && getline(ss, rating, '|') && getline(ss, price)) {
                try {
                    int pid = stoi(id);
                    for (size_t j = 0; j < places.size(); ++j) {
                        if (places[j]->getId() == pid) {
                            ostringstream v;
                            v << places[j]->getRating();
                            rating = v.str();
                            break;
                        }
                    }
                    out << id << '|' << name << '|' << loc << '|' << cat << '|' << rating << '|' << price << '\n';
                } catch (...) {
                    out << lines[i] << '\n';
                }
            } else {
                out << lines[i] << '\n';
            }
        }
    }

public:
    void loadReviews(const vector<Place*>& places) {
        reviews.clear();
        ifstream f("reviews.txt");
        string line;
        while (getline(f, line)) {
            stringstream ss(line);
            string id, tourist, rating, comment;
            if (getline(ss, id, '|') && getline(ss, tourist, '|') && getline(ss, rating, '|') && getline(ss, comment)) {
                try {
                    int pid = stoi(id);
                    double r = stod(rating);
                    bool exists = false;
                    for (size_t j = 0; j < places.size(); ++j) {
                        if (places[j]->getId() == pid) {
                            exists = true;
                        }
                    }
                    if (pid > 0 && !tourist.empty() && !comment.empty() && r >= 1 && r <= 5 && exists) {
                        bool dup = false;
                        for (list<Review>::const_iterator k = reviews.begin(); k != reviews.end(); ++k) {
                            if (k->placeId == pid && lower(k->tourist) == lower(tourist)) {
                                dup = true;
                            }
                        }
                        if (!dup) {
                            reviews.push_back(Review{pid, tourist, comment, r});
                        }
                    }
                } catch (...) {
                    cout << "Invalid review record skipped.\n";
                }
            }
        }
        for (size_t i = 0; i < places.size(); ++i) {
            updateRating(places[i]->getId(), places);
        }
        savePlaceRatings(places);
    }

    void addReview(const string& tourist, const vector<Place*>& places) {
        int id;
        double rating;
        string comment;

        cout << "Enter place ID: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid ID.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool found = false;
        for (size_t i = 0; i < places.size(); ++i) {
            if (places[i]->getId() == id) {
                found = true;
            }
        }
        if (!found) {
            cout << "Place not found.\n";
            return;
        }

        for (list<Review>::const_iterator i = reviews.begin(); i != reviews.end(); ++i) {
            if (i->placeId == id && lower(i->tourist) == lower(tourist)) {
                cout << "You have already reviewed this place.\n";
                return;
            }
        }

        cout << "Enter rating (1-5): ";
        if (!(cin >> rating) || rating < 1 || rating > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid rating.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter comment: ";
        getline(cin, comment);
        if (comment.empty()) {
            cout << "Review cannot be empty.\n";
            return;
        }

        reviews.push_back(Review{id, tourist, comment, rating});
        saveReviews();
        updateRating(id, places);
        savePlaceRatings(places);

        cout << "Review added. Updated average rating: ";
        for (size_t i = 0; i < places.size(); ++i) {
            if (places[i]->getId() == id) {
                cout << places[i]->getRating();
            }
        }
        cout << '\n';
    }

    void displayReviews() const {
        int id;
        cout << "Enter place ID: ";
        if (!(cin >> id)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid ID.\n";
            return;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool found = false;
        for (list<Review>::const_iterator i = reviews.begin(); i != reviews.end(); ++i) {
            if (i->placeId == id) {
                cout << "\nTourist: " << i->tourist
                     << "\nRating: " << i->rating
                     << "\nComment: " << i->comment << '\n';
                found = true;
            }
        }
        if (!found) {
            cout << "No reviews found.\n";
        }
    }
};

#endif
