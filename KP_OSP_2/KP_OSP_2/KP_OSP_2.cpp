#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>
using namespace std;
 
const double PI = acos(-1.0);
const double EPS = 1e-12;
 
struct Pt {
    double x, y;
};
 
Pt operator+(Pt a, Pt b) { return {a.x + b.x, a.y + b.y}; }
Pt operator-(Pt a, Pt b) { return {a.x - b.x, a.y - b.y}; }
Pt operator*(double k, Pt a) { return {k * a.x, k * a.y}; }
double dot(Pt a, Pt b) { return a.x * b.x + a.y * b.y; }
double cross(Pt a, Pt b) { return a.x * b.y - a.y * b.x; }
double dist2(Pt a, Pt b) { Pt d = a - b; return dot(d, d); }
 
// лексикографічне порівняння (спочатку x, потім y)
bool lexLess(const Pt &a, const Pt &b) {
    if (a.x != b.x) return a.x < b.x;
    return a.y < b.y;
}
 
// ---------- Правило 1: точка D ----------
Pt centerD(Pt A, Pt B, Pt C) {
    Pt b = B - A, c = C - A;
    double cr = cross(b, c);
    double scale = max(1.0, sqrt(dot(b, b) * dot(c, c)));
    if (fabs(cr) < 1e-12 * scale) {
        // колінеарні: середнє геометричне полярних координат (r, phi)
        // геометричне середнє = кубічний корінь добутку (cbrt зберігає знак)
        Pt p[3] = {A, B, C};
        double rp = 1.0, fp = 1.0;
        for (int i = 0; i < 3; i++) {
            rp *= hypot(p[i].x, p[i].y);
            fp *= atan2(p[i].y, p[i].x);
        }
        double r = cbrt(rp), f = cbrt(fp);
        return {r * cos(f), r * sin(f)};
    }
    double d = 2.0 * cr;
    double b2 = dot(b, b), c2 = dot(c, c);
    double ux = (c.y * b2 - b.y * c2) / d;
    double uy = (b.x * c2 - c.x * b2) / d;
    return A + Pt{ux, uy};
}
 
// ---------- Один крок: повертає D,E,F,G,H ----------
array<Pt, 5> computeStep(Pt A, Pt B, Pt C) {
    Pt D = centerD(A, B, C);
 
    // E: симетрія C відносно прямої AD
    Pt v = D - A;
    Pt E;
    double vv = dot(v, v);
    if (vv < EPS) {
        E = C; // вироджений випадок: A == D
    } else {
        double t = dot(C - A, v) / vv;
        Pt proj = A + t * v;
        E = 2.0 * proj - C;
    }
 
    // F: поворот B навколо E на 2*pi/7
    double ang = 2.0 * PI / 7.0;
    Pt r = B - E;
    Pt F = E + Pt{r.x * cos(ang) - r.y * sin(ang),
                  r.x * sin(ang) + r.y * cos(ang)};
 
    // G: центр мас A,B,D,E,F з вагами 29,43,53,62,75
    Pt G = (1.0 / 262.0) * (29.0 * A + 43.0 * B + 53.0 * D + 62.0 * E + 75.0 * F);
 
    // H: паралельне перенесення A на вектор 15*DF + 23*EG
    Pt H = A + 15.0 * (F - D) + 23.0 * (G - E);
 
    return {D, E, F, G, H};
}
 
// ---------- Правило 7: вибір наступних A, B, C ----------
void chooseNext(const vector<Pt> &s, Pt &A, Pt &B, Pt &C) {
    // A: найменше x; B: найбільша відстань від початку; C: найбільше y
    // при рівності - перша лексикографічно
    Pt a = s[0], b = s[0], c = s[0];
    for (const Pt &p : s) {
        if (p.x < a.x || (p.x == a.x && lexLess(p, a))) a = p;
        double dp = dot(p, p), db = dot(b, b);
        if (dp > db || (dp == db && lexLess(p, b))) b = p;
        if (p.y > c.y || (p.y == c.y && lexLess(p, c))) c = p;
    }
    A = a; B = b; C = c;
}
 
// ---------- Площа опуклої оболонки (Andrew) ----------
double hullArea(vector<Pt> p) {
    int n = p.size();
    if (n < 3) return 0.0;
    sort(p.begin(), p.end(), lexLess);
    vector<Pt> h(2 * n);
    int k = 0;
    for (int i = 0; i < n; i++) {
        while (k >= 2 && cross(h[k - 1] - h[k - 2], p[i] - h[k - 2]) <= 0) k--;
        h[k++] = p[i];
    }
    for (int i = n - 2, t = k + 1; i >= 0; i--) {
        while (k >= t && cross(h[k - 1] - h[k - 2], p[i] - h[k - 2]) <= 0) k--;
        h[k++] = p[i];
    }
    h.resize(k - 1);
    double s = 0;
    for (size_t i = 0; i < h.size(); i++)
        s += cross(h[i], h[(i + 1) % h.size()]);
    return fabs(s) / 2.0;
}
 
void printPt(Pt p) {
    cout << "(" << p.x << ", " << p.y << ")";
}
 
// ---------- a) перші N елементів ----------
void partA() {
    Pt A, B, C;
    long long N;
    cout << "Введіть A, B, C (x y для кожної) та N: ";
    cin >> A.x >> A.y >> B.x >> B.y >> C.x >> C.y >> N;
 
    vector<Pt> seq = {A, B, C};
    while ((long long)seq.size() < N) {
        auto r = computeStep(A, B, C);
        vector<Pt> sub = {A, B, C};
        for (auto &p : r) { seq.push_back(p); sub.push_back(p); }
        chooseNext(sub, A, B, C);
    }
    seq.resize(N);
    for (size_t i = 0; i < seq.size(); i++) {
        cout << i + 1 << ": ";
        printPt(seq[i]);
        cout << "\n";
    }
}
 
