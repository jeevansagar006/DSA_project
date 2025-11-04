 #include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// #STRUCTURES 
struct Student {
    int roll;
    string name;
};

struct Course {
    string name;
};

struct Attendance {
    int roll;
    string course;
    int classes;
    int present;
    int absent;
};

// #LOADERS 
vector<Student> load_students() {
    vector<Student> students;
    ifstream fin("students.txt");
    string line;
    getline(fin, line); // header
    getline(fin, line); // separator
    int roll; string name;
    while (fin >> roll >> name)
        students.push_back({roll, name});
    sort(students.begin(), students.end(), [](auto &a, auto &b){ return a.roll < b.roll; });
    return students;
}

vector<Course> load_courses() {
    vector<Course> courses;
    ifstream fin("courses.txt");
    string line;
    getline(fin, line);
    getline(fin, line);
    string cname;
    while (fin >> cname)
        courses.push_back({cname});
    return courses;
}

vector<Attendance> load_attendance() {
    vector<Attendance> att;
    ifstream fin("attendance.txt");
    string line;
    getline(fin, line);
    getline(fin, line);
    while (getline(fin, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        int roll, cl, p, a; string cname;
        ss >> roll >> cname >> cl >> p >> a;
        att.push_back({roll, cname, cl, p, a});
    }
    return att;
}

// #SAVERS 
void save_students(const vector<Student> &students) {
    ofstream fout("students.txt");
    fout << "RollNo    Name\n";
    fout << "----------------------\n";
    for (auto &s : students)
        fout << left << setw(10) << s.roll << s.name << "\n";
}

void save_courses(const vector<Course> &courses) {
    ofstream fout("courses.txt");
    fout << "CourseName\n";
    fout << "----------------\n";
    for (auto &c : courses)
        fout << c.name << "\n";
}

void save_attendance(vector<Attendance> att, bool sort_by_roll = true, bool by_percentage = false) {
    if (by_percentage) {
        sort(att.begin(), att.end(), [](auto &a, auto &b) {
            double pa = (a.classes == 0) ? 0.0 : (a.present * 100.0 / a.classes);
            double pb = (b.classes == 0) ? 0.0 : (b.present * 100.0 / b.classes);
            return pa > pb; // descending order
        });
    } else if (sort_by_roll) {
        sort(att.begin(), att.end(), [](auto &a, auto &b) {
            if (a.roll != b.roll) return a.roll < b.roll;
            return a.course < b.course;
        });
    }

    ofstream fout("attendance.txt");
    fout << "RollNo    Course         Classes   Present   Absent   Percentage\n";
    fout << "-----------------------------------------------------------------\n";
    for (auto &cur : att) {
        double perc = (cur.classes == 0) ? 0.0 : (cur.present * 100.0 / cur.classes);
        fout << left << setw(10) << cur.roll
             << setw(15) << cur.course
             << setw(10) << cur.classes
             << setw(10) << cur.present
             << setw(10) << cur.absent
             << fixed << setprecision(2) << perc << "%\n";
    }
}

// #HELPERS
bool attendance_exists(const vector<Attendance> &att, int roll, const string &course) {
    for (auto &a : att)
        if (a.roll == roll && a.course == course)
            return true;
    return false;
}

// #MAIN OPS 
void add_student() {
    int roll; string name;
    cout << "Enter roll number: ";
    cin >> roll;
    cout << "Enter name: ";
    // cin >> name;

    cin.ignore(); // clear leftover newline
    getline(cin, name);


    auto students = load_students();
    students.push_back({roll, name});
    sort(students.begin(), students.end(), [](auto &a, auto &b){ return a.roll < b.roll; });
    save_students(students);

    auto courses = load_courses();
    auto att = load_attendance();

    for (auto &c : courses)
        if (!attendance_exists(att, roll, c.name))
            att.push_back({roll, c.name, 0, 0, 0});

    save_attendance(att);
    cout << "Student added successfully!\n";
}

void add_course() {
    string name;
    cout << "Enter course name: ";
    cin >> name;

    auto courses = load_courses();
    courses.push_back({name});
    save_courses(courses);

    auto students = load_students();
    auto att = load_attendance();

    for (auto &s : students)
        if (!attendance_exists(att, s.roll, name))
            att.push_back({s.roll, name, 0, 0, 0});

    save_attendance(att);
    cout << "Course added successfully!\n";
}

void mark_attendance() {
    int roll; string course;
    cout << "Enter roll number: ";
    cin >> roll;
    cout << "Enter course name: ";
    cin >> course;

    auto att = load_attendance();
    bool found = false;
    for (auto &a : att) {
        if (a.roll == roll && a.course == course) {
            a.classes++;
            char present;
            cout << "Present (y/n): ";
            cin >> present;
            if (present == 'y' || present == 'Y') a.present++;
            else a.absent++;
            found = true;
            break;
        }
    }
    if (!found) cout << "Record not found!\n";
    save_attendance(att);
    cout << "Attendance updated.\n";
}

void delete_student() {
    int roll;
    cout << "Enter roll number to delete: ";
    cin >> roll;

    auto students = load_students();
    auto att = load_attendance();

    students.erase(remove_if(students.begin(), students.end(),
        [&](auto &s){ return s.roll == roll; }), students.end());

    att.erase(remove_if(att.begin(), att.end(),
        [&](auto &a){ return a.roll == roll; }), att.end());

    save_students(students);
    save_attendance(att);
    cout << "Student deleted successfully.\n";
}

void delete_course() {
    string name;
    cout << "Enter course name to delete: ";
    cin >> name;

    auto courses = load_courses();
    auto att = load_attendance();

    courses.erase(remove_if(courses.begin(), courses.end(),
        [&](auto &c){ return c.name == name; }), courses.end());

    att.erase(remove_if(att.begin(), att.end(),
        [&](auto &a){ return a.course == name; }), att.end());

    save_courses(courses);
    save_attendance(att);
    cout << "Course deleted successfully.\n";
}

// #SORTING FUNCTIONS 
void sort_by_percentage() {
    auto att = load_attendance();
    save_attendance(att, false, true);
    cout << "Attendance sorted by percentage (descending).\n";
}

void sort_by_roll() {
    auto att = load_attendance();
    save_attendance(att, true, false);
    cout << "Attendance sorted by roll number.\n";
}

// #MAIN FUNCTION 
int main() {
    while (true) {
        cout << "\n--- Attendance Tracker Menu ---\n";
        cout << "1. Add Course\n";
        cout << "2. Add Student\n";
        cout << "3. Mark Attendance\n";
        cout << "4. Delete Student\n";
        cout << "5. Delete Course\n";
        cout << "6. Sort by Percentage\n";
        cout << "7. Sort by Roll Number\n";
        cout << "8. Exit\n";
        cout << "Enter choice: ";
        int choice; cin >> choice;
        if (choice == 1) add_course();
        else if (choice == 2) add_student();
        else if (choice == 3) mark_attendance();
        else if (choice == 4) delete_student();
        else if (choice == 5) delete_course();
        else if (choice == 6) sort_by_percentage();
        else if (choice == 7) sort_by_roll();
        else if (choice == 8) break;
        else cout << "Invalid choice!\n";
    }
    return 0;
}
