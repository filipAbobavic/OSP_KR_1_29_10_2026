
#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

// ======================= Вхідні функції =======================
// Їх можна замінити на будь-які інші.

// Швидкість частинок v(h), м/с
double v(double h) { return 10.0 + 0.01 * h; }

// Густина частинок rho(h), кг/м^3 (експоненційна атмосфера)
double rho(double h) { return 1.225 * std::exp(-h / 8500.0); }

// Підінтегральна функція: щільність кінетичної енергії на одиницю висоти
double f(double h) {
    double vel = v(h);
    return 0.5 * rho(h) * vel * vel;
}

// ======================= Адаптивний Сімпсон =======================
static double simpsonRec(double a, double b, double eps, double whole,
                         double fa, double fm, double fb, int depth) {
    const int MIN_DEPTH = 3;   // примусове подрібнення, щоб не пропустити особливості
    const int MAX_DEPTH = 50;

    double m = 0.5 * (a + b);
    double lm = 0.5 * (a + m);
    double rm = 0.5 * (m + b);
    double flm = f(lm), frm = f(rm);

    double left = (m - a) / 6.0 * (fa + 4.0 * flm + fm);
    double right = (b - m) / 6.0 * (fm + 4.0 * frm + fb);
    double delta = left + right - whole;

    if (depth >= MAX_DEPTH || (depth >= MIN_DEPTH && std::fabs(delta) <= 15.0 * eps))
        return left + right + delta / 15.0;

    return simpsonRec(a, m, eps / 2.0, left, fa, flm, fm, depth + 1) +
           simpsonRec(m, b, eps / 2.0, right, fm, frm, fb, depth + 1);
}

// Кінетична енергія фрагмента [a, b] з точністю eps  (завдання a)
double kineticEnergy(double a, double b, double eps) {
    if (b <= a) return 0.0;
    double fa = f(a), fb = f(b), fm = f(0.5 * (a + b));
    double whole = (b - a) / 6.0 * (fa + 4.0 * fm + fb);
    return simpsonRec(a, b, eps, whole, fa, fm, fb, 0);
}

// ======================= Завдання (b) =======================
// Повертає вектор висот розрізу s_i
std::vector<double> sliceAtmosphere(double a, double b,
                                    const std::vector<double>& w, double eps) {
    std::vector<double> s;
    s.reserve(w.size());
    double cur = a;

    for (double wi : w) {
        double remaining = kineticEnergy(cur, b, eps);
        double target = wi * remaining;

        double lo = cur, hi = b;
        for (int it = 0; it < 200 && (hi - lo) > eps; ++it) {
            double mid = 0.5 * (lo + hi);
            if (kineticEnergy(cur, mid, eps) < target)
                lo = mid;
            else
                hi = mid;
        }
        cur = 0.5 * (lo + hi);
        s.push_back(cur);
    }
    return s;
}

// ======================= Вимірювання часу =======================
struct Stats {
    double mean_ms;
    double max_ms;
};

Stats measure(const std::function<void()>& task, int runs) {
    std::vector<double> t;
    t.reserve(runs);
    for (int i = 0; i < runs; ++i) {
        auto t0 = std::chrono::steady_clock::now();
        task();
        auto t1 = std::chrono::steady_clock::now();
        t.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
    double mean = std::accumulate(t.begin(), t.end(), 0.0) / t.size();
    double mx = *std::max_element(t.begin(), t.end());
    return {mean, mx};
}

int main() {
    double a, b;
    std::cout << "Введіть a та b (a < b), м: ";
    if (!(std::cin >> a >> b) || a >= b) {
        std::cerr << "Некоректний діапазон висот.\n";
        return 1;
    }

    std::size_t n;
    std::cout << "Кількість коефіцієнтів w_i (1..1000): ";
    if (!(std::cin >> n) || n < 1 || n > 1000) {
        std::cerr << "Кількість має бути від 1 до 1000.\n";
        return 1;
    }

    std::vector<double> w(n);
    std::cout << "Введіть " << n << " коефіцієнтів 0 < w_i < 1:\n";
    for (auto& x : w) {
        if (!(std::cin >> x) || x <= 0.0 || x >= 1.0) {
            std::cerr << "Кожне w_i має бути в інтервалі (0, 1).\n";
            return 1;
        }
    }

    // ---------- Завдання (a) ----------
    const double eps_main = 1e-9;
    double E = kineticEnergy(a, b, eps_main);
    std::cout << std::setprecision(12);
    std::cout << "\n(a) E(" << a << ", " << b << ") = " << E << " Дж/м^2\n";

    // ---------- Завдання (b) ----------
    std::vector<double> s = sliceAtmosphere(a, b, w, eps_main);
    std::cout << "\n(b) Розрізи:\n";
    double prev = a;
    std::size_t shown = std::min<std::size_t>(n, 20);
    for (std::size_t i = 0; i < shown; ++i) {
        double part = kineticEnergy(prev, s[i], eps_main);
        double rest = kineticEnergy(prev, b, eps_main);
        std::cout << "  i=" << i + 1 << "  s=" << s[i]
                  << "  частка відрізаного = " << part / rest
                  << " (задано " << w[i] << ")\n";
        prev = s[i];
    }
    if (n > shown) std::cout << "  ... (показано перші " << shown << " з " << n << ")\n";

    // ---------- Завдання (c) ----------
    const int RUNS = 15;  // не менше 10
    const double eps_list[] = {1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10};

    std::cout << "\n(c) Залежність часу від точності (" << RUNS << " вимірів на значення)\n";
    std::cout << std::left << std::setw(10) << "eps"
              << std::setw(16) << "a: середнє,мс" << std::setw(16) << "a: макс,мс"
              << std::setw(16) << "b: середнє,мс" << std::setw(16) << "b: макс,мс" << "\n";

    volatile double sink = 0.0;  // щоб оптимізатор не видалив обчислення
    std::cout << std::setprecision(5) << std::fixed;
    for (double eps : eps_list) {
        Stats sa = measure([&] { sink = kineticEnergy(a, b, eps); }, RUNS);
        Stats sb = measure([&] {
            auto r = sliceAtmosphere(a, b, w, eps);
            sink = r.back();
        }, RUNS);

        std::cout << std::scientific << std::setprecision(0) << std::left
                  << std::setw(10) << eps << std::fixed << std::setprecision(5)
                  << std::setw(16) << sa.mean_ms << std::setw(16) << sa.max_ms
                  << std::setw(16) << sb.mean_ms << std::setw(16) << sb.max_ms << "\n";
    }
    (void)sink;
    return 0;
}
