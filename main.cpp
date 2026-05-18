#include <bgui.hpp>

int main() {
    bgui::set_up();
    bgui::set_up_glfw(800, 600, "3DDemo");
    bgui::set_up_gl3();
    bgui::set_up_freetype();
    bgui::style_manager::get_instance().apply_theme(bgui::dark_theme());
    auto& w = bgui::get_layout().add<bgui::window>("OpenGl info");
    w.add<bgui::text>(std::string("gl version:\n ") + bgui::get_gl_version(), 0.35f);
    w.add<bgui::text>(std::string("gl vendor:\n ") + bgui::get_gl_vendor(), 0.35f);
    w.add<bgui::text>(std::string("glsl version:\n ") + bgui::get_glsl_version(), 0.35f);

    while (!bgui::should_close_glfw()) {
        bgui::glfw_update(bgui::get_context());
        bgui::on_update();
        bgui::gl3_render(bgui::get_draw_data());
        bgui::swap_glfw();
    }
    bgui::shutdown_freetype();
    bgui::shutdown_gl3();
    bgui::shutdown_glfw();
    bgui::shutdown_lib();

    return 0;
}