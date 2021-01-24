import http.client
import os
import requests
import urllib.parse
import json
from cs50 import SQL

from flask import redirect, render_template, request, session
from functools import wraps

def apology(message, code=400):
    """Render message as an apology to user."""
    def escape(s):
        """
        Escape special characters.

        https://github.com/jacebrowning/memegen#special-characters
        """
        for old, new in [("-", "--"), (" ", "-"), ("_", "__"), ("?", "~q"),
                         ("%", "~p"), ("#", "~h"), ("/", "~s"), ("\"", "''")]:
            s = s.replace(old, new)
        return s
    return render_template("apology.html", top=code, bottom=escape(message)), code



def login_required(f):
    """
    Decorate routes to require login.

    http://flask.pocoo.org/docs/1.0/patterns/viewdecorators/
    """
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if session.get("user_id") is None:
            return redirect("/login")
        return f(*args, **kwargs)
    return decorated_function



def lookup(symbol):

    conn = http.client.HTTPSConnection("apidojo-yahoo-finance-v1.p.rapidapi.com")

    headers = {
        'x-rapidapi-host': "apidojo-yahoo-finance-v1.p.rapidapi.com",
        'x-rapidapi-key': "6926236f28msh36f184e9eb9687dp16bb6ejsn06c71eefd46a"
        }


    conn.request("GET", f"/stock/v2/get-statistics?region=US&symbol={urllib.parse.quote_plus(symbol)}", headers=headers)
    res = conn.getresponse()
    data = res.read().decode("utf-8")

    # Parse Response
    try:
        data2 = json.loads(data)
        return {
            "symbol": data2["quoteType"]["symbol"],
            "name": data2["quoteType"]["longName"],
            "beta": data2["summaryDetail"]["beta"]["raw"],
            "price": data2["price"]["regularMarketPrice"]["raw"],
            "trailing_pe": data2["summaryDetail"]["trailingPE"]["raw"],
            "forward_pe": data2["summaryDetail"]["forwardPE"]["raw"],
            "peg": data2["defaultKeyStatistics"]["pegRatio"]["raw"]
        }
    except (KeyError, TypeError, ValueError):
        return None


def usd(value):
    """Format value as USD."""
    return f"${value:,.2f}"

def decimal(value):
    return f"{value:,.2f}"

def percent(value):
    return f"{value:,.2f}%"