#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>
using namespace std;

// Helpers
static inline string ltrim(const string &s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;
    return s.substr(start);
}

static inline string rtrim(const string &s) {
    if (s.empty()) return s;
    size_t end = s.size();
    while (end > 0 && isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(0, end);
}

static inline string trim(const string &s) {
    return rtrim(ltrim(s));
}

static inline string toLowerCopy(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

static inline string toTitleCase(const string &s) {
    string result;
    result.reserve(s.size());
    bool startOfWord = true;
    for (char ch : s) {
        if (isspace(static_cast<unsigned char>(ch))) {
            result.push_back(ch);
            startOfWord = true;
        } else {
            if (startOfWord) {
                result.push_back(static_cast<char>(toupper(static_cast<unsigned char>(ch))));
                startOfWord = false;
            } else {
                result.push_back(static_cast<char>(tolower(static_cast<unsigned char>(ch))));
            }
        }
    }
    return result;
}

static inline bool parseStudentLine(const string &line, string &name, string &rollNo, string &cgpaStr) {
    // Expected format: Name " : " RollNo " : " CGPA
    size_t first = line.find(':');
    if (first == string::npos) return false;
    size_t second = line.find(':', first + 1);
    if (second == string::npos) return false;
    string n = trim(line.substr(0, first));
    string r = trim(line.substr(first + 1, second - (first + 1)));
    string c = trim(line.substr(second + 1));
    // If tokens contain the separator spaces, remove trailing/leading separators like 'Name  ' or '  Roll'
    // Also remove residual separator characters
    if (!n.empty() && n.back() == ' ') n = rtrim(n);
    if (!r.empty() && r.front() == ' ') r = ltrim(r);
    if (!c.empty() && c.front() == ' ') c = ltrim(c);
    name = n;
    rollNo = r;
    cgpaStr = c;
    return true;
}

static inline void printHeaderAligned() {
    const int NAME_W = 24;
    const int ROLL_W = 12;
    const int CGPA_W = 6;
    cout << left << setw(NAME_W) << "Name"
         << " | " << left << setw(ROLL_W) << "RollNo"
         << " | " << left << setw(CGPA_W) << "CGPA" << endl;
}

static inline void printRowAligned(const string &name, const string &rollNo, const string &cgpaStr) {
    const int NAME_W = 24;
    const int ROLL_W = 12;
    const int CGPA_W = 6;
    cout << left << setw(NAME_W) << name
         << " | " << left << setw(ROLL_W) << rollNo
         << " | " << left << setw(CGPA_W) << cgpaStr << endl;
}

class Student {
private:
    string Name, RollNo;
    float CGPA;

public:
    Student() : Name(""), RollNo(""), CGPA(0.0) {}

    void setName(string name) {
        Name = name;
    }

    void setRollNo(string rollNo) {
        RollNo = rollNo;
    }

    void setCGPA(float cgpa) {
        CGPA = cgpa;
    }

    string getName() {
        return Name;
    }

    string getRollNo() {
        return RollNo;
    }

    float getCGPA() {
        return CGPA;
    }
};

void addStudent(Student s) {
    string name, rollNo;
    float cgpa;

    cout << "Enter Name Of Student: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, name);
    name = toTitleCase(trim(name));
    s.setName(name);

    cout << "Enter RollNo of Student: ";
    cin >> rollNo;
    s.setRollNo(rollNo);

    cout << "Enter CGPA of Student: ";
    cin >> cgpa;
    s.setCGPA(cgpa);

    ofstream out("Student.txt", ios::app);
    if (!out) {
        cout << "ERROR: File Not Opened!" << endl;
    } else {
        out << s.getName() << " : " << s.getRollNo() << " : " << s.getCGPA() << endl;
        cout << "Student Added Successfully!" << endl;
    }
    out.close();
}

void search(Student s) {
    string nameQuery;
    cout << "Enter Name of Student: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, nameQuery);
    nameQuery = toLowerCopy(trim(nameQuery));

    ifstream in("Student.txt");
    if (!in) {
        cout << "Error: File Cannot Be Opened!" << endl;
    } else {
        string line;
        bool found = false;
        while (getline(in, line)) {
            string n, r, c;
            if (!parseStudentLine(line, n, r, c)) {
                continue;
            }
            string nLower = toLowerCopy(trim(n));
            if (nLower.find(nameQuery) != string::npos) {
                if (!found) {
                    printHeaderAligned();
                }
                printRowAligned(toTitleCase(trim(n)), trim(r), trim(c));
                found = true;
            }
        }
        if (!found) {
            cout << "Student Not Found!" << endl;
        }
    }
}

void update() {
    string rollNo;
    cout << "Enter RollNo of Student: ";
    cin >> rollNo;

    ifstream in("Student.txt");
    if (!in) {
        cout << "Error: File Cannot Be Opened!" << endl;
    }

    ofstream out("Student Temp.txt");
    if (!out) {
        cout << "Error: Temporary File Cannot Be Opened!" << endl;
        out.close();
    }

    string line;
    bool found = false;
    float newcgpa = 0.0f;
    string newCGPAStr;
    while (getline(in, line)) {
        string n, r, c;
        if (parseStudentLine(line, n, r, c)) {
            if (trim(r) == trim(rollNo)) {
                if (!found) {
                    cout << "Enter New CGPA: ";
                    cin >> newcgpa;
                    stringstream ss;
                    ss << newcgpa;
                    newCGPAStr = ss.str();
                    cout << "CGPA updated for: " << rollNo << endl;
                    found = true;
                }
                string rebuilt = toTitleCase(trim(n)) + " : " + trim(r) + " : " + newCGPAStr;
                out << rebuilt << endl;
                continue;
            }
        }
        out << line << endl;
    }

    if (!found) {
        cout << "Student Not Found!" << endl;
    }

    out.close();
    in.close();

    remove("Student.txt");
    rename("Student Temp.txt", "Student.txt");
}


void displayAllStudents() {
    ifstream in("Student.txt");
    if (!in) {
        cout << "Error: File Cannot Be Opened!" << endl;
        return;
    }

    string line;
    bool found = false;
    while (getline(in, line)) {
        string n, r, c;
        if (!parseStudentLine(line, n, r, c)) {
            continue;
        }
        if (!found) {
            printHeaderAligned();
        }
        printRowAligned(toTitleCase(trim(n)), trim(r), trim(c));
        found = true;
    }

    if (!found) {
        cout << "No Students Found!" << endl;
    }

    in.close();
}

int main() {
    Student s;
    bool exit = false;
    while (!exit) {
        system("clear");
        cout << "Welcome To Student Management System" << endl;
        cout << "************************************" << endl;
        cout << "1. Add Student." << endl;
        cout << "2. Search Student." << endl;
        cout << "3. Update Student." << endl;
        cout << "4. Display All Students." << endl;
        cout << "5. Exit" << endl;
        cout << "Enter Choice: ";
        int val;
        cin >> val;

        if (val == 1) {
            system("clear");
            addStudent(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
        } else if (val == 2) {
            system("clear");
            search(s);
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        } else if (val == 3) {
            system("clear");
            update();
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        } else if (val == 4) {
            system("clear");
            displayAllStudents();
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        } else if (val == 5) {
            system("clear");
            exit = true;
            cout << "Good Luck!" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        }
    }

    return 0;
}
