#pragma once
#include "imports.hpp"
#include "hack.hpp"

#define RAGEMP
using namespace std;

class VisualsPlayer : public Singleton<VisualsPlayer>, Hack
{
public:
    void Tick();

    // Geerbt �ber Hack
    virtual std::string getCategory() override;
    virtual std::string getName() override;
};