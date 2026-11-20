#ifndef TOURISM_APPLICATION_H
#define TOURISM_APPLICATION_H

#include "User.h"
#include "Place.h"
#include "GuideManager.h"
#include "ReviewManager.h"
#include "SearchManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
using namespace std;
class TourismApplication {
private:
    vector<User*> users;
    vector<Place*> places;

    GuideManager guideManager;
    ReviewManager reviewManager;
    SearchManager searchManager;

    int nextUserId = 1;

    void loadUsers() {
        ifstream file("users.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string id, role, name, username, password, location;
            string language = "Hindi", enabledText = "0";
            string expertise, serviceLocation, guideRatingText = "0";

            if (getline(ss, id, '|') &&
                getline(ss, role, '|') &&
                getline(ss, name, '|') &&
                getline(ss, username, '|') &&
                getline(ss, password, '|') &&
                getline(ss, location, '|')) {

                getline(ss, language, '|');
                getline(ss, enabledText, '|');
                getline(ss, expertise, '|');
                getline(ss, serviceLocation, '|');
                getline(ss, guideRatingText);

                try {
                    int userId = stoi(id);
                    User* u = nullptr;

                    if (role == "1") {
                        u = new Tourist(
                            userId, name, username, password, location
                        );
                    } else if (role == "2") {
                        u = new Local(
                            userId, name, username, password, location,
                            language.empty() ? "Hindi" : language,
                            enabledText == "1", expertise, serviceLocation,
                            guideRatingText.empty() ? 0.0 : stod(guideRatingText)
                        );
                    }

                    if (u != nullptr) {
                        users.push_back(u);
                        nextUserId = max(nextUserId, userId + 1);
                    }
                } catch (...) {
                    cout << "Invalid user record skipped.\n";
                }
            }
        }
    }

    void loadPlaces() {
        ifstream file("places.txt");
        string line;

        while (getline(file, line)) {
            stringstream ss(line);

            string id, name, location, category, rating, price;

            if (getline(ss, id, '|') &&
                getline(ss, name, '|') &&
                getline(ss, location, '|') &&
                getline(ss, category, '|') &&
                getline(ss, rating, '|') &&
                getline(ss, price)) {

                try {
                    int i = stoi(id);
                    double r = stod(rating);
                    double p = stod(price);

                    if (category == "Hotel") {
                        places.push_back(
                            new Hotel(i, name, location, category, r, p)
                        );
                    } else if (category == "Food Place") {
                        places.push_back(
                            new FoodPlace(i, name, location, category, r, p)
                        );
                    } else {
                        places.push_back(
                            new TouristPlace(i, name, location, category, r, p)
                        );
                    }
                } catch (...) {
                    cout << "Invalid place record skipped.\n";
                }
            }
        }
    }

