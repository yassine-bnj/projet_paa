#pragma once
#include <vector>
#include <string>

namespace Yalta {
    class Case;
    class Plateau;

    class IMouvement {
    public:
        virtual ~IMouvement() = default;
        virtual std::vector<Case*> calculerCoups(Case* origine, const Plateau& plateau) const = 0;
        virtual std::string getNom() const = 0;
    };
}