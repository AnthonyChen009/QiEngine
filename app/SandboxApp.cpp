#include "Application.hpp"
#include <iostream>
#include <QiEngine.hpp>
#include <memory>

class Sandbox : public Qi::Application {
public:
    Sandbox() {

    }
    ~Sandbox() {

    }
};

std::unique_ptr<Qi::Application> Qi::createApplication() {
    return std::make_unique<Sandbox>();
}