    void registerUser() {
        int role;
        string name, username, password, location;

        cout << "\n1. Tourist\n2. Local\nChoice: ";

        if (!(cin >> role)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid role.\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (role != 1 && role != 2) {
            cout << "Invalid role.\n";
            return;
        }

        cout << "Name: ";
        getline(cin, name);

        cout << "Username: ";
        getline(cin, username);

        for (const auto* u : users) {
            if (u->getUsername() == username) {
                cout << "Username already exists.\n";
                return;
            }
        }

        cout << "Password: ";
        getline(cin, password);

        cout << "Location: ";
        getline(cin, location);

        User* u = nullptr;

        if (role == 1) {
            u = new Tourist(
                nextUserId, name, username, password, location
            );
        } else {
            u = new Local(
                nextUserId, name, username, password,
                location, "Hindi"
            );
        }

        users.push_back(u);
        if (role == 2) guideManager.addLocal(static_cast<Local*>(u));

        ofstream file("users.txt", ios::app);

        if (!file) {
            users.pop_back();
            delete u;
            cout << "Unable to save user.\n";
            return;
        }

        file << nextUserId << '|'
             << role << '|'
             << name << '|'
             << username << '|'
             << password << '|'
             << location << '\n';

        nextUserId++;

        cout << "Registration successful!\n";
    }

    void saveUserData() {
        ofstream file("users.txt");
        if (!file) {
            cout << "Unable to save user data.\\n";
            return;
        }

        for (const auto* user : users) {
            const Local* local = dynamic_cast<const Local*>(user);
            file << user->getId() << '|'
                 << (user->getRole() == "Tourist" ? 1 : 2) << '|'
                 << user->getName() << '|'
                 << user->getUsername() << '|'
                 << user->getPasswordForStorage() << '|'
                 << user->getLocation();

            if (local != nullptr) {
                file << '|' << local->getLanguage()
                     << '|' << (local->isGuide() ? 1 : 0)
                     << '|' << local->getExpertise()
                     << '|' << local->getServiceLocation()
                     << '|' << local->getGuideRating();
            } else {
                file << "|Hindi|0|||0";
            }
            file << '\n';
        }
    }

    User* loginUser() {
        string username, password;

        cout << "\nUsername: ";
        getline(cin, username);

        cout << "Password: ";
        getline(cin, password);

        for (auto* u : users) {
            if (u->login(username, password)) {
                cout << "\nLogin successful. Welcome, "
                     << u->getName() << "!\n";
                return u;
            }
        }

        cout << "Invalid username or password.\n";
        return nullptr;
    }

    void touristDashboard(User* tourist) {
        int choice;

        do {
            cout << "\n========== TOURIST DASHBOARD ==========\n"
                 << "1. Explore Places\n"
                 << "2. Search Places\n"
                 << "3. Sort Places by Rating\n"
                 << "4. View Local Guides\n"
                 << "5. Send Guide Request\n"
                 << "6. Add Review\n"
                 << "7. View Reviews\n"
                 << "8. View Profile\n"
                 << "9. Search by Category\n"
                 << "10. Sort by Price\n"
                 << "11. Exact Search (Hash Table)\n"
                 << "12. Shortest Route (Dijkstra)\n"
                 << "13. BFS/DFS Route Traversal\n"
                 << "0. Logout\n"
                 << "Choice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice.\n";
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    searchManager.displayPlaces();
                    break;

                case 2:
                    searchManager.searchByName();
                    break;

                case 3:
                    searchManager.sortByRating();
                    break;

                case 4:
                    guideManager.displayGuides();
                    break;

                case 5:
                    guideManager.sendRequest(tourist->getName());
                    break;

                case 6:
                    reviewManager.addReview(tourist->getName(), places);
                    for (auto* place : places)
                        place->setRating(reviewManager.averageRating(place->getId(), place->getRating()));
                    break;

                case 7:
                    reviewManager.displayReviews();
                    break;

                case 8:
                    tourist->displayProfile();
                    break;
                case 9: searchManager.searchByCategory(); break;
                case 10: searchManager.sortByPrice(); break;
                case 11: searchManager.showHashLookup(); break;
                case 12: searchManager.routeMenu(); break;
                case 13: searchManager.graphTraversal(); break;

                case 0:
                    cout << "Logged out successfully.\n";
                    break;

                default:
                    cout << "Invalid choice.\n";
            }

        } while (choice != 0);
    }

    void localDashboard(Local* local) {
    int choice;

    do {
        cout << "\n========== LOCAL DASHBOARD ==========\n";
        cout << "1. View Profile\n";
        cout << "2. View Tourist Places\n";
        cout << "3. Update My Local/Guide Profile\n";
        cout << "4. View Incoming Guide Requests\n";
        cout << "5. Accept/Reject Next Request\n";
        cout << "6. View Request History (Stack)\n";
        cout << "7. Priority Queue Demo\n";
        cout << "0. Logout\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Try again.\\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                local->displayProfile();
                break;

            case 2:
                searchManager.displayPlaces();
                break;

            case 3:
                local->createOrUpdateGuideProfile();
                guideManager.addLocal(local);
                saveUserData();
                break;

            case 4:
                guideManager.displayRequests();
                break;
            case 5: guideManager.processNextRequest(); break;
            case 6: guideManager.displayHistory(); break;
            case 7: guideManager.demonstratePriorityQueue(); break;

            case 0:
                cout << "Logging out...\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);
}

public:
    TourismApplication()
        : searchManager(places) {
        loadUsers();
        loadPlaces();
        guideManager.loadRequests();
        for (auto* user : users) {
            Local* local = dynamic_cast<Local*>(user);
            if (local != nullptr) {
                guideManager.addLocal(local);
            }
        }
        reviewManager.loadReviews();
        for (auto* place : places)
            place->setRating(reviewManager.averageRating(place->getId(), place->getRating()));
    }

    ~TourismApplication() {
        for (auto* u : users)
            delete u;

        for (auto* p : places)
            delete p;
    }

    void run() {
        int choice;

        do {
            cout << "\n===== CONNECTING TOURISTS & LOCALS =====\n"
                 << "1. Register\n"
                 << "2. Login\n"
                 << "0. Exit\n"
                 << "Choice: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid choice.\n";
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
                case 1:
                    registerUser();
                    break;

                case 2: {
                    User* user = loginUser();

                    if (user != nullptr) {
                        if (user->getRole() == "Tourist") {
                            touristDashboard(user);
                        } else if (user->getRole() == "Local") {
                            Local* local = dynamic_cast<Local*>(user);
                            if (local != nullptr) {
                                localDashboard(local);
                            }
                        }
                    }

                    break;
                }

                case 0:
                    cout << "Thank you for using our application!\n";
                    break;

                default:
                    cout << "Invalid choice.\n";
            }

        } while (choice != 0);
    }
};

#endif
