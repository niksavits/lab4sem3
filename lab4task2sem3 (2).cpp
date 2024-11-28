#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>

using namespace std;

struct ExamResult {
    string subject;
    int score;
};

struct Student {
    string fullName;
    int age;
    int schoolNumber;
    vector<ExamResult> examResults;
};

void processWithoutMultithreading(const vector<Student>& students, map<int, int>& schoolCount) {
    for (const auto& student : students) {
        bool has100Score = false;
        for (const auto& exam : student.examResults) {
            if (exam.score == 100) {
                has100Score = true;
                break;
            }
        }
        if (has100Score) {
            schoolCount[student.schoolNumber]++;
        }
    }
}

void processWithMultithreading(const vector<Student>& students, map<int, int>& schoolCount, int numThreads) {
    mutex mtx;
    vector<thread> threads;

    auto processChunk = [&](int start, int end) {
        map<int, int> localCount;
        for (int i = start; i < end; ++i) {
            bool has100Score = false;
            for (const auto& exam : students[i].examResults) {
                if (exam.score == 100) {
                    has100Score = true;
                    break;
                }
            }
            if (has100Score) {
                localCount[students[i].schoolNumber]++;
            }
        }
        lock_guard<mutex> lock(mtx);
        for (const auto& entry : localCount) {
            schoolCount[entry.first] += entry.second;
        }
    };

    int chunkSize = students.size() / numThreads;
    int remainder = students.size() % numThreads;

    int start = 0;
    for (int i = 0; i < numThreads; ++i) {
        int end = start + chunkSize + (remainder > 0 ? 1 : 0);
        threads.emplace_back(processChunk, start, end);
        start = end;
        remainder--;
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

void printTop3Schools(const map<int, int>& schoolCount) {
    vector<pair<int, int>> sortedSchools(schoolCount.begin(), schoolCount.end());
    sort(sortedSchools.begin(), sortedSchools.end(), [](const auto& a, const auto& b) {
        return b.second < a.second;
    });

    cout << "ТОП-3 школы по количеству школьников, сдавших хотя бы один экзамен на 100 баллов:\n";
    for (int i = 0; i < 3 && i < sortedSchools.size(); ++i) {
        cout << "Школа номер: " << sortedSchools[i].first << ", Количество: " << sortedSchools[i].second << '\n';
    }
}

int main() {
    int numStudents, numThreads;
    cout << "Введите количество учеников: ";
    cin >> numStudents;
    cout << "Введите количество потоков: ";
    cin >> numThreads;

    vector<Student> students(numStudents);

    // Заполнение данных о студентах
    for (auto& student : students) {
        cout << "Введите ФИО ученика: ";
        cin.ignore();
        getline(cin, student.fullName);
        cout << "Введите возраст ученика: ";
        cin >> student.age;
        cout << "Введите номер школы: ";
        cin >> student.schoolNumber;

        int numExams;
        cout << "Введите количество экзаменов: ";
        cin >> numExams;

        for (int j = 0; j < numExams; ++j) {
            ExamResult result;
            cout << "Введите название предмета: ";
            cin.ignore();
            getline(cin, result.subject);
            cout << "Введите количество баллов: ";
            cin >> result.score;
            student.examResults.push_back(result);
        }
    }

    map<int, int> schoolCount;

    // Время выполнения без многопоточности
    auto start = chrono::high_resolution_clock::now();
    processWithoutMultithreading(students, schoolCount);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Время обработки без многопоточности: " << duration.count() << " секунд\n";
    // Время выполнения с многопоточностью
    schoolCount.clear();
    start = chrono::high_resolution_clock::now();
    processWithMultithreading(students, schoolCount, numThreads);
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    cout << "Время обработки с использованием многопоточности: " << duration.count() << " секунд\n";

    // Вывод ТОП-3 школ
    printTop3Schools(schoolCount);

    return 0;
}