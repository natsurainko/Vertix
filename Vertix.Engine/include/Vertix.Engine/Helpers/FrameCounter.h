//
// Created by Natsurainko on 2025/12/26.
//

#pragma once

#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

namespace Vertix::Engine {
    class FrameCounter {
        std::vector<double> deltaSamples;
        int                 sampleCount = 0;

    public:
        void IncrementCounter(const double deltaTime) {
            deltaSamples.push_back(deltaTime);
            sampleCount++;

            if (sampleCount >= 1000) {
                double sum = 0, minDelta = 1e9, maxDelta = 0;
                for (double dt : deltaSamples) {
                    sum      += dt;
                    minDelta = (std::min)(minDelta, dt);
                    maxDelta = (std::max)(maxDelta, dt);
                }

                const double avgDelta = sum / deltaSamples.size();

                std::cout << "=== Frame Time Analysis (1000 frames) ===" << std::endl;
                std::cout << "Average: " << (avgDelta * 1000.0) << " ms" << std::endl;
                std::cout << "Min: " << (minDelta * 1000.0) << " ms" << std::endl;
                std::cout << "Max: " << (maxDelta * 1000.0) << " ms" << std::endl;
                std::cout << "Expected (100Hz): 10.0 ms" << std::endl;
                std::cout << std::endl;

                deltaSamples.clear();
                sampleCount = 0;
            }
        }
    };
}
