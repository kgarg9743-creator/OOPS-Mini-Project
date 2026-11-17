#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

class Train {
protected:
    int trainNo;
    string name;
    string source;
    string destination;
    int distance;
    int totalSeats;
    int availableSeats;

public:
    Train(int no, string n, string s, string d, int dist, int seats) {
        trainNo = no;
        name = n;
        source = s;
        destination = d;
        distance = dist;
        totalSeats = seats;
        availableSeats = seats;
    }

    virtual ~Train() {}

    virtual double calculateFare() = 0;
    virtual string getType() = 0;

    int getTrainNo() { return trainNo; }
    string getName() { return name; }
    int getTotalSeats() { return totalSeats; }
    int getAvailableSeats() { return availableSeats; }

    void bookSeat() { availableSeats--; }
    void freeSeat() { availableSeats++; }

    void display() {
        cout << left << setw(8) << trainNo << setw(18) << name
             << setw(11) << getType() << setw(10) << source
             << setw(10) << destination << setw(8) << availableSeats
             << calculateFare() << endl;
    }
};

class ExpressTrain : public Train {
public:
    ExpressTrain(int no, string n, string s, string d, int dist, int seats)
        : Train(no, n, s, d, dist, seats) {}

    double calculateFare() override { return distance * 1.2; }
    string getType() override { return "Express"; }
};

class SuperfastTrain : public Train {
public:
    SuperfastTrain(int no, string n, string s, string d, int dist, int seats)
        : Train(no, n, s, d, dist, seats) {}

    double calculateFare() override { return distance * 1.5 + 50; }
    string getType() override { return "Superfast"; }
};

class RajdhaniTrain : public Train {
public:
    RajdhaniTrain(int no, string n, string s, string d, int dist, int seats)
        : Train(no, n, s, d, dist, seats) {}

    double calculateFare() override { return distance * 2.0 + 150; }
    string getType() override { return "Rajdhani"; }
};

class Passenger {
protected:
    string name;
    int age;
    char gender;

public:
    Passenger(string n, int a, char g) {
        name = n;
        age = a;
        gender = g;
    }

    virtual ~Passenger() {}

    virtual double getDiscount() { return 0; }
    virtual string getCategory() { return "General"; }

    string getName() { return name; }
    int getAge() { return age; }
    char getGender() { return gender; }
};

class SeniorCitizen : public Passenger {
public:
    SeniorCitizen(string n, int a, char g) : Passenger(n, a, g) {}

    double getDiscount() override { return 0.40; }
    string getCategory() override { return "Senior"; }
};

class Student : public Passenger {
public:
    Student(string n, int a, char g) : Passenger(n, a, g) {}

    double getDiscount() override { return 0.25; }
    string getCategory() override { return "Student"; }
};

class Ticket {
private:
    int pnr;
    string name;
    int age;
    char gender;
    string category;
    int trainNo;
    int seatNo;
    double fare;
    string status;

public:
    Ticket(int p, string n, int a, char g, string c,
           int t, int s, double f, string st) {
        pnr = p;
        name = n;
        age = a;
        gender = g;
        category = c;
        trainNo = t;
        seatNo = s;
        fare = f;
        status = st;
    }

    int getPNR() { return pnr; }
    string getName() { return name; }
    int getAge() { return age; }
    char getGender() { return gender; }
    string getCategory() { return category; }
    int getTrainNo() { return trainNo; }
    int getSeatNo() { return seatNo; }
    double getFare() { return fare; }
    string getStatus() { return status; }

    void cancel() { status = "CANCELLED"; }

    void display() {
        cout << "\n------------- TICKET -------------\n";
        cout << "PNR       : " << pnr << endl;
        cout << "Name      : " << name << " (" << age << "/" << gender << ")" << endl;
        cout << "Category  : " << category << endl;
        cout << "Train No  : " << trainNo << endl;
        cout << "Seat No   : " << seatNo << endl;
        cout << "Fare      : Rs " << fare << endl;
        cout << "Status    : " << status << endl;
        cout << "----------------------------------\n";
    }
};

class Reservation {
private:
    vector<Train *> trains;
    vector<Ticket> tickets;
    int nextPNR;

