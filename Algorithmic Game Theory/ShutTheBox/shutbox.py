# Assignment goal: to return optimal move / expected wins for a player given any position in Shut The Box.

import sys
import math
import bisect

memo1 = {}
memo2 = {}

def main():

    

    # The board will always be sys.argv[2], so we can immediately record that
    board = []
    for i in sys.argv[3]:
        board.append(int(i)) # Board = [1, 2, 3, 4, 5, 6, 7, 8, 9]

    # Check to see whether we are helping player one or player two
    if sys.argv[1] == "--one":
        player1(board)
    else:
        player2(board)

def player1(board):
    if sys.argv[2] == "--expect":
        answer = expect_1(board)
        print("{0:.6f}".format(answer))
    else:
        print(move_1(board, int(sys.argv[4])))

def player2(board):
    if sys.argv[2] == "--expect":
        answer = expect_2(board, int(sys.argv[4]))
        print("{0:.6f}".format(answer))
    else:
        print(move_2(board, int(sys.argv[5]), int(sys.argv[4])))

# Function to return the optimal move for Player 2 given the board and a roll
def move_2(board, roll, score1):   
    move_opt = None
    max_wins = -0.1

    for move in moves(board, roll):
        next_board = result(board, move)
        next_tuple = tuple(next_board)
        if (next_tuple, score1) in memo2:
            move_wins = memo2[(next_tuple, score1)]
        else:
            move_wins = expect_2(next_board, score1)
            memo2[(next_tuple, score1)] = move_wins
        if move_wins > max_wins:
            move_opt = move
            max_wins = move_wins
        undo(board, move)
    
    return move_opt

# Function to return the expected number of wins for Player 2 given the board
def expect_2(board, score1):
    if (tuple(board), score1) in memo2:
        return memo2[(tuple(board), score1)]
    num_wins = 0.0
    prob_move = 0.0
    
    # if player 1 shut the box, we don't get to go
    if score1 == 0:
        return 0.0
    if len(board) == 0: # Player 2 has shut the box
        return 1.0
    if sum(board) < score1: # We have already beaten Player 1, therefore game is over
        return 1.0
    
    opt_moves = []
    if sum(board) > 6: # Greater than 6 showing on the board so we're rolling two dice
        dice = 2
        for j in range(2, 13):
            if move_2(board, j, score1):
                opt_moves.append(move_2(board, j, score1))
    else:
        dice = 1
        for i in range(1, 7):
            if move_2(board, i, score1):
                opt_moves.append(move_2(board, i, score1))
    
    if len(opt_moves) == 0: # We have run out of moves
        if sum(board) == score1:
            return 0.5
        else: # Already accounted for case where we have a lower score
            return 0.0
    
    # Now we are playing: iterate through the possible optimal moves and multiply their value by probability the roll occurs
    for move in opt_moves:
        prob_move += prob(sum(move), dice)
        num_wins += prob(sum(move), dice) * expect_2(result(board, move), score1)
        undo(board, move)
    # Case where we tie if we don't successfully roll another move:
    if sum(board) == score1:
        num_wins += (1 - prob_move) * 0.5
    
    return num_wins

# Function to return the expected number of wins for Player 1 given the board
def expect_1(board):
    num_wins1 = 0.0
    prob_move = 0.0
    if len(board) == 0: # Case where the box is shut and we have won
        return 1.0
    
    # Find all possible moves from this point
    opt_moves1 = []
    if sum(board) > 6: # Greater than 6 showing on the board so we're rolling two dice
        dice1 = 2
        for i in range(2, 13):
            if move_1(board, i):
                opt_moves1.append(move_1(board, i))
    else:
        dice1 = 1
        for j in range(1, 7):
            if move_1(board, j):
                opt_moves1.append(move_1(board, j))
    
    score2 = ((1, 2, 3, 4, 5, 6, 7, 8, 9), sum(board))
    if score2 not in memo2:
        memo2[score2] = expect_2([1, 2, 3, 4, 5, 6, 7, 8, 9], sum(board))

    if len(opt_moves1) == 0: # Game is over
        return 1 - memo2[score2]

    # Otherwise, we are still playing: weight the probabilities of each move and add to expected val
    for move in opt_moves1:
        prob_move += prob(sum(move), dice1)
        num_wins1 += prob(sum(move), dice1) * expect_1(result(board, move))
        undo(board, move)
    # Account for probability that you don't roll those values, in which case board is actually terminal
    num_wins1 += (1 - prob_move) * (1 - memo2[score2])
    
    return num_wins1


# Function to return the optimal move for Player 1 given the board and a roll
def move_1(board, roll):
    move_opt1 = None
    max_wins1 = -0.1

    for move in moves(board, roll):
        next_board1 = result(board, move)
        next_tuple1 = tuple(next_board1)
        if next_tuple1 in memo1:
            move_wins1 = memo1[next_tuple1]
        else:
            move_wins1 = expect_1(next_board1)
            memo1[next_tuple1] = move_wins1
        if move_wins1 > max_wins1:
            move_opt1 = move
            max_wins1 = move_wins1
        undo(board, move)
    
    return move_opt1

# Function to undo a move on a board
def undo(board, move):
    for i in move:
        bisect.insort(board, i)
    return board

# Function to update the board after making a move
def result(board, move):
    for i in move:
        board.remove(i)
    return board


# Function to determine probability of a specific roll
def prob(roll, dice):
    if dice == 1:
        if roll > 0 and roll < 7:
            return 1 / 6
    else:
        odds = 0
        list1 = [1, 2, 3, 4, 5, 6]
        list2 = [1, 2, 3, 4, 5, 6]

        for i in list1:
            for j in list2:
                if i + j == roll:
                    odds += 1
                    break
        return odds / 36

# Function to find all possible moves that equal a given sum on the board
def moves(board, mysum):
    answer = []
    # First: if we have the number, add that
    for i in board:
        if i == mysum:
            answer.append([i])
            break
    # Second: look for two sum
    if mysum > 2 and len(board) >= 2 and len(two_sum(board, mysum)) > 0:
        for j in two_sum(board, mysum):
            answer.append(j)
    # Third: look for three sum
    if mysum > 5 and len(board) >= 3 and len(three_sum(board, mysum)) > 0:
        for k in three_sum(board, mysum):
            answer.append(k)
    if mysum > 9 and len(board) >= 4 and len(four_sum(board, mysum)) > 0:
        for l in four_sum(board, mysum):
            answer.append(l)
    return answer

def two_sum(board, mysum):
    two_moves = []
    h = {}

    for i, num in enumerate(board):
        n = mysum - num
        if n not in h:
            h[num] = i
        else:
            two_moves.append([n, num])
    return two_moves

def three_sum(board, mysum):
    # run two_sum for every i under 4 (no possible three sums where 4 is the smallest number)
    i = 0
    three_moves = []

    while board[i] < 4 and i < len(board) - 2:
        i_val = board[i]
        two_sums = two_sum(board[i+1:], mysum - i_val)
        if len(two_sums) > 0:
            for val in two_sums:
                val.insert(0, i_val)
                three_moves.append(val)
        i += 1
    return three_moves

def four_sum(board, mysum):
    # run three_sum for every combination (only valid for largest numbers and when first number = 1)
    i = 0
    four_moves = []

    while board[i] < 2 and i < len(board) - 3:
        i_val = board[i]
        three_sums = three_sum(board[i + 1:], mysum - i_val)
        if len(three_sums) > 0:
            for val in three_sums:
                val.insert(0, i_val)
                four_moves.append(val)
        i += 1
    return four_moves


            
    





















if __name__ == "__main__":
    main()