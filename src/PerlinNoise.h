#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <cmath>

class PerlinNoise
{
private:
    std::vector<int> permutations;

    double lerp(double t, double a, double b) const
    {
        return a + t * (b - a);
    }

    double fade(double t) const
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double grad(int hash, double x, double y) const
    {
        switch(hash & 7) {
        case 0: return  x + y; //(1, 1)
        case 1: return -x + y; //(-1, 1)
        case 2: return  x - y; //(1, -1)
        case 3: return -x - y; //(-1, -1)
        case 4: return  x; //(1, 0)
        case 5: return -x; //(-1, 0)
        case 6: return  y; //(0, 1)
        case 7: return -y; //(0, -1)
        default: return 0;
        }
    }

public:
    PerlinNoise(unsigned int seed = 0)
    {
        permutations.resize(256);

        std::iota(permutations.begin(), permutations.end(), 0);

        std::mt19937 generator(seed);
        std::shuffle(permutations.begin(), permutations.end(), generator);

        permutations.insert(permutations.end(), permutations.begin(), permutations.end());
    }

    double noise(double x, double y) const
    {
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;

        x -= floor(x);
        y -= floor(y);

        double u = fade(x);
        double v = fade(y);

        int A = permutations[X] + Y;
        int B = permutations[X + 1] + Y;

        double res = lerp(v,
                          lerp(u, grad(permutations[A], x, y), grad(permutations[B], x - 1, y)),
                          lerp(u, grad(permutations[A + 1], x, y - 1), grad(permutations[B + 1], x - 1, y - 1)));

        return (res + 1.0) / 2.0;
    }

    double octaveNoise(double x, double y, int octaves, double persistence) const
    {
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double maxValue = 0;

        for(int i=0; i<octaves; i++) {
            total += noise(x * frequency, y * frequency) * amplitude;

            maxValue += amplitude;

            amplitude *= persistence;
            frequency *= 2;
        }

        return total / maxValue;
    }
};

#endif // PERLINNOISE_H
