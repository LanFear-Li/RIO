#include "renderer.hpp"

int main()
{
    Renderer renderer;

    renderer.load();
    renderer.run();

    Renderer::exit();
    return 0;
}
