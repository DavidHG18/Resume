# takes an object that models the two-minute drill of NFL Strategy
# and a time limit in seconds and returns a function that takes 
# the non-terminal position in the game and returns the index of 
# the selected offensive play.


# features: 
# 1) yards-to-first / downs left
# 2) yards-to-score / time left

# position: 4-tuples
# (remaining yards needed to score, downs left, yards to first, time remaining in 5-second ticks)

# result function:
# takes a position and action and returns (resulting position, (yards gained, time-elapsed, turnover boolean))

# terminal states:
# set Q^(s, a) = 0

import random
import nfl_strategy as nfl
import time

def q_learn(model, limit): # takes a game and time limit and returns a trained policy

    # initialize Q table as hashtable
    Qtable = {}

    # initialize constants
    epsilon, alpha = 0.1, 0.25

    # initialize Qtable entries to 0
    superstates = []
    superstates.append((-1,-1))

    actions = []
    for i in range(3):
        for j in range(3):
            superstates.append((i,j))
    for k in range(model.offensive_playbook_size()):
        actions.append(k)
    for superstate in superstates:
        for action in actions:
            Qtable[(superstate, action)] = 0    
    
    def fxn(state): # this just returns optimal action given the Q-values I've already trained

        # find best action
        return best_action(state)

    def train(position): # function to train
        gamma = 1

        while model.game_over(position) == False:
            superstate = to_superstate(position)
            #print(superstate)
            if random.random() > epsilon:
                action = best_action(position)
            else:
                action = random.randrange(0, model.offensive_playbook_size())
            next_position, play_results = model.result(position, action)
            reward = model.win(next_position)
            
            # add to reward if a first down
            yards_to_score, downs_left, yards_to_first, time_left = next_position
            yards_gained, ticks_elapsed, turnover = play_results
            #print(yards_gained / ticks_elapsed)
            
            

            next_superstate = to_superstate(next_position)
            next_action = best_action(next_position)

            # update qval
            Qtable[(superstate, action)] += alpha * (reward +  (Qtable[(next_superstate, next_action)] / gamma) - Qtable[(superstate, action)])

            # go to next state
            position = next_position

    
    def to_superstate(position): # consider adding more superstates

        # position: 4-tuples
        # (remaining yards needed to score, downs left, yards to first, time remaining in 5-second ticks)
        if model.game_over(position):
            return (-1,-1)

        yards_to_score, downs_left, yards_to_first, time_left = position
        r1 = yards_to_first / downs_left
        r2 = yards_to_score / time_left

        if r1 <= 2 and r2 <= 2:
            superstate = (0,0)      
        elif r1 <= 2 and 2 < r2 < 5:
            superstate = (0,1)
        elif r1 <= 2 and r2 >= 5:
            superstate = (0,2)
        elif 2 < r1 < 5 and r2 <= 2:
            superstate = (1,0)
        elif 2 < r1 < 5 and 2 < r2 < 5:
            superstate = (1,1)
        elif 2 < r1 < 5 and r2 >= 5:
            superstate = (1,2)
        elif r1 >= 5 and r2 <= 2:
            superstate = (2,0)
        elif r1 >= 5 and 2 < r2 < 5:
            superstate = (2,1)
        elif r1 >= 5 and r2 >= 5:
            superstate = (2,2)

        return superstate
    
    def best_action(position): # function to pick and return the best action given a Qtable and position

        # using the actions variable declared above (assuming the offensive playbook doesn't change)
        max_value = float('-inf')
        #print(Qtable)
        #print(max_value)
        best = None
        superstate = to_superstate(position)
        for action in actions:
            #print(best)
            if model.game_over(position):
                qval = 0
            else:
                qval = Qtable[(superstate, action)]
            if qval > max_value:
                max_value = qval
                best = action

        return best
    
    # start training 
    timeout_start = time.time()

    while time.time() < timeout_start + (limit - 0.1):
        train(model.initial_position())
        epsilon = max(2e-04, epsilon*0.9999)
        alpha = max(2e-07, alpha * 0.9999)
    
    # returning function at the end
    return fxn