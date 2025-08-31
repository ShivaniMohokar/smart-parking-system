#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <limits.h>
using namespace std;

struct ParkingSlot {
    bool available;
    string address;
    string vehicleNo;
    string bookingTime;
    string endTime;
    int durationMinutes;
    double parkingFee;
    int proximity;

    ParkingSlot(string addr = "", int prox = 0)
        : available(true), address(addr), vehicleNo(""),
          bookingTime(""), endTime(""), durationMinutes(0),
          parkingFee(0.0), proximity(prox) {}
};

vector<string> locations = {
    "JM Road", "FC Road", "Swargate", "Pune Station", "Amanora",
    "Phoenix Mall", "Okayama Garden", "Dagdusheth Temple", "Kothrud", "Hadapsar"
};

vector<string> addresses = {
    "Chastwe Ground Pay & Park - 0.6 km from JM Road",
    "Arai Trek Parking - 1.8 km from FC Road",
    "Saras Baug Parking Lot - 1.2 km from Swargate",
    "Hamalwada Pay And Park (PMC) - 0.9 km from Pune Station",
    "Amanora Town Centre Parking - 0.2 km from Amanora",
    "Phoenix Mall Parking - 0.1 km from Phoenix Mall",
    "Ring Waterfall Parking - 1.5 km from Okayama Garden",
    "Star Auto Services Branch 4 - 1.0 km from Dagdusheth Temple",
    "Arai Trek Parking - 0.5 km from Kothrud",
    "Dhanshree Truck Parking - 2.0 km from Hadapsar"
};

vector<vector<ParkingSlot>> parkingSlots;
vector<vector<pair<int, int>>> graph;

void initialize() {
    graph.clear();
    parkingSlots.clear();
    for (int i = 0; i < 10; ++i) {
        graph.push_back({});
    }

    // Example road connections between locations (weights in km)
    graph[0].push_back({1, 1});
    graph[1].push_back({0, 1});
    graph[2].push_back({3, 2});
    graph[3].push_back({2, 2});
    graph[4].push_back({5, 1});
    graph[5].push_back({4, 1});
    graph[6].push_back({7, 1});
    graph[7].push_back({6, 1});
    graph[8].push_back({9, 2});
    graph[9].push_back({8, 2});

    for (int i = 0; i < locations.size(); ++i) {
        vector<ParkingSlot> slots;
        for (int j = 0; j < 10; ++j) {
            int proximity = rand() % 5 + 1;
            slots.emplace_back(addresses[i], proximity);
        }
        parkingSlots.push_back(slots);
    }
}

string getCurrentTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", ltm);
    return string(timeStr);
}

string calculateEndTime(int hours, int minutes) {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    ltm->tm_hour += hours;
    ltm->tm_min += minutes;
    mktime(ltm);
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", ltm);
    return string(timeStr);
}

double calculateParkingFee(int hours, int minutes) {
    double fee = hours * 50; // ₹50 per hour
    if (minutes > 0) fee += 25;
    return fee;
}

