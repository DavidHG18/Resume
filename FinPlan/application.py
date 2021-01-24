# David Hidalgo-Gato and Bartolomeo Rondelli

# CS50 - Final Project - FinPlan

# Implement a site that allows users to easily access the projected value of their portfolio as described by the capital asset pricing model

import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime

from helpers import apology, login_required, lookup, usd, percent, decimal

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///project.db")

@app.route("/")
@login_required
def index():
    """ Give user the value of their portfolio, along with expected return """

    # Create risk-free and market return variables
    rf = 0.0175
    market = 0.08

    # Extract needed values from history table
    portfolio = db.execute("SELECT symbol, name, SUM(shares), price, value, beta, trailing_pe, forward_pe, peg FROM history WHERE user_id = :user_id GROUP BY symbol HAVING SUM(shares) > 0", user_id = session["user_id"])

    # Create list to store values for each stock
    index = []

    # Create list of keys
    keys = ['symbol', 'name', 'SUM(shares)', 'price', 'value', 'beta', 'trailing_pe', 'forward_pe', 'peg']

    for i in range(len(portfolio)):
        info = []
        stock = lookup(portfolio[i][keys[0]])
        portfolio[i][keys[3]] = stock['price']
        portfolio[i][keys[4]] = portfolio[i][keys[2]] * portfolio[i][keys[3]]
        portfolio[i][keys[5]] = stock['beta']
        portfolio[i][keys[6]] = stock['trailing_pe']
        portfolio[i][keys[7]] = stock['forward_pe']
        portfolio[i][keys[8]] = stock['peg']

        for j in range(len(keys)):
            info.append(portfolio[i][keys[j]])
        index.append(info)

    # Find expected return by dividing price by trailing pe, projecting earnings growth, and finding new price using forward pe
    ER_pe = []
    ER_capm = []
    for i in range(len(index)):
        old_earnings = (index[i][3] / index[i][6])
        earnings_growthpct = (index[i][6]  / index[i][8]) / 100
        new_earnings = (old_earnings * (1 + earnings_growthpct))
        future_price = new_earnings * index[i][7]
        ER_pe.append(1 - (future_price / index[i][3]))
        ER_capm.append(rf + index[i][5] * (market - rf))

    # Find total value of stock purchases
    total = 0
    for i in range(len(index)):
        total = total + index[i][4]

    # Find weighted average beta of portfolio by multiplying beta of stock by percent
    percentages = []
    portfolio_beta = 0
    portfolio_ER_pe = 0
    portfolio_ER_capm = 0
    for i in range(len(index)):
        percentages.append(index[i][4] / total)
        beta_stock = index[i][5]
        portfolio_beta = portfolio_beta + (percentages[i] * beta_stock)
        portfolio_ER_pe = portfolio_ER_pe + (percentages[i] * ER_pe[i])
        portfolio_ER_capm = portfolio_ER_capm + (percentages[i] * ER_capm[i])

    # Change appropriate values to display correctly
    for i in range(len(ER_pe)):
        ER_pe[i] = percent(100 * ER_pe[i])
        ER_capm[i] = percent(100 * ER_capm[i])
    portfolio_ER_pe_percent = percent(100 * portfolio_ER_pe)
    portfolio_ER_capm_percent = percent(100 * portfolio_ER_capm)

    portfolio_beta = decimal(portfolio_beta)

    for i in range(len(index)):
        index[i][3] = usd(index[i][3])
        index[i][4] = usd(index[i][4])
        index[i][5] = decimal(index[i][5])

    # Create list for a table row with all the portfolio numbers
    portfolio_data = ["PORTFOLIO", " ", portfolio_beta, portfolio_ER_pe_percent, portfolio_ER_capm_percent]

    # Create list of lists for stock data
    display = []
    bad = "BAD"

    if portfolio_ER_pe < portfolio_ER_capm:
        portfolio_data.append(bad)

    for i in range(len(index)):
        info = []
        info.append(index[i][0])
        info.append(index[i][2])
        info.append(index[i][5])
        info.append(ER_pe[i])
        info.append(ER_capm[i])
        ERPE = ER_pe[i]
        ERCAPM = ER_capm[i]
        ERPE = ERPE[:-1]
        ERCAPM = ERCAPM[:-1]
        if float(ERPE) < float(ERCAPM):
            info.append(bad)
        display.append(info)

        print(display)


    return render_template("index.html", portfolio=portfolio_data, display=display)

