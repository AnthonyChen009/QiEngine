#include "ImGuiLayer.hpp"
#include "core/Application.hpp"
#include "core/Window.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "renderer/Renderer.hpp"

namespace Qi {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {

}

void ImGuiLayer::onAttach() {
    Application& app = Application::get();
	Renderer::getBackend()->initImGui(&app.getWindow());
}

void ImGuiLayer::onDetach() {
    Renderer::getBackend()->shutdownImGui();
}

void ImGuiLayer::onEvent(Event& event) {
    if (m_blockEvents) {
        ImGuiIO& io = ImGui::GetIO();
        event.handled |= event.isInCategory(eventCategoryMouse) & io.WantCaptureMouse;
        event.handled |= event.isInCategory(eventCategoryKeyboard) & io.WantCaptureKeyboard;
    }
}

void ImGuiLayer::onImGuiRender() {
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiLayer::begin() {
    Renderer::getBackend()->beginImGuiFrame();
}

void ImGuiLayer::end() {
    Renderer::getBackend()->renderImGui();
}

}
