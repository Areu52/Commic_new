#include <iostream>
#include <fstream>

#include <cmath>
#include <vector>
#include <algorithm>

#include <chrono>

using namespace std;
using namespace chrono;

// ��������� ��� �������� ��������� �����
struct Tochechka
{
    double x, y;

    // ����������� ��������� ��� ������������� ���������
    Tochechka(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

// ������� ��� ���������� ��������� ���������� ����� ����� �������
double Evclidick(Tochechka& a, Tochechka& b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// ������� ��� ���������� ������� ���������� ����� ����� �������
vector<vector<double>> build_distance_matrix(vector<Tochechka>& A)
{
    int N = A.size(), i, j;
    vector<vector<double>> matrica(N, vector<double>(N, 0.0));

    // ������������ ������ ������� ���������� � �������������� OpenMP
    #pragma omp parallel for schedule(dynamic)
    for (i = 0; i < N; i++)
    {
        for (j = i + 1; j < N; j++)
        {
            matrica[i][j] = matrica[j][i] = Evclidick(A[i], A[j]); // ���������� ������� ����������
        }
    }
    return matrica;
}

// ������� ��� ���������� ����� ��������� ��������
double total_distance(const vector<int>& a, const vector<vector<double>>& matrica)
{
    double distance = 0.0;
    int c = a.size() - 1;
    for (int i = 0; i < c; i++)
    {
        distance = distance + matrica[a[i]][a[i + 1]]; // ������������ ����������
    }
    return distance;
}

// ������� ��� ����������� ������� �������� � ����
vector<int> Sum(vector<int>& a, int a_size, vector<int>& b, int b_size, vector<int>& c, int c_size, vector<int>& d, int d_size)
{
    int i = 0;
    vector<int> sum;
    for (i; i < a_size; i++)
    {
        sum.push_back(a[i]); // ��������� �������� �� ������� a
    }
    for (i = 0; i < b_size; i++)
    {
        sum.push_back(b[i]); // ��������� �������� �� ������� b
    }
    for (i = 0; i < c_size; i++)
    {
        sum.push_back(c[i]); // ��������� �������� �� ������� c
    }
    for (i = 0; i < d_size; i++)
    {
        sum.push_back(d[i]); // ��������� �������� �� ������� d
    }
    return sum;
}

// ������� ��� ��������� �������
vector<int> reverse_vector(vector<int>& victor)
{
    vector<int> v = victor;
    reverse(v.begin(), v.end()); // ������������� ������
    return v;
}

// ������� ��� ����������� �������� � �������������� 3-opt
vector<int> opt3(vector<int> a, int N, const vector<vector<double>>& mat)
{
    if (N < 10) return a; // ���� ���������� ����� ������ 10, �� ��������� �����������

    int d52 = 0, i, k, j, ii, A_size, N1 = N - 4, N2 = N - 3, N3 = N - 1;
    double min_distance = total_distance(a, mat), distance;
    bool fl = true;
    vector<int> b = a, v1, v2, v3, v4, v2_rev, v3_rev;
    vector<vector<int>> A;
    vector<int>::iterator beg;

    // �������� ���� �����������
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
                    v1.assign(beg, beg + i); // ��������� ������� �� 4 �����
                    v2.assign(beg + i, beg + j);
                    v3.assign(beg + j, beg + k);
                    v4.assign(beg + k, a.end());

                    if (!v1.empty() && !v2.empty() && !v3.empty() && !v4.empty())
                    {
                        v3_rev = reverse_vector(v3); // ������������� ������ v3
                        v2_rev = reverse_vector(v2); // ������������� ������ v2

                        // ���������� ��� ��������� ��������, ���������� ����� ������������ ���������
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

                        // �������� �� ���� ��������� ��������� � �������� ���, ������� ������������ ����������
                        ii = 0;
                        for (ii; ii < A_size; ++ii)
                        {
                            distance = total_distance(A[ii], mat);

                            #pragma omp critical
                            {
                                if (distance < min_distance)
                                {
                                    min_distance = distance; // ��������� ���������� ����������
                                    b = A[ii]; // ��������� ����� ������ �������
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
            a = b; // ���� ��� ������� �������, ��������� ���
        }
    }

    return a;
}

int main()
{
    // �������� ����� � �������� �������
    ifstream file("tsp_51_1");
    int N, i;
    file >> N;

    // ������������� ���������� ��������
    vector<int> a(N);
    for (i = 0; i < N; i++)
    {
        // ������ ��������� ������� 1 2 3 4 ...
        a[i] = i;
    }

    // ������ ��������� �����
    vector<Tochechka> coordinats(N);
    for (i = 0; i < N; i++)
    {
        // ������ ���������� ������ �����
        file >> coordinats[i].x >> coordinats[i].y;
    }

    // ��������� ����
    file.close();

    high_resolution_clock::time_point start, end;
    long long time;
    double distance;

    // ������ ������� ���������� ����� �������
    vector<vector<double>> dist_matrix = build_distance_matrix(coordinats);

    // �������� ����� ������� ���������� �����������
    start = high_resolution_clock::now();
    // ����������� �������� � �������������� 3-opt
    a = opt3(a, a.size(), dist_matrix);
    // ����������� ����� ������� ���������� �����������
    end = high_resolution_clock::now();

    // ������������ ����� ����������
    time = duration_cast<milliseconds>(end - start).count();

    // ������������ �������� ����� ��������
    distance = total_distance(a, dist_matrix);

    // ������� �������� ���������
    cout << fixed << distance << " 1" << endl;

    for (i = 0; i < a.size(); i++)
    {
        cout << a[i] << " "; // ������� ������� ����� ��������
    }

    cout << endl;

    // ������� ����� ������ ��������� opt3 � ��������
    cout << "Execution time of opt3: " << time << " milliseconds" << endl;

    return 0;
}
