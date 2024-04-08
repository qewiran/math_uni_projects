#include "../hdr/iter_method.hpp"
#include "../hdr/LUdecomp.hpp"
#include "../hdr/matrix.hpp"
#include "../hdr/QRdecomp.hpp"
#include <chrono>
#include <thread>

int main() {
  using namespace std::chrono_literals;

  Matrix<double, 3, 3> a{};
  Matrix<double, 3, 1> b{};

  a.makeCustom(std::cin);

  // std::this_thread::sleep_for(1000ms);
  // b.makeRandom(0);

  // std::cout << hessenbergForm(a) << '\n';

  auto result = QRdecomp(a);
  auto &q = std::get<0>(result);
  auto &r = std::get<1>(result);
  std::cout << q << '\n' << r << '\n';
}
