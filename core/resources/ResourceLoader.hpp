#include "Base.hpp"
#include "Resource.hpp"

namespace Qi {
class ResourceLoader {
public:
    static void Init();
    static void Shutdown();

    template<typename T>
    static Ref<T> Load(const std::string& path);

private:
    static std::unordered_map<std::string, Ref<Resource>> s_Cache;
};
}
