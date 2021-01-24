# Assignment goal: to write a program that outputs a solution to a Tipover puzzle

import sys
from copy import deepcopy

sys.setrecursionlimit(2500)

def main():

    line_list = sys.stdin.readlines()

    # Read into height and width variables
    height, width = [int(x) for x in line_list[0].split()]

    # Read next line into position
    x, y = [int(x) for x in line_list[1].split()]
    position = (x, y)

    # Initialize and fill starting game board
    board = []
    for i in range(height):
        row = []
        for j in range(width):
            row.append(line_list[i+2][j])
        board.append(row)    

    answer = find_solution(position, board)

    if answer != None:
        for a, b, c, d in answer:
            print(a, b, c, d)

def find_solution(position, board):
    if winning_state(position, board):
        return []
    
    for move in moves(position, board):
        next_move = update(move, board)
        solution = find_solution(next_move[0], next_move[1])
        if solution is not None:
            return [move] + solution
    return None
    
# Function to update the board given a command (return the new position and updated board)
def update(command, board):
    x = command[0]
    y = command[1]
    direction = (command[2], command[3])
    board_copy = deepcopy(board)
    value = int(board_copy[x][y])
    board_copy[x][y] = '.'

    if direction == (0, 1):
        # right direction
        for i in range(1, value + 1):
            board_copy[x][y + i] = 'x'
        index = (x, y + value)
    elif direction == (0, -1):
        for i in range(1, value + 1):
            board_copy[x][y - i] = 'x'
        index = (x, y - value)
    elif direction == (1, 0):
        for i in range(1, value + 1):
            board_copy[x + i][y] = 'x'
        index = (x + value, y)
    elif direction == (-1, 0):
        for i in range(1, value + 1):
            board_copy[x - i][y] = 'x'
        index = (x - value, y)
    
    return index, board_copy
    
def moves(position, board):
    answer = []
    for move in dfs(position, board):
        # account for other crates that are reachable
        if crate(move, board):
           for x in tips(move, board):
               answer.append(x)
    return answer

def crate(position, board):
    value = board[position[0]][position[1]]
    if value != '.' and value != 'x':
        return True
    else:
        return False 

def tips(position, board):
    answer = []
    x = position[0]
    y = position[1]
    if crate(position, board) == False:
        return None
    value = int(board[x][y])
    if tip_down(value, position, board):
        answer.append((x, y, 1, 0)) 
    if tip_left(value, position, board):
        answer.append((x, y, 0, -1))
    if tip_right(value, position, board):
        answer.append((x, y, 0, 1))
    if tip_up(value, position, board):
        answer.append((x, y, -1, 0))
    return answer

# Function to implement depth first search
def dfs(position, board, visited=None):
    if visited is None:
        visited = set()
    
    visited.add(position)

    x = position[0]
    y = position[1]

    if x >= 1 and board[x-1][y] != '.' and (x-1,y) not in visited:
        dfs((x-1, y), board, visited)
    if x < len(board) - 1 and board[x+1][y] != '.' and (x+1,y) not in visited:
        dfs((x+1, y), board, visited)
    if y >= 1 and board[x][y - 1] != '.' and (x,y-1) not in visited:
        dfs((x, y - 1), board, visited)
    if y < len(board[0]) - 1 and board[x][y + 1] != '.' and (x,y+1) not in visited:
        dfs((x, y + 1), board, visited)

    return visited

# Functions to tip a crate and update the board
def tip_right(crate_height, position, board):
    board_copy = deepcopy(board)
    # Returns None if illegal move, returns new board if legal move
    width = len(board[0])
    if position[1] + crate_height >= width: # Out of bounds condition
        return None
    for i in range(1, crate_height + 1):
        if board_copy[position[0]][position[1] + i] != '.': # Condition where not empty
            return None
    return True

def tip_left(crate_height, position, board):
    board_copy = deepcopy(board)
    # Returns None if illegal move, returns new board if legal move
    if position[1] - crate_height < 0: # Out of bounds condition
        return None
    for i in range(1, crate_height + 1):
        if board_copy[position[0]][position[1] - i] != '.': # Condition where not empty
            return None
        board_copy[position[0]][position[1] - i] = 'x'
    return True

def tip_up(crate_height, position, board):
    board_copy = deepcopy(board)
    # Returns None if illegal move, returns new board if legal move
    if position[0] - crate_height < 0: # Out of bounds condition
        return None
    for i in range(1, crate_height + 1):
        if board_copy[position[0] - i][position[1]] != '.': # Condition where not empty
            return None
        board_copy[position[0] - i][position[1]] = 'x'
    return True

def tip_down(crate_height, position, board):
    board_copy = deepcopy(board)
    # Returns None if illegal move, returns new board if legal move
    height = len(board)
    if position[0] + crate_height >= height: # Out of bounds condition
        return None
    for i in range(1, crate_height + 1):
        if board_copy[position[0] + i][position[1]] != '.': # Condition where not empty
            return None
        board_copy[position[0] + i][position[1]] = 'x'
    return True

# Function to determine if the player is in a winning state
def winning_state(position, board):
    for option in dfs(position, board):
        if check_left(option, board) or check_right(option, board) or check_up(option, board) or check_down(option, board):
            return True
    return False

def check_up(position, board):
    x = position[0]
    y = position[1]
    if x - 1 >= 0 and board[x - 1][y] == '*':
        return True
    else:
        return False

def check_down(position, board):
    height = len(board)
    x = position[0]
    y = position[1]
    if x + 1 < height and board[x + 1][y] == '*':
        return True
    else:
        return False

def check_left(position, board):
    x = position[0]
    y = position[1]
    if y - 1 >= 0 and board[x][y - 1] == '*':
        return True
    else:
        return False

def check_right(position, board):
    width = len(board[0])
    x = position[0]
    y = position[1]
    if y + 1 < width and board[x][y + 1] == '*':
        return True
    else:
        return False
    
if __name__ == "__main__":
    main()