    Train *findTrain(int no) {
        for (int i = 0; i < trains.size(); i++) {
            if (trains[i]->getTrainNo() == no)
                return trains[i];
        }
        return nullptr;
    }

    int findTicket(int pnr) {
        for (int i = 0; i < tickets.size(); i++) {
            if (tickets[i].getPNR() == pnr)
                return i;
        }
        return -1;
    }

    int findFreeSeat(Train *t) {
        for (int seat = 1; seat <= t->getTotalSeats(); seat++) {
            bool used = false;
            for (int i = 0; i < tickets.size(); i++) {
                if (tickets[i].getStatus() == "CONFIRMED" &&
                    tickets[i].getTrainNo() == t->getTrainNo() &&
                    tickets[i].getSeatNo() == seat) {
                    used = true;
                }
            }
            if (!used)
                return seat;
        }
        return -1;
    }

    void saveTickets() {
        ofstream file("tickets.txt");
        for (int i = 0; i < tickets.size(); i++) {
            file << tickets[i].getName() << endl;
            file << tickets[i].getPNR() << " " << tickets[i].getAge() << " "
                 << tickets[i].getGender() << " " << tickets[i].getCategory() << " "
                 << tickets[i].getTrainNo() << " " << tickets[i].getSeatNo() << " "
                 << tickets[i].getFare() << " " << tickets[i].getStatus() << endl;
        }
        file.close();
    }

    void loadTickets() {
        ifstream file("tickets.txt");
        if (!file)
            return;

        int pnr, age, trainNo, seat;
        char gender;
        double fare;
        string name, category, status;

        while (getline(file, name)) {
            if (!(file >> pnr >> age >> gender >> category >> trainNo >> seat >> fare >> status))
                break;
            file.ignore(1000, '\n');

            tickets.push_back(Ticket(pnr, name, age, gender, category,
                                     trainNo, seat, fare, status));

            if (pnr >= nextPNR)
                nextPNR = pnr + 1;

            Train *t = findTrain(trainNo);
            if (t != nullptr && status == "CONFIRMED")
                t->bookSeat();
        }
        file.close();
    }

public:
    Reservation() {
        nextPNR = 1001;

        trains.push_back(new ExpressTrain(11002, "Doon Express", "Delhi", "Dehradun", 300, 50));
        trains.push_back(new ExpressTrain(11001, "Ganga Express", "Delhi", "Kolkata", 1450, 40));
        trains.push_back(new SuperfastTrain(12301, "Mumbai Superfast", "Delhi", "Mumbai", 1400, 30));
        trains.push_back(new RajdhaniTrain(12951, "Rajdhani Special", "Delhi", "Chennai", 2180, 20));

        loadTickets();
    }

    ~Reservation() {
        for (int i = 0; i < trains.size(); i++)
            delete trains[i];
    }

    void showTrains() {
        cout << "\n--------------------- AVAILABLE TRAINS ---------------------\n";
        cout << left << setw(8) << "No" << setw(18) << "Name" << setw(11) << "Type"
             << setw(10) << "From" << setw(10) << "To" << setw(8) << "Seats"
             << "Fare(Rs)" << endl;
        for (int i = 0; i < trains.size(); i++)
            trains[i]->display();
    }

    void bookTicket() {
        showTrains();

        int trainNo = 0;
        cout << "\nEnter train number: ";
        cin >> trainNo;
        if (cin.fail())
            throw "Invalid input";

        Train *t = findTrain(trainNo);
        if (t == nullptr)
            throw "Train not found";
        if (t->getAvailableSeats() == 0)
            throw "No seats available on this train";

        string name;
        int age = 0;
        char gender = 'O';
        int choice = 0;

        cin.ignore(1000, '\n');
        cout << "Passenger name: ";
        getline(cin, name);
        if (name.empty())
            throw "Name cannot be empty";

        cout << "Age: ";
        cin >> age;
        cout << "Gender (M/F/O): ";
        cin >> gender;
        cout << "Category (1.General  2.Senior Citizen  3.Student): ";
        cin >> choice;

        if (cin.fail())
            throw "Invalid input";
        if (age < 1 || age > 120)
            throw "Invalid age";
        if (gender >= 'a' && gender <= 'z')
            gender = gender - 32;
        if (gender != 'M' && gender != 'F' && gender != 'O')
            throw "Invalid gender";

        Passenger *p;
        if (choice == 1) {
            p = new Passenger(name, age, gender);
        } else if (choice == 2) {
            if (age < 60)
                throw "Senior citizen must be 60 or above";
            p = new SeniorCitizen(name, age, gender);
        } else if (choice == 3) {
            p = new Student(name, age, gender);
        } else {
            throw "Invalid category";
        }

        double baseFare = t->calculateFare();
        double finalFare = baseFare * (1 - p->getDiscount());

        cout << "\nBase fare : Rs " << baseFare << endl;
        cout << "Discount  : " << p->getDiscount() * 100 << "%" << endl;
        cout << "You pay   : Rs " << finalFare << endl;

        char confirm;
        cout << "Confirm booking? (y/n): ";
        cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            delete p;
            cout << "Booking cancelled.\n";
            return;
        }

