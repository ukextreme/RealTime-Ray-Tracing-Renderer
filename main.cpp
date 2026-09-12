// Real-time ray tracing renderer — minimal path-traced core.
// Build: g++ -std=c++20 -O2 main.cpp -o rt && ./rt > out.ppm

#include <cmath>
#include <cstdio>
#include <limits>
#include <random>
#include <vector>

struct Vec3 {
    double x{}, y{}, z{};
    Vec3 operator+(const Vec3& b) const { return {x + b.x, y + b.y, z + b.z}; }
    Vec3 operator-(const Vec3& b) const { return {x - b.x, y - b.y, z - b.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    Vec3 operator*(const Vec3& b) const { return {x * b.x, y * b.y, z * b.z}; }
    double dot(const Vec3& b) const { return x * b.x + y * b.y + z * b.z; }
    Vec3 unit() const { double l = std::sqrt(dot(*this)); return {x / l, y / l, z / l}; }
};

struct Sphere {
    Vec3 center;
    double radius;
    Vec3 albedo;
    Vec3 emission;
};

struct Hit {
    double t{std::numeric_limits<double>::infinity()};
    Vec3 point, normal;
    const Sphere* obj{nullptr};
};

const std::vector<Sphere> kScene = {
    {{0, -1001, -6}, 1000, {0.75, 0.75, 0.75}, {0, 0, 0}},   // floor
    {{-1.1, 0, -6}, 1.0, {0.85, 0.25, 0.25}, {0, 0, 0}},     // red
    {{1.1, 0, -6}, 1.0, {0.25, 0.45, 0.85}, {0, 0, 0}},      // blue
    {{0, 4.2, -6}, 2.0, {0, 0, 0}, {6, 6, 6}},               // light
};

// Analytic ray-sphere intersection; returns nearest hit in front of the origin.
Hit trace(const Vec3& origin, const Vec3& dir) {
    Hit best;
    for (const auto& s : kScene) {
        Vec3 oc = origin - s.center;
        double b = oc.dot(dir), c = oc.dot(oc) - s.radius * s.radius;
        double disc = b * b - c;
        if (disc < 0) continue;
        double t = -b - std::sqrt(disc);
        if (t < 1e-4) t = -b + std::sqrt(disc);
        if (t > 1e-4 && t < best.t) {
            best.t = t;
            best.point = origin + dir * t;
            best.normal = (best.point - s.center).unit();
            best.obj = &s;
        }
    }
    return best;
}

Vec3 randomHemisphere(const Vec3& n, std::mt19937& rng) {
    std::uniform_real_distribution<double> u(-1.0, 1.0);
    Vec3 d;
    do { d = {u(rng), u(rng), u(rng)}; } while (d.dot(d) > 1.0);
    d = d.unit();
    return d.dot(n) < 0 ? d * -1.0 : d;
}

Vec3 radiance(const Vec3& origin, const Vec3& dir, int depth, std::mt19937& rng) {
    if (depth <= 0) return {0, 0, 0};
    Hit h = trace(origin, dir);
    if (!h.obj) return {0.55, 0.7, 1.0};  // sky
    Vec3 bounce = radiance(h.point, randomHemisphere(h.normal, rng), depth - 1, rng);
    return h.obj->emission + h.obj->albedo * bounce;
}

int main() {
    constexpr int kWidth = 320, kHeight = 180, kSamples = 32, kDepth = 5;
    std::mt19937 rng(1337);
    std::uniform_real_distribution<double> jitter(0.0, 1.0);

    std::printf("P3\n%d %d\n255\n", kWidth, kHeight);
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            Vec3 c{};
            for (int s = 0; s < kSamples; ++s) {
                double u = (x + jitter(rng)) / kWidth * 2 - 1;
                double v = 1 - (y + jitter(rng)) / kHeight * 2;
                u *= double(kWidth) / kHeight;
                c = c + radiance({0, 0.5, 0}, Vec3{u, v, -1}.unit(), kDepth, rng);
            }
            c = c * (1.0 / kSamples);
            auto encode = [](double v) {
                return int(255.0 * std::pow(v < 0 ? 0 : v > 1 ? 1 : v, 1 / 2.2) + 0.5);
            };
            std::printf("%d %d %d\n", encode(c.x), encode(c.y), encode(c.z));
        }
    }
    return 0;
}