vector<int> dijkstra(int start) {
    vector<int> dist(10, INT_MAX);
    dist[start] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});
    while (!pq.empty()) {
        int u = pq.top().second, d = pq.top().first;
        pq.pop();
        if (d > dist[u]) continue;
        for (auto &neighbor : graph[u]) {
            int v = neighbor.first, weight = neighbor.second;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}

void displaySlotStatus(int locationIndex) {
    cout << "\nParking slot status at " << locations[locationIndex] << ":\n";
    cout << "Address: " << addresses[locationIndex] << "\n\n";
    for (int i = 0; i < parkingSlots[locationIndex].size(); ++i) {
        cout << "Slot " << i + 1 << ": ";
        if (parkingSlots[locationIndex][i].available) {
            cout << "\033[32mAvailable\033[0m\n";
        } else {
            auto &ps = parkingSlots[locationIndex][i];
            cout << "\033[31mOccupied\033[0m (Vehicle No: " << ps.vehicleNo
                 << " | Time: " << ps.bookingTime << " to " << ps.endTime << ")\n";
        }
    }
}

void cancelBooking() {
    cout << "\nEnter location index (1 to 10) to cancel booking: ";
    int loc;
    cin >> loc;
    loc--;
    displaySlotStatus(loc);

    cout << "Enter slot number to cancel (1-10): ";
    int slot;
    cin >> slot;

    ParkingSlot &ps = parkingSlots[loc][slot - 1];
    if (ps.available) {
        cout << "\n\033[33mSlot is already available.\033[0m\n";
    } else {
        ps.available = true;
        ps.vehicleNo = "";
        ps.bookingTime = "";
        ps.endTime = "";
        ps.durationMinutes = 0;
        ps.parkingFee = 0.0;

        cout << "\n\033[32mBooking cancelled successfully.\033[0m Slot " << slot
             << " at " << locations[loc] << " is now available.\n";
    }
}

void bookParking() {
    cout << "\nAvailable Locations:\n";
    for (int i = 0; i < locations.size(); ++i)
        cout << i + 1 << ". " << locations[i] << endl;

    int start;
    cout << "Enter your current location index (1 to 10): ";
    cin >> start;
    start--;

    // Run Dijkstra for distance calculations
    vector<int> dist = dijkstra(start);

    // Check if slots available at chosen location
    bool slotAvailable = false;
    for (auto &slot : parkingSlots[start])
        if (slot.available) slotAvailable = true;

    if (!slotAvailable) {
        cout << "\n\033[31mNo available slots at " << locations[start] << ".\033[0m\n";

        // Suggest nearest alternative
        int bestLoc = -1, bestDist = INT_MAX;
        for (int i = 0; i < locations.size(); i++) {
            if (i == start) continue;
            for (auto &slot : parkingSlots[i]) {
                if (slot.available && dist[i] < bestDist) {
                    bestLoc = i;
                    bestDist = dist[i];
                    break;
                }
            }
        }

        if (bestLoc != -1) {
            cout << "Nearest available parking is at \033[32m"
                 << locations[bestLoc] << "\033[0m ("
                 << bestDist << " km away).\n";
        } else {
            cout << "Sorry, no slots available anywhere right now.\n";
        }
        return;
    }

    // Proceed with booking at chosen location
    displaySlotStatus(start);
    int slot;
    cout << "Enter slot number to book (1-10): ";
    cin >> slot;

    ParkingSlot &ps = parkingSlots[start][slot - 1];
    if (!ps.available) {
        cout << "\033[31mSlot already occupied. Try another slot.\033[0m\n";
        return;
    }

    string name, vehicleNo;
    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter your vehicle number: ";
    getline(cin, vehicleNo);

    int hours, minutes;
    cout << "Enter parking duration:\nHours: ";
    cin >> hours;
    cout << "Minutes: ";
    cin >> minutes;

    double fee = calculateParkingFee(hours, minutes);
    ps.available = false;
    ps.vehicleNo = vehicleNo;
    ps.bookingTime = getCurrentTime();
    ps.endTime = calculateEndTime(hours, minutes);
    ps.durationMinutes = hours * 60 + minutes;
    ps.parkingFee = fee;

    cout << "\n\033[32mBooking Successful!\033[0m\n";
    cout << "Slot " << slot << " at " << locations[start] << " is booked.\n";
    cout << "Booking Details:\n"
         << "Name: " << name << "\n"
         << "Vehicle No: " << vehicleNo << "\n"
         << "Booking Time: " << ps.bookingTime << "\n"
         << "End Time: " << ps.endTime << "\n"
         << "Duration: " << hours << " hours " << minutes << " minutes\n"
         << "Parking Fee: Rs " << fixed << setprecision(2) << fee << "\n";
}

int main() {
    initialize();
    while (true) {
        cout << "\n===========================================\n";
        cout << "     SMART PARKING SYSTEM - PUNE      \n";
        cout << "===========================================\n";

        cout << "1. Book Parking Slot\n";
        cout << "2. Cancel Booking\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1)
            bookParking();
        else if (choice == 2)
            cancelBooking();
        else if (choice == 3)
            break;
        else
            cout << "\033[31mInvalid choice. Try again.\033[0m\n";
    }
    return 0;
}
