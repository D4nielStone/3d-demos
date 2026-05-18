#include <bgui.hpp>

int main() {
    bgui::set_up();
    bgui::set_up_glfw(800, 600, "3DDemo");
    bgui::set_up_gl3();
    bgui::set_up_freetype();
    bgui::style_manager::get_instance().apply_theme(bgui::dark_theme());
    auto& w = bgui::get_layout().add<bgui::window>("infos");
    auto& w2 = bgui::get_layout().add<bgui::window>("infos");
    bgui::style_manager::get_instance().set_class("transparency", bgui::style{
        .visual = {
            .background = bgui::color{0, 5, 5, 0.5f}
        }
    });
    bgui::style_manager::get_instance().set_class("center", bgui::style{
        .layout = {
            .size_mode = bgui::vec<2, bgui::mode>{bgui::mode::match_parent, bgui::mode::wrap_content},
            .align = bgui::vec<2, bgui::alignment>{bgui::alignment::center, bgui::alignment::center}
        }
    });
    w2.add_class("transparency");
    w2.add<bgui::text>("Hello world", 0.5f).add_class("center");
    w2.add<bgui::text>("Hello world", 0.5f).add_class("center");
    w2.add<bgui::text>("Hello world", 0.5f).add_class("center");
    w2.add<bgui::text>("Hello world", 0.5f).add_class("center");
    w2.add<bgui::text>("Hello world", 0.5f).add_class("center");

    while (!bgui::should_close_glfw()) {
        bgui::on_update();
        bgui::glfw_update(bgui::get_context());
        bgui::gl3_render(bgui::get_draw_data());
        bgui::swap_glfw();
    }
    bgui::shutdown_freetype();
    bgui::shutdown_gl3();
    bgui::shutdown_glfw();
    bgui::shutdown_lib();

    return 0;
}