// ---------- b) обчислення до зупинки + пари точок ----------
void partB() {
    Pt A, B, C;
    double Alim;
    long long M;
    cout << "Введіть A, B, C (x y для кожної), площу Alim та M: ";
    cin >> A.x >> A.y >> B.x >> B.y >> C.x >> C.y >> Alim >> M;
 
    vector<Pt> seq = {A, B, C};
    long long steps = 0;
    while (true) {
        auto r = computeStep(A, B, C);
        vector<Pt> sub = {A, B, C};
        for (auto &p : r) { seq.push_back(p); sub.push_back(p); }
        steps++;
        double area = hullArea(seq);
        if (area > Alim) {
            cout << "Зупинка: площа " << area << " > " << Alim << "\n";
            break;
        }
        if (steps > M) {
            cout << "Зупинка: кількість кроків " << steps << " > " << M << "\n";
            break;
        }
        chooseNext(sub, A, B, C);
    }
    int n = seq.size();
    cout << "Кроків: " << steps << ", точок у послідовності: " << n << "\n";
 
    const size_t KMAX = 39, KMIN = 26;
    typedef tuple<double, int, int> T;
    priority_queue<T, vector<T>, greater<T>> big;  // мін-купа, тримає 39 найбільших
    priority_queue<T> small;                        // макс-купа, тримає 26 найменших
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            double d = dist2(seq[i], seq[j]);
            if (big.size() < KMAX) big.push({d, i, j});
            else if (d > get<0>(big.top())) { big.pop(); big.push({d, i, j}); }
            if (small.size() < KMIN) small.push({d, i, j});
            else if (d < get<0>(small.top())) { small.pop(); small.push({d, i, j}); }
        }
 
    vector<T> vb, vs;
    while (!big.empty()) { vb.push_back(big.top()); big.pop(); }
    while (!small.empty()) { vs.push_back(small.top()); small.pop(); }
    reverse(vb.begin(), vb.end()); // від найбільшої до менших
    reverse(vs.begin(), vs.end()); // від найменшої до більших
 
    cout << "\n" << vb.size() << " пар з найбільшими відстанями:\n";
    for (auto &t : vb) {
        cout << sqrt(get<0>(t)) << "  ";
        printPt(seq[get<1>(t)]); cout << " - "; printPt(seq[get<2>(t)]); cout << "\n";
    }
    cout << "\n" << vs.size() << " пар з найменшими відстанями:\n";
    for (auto &t : vs) {
        cout << sqrt(get<0>(t)) << "  ";
        printPt(seq[get<1>(t)]); cout << " - "; printPt(seq[get<2>(t)]); cout << "\n";
    }
}
 
// ---------- c) цілочисельний варіант ----------
double roundClamp(double v, int L) {
    if (std::isnan(v)) return 0;
    double lim = L - 1;
    v = std::round(v);
    if (v > lim) v = lim;
    if (v < -lim) v = -lim;
    return v;
}
 
// повертає 0 - площа, 1 - кроки, 2 - <2 нових точок
int runC(Pt A, Pt B, Pt C, int L, double Alim, long long M) {
    vector<Pt> seq = {A, B, C};
    set<pair<int, int>> used;
    for (auto &p : seq) used.insert({(int)p.x, (int)p.y});
    long long steps = 0;
    while (true) {
        auto r = computeStep(A, B, C);
        vector<Pt> sub = {A, B, C};
        int added = 0;
        for (auto &p : r) {
            Pt q = {roundClamp(p.x, L), roundClamp(p.y, L)};
            sub.push_back(q);
            if (used.insert({(int)q.x, (int)q.y}).second) {
                seq.push_back(q);
                added++;
            }
        }
        steps++;
        if (hullArea(seq) > Alim) return 0;
        if (steps > M) return 1;
        if (added < 2) return 2;
        chooseNext(sub, A, B, C);
    }
}
 
void partC() {
    int L;
    double Alim;
    long long M;
    cout << "Введіть L, площу Alim та M: ";
    cin >> L >> Alim >> M;
    if (L < 1) { cout << "L має бути натуральним\n"; return; }
 
    vector<Pt> grid;
    for (int x = -(L - 1); x <= L - 1; x++)
        for (int y = -(L - 1); y <= L - 1; y++)
            grid.push_back({(double)x, (double)y});
 
    long long cnt[3] = {0, 0, 0}, total = 0;
    int P = grid.size();
    // впорядковані трійки різних точок (порядок A, B, C впливає на результат)
    for (int i = 0; i < P; i++)
        for (int j = 0; j < P; j++) {
            if (j == i) continue;
            for (int k = 0; k < P; k++) {
                if (k == i || k == j) continue;
                cnt[runC(grid[i], grid[j], grid[k], L, Alim, M)]++;
                total++;
            }
        }
    if (total == 0) { cout << "Немає трійок\n"; return; }
    cout << fixed << setprecision(4);
    cout << "Усього трійок: " << total << "\n";
    cout << "Зупинка за площею:        " << 100.0 * cnt[0] / total << " %\n";
    cout << "Зупинка за кількістю кроків: " << 100.0 * cnt[1] / total << " %\n";
    cout << "Зупинка (<2 нових точок):    " << 100.0 * cnt[2] / total << " %\n";
}
 
int main() {
    cout << setprecision(10);
    cout << "Оберіть частину (a, b, c): ";
    char ch;
    cin >> ch;
    if (ch == 'a' || ch == 'A') partA();
    else if (ch == 'b' || ch == 'B') partB();
    else if (ch == 'c' || ch == 'C') partC();
    else cout << "Невідома частина\n";
    return 0;
}