@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():

    if request.method == "GET":
        return render_template("buy.html")

    else:
        # If nothing entered, return apology
        if not request.form.get("symbol"):
            return apology("Symbol not found.")

        # If symbol does not exist, return apology
        elif lookup(request.form.get("symbol")) == None:
            return apology("Symbol not found.")

        # If shares not entered, return apology
        elif not request.form.get("shares"):
            return apology("Please enter appropriate number of shares.")

        # Assign values to variables
        price = lookup(request.form.get("symbol"))["price"]
        shares = int(request.form.get("shares"))
        symbol = request.form.get("symbol").upper()
        time = datetime.now()
        name = lookup(request.form.get("symbol"))["name"]
        value = price * shares
        user_id = session["user_id"]

        if shares < 0:
            return apology("Please enter appropriate number of shares.")

        # Extract user's current cash balance
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])

        # Adjust cash balance (db.execute returns a list of dictionaries, need to index dictionary to get integer value)
        cash = cash[0]['cash'] - (price * shares)

        # If cash balance is less than zero, return apology
        if cash < 0:
            return apology("cannot afford to make purchase.")

        # Update datatable to contain new cash balance
        db.execute("UPDATE users SET cash = :cash WHERE id = :user_id", cash=cash, user_id=user_id)

        # Insert purchase into portfolio and history
        db.execute("INSERT INTO history (symbol, name, shares, price, value, time, user_id) VALUES(:symbol, :name, :shares, :price, :value, :time, :user_id)", symbol=symbol, name=name, shares=shares, price=price, value=value, time=time, user_id=user_id)

        # Return to main screen
        return redirect("/")

@app.route("/history")
@login_required
def history():

    data = db.execute("SELECT symbol, shares, price, time FROM history ORDER BY time DESC")
    for i in range(len(data)):
        data[i]['price'] = usd(data[i]['price'])

    # Filter datatable to just return values, not a list of dictionaries
    keys = ['symbol', 'shares', 'price', 'time']
    history = []

    for i in range(len(data)):
        info = []
        for j in range(4):
            info.append(data[i][keys[j]])
        history.append(info)

    return render_template("history.html", history=history)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "GET":
        return render_template("quote.html")

    else:
        # If nothing entered, return apology
        if not request.form.get("symbol"):
            return apology("Symbol not found.")

        # If symbol does not exist, return apology
        if lookup(request.form.get("symbol")) == None:
            return apology("Symbol not found.")

        # Re-route to quoted.html
        ticker = lookup(request.form.get("symbol"))
        ticker['price'] = usd(ticker['price'])
        ticker['symbol'] = ticker['symbol'].upper()

        # Set variables to make it easier to translate to quoted.html
        name = ticker['name']
        price = ticker['price']
        symbol = ticker['symbol']
        beta = ticker['beta']
        eps = ticker['forward_eps']
        pe = ticker['forward_pe']

        return render_template("quoted.html", name=name, price=price, symbol=symbol)



@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    if request.method == "GET":
        return render_template("register.html")

    else:

        #Query database to check for repeated usernames/passwords later on
        rows = db.execute("SELECT * FROM users")

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Ensure that username doesn't already exist in database
        for username in rows:
            if username["username"] == request.form.get("username"):
                return apology("username already exists.", 403)

        # Get name, password, and confirmation from form
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure that password and confirmation are equal to each other
        if confirmation != password:
            return apology("password and confirmation not equal")

        # Ensure that password ia appropriate length
        if len(password) < 8:
            return apology("password must be greater than or equal to 8 characters.")

        # Insert new data into users database
        db.execute("INSERT INTO users (username, hash) VALUES (:username, :hash_val)", username=username, hash_val=generate_password_hash(password))

        return redirect("/")

@app.route("/definitions", methods=["GET", "POST"])
@login_required
def definitions():
    """Go to definitions"""
    return render_template("definitions.html")


