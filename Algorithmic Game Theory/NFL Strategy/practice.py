



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
        elif r1 <= 2 and 2 < r2 < 3:
            superstate = (0,1)
        elif r1 <= 2 and 3 <= r2 < 5:
            superstate = (0,2)
        elif r1 <= 2 and r2 >= 5:
            superstate = (0,3)
        elif 2 < r1 < 3 and r2 <= 2:
            superstate = (1,0)
        elif 2 < r1 < 3 and 2 < r2 < 3:
            superstate = (1,1)
        elif 2 < r1 < 3 and 3 <= r2 < 5:
            superstate = (1,2)
        elif 2 < r1 < 3 and r2 >= 5:
            superstate = (1,3)
        elif 3 <= r1 < 5 and r2 <= 2:
            superstate = (2,0)
        elif 3 <= r1 < 5 and 2 < r2 < 3:
            superstate = (2,1)
        elif 3 <= r1 < 5 and 3 <= r2 < 5:
            superstate = (2,2)
        elif 3 <= r1 < 5 and r2 >= 5:
            superstate = (2,3)
        elif r1 >= 5 and r2 <= 2:
            superstate = (3,0)
        elif r1 >= 5 and 2 < r2 < 3:
            superstate = (3,1)
        elif r1 >= 5 and 3 <= r2 < 5:
            superstate = (3,2)
        elif r1 >= 5 and r2 >= 5:
            superstate = (3,3)

        freqtable[superstate] += 1
        return superstate