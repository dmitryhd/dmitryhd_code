#!/usr/bin/env python
#-*- coding: utf-8 -*-

import math
import sys

precision = int(sys.argv[1])
iterations = int(sys.argv[2])

#sine

res = 0.0
x = 1.0
for i in range(iterations):
  for j in range(precision):
    res += (-1)**j * x**(2*j)/math.factorial(2*j)
