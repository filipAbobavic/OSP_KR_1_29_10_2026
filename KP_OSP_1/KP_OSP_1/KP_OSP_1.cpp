#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

long long N = 7;

//Остача по модулю
long long mod(long long x)
{
    return (x % N + N) % N;
}

//Додавання по модулю
long long add(long long a, long long b)
{
    return mod(a + b);
}

//Віднімання по модулю
long long sub(long long a, long long b)
{
    return mod(a - b);
}

//Множення по модулю
long long mul(long long a, long long b)
{
    return mod(a * b);
}

long long power(long long a, long long b)
{
    return mod(pow(a, b));
}

//Перевірка на просте число
bool is_prime(long long x)
{
    if (x < 2)
        return false;
    
    for (long long i = 2; i < x; i++)
        if (x % i == 0) return false;
    return true;
}

//Перевірка на наявність простого числа
bool has_prime(vector<long long> v)
{
    for (long long i : v)
        if (is_prime(i)) return true;
    return false;
}

//Обчислення наступного кортежу
vector<long long> next_cot(vector<long long> v)
{
    vector<long long> nc;
    if (v.size() == 2)
         nc = {
        power(v[0], v[1]),
        power(v[1], v[0]),
        add(71, mul(power(v[0], 7), power(v[1], 13) ))
    };
    else 
         nc ={
        add(mul(7, power(v[0], v[1])), mul(19, power(v[2], v[0])) ),
        sub(mul(31, power(v[2], v[1])), mul(47, power(v[0], add(v[2], v[1]))) )
    };
    
    if (v.size() == 2 && has_prime(nc))
        return {
        nc[0],
        add(power(nc[0], nc[1]), power(nc[1], nc[0])),
        nc[1]
    };
    if (v.size() == 3 && has_prime(nc))
        return {
        add(power(nc[0], nc[1]), power(nc[2], nc[0])),
        sub(power(nc[1], nc[2]), power(nc[1], nc[0]))
    };
    return nc;
}


//Завдання a
vector<long long> a_n(vector<long long> v, long long n)
{
    if (n == 0) return v;
    
    vector<long long> a = next_cot(v);
    for (long long i = 1; i < n; i++) a = next_cot(a);
    return a;
}

//Завдання b
void task_b(vector<long long> v)
{
    vector<vector<long long>> j = {v};
    vector<long long> a = next_cot(v);
    
    j.push_back(a);
    bool is_repeated = false;
    
    if (a == v) is_repeated = true;
    
    //Створення масиву кортежів
    while (!is_repeated)
    {
        a = next_cot(a);
        j.push_back(a);
        for (long long i = 0; i < j.size() - 1; i++) 
            if (j[j.size() - 1] == j[i]) is_repeated = true;
    }
    
    
    //Виведення масиву кортежів
    for (long long i = 0; i < j.size(); i++)
    {
        if (j[i].size() == 2) cout << "(" << j[i][0] << ", " << j[i][1] << ") " << endl;
        else cout << "(" << j[i][0] << ", " << j[i][1] << ", " << j[i][2] <<") " << endl;
    }
    
    //Кількість кроків
    cout << "Number of steps: " << j.size() - 1 << endl; 
    
    //Максимальний елемент
    long long max_number = 0;

    for (long long i = 0; i < j.size(); i++)
        max_number = max(max_number,*max_element(j[i].begin(), j[i].end()));
    cout << "Max element: " << max_number << endl;
}

