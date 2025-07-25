#include <iostream>

#include "HPIValid.hpp"
#include "config.hpp"
#include "result_collector.hpp"

int main() {
  std::string input_file("../experiments/data_examples/flight_r1001_c109.csv");
  hpiv::Config cfg;
  hpiv::ResultCollector RC;

  hpiv::HPIValid(input_file, cfg, RC);

  std::cout << RC.uccs() << " UCCs found after " << RC.time(hpiv::timer::total)
            << "s" << std::endl;

  return 0;
}
