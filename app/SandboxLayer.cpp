#include "SandboxLayer.hpp"
#include "QiEngine.hpp"
#include "core/Application.hpp"
#include "imgui.h"
#include "events/VsyncEvent.hpp"

SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
}

void SandboxLayer::onAttach() {
    m_scene = QiNew<SandboxScene>();
    m_scene->onReady();
}

void SandboxLayer::onDetach() {
    QiDelete(m_scene);
    m_scene = nullptr;
}

void SandboxLayer::onUpdate(Qi::Timestep ts) {
    m_timeStep = ts.GetSeconds();
    m_scene->onTick(ts);

}
void SandboxLayer::onEvent(Qi::Event& e) {
    m_scene->onEvent(e);
}

void SandboxLayer::onImGuiRender() {
    ImGui::Begin("Debug");

    static float fpsHistory[100] = {};
    static float ftHistory[100] = {};
    static int offset = 0;

    static float sampleTimer = 0.0f;

    float fps = 1.0f / m_timeStep;
    float ft = m_timeStep * 1000.0f;

    sampleTimer += m_timeStep;

    if (sampleTimer >= 0.1f) { // sample 10x/sec
        fpsHistory[offset] = fps;
        ftHistory[offset] = ft;

        offset = (offset + 1) % 100;

        sampleTimer = 0.0f;
    }

    Qi::Application& app = Qi::Application::get();

    float maxFps = *std::max_element(fpsHistory, fpsHistory + 100);
    float maxFt = *std::max_element(ftHistory, ftHistory + 100);

    if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("FPS: %.1f", fps);
        ImGui::PlotLines("##fps", fpsHistory, 100, offset, "FPS", 0.0f, maxFps * 1.1f, ImVec2(0, 50));
        ImGui::Text("Frame Time: %.3f ms", ft);
        ImGui::PlotLines("##ft", ftHistory, 100, offset, "Frame Time", 0.0f, maxFt, ImVec2(0, 50));
        ImGui::Text("Update Time: %.3f ms", app.getUpdateTimeMs());
        ImGui::Text("Render Time: %.3f ms", app.getRenderTimeMs());
        ImGui::Text("GPU Frame Time: --");
    }

    if (ImGui::CollapsingHeader("Renderer")) {
        ImGui::Text("Backend: Vulkan");
        ImGui::Text("GPU: placeholder");
        ImGui::Text("Draw Calls: 0");
        ImGui::Text("Quads: 0");
        ImGui::Text("Textures: 0");
        ImGui::Text("Pipeline: placeholder");
    }

    if (ImGui::CollapsingHeader("Scene")) {
        ImGui::Text("Entities: 0");
        ImGui::Text("Selected: None");

        if (ImGui::TreeNode("Transform")) {
            static float pos[3] = { 0, 0, 0 };
            static float rot[3] = { 0, 0, 0 };
            static float scale[3] = { 1, 1, 1 };

            ImGui::DragFloat3("Position", pos, 0.1f);
            ImGui::DragFloat3("Rotation", rot, 0.1f);
            ImGui::DragFloat3("Scale", scale, 0.1f);

            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Camera")) {
        static float camPos[3] = { 0, 0, 0 };
        static float zoom = 1.0f;

        ImGui::DragFloat3("Position", camPos, 0.1f);
        ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 100.0f);
        ImGui::Text("Projection: Orthographic");
        ImGui::Text("Active Camera: None");
    }

    if (ImGui::CollapsingHeader("Window")) {
        static bool vsync = false;

        ImGui::Text("API: Vulkan");
        ImGui::Text("Resolution: 1280 x 720");
        if (ImGui::Checkbox("VSync", &vsync)) {
            Qi::VSyncEvent event(vsync);
            Qi::Application::get().onEvent(event);
        }
    }

    if (ImGui::CollapsingHeader("Assets")) {
        ImGui::Text("Loaded Textures: 0");
        ImGui::Text("Texture Cache: 0");

        if (ImGui::Button("Reload Shaders")) {
            // TODO
        }
    }

    if (ImGui::CollapsingHeader("Debug Draw")) {
        static float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        static bool wireframe = false;
        static bool showCollision = false;
        static bool showBounds = false;
        static bool showGizmos = false;

        ImGui::ColorEdit4("Clear Color", clearColor);
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::Checkbox("Collision Boxes", &showCollision);
        ImGui::Checkbox("Sprite Bounds", &showBounds);
        ImGui::Checkbox("Transform Gizmos", &showGizmos);
    }

    ImGui::End();
}
