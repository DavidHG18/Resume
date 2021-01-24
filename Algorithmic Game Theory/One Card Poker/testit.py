import subprocess

print()
print("Hi Team! Welcome to my final project. I've attempted to find equilibrium strategies for both Player 1 and \
Player 2 in One Card Poker. I found this assignment to be very similar to the Blotto assignment, with just two key differences:")

print()
print("The first is that OCP is a stochastic game. Regardless of which strategy the Player takes on, their behavior is dependent on what card they draw. \
In Blotto, there was no stochastic component -- we could calculate the outcome of a game purely based on the strategies of each player. \
In OCP, we have to calculate the expected value of a combination of two strategies. I made the assumption that each card was equally likely to \
be drawn by both players, and therefore each pairing of (Card1, Card2) has a probability of 1/12 (the probability of the first card being drawn is 1/4, and \
the probability of the second card being drawn is 1/3). I calculated the EV of each strategy pairing by summing up the probability-weighted outcomes \
between the two strategies given each possible distribution of cards.")

print()
print("The second is that OCP is not a symmetric game. Even though the way we are representing strategies is the same \
(a sequence of 1s and 0s corresponding to whether the player would be or not), they mean entirely different things for Player 1 \
and Player 2. This is relevant both in Find() and in Verify(): in Find(), I hda to set up the payoff matrix differently based on which \
player the equilibrium was being requested for. In Verify(), I had to run Find() from the perspective of the opposing \
player and enter that opposing strategy as the equilibrium to be compared against.")

print()
print("I'm happy to answer any questions should you have them. Enjoy!")

print()
subprocess.run("make")

print()
print()
print("FIND")

print()
print()
print("Finding equilibrium for Player 1 (Deck of 4)")
subprocess.run("./OCP --find 4 1", shell=True, check=True)

print()
print("Finding equilibrium for Player 2 (Deck of 4)")
subprocess.run("./OCP --find 4 2", shell=True, check=True)

print()
print("Finding equilibrium for Player 1 (Deck of 5)")
subprocess.run("./OCP --find 5 1", shell=True, check=True)

print()
print("Finding equilibrium for Player 2 (Deck of 5)")
subprocess.run("./OCP --find 5 2", shell=True, check=True)

print()
print()
print("VERIFY")

print()
print()
print("Verifying correct equilibrium for Player 1 (Deck of 4)")
subprocess.run("./OCP --find 4 1 | ./OCP --verify 4 1", shell=True, check=True)

print()
print("Verifying incorrect equilibrium for Player 1 (Deck of 4)")
subprocess.run("./OCP --find 4 2 | ./OCP --verify 4 1", shell=True, check=True)

print()
print("Verifying correct equilibrium for Player 2 (Deck of 4)")
subprocess.run("./OCP --find 4 2 | ./OCP --verify 4 2", shell=True, check=True)

print()
print("Verifying incorrect equilibrium for Player 2 (Deck of 4)")
subprocess.run("./OCP --find 4 1 | ./OCP --verify 4 2", shell=True, check=True)

print()
print("Verifying correct equilibrium for Player 1 (Deck of 5)")
subprocess.run("./OCP --find 5 1 | ./OCP --verify 5 1", shell=True, check=True)

print()
print("Verifying incorrect equilibrium for Player 1 (Deck of 5)")
subprocess.run("./OCP --find 5 2 | ./OCP --verify 5 1", shell=True, check=True)

print()
print("Verifying correct equilibrium for Player 2 (Deck of 5)")
subprocess.run("./OCP --find 5 2 | ./OCP --verify 5 2", shell=True, check=True)

print()
print("Verifying incorrect equilibrium for Player 2 (Deck of 5)")
subprocess.run("./OCP --find 5 1 | ./OCP --verify 5 2", shell=True, check=True)
