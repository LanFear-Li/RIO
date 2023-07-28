#include "renderer.hpp"

#include <utility>

Renderer::Renderer()
{
    config();

    // camera, shader and model creation
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    shader = new Shader(FileSystem::getPath("/shaders/shader.vert").c_str(), FileSystem::getPath("/shaders/shader.frag").c_str());
    model = new Model(FileSystem::getPath("assets/models/nanosuit/nanosuit.obj"));

    register_rx();
}

void Renderer::config()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Renderer", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(false);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void Renderer::load()
{
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
}

void Renderer::run()
{
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        shader->use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera->GetViewMatrix();
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // render the loaded model
        glm::mat4 mat_model = glm::mat4(1.0f);
        mat_model = glm::translate(mat_model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        mat_model = glm::scale(mat_model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        shader->setMat4("model", mat_model);
        model->Draw(*shader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Renderer::exit()
{
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

void Renderer::register_rx()
{
    // Register the lambda callback function
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Renderer::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->ProcessKeyboard(FORWARD, deltaTime * moveSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime * moveSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime * moveSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime * moveSpeed);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        upKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && upKeyPressed) {
        moveSpeed = moveSpeed * 2.0f;
        upKeyPressed = false;
    }

    // Control move speed with up and down.
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        downKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE && downKeyPressed) {
        moveSpeed = moveSpeed / 2.0f;
        downKeyPressed = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Renderer::mouse_cursor_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    auto* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (instance) {
        cout << "mouse_cursor_callback" << endl;
        if (!instance->rightMousePressd) {
            return;
        }

        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (instance->firstMouse) {
            instance->lastX = xpos;
            instance->lastY = ypos;
            instance->firstMouse = false;
        }

        float xoffset = xpos - instance->lastX;
        float yoffset = instance->lastY - ypos; // reversed since y-coordinates go from bottom to top

        instance->lastX = xpos;
        instance->lastY = ypos;

        instance->camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Renderer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (instance) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            instance->rightMousePressd = true;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            instance->rightMousePressd = false;
            instance->firstMouse = true;
        }
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Renderer::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* instance = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
