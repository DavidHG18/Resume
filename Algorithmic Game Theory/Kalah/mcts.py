import random
import sys
import minimax as minimax
import kalah
import math

class TreeNode:
    def __init__(self, data):
        self.children = [] # contains tuples of the resulting position node and the move it takes to get there
        self.parent = None # this only contains the nodes of the parents
        self.data = data
        self.total_plays = 0
        self.reward = 0

def mcts_strategy(iterations):
    def fxn(pos):
        position_node = TreeNode(pos)
        for i in range(iterations):
            mcts(position_node)
        # go through first options and pick the one with the highest win rate (reward / total_plays)
        if position_node.data.next_player() == 0:
            value = float('-inf')
        else:
            value = float('inf')
        # once we update the tree (iterations) times, we pick the child node with the most promising value
        for move, next_pos in position_node.children:
            #print(value)
            #print("Move")
            #print(position_node.data.next_player())
            comp_value = next_pos.reward / next_pos.total_plays
            #print(comp_value)
            if position_node.data.next_player() == 0 and comp_value > value:
                best_move = move
                value = comp_value
            elif position_node.data.next_player() == 1 and comp_value < value:
                best_move = move
                value = comp_value
        #print("Best value: " + str(best_move))
        return best_move # move is the best choice out of our possible moves (which will be an indexed pit)
    return fxn

def mcts(position):
    # Runs mcts on a game from the given position
    while position.data.game_over() == False and len(position.data.legal_moves()) == len(position.children): # keep going until expandable or terminal
        # insert the UCB function here
        if position.data.next_player() == 0:
            value = float('-inf')
        else:
            value = float('inf')
        for move, next_pos in position.children: # compare scores in UCB function and pick maximum
            if position.data.next_player() == 0:
                comp_value_UCB = (next_pos.reward / next_pos.total_plays) + math.sqrt(1.2 * math.log(position.total_plays) / next_pos.total_plays)
                if comp_value_UCB > value:
                    best_pos = next_pos
                    value = comp_value_UCB
            elif position.data.next_player() == 1:
                comp_value_UCB = (next_pos.reward / next_pos.total_plays) - math.sqrt(1.2 * math.log(position.total_plays) / next_pos.total_plays)
                if comp_value_UCB < value:
                    best_pos = next_pos
                    value = comp_value_UCB
        # now we have the best move: move the position to the corresponding tree node
        position = best_pos
    
    # create a new node and simulate to the end
    if position.data.game_over() == True:
        win_val = position.data.winner()
        recursive_node = position

    else:
        # pick a legal move not in the existing moves children
        possible_moves = position.data.legal_moves() # need to check this for already-existing moves
        for move, next_pos in position.children:
            possible_moves.remove(move)
        
        # code for initializing node if I choose among remaining children randomly
        new_move = random.choice(possible_moves)
        new_position = TreeNode(position.data.result(new_move))
        position.children.append((new_move, new_position))
        new_position.parent = position
        final_move = new_position.data
    
        # iterate through to the end of the tree 
        while final_move.game_over() == False:
            final_move = final_move.result(random.choice(final_move.legal_moves()))

        # win value is equal to the result of the terminal move
        win_val = final_move.winner()
        recursive_node = new_position
    
    # iterate up through tree
    backpropagate(recursive_node, win_val)


def backpropagate(node, value):
    node.total_plays += 1
    node.reward += value
    if node.parent != None:
        backpropagate(node.parent, value)

    

