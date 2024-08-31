#include "renderer.hpp"

int main(int argc, char* argv[])
{
    auto scene_name = "default-scene";

    if (argc == 2) {
        scene_name = argv[1];
    }

    Renderer renderer(scene_name);
    renderer.run();
    return 0;
}
