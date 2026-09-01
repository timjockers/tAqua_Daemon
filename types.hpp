#pragma once

#include <cstddef>
#include <array>
#include <algorithm>


// The 3 possible relay configurations
enum class RelayConfig {
    UNUSED,
    VALVE,
    PERMANENTPOWER
};


template <typename Enum, std::size_t N>
std::array<unsigned int, N> toUIntArray(const std::array<Enum, N>& input)
{
    std::array<unsigned int, N> output{};

    std::transform(
        input.begin(),
        input.end(),
        output.begin(),
        [](Enum value) {
            return static_cast<unsigned int>(value);
        }
    );

    return output;
}


// Relay pins
enum class Relay {
    R1 = 18,
    R2 = 23,
    R3 = 24,
    R4 = 25,
    R5 = 12,
    R6 = 16,
    R7 = 20,
    R8 = 21
};

constexpr std::array<Relay, 8> RELAYS = {
    Relay::R1,
    Relay::R2,
    Relay::R3,
    Relay::R4,
    Relay::R5,
    Relay::R6,
    Relay::R7,
    Relay::R8
};

constexpr std::size_t relayIndex(Relay relay)
{
    switch (relay) {
        case Relay::R1: return 0;
        case Relay::R2: return 1;
        case Relay::R3: return 2;
        case Relay::R4: return 3;
        case Relay::R5: return 4;
        case Relay::R6: return 5;
        case Relay::R7: return 6;
        case Relay::R8: return 7;
    }

    return 0;
}


// Yellow LED pins
enum class YLED {
    L1 = 18,
    L2 = 23,
    L3 = 24,
    L4 = 25,
    L5 = 12,
    L6 = 16,
    L7 = 20,
    L8 = 21
};

constexpr std::array<YLED, 8> YLEDS = {
    YLED::L1,
    YLED::L2,
    YLED::L3,
    YLED::L4,
    YLED::L5,
    YLED::L6,
    YLED::L7,
    YLED::L8
};

constexpr std::size_t YLEDIndex(YLED yled)
{
    switch (yled) {
        case YLED::L1: return 0;
        case YLED::L2: return 1;
        case YLED::L3: return 2;
        case YLED::L4: return 3;
        case YLED::L5: return 4;
        case YLED::L6: return 5;
        case YLED::L7: return 6;
        case YLED::L8: return 7;
    }

    return 0;
}