//Завдання с
void task_c(vector<long long> s)
{
    vector<vector<long long>> duo;
    vector<vector<long long>> trio;

    for (long long i = 0; i < s.size(); i++)
        for (long long j = 0; j < s.size(); j++)
        {
            duo.push_back({s[i], s[j]});

            for (long long k = 0; k < s.size(); k++)
                trio.push_back({s[i], s[j], s[k]});
        }

    vector<vector<vector<long long>>> tv;
    vector<long long> steps;
    vector<long long> MaxEl;

    // Обробка пар
    for (long long i = 0; i < duo.size(); i++)
    {
        vector<vector<long long>> current;
        vector<long long> a = duo[i];

        while (true)
        {
            bool is_repeated = false;

            for (long long j = 0; j < current.size(); j++)
            {
                if (current[j] == a)
                {
                    is_repeated = true;
                    break;
                }
            }

            if (is_repeated)
                break;

            current.push_back(a);
            MaxEl.push_back(*max_element(a.begin(), a.end()));
            a = next_cot(a);
        }

        steps.push_back(current.size());
        tv.push_back(current);
    }

    // Обробка трійок
    for (long long i = 0; i < trio.size(); i++)
    {
        vector<vector<long long>> current;
        vector<long long> a = trio[i];

        while (true)
        {
            bool is_repeated = false;

            for (long long j = 0; j < current.size(); j++)
            {
                if (current[j] == a)
                {
                    is_repeated = true;
                    break;
                }
            }

            if (is_repeated)
                break;

            current.push_back(a);
            MaxEl.push_back(*max_element(a.begin(), a.end()));
            a = next_cot(a);
        }

        steps.push_back(current.size());
        tv.push_back(current);
    }

    // Сортування
    sort(steps.begin(), steps.end(), greater<long long>());
    sort(MaxEl.begin(), MaxEl.end(), greater<long long>());

    // Виведення 37 найбільших кількостей кроків
    cout << "\nMax steps: ";

    for (long long i = 0; i < min(37LL, (long long)steps.size()); i++)
        cout << steps[i] << " ";

    cout << endl;

    // Виведення 29 найбільших значень
    cout << "Max elements: ";

    for (long long i = 0; i < min(29LL, (long long)MaxEl.size()); i++)
        cout << MaxEl[i] << " ";

    cout << endl;

    // Виведення унікальних циклів
    cout << "\nUnique cycles:" << endl;

    vector<vector<vector<long long>>> cycles;

    for (long long i = 0; i < tv.size(); i++)
    {
        vector<vector<long long>> cycle;
        long long start = 0;

        // Знаходимо перший кортеж циклу
        vector<long long> a = next_cot(tv[i].back());

        for (long long j = 0; j < tv[i].size(); j++)
        {
            if (tv[i][j] == a)
            {
                start = j;
                break;
            }
        }

        // Записуємо кортежі циклу
        for (long long j = start; j < tv[i].size(); j++)
            cycle.push_back(tv[i][j]);

        // Перевірка унікальності з урахуванням різного початку циклу
        bool found = false;

        for (long long j = 0; j < cycles.size(); j++)
        {
            if (cycles[j].size() != cycle.size())
                continue;

            for (long long k = 0; k < cycle.size(); k++)
            {
                bool equal = true;

                for (long long l = 0; l < cycle.size(); l++)
                {
                    if (cycle[l] != cycles[j][(k + l) % cycle.size()])
                    {
                        equal = false;
                        break;
                    }
                }

                if (equal)
                {
                    found = true;
                    break;
                }
            }

            if (found)
                break;
        }

        if (!found)
        {
            cycles.push_back(cycle);

            cout << "Cycle " << cycles.size() << ": ";

            for (long long j = 0; j < cycle.size(); j++)
            {
                cout << "(";

                for (long long k = 0; k < cycle[j].size(); k++)
                {
                    cout << cycle[j][k];

                    if (k != cycle[j].size() - 1)
                        cout << ", ";
                }

                cout << ") ";
            }

            cout << endl;
        }
    }

    cout << "Number of unique cycles: " << cycles.size() << endl;
}

int main()
{
    cout << "For task a" << endl;
    cout << endl;
    cout << "Enter n" << endl;
    long long n;
    cin >> n;
    cout << endl;
    cout << "For tasks a and b" << endl;
    cout << endl;
    cout << "Enter length of cottage(2 or 3)" << endl;
    long long length;
    cin >> length;
    cout << "Enter the cottage" << endl;
    vector<long long> v(length);
    for (long long i = 0; i < length; i++)
        cin >> v[i]; 
    
    vector<long long> res_a = a_n(v, n);
    for (long long i : res_a) cout << "Taks a: " << i << ' ';
    cout << endl;
    cout << "Taks b: ";
    task_b(v);
    cout << endl;
    //Task 3
    cout << "Taks c: ";
    cout << "How many numbers do you want to have?" << endl;
    long long num;
    cin >> num;
    vector<long long> s(num);
    cout << "Enter the set: " << endl;
    for (long long i = 0; i < num; i++) cin >> s[i];
    task_c(s);
    
    
    return 0;
}
