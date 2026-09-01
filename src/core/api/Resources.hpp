#include "core/Application.hpp"

namespace Qi {

class Resources {

public:
    template<typename T>
    static std::shared_ptr<T> Load(const std::string& path) {
        return Application::get().getResourceLoader().Load<T>(path);
    }

    template<typename T, typename... Args>
    static std::shared_ptr<T> Create(Args&&... args) {
        return Application::get().getResourceLoader().Create<T>(std::forward<Args>(args)...);
    }
};

}
