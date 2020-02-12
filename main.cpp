#include "for_constexpr.hpp"
#include <array>
#include <iostream>
#include <vector>

struct object {};

template <typename T> struct node : object {
  void render() const { static_cast<T *>(this)->render(); }
  constexpr std::size_t get_id() const noexcept {
    return static_cast<T *>(this)->get_id();
  }
};

struct square : node<square> {
  void render() const { std::cout << "render square\n"; }
  constexpr std::size_t get_id() const noexcept { return 0u; }
};

struct circle : node<circle> {
  void render() const { std::cout << "render circle\n"; }
  constexpr std::size_t get_id() const noexcept { return 1u; }
};

template <typename T> struct type_is { using type = T; };

template <std::size_t type_id> struct get_type : type_is<object> {};
template <> struct get_type<0u> : type_is<square> {};
template <> struct get_type<1u> : type_is<circle> {};

template <std::size_t type_id>
using get_type_t = typename get_type<type_id>::type;

struct Scene {
  void render() {
    for_constexpr<for_bounds<0, 2>>([](auto i) {
      auto id = i;
      using curr_type = typename get_type<id>::type;
      for (const auto &node : m_nodes<curr_type>) {
        node.render();
      }
    });
  }

  template <typename T> void add_node(T node) {
    constexpr auto id = node.get_id();
    using curr_type = typename get_type<id>::type;
    m_nodes<curr_type>.push_back(std::move(node));
  }

  template <typename T> inline static std::vector<T> m_nodes;
  static constexpr std::array<std::size_t, 2> m_type_ids = {0u, 1u};
};

int main() {
  Scene scene;
  scene.add_node(square{});
  scene.add_node(square{});
  scene.add_node(circle{});
  scene.render();
}
