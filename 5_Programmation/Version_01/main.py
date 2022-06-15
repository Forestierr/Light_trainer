"""Light Trainer is a project based on Blaze Pod product.

© Robin Forestier - Light Trainer - 2021/2022
"""

from flask import Flask, render_template
from werkzeug.exceptions import HTTPException

app = Flask(__name__)


@app.errorhandler(HTTPException)
def handle_exception(e):
    """handle_exception : handle http error.

    :param e: The http error.
    :type e: HTTPException
    :return: The HTML error page.
    :rtype: render_template
    """

    e = str(e)  # 404 not ...  :  The request URL ...
    code = e.split(":")  # 404 Not Found

    # This is a function that takes in an error code and returns the error page for that error.
    return render_template('404.html', error=code[1], title=code[0]), code[0][0:3]


@app.route("/")
def accueil():
    """accueil is a main page.
    The user can choise the mode to use."""

    return render_template("accueil.html")


@app.route("/info")
def info():
    """The info page is an help page.
    It contains only text."""

    return render_template("info.html")


@app.route("/settings", methods=['POST', 'GET'])
def settings():
    """settings page not use for the moment."""

    return render_template("settings.html")


@app.route("/mode-1", methods=['POST', 'GET'])
def mode1():
    """mode 1.
    This mode is : """

    return render_template("mode-1.html")


@app.route("/mode-2", methods=['POST', 'GET'])
def mode2():
    """mode 2.
    This mode is : """

    return render_template("mode-2.html")


@app.route("/mode-3", methods=['POST', 'GET'])
def mode3():
    """mode 3.
    This mode is : """

    return render_template("mode-3.html")


if __name__ == "__main__":
    # Start the Flask server
    app.run(host='0.0.0.0', port=80, debug=True)
