# Program to both determine a winning strategy for One Card Poker as well as verify whether a strategy is an equilibrium

import sys
import scipy.optimize
import numpy as np
import itertools
import random

def main():

    cards = int(sys.argv[2]) * 2
    player = int(sys.argv[3])

    if sys.argv[1] == "--find":
        return Find(player, cards, True)
    else:
        return Verify(player, cards)

def Verify(player, cards):
    tolerance = 10e-7


    # 2) initialize dictionary with strategies
    strat_dict1 = {}

    # 3) add all other corresponding strategies and probabilities to dictionary
    line_list = sys.stdin.readlines()
    for i in range(len(line_list)):
        line_list[i] = line_list[i].strip().split(',')
        strategy = []
        for j in range(cards):
            strategy.append(int(line_list[i][j]))
        # now we add the corresponding probability to that strategy in strat_dict
        strat_dict1[tuple(strategy)] = float(line_list[i][-1])

    strat_dict2 = {}
    if player == 1:
        strat_dict2 = Find(2, cards, False)
    else:
        strat_dict2 = Find(1, cards, False)
    # iterate through strat_dict to count total value, initiate rows vector to iterate through later
    row_us = []
    value = 0
    for i in strat_dict1: # iterating through keys
        row_total1 = 0.0
        for j in strat_dict2:
            if player == 1:
                value += strat_dict1[i] * strat_dict2[j] * OCP1_expected_val(i, j, int(cards / 2)) # adding to value prob(x) * prob(y) * value(x, y)
                row_total1 += strat_dict2[j] * OCP1_expected_val(i, j, int(cards / 2)) # single pure strategy being played against mixed strategy
            else:
                value += strat_dict1[i] * strat_dict2[j] * OCP2_expected_val(i, j, int(cards / 2))
                row_total1 += strat_dict2[j] * OCP2_expected_val(i, j, int(cards / 2))
        row_us.append(row_total1)
    
    row_them = []
    for i in strat_dict2:
        row_total2 = 0.0
        for j in strat_dict1:
            if player == 1: # Add up values for all pure P2 strategies
                row_total2 += strat_dict1[j] * OCP1_expected_val(j, i, int(cards / 2)) # single pure strategy being played against mixed strategy
            else:
                row_total2 += strat_dict1[j] * OCP2_expected_val(j, i, int(cards / 2))
        row_them.append(row_total2)
            


    # checking condition: things get worse if x goes to pure, better if y goes to pure
    for i in row_us:
        if i - value > tolerance:
            print(str(i) + " > " + str(value))
            print("Not an equilibrium.")
            return 0
    for j in row_them:
        if value - j > tolerance: # if the second player can perform better by minimizing score
            print(str(j) + " < " + str(value))
            print("Not an equilibrium.")
            return 0

    print("PASSED")
    return 0

def Find(player, cards, print_me):
    tolerance = 10e-7

    strategies = binary_list(cards)
    payoff_matrix = []

    # Fill out payoff matrix
    for i in strategies:
        row = []
        for j in strategies:
            if player == 1:
                row.append(OCP1_expected_val(i, j, int(cards / 2)) + 3)
            else: # sys.argv[1] == '2'
                row.append(OCP2_expected_val(i, j, int(cards / 2)) + 3)
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
    probabilities = [pi * value for pi in result.x]
    equilibrium = {}
    sum1 = sum2 = 0
    for strategy in range(len(probabilities)):
        if probabilities[strategy] > tolerance:
            if print_me == True:
                for i in strategies[strategy]:
                    print(i, end=",")
                print(probabilities[strategy])
                sum1 += 1
            equilibrium[tuple(strategies[strategy])] = probabilities[strategy]
        else:
            sum2 += 1
    return equilibrium

def OCP1_expected_val(player1, player2, cards): # returns the expected value as a float
    
    return np.sum([(outcome * 1 / 12) for outcome in play_OCP1(player1, player2, cards)])

def OCP2_expected_val(player2, player1, cards): # returns the expected value as a float
    
    return np.sum([(outcome * 1 / 12) for outcome in play_OCP2(player2, player1, cards)])

def play_OCP2(player2, player1, cards): # returns a list of all of the outcomes
    answer = []
    cardlist2 = [i for i in range(cards)]
    
    
    for card2 in cardlist2:
        cardlist1 = [i for i in range(cards)]
        cardlist1.remove(card2)
        for card1 in cardlist1:
            bet1, bet3 = player1[card1], player1[card1+cards]
            bet2 = player2[card2+cards] if bet1 else player2[card2]

            if not bet1 and not bet2:  # Neither player bets
                if card1 < card2:
                    answer.append(-1) 
                else:
                    answer.append(1)
            elif not bet2: # P1 bets and P2 doesn't
                answer.append(-1)
            elif bet1 and bet2: # They both bet initially
                if card1 < card2:
                    answer.append(-2)
                else:
                    answer.append(2)
            else: # We go to round 3
                if bet3 and card1 < card2: # Both players bet in Round 3
                    answer.append(-2)
                elif bet3:
                    answer.append(2)
                else: # Player 2 bets and player 1 doesn't
                    answer.append(1)
    return np.array(answer)

def play_OCP1(player1, player2, cards): # returns a list of all of the outcomes
    answer = []
    cardlist = [i for i in range(cards)]
    
    
    for card1 in cardlist:
        cardlist2 = [i for i in range(cards)]
        cardlist2.remove(card1)
        for card2 in cardlist2:
            bet1, bet3 = player1[card1], player1[card1+cards]
            bet2 = player2[card2+cards] if bet1 else player2[card2]

            if not bet1 and not bet2:  # Neither player bets
                if card1 < card2:
                    answer.append(1) 
                else:
                    answer.append(-1)
            elif not bet2: # P1 bets and P2 doesn't
                answer.append(1)
            elif bet1 and bet2: # They both bet initially
                if card1 < card2:
                    answer.append(2)
                else:
                    answer.append(-2)
            else: # We go to round 3
                if bet3 and card1 < card2: # Both players bet in Round 3
                    answer.append(2)
                elif bet3:
                    answer.append(-2)
                else: # Player 2 bets and player 1 doesn't
                    answer.append(-1)
    return np.array(answer)


def binary_list(n): # given n (integer), return all binary numbers with n integers
    """
    Taken from Stack Overflow user Volatility
    https://stackoverflow.com/questions/14931769/how-to-get-all-combination-of-n-binary-value

    """ 
    answer = [list(i) for i in itertools.product([0,1], repeat = n)]
    return answer




























if __name__ == "__main__":
    main()