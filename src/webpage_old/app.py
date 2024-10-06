"""
- Author : Arun CS
- Date : 2024-10-03
- for v0.0.2
"""

import os

import solar_scraper
from flask import Flask, render_template
from flask_bootstrap import Bootstrap4
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)
bootstrap = Bootstrap4(app)
esp_url = "http://192.168.48.50/data"
esp_scraper = solar_scraper.Scraper(esp_url)

# TODO: Fix this later
# For Database HACK:
basedir = os.path.abspath(os.path.dirname(__file__))

app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///" + os.path.join(
    basedir, "database.db"
)
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

db = SQLAlchemy(app)


class Profile(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(20), unique=True, nullable=False)
    age = db.Column(db.Integer, nullable=False)

    # How the one object in the database look like
    def __repr__(self):
        return f"Profile {self.name} Age: {self.age}"


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/<name>")
def user(name):
    return render_template("user.html", name=name)


@app.route("/battery")
def page_battery():
    return render_template("battery.html")


# When opening this point it should show plots of graphs
@app.route("/plots")
def page_plots():

    labels = [
        "0:00 AM",
        "1:00 AM",
        "2:00 AM",
        "3:00 AM",
        "4:00 AM",
        "5:00 AM",
        "6:00 AM",
        "7:00 AM",
        "8:00 AM",
        "9:00 AM",
        "10:00 AM",
        "11:00 AM",
        "12:00 PM",
        "1:00 PM",
        "2:00 PM",
        "3:00 PM",
        "4:00 PM",
        "5:00 PM",
        "6:00 PM",
        "7:00 PM",
        "8:00 PM",
        "9:00 PM",
        "10:00 PM",
        "11:00 PM",
    ]
    data = esp_scraper.get_data()

    return render_template(template_name_or_list="plots.html", data=data, labels=labels)


if __name__ == "__main__":
    app.run(debug=True)
