#include <iostream>
#include <fstream>

#include <cmath>
#include <vector>
#include <algorithm>

#include <chrono>

using namespace std;
using namespace chrono;

// Структура для хранения координат точки
struct Tochechka
{
    double x, y;

    // Конструктор структуры для инициализации координат
    Tochechka(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

// Функция для вычисления Евклидова расстояния между двумя точками
double Evclidick(Tochechka& a, Tochechka& b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Функция для построения матрицы расстояний между всеми точками
vector<vector<double>> build_distance_matrix(vector<Tochechka>& A)
{
    int N = A.size(), i, j;
    vector<vector<double>> matrica(N, vector<double>(N, 0.0));

    // Параллельный расчет матрицы расстояний с использованием OpenMP
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < N; i++)
    {
        for (j = i + 1; j < N; j++)
        {
            matrica[i][j] = matrica[j][i] = Evclidick(A[i], A[j]); // Заполнение матрицы расстояний
        }
    }
    return matrica;
}

// Функция для вычисления общей дистанции маршрута
double total_distance(const vector<int>& a, const vector<vector<double>>& matrica)
{
    double distance = 0.0;
    int c = a.size() - 1;
    for (int i = 0; i < c; i++)
    {
        distance = distance + matrica[a[i]][a[i + 1]]; // Суммирование расстояний
    }
    return distance;
}

// Функция для объединения четырех векторов в один
vector<int> Sum(vector<int>& a, int a_size, vector<int>& b, int b_size, vector<int>& c, int c_size, vector<int>& d, int d_size)
{
    int i = 0;
    vector<int> sum;
    for (i; i < a_size; i++)
    {
        sum.push_back(a[i]); // Добавляем элементы из вектора a
    }
    for (i = 0; i < b_size; i++)
    {
        sum.push_back(b[i]); // Добавляем элементы из вектора b
    }
    for (i = 0; i < c_size; i++)
    {
        sum.push_back(c[i]); // Добавляем элементы из вектора c
    }
    for (i = 0; i < d_size; i++)
    {
        sum.push_back(d[i]); // Добавляем элементы из вектора d
    }
    return sum;
}

// Функция для разворота вектора
vector<int> reverse_vector(vector<int>& victor)
{
    vector<int> v = victor;
    reverse(v.begin(), v.end()); // Разворачиваем вектор
    return v;
}

// Функция для оптимизации маршрута с использованием 3-opt
vector<int> opt3(vector<int> a, int N, const vector<vector<double>>& mat)
{
    if (N < 10) return a; // Если количество точек меньше 10, не применяем оптимизацию

    int d52 = 0, i, k, j, ii, A_size, N1 = N - 4, N2 = N - 3, N3 = N - 1;
    double min_distance = total_distance(a, mat), distance;
    bool fl = true;
    vector<int> b = a, v1, v2, v3, v4, v2_rev, v3_rev;
    vector<vector<int>> A;
    vector<int>::iterator beg;

    // Основной цикл оптимизации
    while (fl == true)
    {
        fl = !fl;

        for (i = 1; i < N1; i++)
        {
            for (j = i + 1; j < N2; j++)
            {
                for (k = j + 1; k < N3; k++)
                {
                    beg = a.begin();
                    v1.assign(beg, beg + i); // Разделяем маршрут на 4 части
                    v2.assign(beg + i, beg + j);
                    v3.assign(beg + j, beg + k);
                    v4.assign(beg + k, a.end());

                    if (!v1.empty() && !v2.empty() && !v3.empty() && !v4.empty())
                    {
                        v3_rev = reverse_vector(v3); // Разворачиваем вектор v3
                        v2_rev = reverse_vector(v2); // Разворачиваем вектор v2

                        // Генерируем все возможные маршруты, полученные после перестановки сегментов
                        A = { Sum(v1, v1.size(), v2, v2.size(), v3, v3.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v2_rev, v2_rev.size(), v3, v3.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v2, v2.size(), v3_rev, v3_rev.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v2_rev, v2_rev.size(), v3_rev, v3_rev.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v3, v3.size(), v2, v2.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v3_rev, v3_rev.size(), v2, v2.size(), v4, v4.size()),
                              Sum(v1, v1.size(), v2, v2.size(), v4, v4.size(), v3, v3.size()),
                              Sum(v1, v1.size(), v4, v4.size(), v3, v3.size(), v2, v2.size())
                            };
                        A_size = A.size();

                        // Проходим по всем возможным маршрутам и выбираем тот, который минимизирует расстояние
                        ii = 0;
                        for (ii; ii < A_size; ++ii)
                        {
                            distance = total_distance(A[ii], mat);

                            #pragma omp critical
                            {
                                if (distance < min_distance)
                                {
                                    min_distance = distance; // Обновляем наименьшее расстояние
                                    b = A[ii]; // Сохраняем новый лучший маршрут
                                    fl = true;
                                }
                            }
                        }
                    }
                    else
                    {
                         d52 += 52;
                    }
                }
            }
        }

        if (fl == true)
        {
            a = b; // Если был улучшен маршрут, обновляем его
        }
    }

    return a;
}

int main()
{
    // Открытие файла с входными данными
    ifstream file("tsp_51_1");
    int N, i;
    file >> N;

    // Инициализация стартового маршрута
    vector<int> a(N);
    for (i = 0; i < N; i++)
    {
        // Делаем стартовый маршрут 1 2 3 4 ...
        a[i] = i;
    }

    // Чтение координат точек
    vector<Tochechka> coordinats(N);
    for (i = 0; i < N; i++)
    {
        // Читаем координаты каждой точки
        file >> coordinats[i].x >> coordinats[i].y;
    }

    // Закрываем файл
    file.close();

    high_resolution_clock::time_point start, end;
    long long time;
    double distance;

    // Строим матрицу расстояний между точками
    vector<vector<double>> dist_matrix = build_distance_matrix(coordinats);

    // Начинаем замер времени выполнения оптимизации
    start = high_resolution_clock::now();
    // Оптимизация маршрута с использованием 3-opt
    a = opt3(a, a.size(), dist_matrix);
    // Заканчиваем замер времени выполнения оптимизации
    end = high_resolution_clock::now();

    // Подсчитываем время выполнения
    time = duration_cast<milliseconds>(end - start).count();

    // Подсчитываем итоговую длину маршрута
    distance = total_distance(a, dist_matrix);

    // Выводим итоговую дистанцию
    cout << fixed << distance << " 1" << endl;

    for (i = 0; i < a.size(); i++)
    {
        cout << a[i] << " "; // Выводим порядок точек маршрута
    }

    cout << endl;

    // Выводим время работы алгоритма opt3 с потоками
    cout << "Execution time of opt3: " << time << " milliseconds" << endl;

    return 0;
}
