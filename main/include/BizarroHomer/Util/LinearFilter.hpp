// Copyright (c) 2009-2023 FIRST and other WPILib contributors All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
// 
//     Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//     Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//     Neither the name of FIRST, WPILib, nor the names of other WPILib contributors may be used to endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY FIRST AND OTHER WPILIB CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY NONINFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FIRST OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <BizarroHomer/Util/CircularBuffer.hpp>
#include <span>
#include <initializer_list>
#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

template <class T>
class LinearFilter {
public:
  LinearFilter(std::span<const double> ff_gains, std::span<const double> fb_gains)
  : inputs(ff_gains.size()), outputs(fb_gains.size()),
    input_gains(ff_gains.begin(), ff_gains.end()), output_gains(fb_gains.begin(), fb_gains.end()) {
    
    for (size_t i = 0; i < ff_gains.size(); ++i) {
      inputs.emplace_front(0.0);
    }
    for (size_t i = 0; i < fb_gains.size(); ++i) {
      outputs.emplace_front(0.0);
    }
  }
  
  LinearFilter(std::initializer_list<double> ff_gains, std::initializer_list<double> fb_gains)
  : LinearFilter({ff_gains.begin(), ff_gains.end()}, {fb_gains.begin(), fb_gains.end()}) {}
  
  static LinearFilter<T> single_pole_iir(double time_constant, double period) {
    double gain = std::exp(-period / time_constant);
    return LinearFilter({1.0 - gain}, {-gain});
  }
  
  static LinearFilter<T> moving_average(int taps) {
    assert(taps > 0);
    
    std::vector<double> gains(taps, 1.0 / taps);
    return LinearFilter(gains, {});
  }
  
  void reset() {
    std::fill(inputs.begin(), inputs.end(), T{0.0});
    std::fill(outputs.begin(), outputs.end(), T{0.0});
  }
  
  T calculate(T input) {
    T ret_val{0.0};
    
    // Rotate the inputs
    if (input_gains.size() > 0) {
      inputs.push_front(input);
    }
    
    // calculate the new value
    for (size_t i = 0; i < input_gains.size(); ++i) {
      ret_val += inputs[i] * input_gains[i];
    }
    for (size_t i = 0; i < output_gains.size(); ++i) {
      ret_val -= outputs[i] * output_gains[i];
    }
    
    // Rotate the outputs
    if (output_gains.size() > 0) {
      outputs.push_front(ret_val);
    }
    
    return ret_val;
  }
  
 private:
  CircularBuffer<T> inputs;
  CircularBuffer<T> outputs;
  std::vector<double> input_gains;
  std::vector<double> output_gains;
};
