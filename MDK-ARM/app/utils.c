#include "utils.h"
const float k_=0.1;
float fof_update(float input) {

  input_ = input;
  output_ = output_ * (1 - k_) + input_ * k_;
  return output_;
	
}

