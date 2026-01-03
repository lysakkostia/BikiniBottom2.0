#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <random>

namespace RandGenerator
{
    inline std::mt19937& generator()
    {
        static std::mt19937 gen{ std::random_device{}() };
        return gen;
    }

    inline int randIntInInterval(int left, int right)
    {
        std::uniform_int_distribution<int> dist(left, right);
        return dist(generator());
    }

    inline double randDoubleInInterval(double left, double right)
    {
        std::uniform_real_distribution<double> dist(left, right);
        return dist(generator());
    }
}

#endif // RANDOMGENERATOR_H