        int seat = findFreeSeat(t);
        tickets.push_back(Ticket(nextPNR, p->getName(), p->getAge(), p->getGender(),
                                 p->getCategory(), trainNo, seat, finalFare, "CONFIRMED"));
        delete p;

        t->bookSeat();
        nextPNR++;
        saveTickets();

        cout << "\nTicket booked successfully!";
        tickets.back().display();
    }

    void cancelTicket() {
        int pnr = 0;
        cout << "Enter PNR to cancel: ";
        cin >> pnr;
        if (cin.fail())
            throw "Invalid input";

        int index = findTicket(pnr);
        if (index == -1)
            throw "PNR not found";
        if (tickets[index].getStatus() != "CONFIRMED")
            throw "Ticket is already cancelled";

        tickets[index].display();
        double refund = tickets[index].getFare() * 0.75;
        cout << "Refund (75% of fare): Rs " << refund << endl;

        char confirm;
        cout << "Cancel this ticket? (y/n): ";
        cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            cout << "Cancellation aborted.\n";
            return;
        }

        tickets[index].cancel();
        Train *t = findTrain(tickets[index].getTrainNo());
        if (t != nullptr)
            t->freeSeat();

        saveTickets();
        cout << "Ticket cancelled. Rs " << refund << " will be refunded.\n";
    }

    void checkStatus() {
        int pnr = 0;
        cout << "Enter PNR: ";
        cin >> pnr;
        if (cin.fail())
            throw "Invalid input";

        int index = findTicket(pnr);
        if (index == -1)
            throw "PNR not found";

        tickets[index].display();
    }

    void showAllTickets() {
        if (tickets.empty()) {
            cout << "\nNo tickets booked yet.\n";
            return;
        }

        cout << "\n" << left << setw(8) << "PNR" << setw(22) << "Name" << setw(8) << "Train"
             << setw(6) << "Seat" << setw(10) << "Fare" << "Status" << endl;
        for (int i = 0; i < tickets.size(); i++) {
            cout << left << setw(8) << tickets[i].getPNR() << setw(22) << tickets[i].getName()
                 << setw(8) << tickets[i].getTrainNo() << setw(6) << tickets[i].getSeatNo()
                 << setw(10) << tickets[i].getFare() << tickets[i].getStatus() << endl;
        }
    }
};

int main() {
    Reservation r;
    int choice = 0;

    cout << fixed << setprecision(2);
    cout << "==================================\n";
    cout << "   RAILWAY RESERVATION SYSTEM\n";
    cout << "==================================\n";

    while (true) {
        cout << "\n--------------- MENU ---------------\n";
        cout << "1. View trains\n";
        cout << "2. Book ticket\n";
        cout << "3. Cancel ticket\n";
        cout << "4. Check PNR status\n";
        cout << "5. View all tickets\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.eof())
            break;

        try {
            if (cin.fail())
                throw "Please enter a number from 1 to 6";

            switch (choice) {
            case 1: r.showTrains(); break;
            case 2: r.bookTicket(); break;
            case 3: r.cancelTicket(); break;
            case 4: r.checkStatus(); break;
            case 5: r.showAllTickets(); break;
            case 6:
                cout << "\nThank you! Goodbye.\n";
                return 0;
            default:
                throw "Please enter a number from 1 to 6";
            }
        } catch (const char *msg) {
            cout << "\nError: " << msg << endl;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
            }
        }
    }

    return 0;
}
