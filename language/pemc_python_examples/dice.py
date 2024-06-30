# The MIT License (MIT)
#
# Copyright (c) 2014-2018, Institute for Software & Systems Engineering
# Copyright (c) 2018-2024, Johannes Leupolz
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
import pypemc


class MyModel(pypemc.PemcModel):
    def __init__(self):
        super().__init__()

    def get_initial_state_value(self):
        return 0

    def step(self):
        print("Dice model step!")

        state = self.get_state()
        print("Before step " + str(state))

        if state == 0:
            # either 1, 2, 3 or 3, 4, 5
            state = self.choose(123, 456)
        elif state == 123:
            # either 1, 2 or 3+rethrow
            state = self.choose(12, 3123)
        elif state == 12:
            # either 1 or 2
            state = self.choose(1, 2)
        elif state == 3123:
            # either 3 or rethrow(1, 2, 3)
            state = self.choose(3, 123)
        elif state == 456:
            # either 4, 5 or 6+rethrow
            state = self.choose(45, 6456)
        elif state == 45:
            # either 4 or 5
            state = self.choose(4, 5)
        elif state == 6456:
            # either 6 or rethrow(4, 5, 6)
            state = self.choose(6, 456)
        self.set_state(state)

    def evaluate_formula(self):
        state = self.get_state()
        return state == 6


reachable = pypemc.check_reachability_in_executable_model(MyModel)
print("The reachability is "+str(reachable))
lmc = pypemc.build_lmc_from_executable_model(MyModel)
# probability_4steps = pypemc.calculate_probability_to_reach_state_within_bound(
#    lmc, 4)
# print("The probability in 4 steps is "+str(probability_4steps))
