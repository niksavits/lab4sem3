#include <iostream>
#include <string.h>
#include <vector>

using namespace std;

#define P 3  // Общее количество процессов
#define R 3  // Общее количество типов ресурсов

int total = 0; // Переменная для подсчета общего количества безопасных последовательностей

// Функция для проверки, может ли процесс быть выделен
bool is_available(int process_id, int allocated[][R], int max[][R], int need[][R], int available[]) {
    bool flag = true;  // Флаг для отслеживания, можно ли выделить ресурсы процессу

    // Проверяем, есть ли достаточно ресурсов для процесса
    for (int i = 0; i < R; i++) {
        // Если потребности процесса превышают доступные ресурсы, то процесс не может быть выделен
        if (need[process_id][i] > available[i]) {
            flag = false;
        }
    }

    return flag;  // Возвращаем результат проверки
}

// Функция для поиска и печати безопасных последовательностей
void safe_sequence(bool marked[], int allocated[][R], int max[][R], int need[][R], int available[], vector<int> safe) {
    for (int i = 0; i < P; i++) {
        // Если процесс ещё не отмечен и может быть выделен
        if (!marked[i] && is_available(i, allocated, max, need, available)) {
            // Помечаем процесс как использованный
            marked[i] = true;

            // Обновляем доступные ресурсы, освобождая ресурсы, которые использует процесс i
            for (int j = 0; j < R; j++) {
                available[j] += allocated[i][j];
            }

            safe.push_back(i);  // Добавляем процесс в безопасную последовательность

            // Рекурсивно пытаемся найти безопасную последовательность с текущим процессом
            safe_sequence(marked, allocated, max, need, available, safe);
            safe.pop_back();  // Отменяем добавление текущего процесса, если дальше не получится найти решение

            // Снимаем отметку с процесса
            marked[i] = false;

            // Отменяем изменение доступных ресурсов, возвращая их в первоначальное состояние
            for (int j = 0; j < R; j++) {
                available[j] -= allocated[i][j];
            }
        }
    }

    // Если найден полный список безопасных процессов (размер safe равен количеству процессов)
    if (safe.size() == P) {
        total++;  // Увеличиваем количество найденных безопасных последовательностей
        // Печатаем текущую безопасную последовательность
        for (int i = 0; i < P; i++) {
            cout << "P" << safe[i] + 1;  // Печатаем номер процесса (сдвигаем на 1, т.к. индексация с 0)
            if (i != (P - 1))
                cout << "--> ";  // Разделитель между процессами
        }
        cout << endl;  // Переход на новую строку после печати последовательности
    }
}

// Главная функция
int main() {
    int allocated[P][R], max[P][R], resources[R], available[R], need[P][R];  // Объявляем массивы для данных

    // Ввод матрицы распределённых ресурсов (P x R)
    cout << "\nEnter the allocated resources matrix (P x R):\n";
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            cin >> allocated[i][j];  // Вводим значения для каждого процесса и ресурса
        }
    }

    // Ввод матрицы максимальных потребностей ресурсов (P x R)
    cout << "\nEnter the maximum resources matrix (P x R):\n";
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            cin >> max[i][j];  // Вводим максимальные потребности для каждого процесса и ресурса
        }
    }

    // Ввод общего количества доступных ресурсов
    cout << "\nEnter the total resources available (R):\n";
    for (int i = 0; i < R; i++) {
        cin >> resources[i];  // Вводим общее количество доступных ресурсов для каждого типа
    }

    // Вычисление доступных ресурсов, исключая те, которые уже распределены
    for (int i = 0; i < R; i++) {
        int sum = 0;  // Сумма всех выделенных ресурсов этого типа
        for (int j = 0; j < P; j++) {
            sum += allocated[j][i];  // Складываем все выделенные ресурсы для типа i
        }
        available[i] = resources[i] - sum;  // Вычисляем, сколько доступных ресурсов этого типа осталось
    }

    // Заполнение матрицы потребностей (need), где need[i][j] = max[i][j] - allocated[i][j]
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            need[i][j] = max[i][j] - allocated[i][j];  // Вычисляем потребности каждого процесса в каждом ресурсе
        }
    }

    // Инициализируем вектор для безопасных последовательностей и массив для пометок
    vector<int> safe;
    bool marked[P];  // Массив для отслеживания, был ли процесс использован в безопасной последовательности
    memset(marked, false, sizeof(marked));  // Инициализируем все элементы массива marked значением false

    // Печать безопасных последовательностей
    cout << "\nSafe sequences are:\n";
    safe_sequence(marked, allocated, max, need, available, safe);  // Рекурсивно ищем и выводим все безопасные последовательности

    // Вывод общего числа безопасных последовательностей
    cout << "\nThere are total " << total << " safe-sequences" << endl;

    return 0;  // Завершаем программу
}
