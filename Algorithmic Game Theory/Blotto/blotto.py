# Program to both determine a winning strategy for Blotto as well as verify whether a strategy is an equilibrium

# OFFICE HOURS: My program is passing all but one test,
# I'm worried that I'm not appropriately shifting by a constant,
# My verify function doesn't work with inappropriate input so it may be falsely passing
# Incorrect input from my find function (which would explain why find is working).

import sys
import scipy.optimize
import numpy as np
import itertools

def main():

    if sys.argv[1] == "--find":
        return Find()
    else:
        return Verify()

def Verify():
    if sys.argv[2] == "--tolerance":
        tolerance = float(sys.argv[3])
        switch = True
    else:
        tolerance = 10.0e-6
        switch = False

    battlefields = []

    if switch:
        # this means that tolerance was indicated so we need to start at sys.argv[6]
        for i in range(5, len(sys.argv)):
            battlefields.append(int(sys.argv[i]))
        if sys.argv[4] == "--win":
            win = True
        else:
            win = False
    else:
        for i in range(3, len(sys.argv)):
            battlefields.append(int(sys.argv[i]))
        if sys.argv[2] == "--win":
            win = True
        else:
            win = False
    
    # Read in number of units (needs to be done with the first line of stdin)
    units = 0
    line_list = sys.stdin.readlines()
    line_list[0] = line_list[0].strip().split(',')
    first_strategy = []
    for i in range(len(battlefields)):
        units += int(line_list[0][i])
        first_strategy.append(int(line_list[0][i]))
    probability1 = float(line_list[0][-1])


    # 2) initialize dictionary with strategies and corresponding probabilities -- add first strategy
    strat_dict = {}
    strat_list = list(sums(len(battlefields), units))
    for i in range(len(strat_list)):
        strat_dict[strat_list[i]] = 0
    strat_dict[tuple(first_strategy)] = probability1

    # 3) add all other corresponding strategies and probabilities to dictionary
    for i in range(1, len(line_list)):
        line_list[i] = line_list[i].strip().split(',')
        strategy = []
        for j in range(len(battlefields)):
            strategy.append(int(line_list[i][j]))
        # now we add the corresponding probability to that strategy in strat_dict
        strat_dict[tuple(strategy)] = float(line_list[i][-1])

    # iterate through strat_dict to count total value, initiate rows vector to iterate through later
    row = []
    value = 0
    for i in strat_dict: # strat_dict:
        row_total = 0.0
        for j in strat_dict:
            if win:
                value += strat_dict[i] * strat_dict[j] * play_blotto_win(i, j, battlefields) # adding to value prob(x) * prob(y) * value(x, y)
                row_total += strat_dict[j] * play_blotto_win(i, j, battlefields) # single pure strategy being played against mixed strategy
            else:
                value += strat_dict[i] * strat_dict[j] * play_blotto_score(i, j, battlefields)
                row_total += strat_dict[j] * play_blotto_score(i, j, battlefields)
        row.append(row_total)

    # checking condition: things get worse if x goes to pure, better if y goes to pure
    for i in row:
        if i - value > tolerance:
            print(str(i) + ">" + str(value))
            print("Not an equilibrium.")
            return 0

    print("PASSED")
    return 0

def Find():
    if sys.argv[2] == "--tolerance":
        tolerance = float(sys.argv[3])
        switch = True
    else:
        tolerance = 10.0e-6
        switch = False

    battlefields = []

    if switch:
        # this means that tolerance was indicated so we need to start at sys.argv[6]
        units = int(sys.argv[6])
        for i in range(7, len(sys.argv)):
            battlefields.append(int(sys.argv[i]))
        if sys.argv[4] == "--win":
            win = True
        else:
            win = False
    else:
        units = int(sys.argv[4])
        for i in range(5, len(sys.argv)):
            battlefields.append(int(sys.argv[i]))
        if sys.argv[2] == "--win":
            win = True
        else:
            win = False

    strategies = list(sums(len(battlefields), units))
    payoff_matrix = []

    # Fill out payoff matrix
    for i in strategies:
        row = []
        for j in strategies:
            if win:
                row.append(play_blotto_win(i, j, battlefields) + 1)
            else:
                row.append(play_blotto_score(i, j, battlefields) + 1)
        payoff_matrix.append(row)
    
    # transform payoff matrix for function
    bounds = (0, 1 / np.amin(payoff_matrix))
    # print(np.amin(payoff_matrix))
    payoff_matrix = -1 * np.transpose(payoff_matrix)
    
    rows, cols = len(payoff_matrix), len(payoff_matrix)
    # Question concerning finding of b_ub and c
    b_ub = [-1.0] * cols
    c = [1.0] * rows

    # may run into an issue with undefined values (v = 0), need to alter values in payoff to make sure min val > 0 (hint: shifting value by consstant)
    result = scipy.optimize.linprog(c, payoff_matrix, b_ub, None, None, bounds, method= 'interior-point')

    value = 1.0 / (result.fun)
    x = [pi * value for pi in result.x]

    for answer in range(len(x)):
        if x[answer] > tolerance:
            for i in strategies[answer]:
                print(i, end=",")
            print(x[answer])


def play_blotto_win(player1, player2, battlefield): # gives win value of a game between two players given their strategies
    total = 0
    goal = sum(battlefield)

    for i in range(len(battlefield)):
        if player1[i] > player2[i]:
            total += battlefield[i]
        elif player1[i] == player2[i]:
            total += battlefield[i] / 2
    if total > goal / 2:
        return 1.0
    elif total == goal / 2:
        return 0.5
    else:
        return 0.0

def play_blotto_score(player1, player2, battlefield): # gives expected score of a game between two players given their strategies
    total = 0

    for i in range(len(battlefield)):
        if player1[i] > player2[i]:
            total += battlefield[i]
        elif player1[i] == player2[i]:
            total += battlefield[i] / 2
    
    return total

def sums(length, total_sum):
    if length == 1:
        yield (total_sum,)
    else:
        for value in range(total_sum + 1):
            for permutation in sums(length - 1, total_sum - value):
                yield (value,) + permutation




























if __name__ == "__main__":
    main()