@app.route("/toggle", methods=["GET", "POST"])
@login_required
def toggle():
    """Toggle Equation Parameters"""

    if request.method == "GET":
        return render_template("toggle.html")

    if request.method == "POST":
        # Create risk-free and market return variables
        rf = float(request.form.get("rfform"))
        market = float(request.form.get("mrform"))
        print(rf)
        print(market)

        # Extract needed values from history table
        portfolio = db.execute("SELECT symbol, name, SUM(shares), price, value, beta, trailing_pe, forward_pe, peg FROM history WHERE user_id = :user_id GROUP BY symbol HAVING SUM(shares) > 0", user_id = session["user_id"])

        # Create list to store values for each stock
        index = []

        # Create list of keys
        keys = ['symbol', 'name', 'SUM(shares)', 'price', 'value', 'beta', 'trailing_pe', 'forward_pe', 'peg']

        for i in range(len(portfolio)):
            info = []
            stock = lookup(portfolio[i][keys[0]])
            portfolio[i][keys[3]] = stock['price']
            portfolio[i][keys[4]] = portfolio[i][keys[2]] * portfolio[i][keys[3]]
            portfolio[i][keys[5]] = stock['beta']
            portfolio[i][keys[6]] = stock['trailing_pe']
            portfolio[i][keys[7]] = stock['forward_pe']
            portfolio[i][keys[8]] = stock['peg']

            for j in range(len(keys)):
                info.append(portfolio[i][keys[j]])
            index.append(info)

        # Find expected return by dividing price by trailing pe, projecting earnings growth, and finding new price using forward pe
        ER_pe = []
        ER_capm = []
        for i in range(len(index)):
            old_earnings = (index[i][3] / index[i][6])
            earnings_growthpct = (index[i][6]  / index[i][8]) / 100
            new_earnings = (old_earnings * (1 + earnings_growthpct))
            future_price = new_earnings * index[i][7]
            ER_pe.append(1 - (future_price / index[i][3]))
            ER_capm.append(rf + index[i][5] * (market - rf))

        # Find total value of stock purchases
        total = 0
        for i in range(len(index)):
            total = total + index[i][4]

        # Find weighted average beta of portfolio by multiplying beta of stock by percent
        percentages = []
        portfolio_beta = 0
        portfolio_ER_pe = 0
        portfolio_ER_capm = 0
        for i in range(len(index)):
            percentages.append(index[i][4] / total)
            beta_stock = index[i][5]
            portfolio_beta = portfolio_beta + (percentages[i] * beta_stock)
            portfolio_ER_pe = portfolio_ER_pe + (percentages[i] * ER_pe[i])
            portfolio_ER_capm = portfolio_ER_capm + (percentages[i] * ER_capm[i])

        # Change appropriate values to display correctly
        for i in range(len(ER_pe)):
            ER_pe[i] = percent(100 * ER_pe[i])
            ER_capm[i] = percent(100 * ER_capm[i])
        portfolio_ER_pe_percent = percent(100 * portfolio_ER_pe)
        portfolio_ER_capm_percent = percent(100 * portfolio_ER_capm)

        portfolio_beta = decimal(portfolio_beta)

        for i in range(len(index)):
            index[i][3] = usd(index[i][3])
            index[i][4] = usd(index[i][4])
            index[i][5] = decimal(index[i][5])

        # Create list for a table row with all the portfolio numbers
        portfolio_data = ["PORTFOLIO", " ", portfolio_beta, portfolio_ER_pe_percent, portfolio_ER_capm_percent]

        # Create list of lists for stock data
        display = []
        bad = "BAD"

        if portfolio_ER_pe < portfolio_ER_capm:
            portfolio_data.append(bad)

        for i in range(len(index)):
            info = []
            info.append(index[i][0])
            info.append(index[i][2])
            info.append(index[i][5])
            info.append(ER_pe[i])
            info.append(ER_capm[i])
            ERPE = ER_pe[i]
            ERCAPM = ER_capm[i]
            ERPE = ERPE[:-1]
            ERCAPM = ERCAPM[:-1]
            if float(ERPE) < float(ERCAPM):
                info.append(bad)
            display.append(info)



        return render_template("index.html", portfolio=portfolio_data, display=display)




@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "GET":
        return render_template("sell.html")

    else:
        # If nothing entered, return apology
        if not request.form.get("symbol"):
            return apology("Symbol not found.", 400)

        # If symbol does not exist, return apology
        elif lookup(request.form.get("symbol")) == None:
            return apology("Symbol not found.", 400)

        # If shares not entered, return apology
        elif not request.form.get("shares"):
            return apology("Please enter appropriate number of shares.", 400)

        # If shares not a positive integer, return apology
        elif int(request.form.get("shares")) < 1:
            return apology("Please enter appropriate number of shares.", 400)

        # Extract stocks with more than zero shares purchased in history
        portfolio = db.execute("SELECT symbol, name, SUM(shares), price, value FROM history WHERE user_id = :user_id GROUP BY symbol HAVING SUM(shares) > 0", user_id=session["user_id"])

        # Create list to store values for each stock
        index = []

        # Create list to store key values to index portfolio more easily
        keys = ['symbol', 'name', 'SUM(shares)', 'price', 'value']

        # Take information from portfolio and put it into index
        for i in range(len(portfolio)):
            # Create empty list to store respective values of portfolio keys
            info = []
            # Set value equal to shares times price
            portfolio[i][keys[3]] = lookup(portfolio[i][keys[0]])["price"]
            portfolio[i][keys[4]]= portfolio[i][keys[3]] * portfolio[i][keys[2]]
            for j in range(5):
                info.append(portfolio[i][keys[j]])
            index.append(info)

        # Assign values to variables
        price = lookup(request.form.get("symbol"))["price"]
        shares = int(request.form.get("shares"))
        symbol = request.form.get("symbol").upper()
        time = datetime.now()
        value = price * shares
        name = lookup(request.form.get("symbol"))["name"]
        user_id = session["user_id"]

        # Want to enter index at the location where the symbol matches the symbol in question
        temp = 0
        for i in range(len(index)):
            if index[i][0] == symbol:
                temp = i

        # If shares more than shares you already own, return apology
        if index[temp][2] - shares < 0:
            return apology("You cannot sell more shares than you own!")

        # Extract the user's cash balance
        cash = db.execute("SELECT cash FROM users WHERE id = :user_id", user_id=user_id)

        # Adjust cash balance (db.execute returns a list of dictionaries, need to index dictionary to get integer value)
        cash = cash[0]['cash'] + (price * shares)

        # Update shares value to indicate sale
        shares = shares * (-1)

        #Insert new cash value into users
        db.execute("UPDATE users SET cash = :cash WHERE id = :user_id", cash=cash, user_id=user_id)

        # Add to history table
        db.execute("INSERT INTO history (symbol, name, shares, price, value, time, user_id) VALUES(:symbol, :name, :shares, :price, :value, :time, :user_id)", symbol=symbol, name=name, shares=shares, price=price, value=value, time=time, user_id=user_id)

        return redirect("/")



def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
