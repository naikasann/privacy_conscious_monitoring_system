from flask import Flask
from flask import render_template, redirect, jsonify
from flask import request
from flask import send_from_directory
from flask_httpauth import HTTPBasicAuth

import requests
import csv
import datetime
import json
import os

# flask app create
app = Flask(__name__)
app.secret_key = b"88888888"
# Basic Certification
auth = HTTPBasicAuth()
# for basic certification.
users = {"admin": "admin"}
userdatas = "userdata.csv"

def send_line_notify(notification_message):
    # Status of the line app to be notified
    line_notify_token = 'TOKENNNNN'
    line_notify_api = 'https://notify-api.line.me/api/notify'
    headers = {'Authorization': f'Bearer {line_notify_token}'}
    data = {'message': f'message: {notification_message}'}
    requests.post(line_notify_api, headers = headers, data = data)

# get basic certification user name and password
@auth.get_password
def get_pw(username):
    if username in users:
        return users.get(username)
    return None

@app.route("/")
@auth.login_required
def inside():
    # take all data.
    datalist = []
    with open(userdatas, "r", encoding="utf-8") as userfile:
        reader = csv.reader(userfile)
        for row in reader:
            # DEVID, TIME, DATA, lat, lng, radius.
            datalist.append([row[1], row[2], (row[3], row[4]), row[5]])
    # Put the new data at the top.
    datalist.reverse()
    return render_template("/home.html", datalist=datalist, title="おじいちゃんチェッカー")

@app.route("/check")
def check():
    # Messages for checking
    send_msg = "おじいちゃんの状態 : {}\n"\
                "https://www.google.com/maps?q={},{} \n"\
                "誤差半径は{}mです。\n "\
                "最終更新時間 : {}"
    # take all data (There's only room for improvement.)
    datalist = []
    with open(userdatas, "r", encoding="utf-8") as userfile:
        reader = csv.reader(userfile)
        for row in reader:
            # TIME, DATA, lat, lng, radius.
            datalist.append([row[1], row[2], row[3], row[4], row[5]])
    # Extract the last data in the list
    last_datalist = datalist[len(datalist) - 1]

    # create msg and line notify.
    send_msg = send_msg.format(last_datalist[1], last_datalist[2], last_datalist[3], last_datalist[4], last_datalist[0])
    send_line_notify(send_msg)

    return redirect("/home")

@app.route("/dataform", methods = ["POST"])
def data_form():
    # Message when a fall is detected
    Tumble_msg = "おじいちゃんの転倒検知を検知しました！\n"\
                    "https://www.google.com/maps?q={},{} \n"\
                    "誤差半径は{}mです。 "\
                    "安否の確認を行うのか、探してみてください"

    tumbling_flg = False
    # communicating with the correct means of request?
    if request.method == 'POST':
        # Find out who sent it (only SIGFOX will receive it this time)
        if request.headers.get('User-Agent') == "SIGFOX":
            # Retrieve the body information and convert it to json format
            request_data = json.loads(request.data)
            # Retrieving the data needed for the database. (Device ID, receive time, actual raw data)
            device_id = request_data["device"]
            recv_time = datetime.datetime.fromtimestamp(request_data["time"])
            data = request_data["data"]
            if int(data) == 1111:
                data = "〇〇部屋にいます。"
            elif int(data) == 1112:
                data = "〇〇部屋にいます。"
            else:
                data = "転倒しました！"
                tumbling_flg = True

            # Extracting location information from json. If have GPS => Location information acquisition()
            if request_data["computedLocation"]["status"] == 1:
                lat, lng = (int(request_data["computedLocation"]["lat"]), int(request_data["computedLocation"]["lng"]))
                # Radius of error (meters)
                radius = int(request_data["computedLocation"]["radius"])
            else:
                # donthave => Register Null Island(joke :DDDDDDDDDDDDDDD)
                lat, lng = (0, 0)
                radius = 0

            # write database(csv)
            with open(userdatas, "a", encoding="utf-8") as userdata:
                writer = csv.writer(userdata)
                writer.writerow([device_id, recv_time, data, lat, lng, radius])

            # Notify the line as we have confirmed a fall.
            if tumbling_flg:
                Tumble_msg.format(lat, lng, radius)
                send_line_notify(Tumble_msg)

        else:
            # If the POST communication was from a different agent.This time you reply with a json and do nothing.
            return jsonify({'status':'It is sent by an unregistered agent. We are unable to receive it.'})
    else:
        # if not post.
        return jsonify({"status":"It's being sent in a different way."})

    return jsonify({'status':'OK'})

@app.route("/information")
def login():
    return render_template("/information.html", title="お問い合わせ")

@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                               'favicon.ico', mimetype='image/vnd.microsoft.icon')

@app.errorhandler(404)
def page_not_found(error):
    return render_template('/error/404.html')

if __name__ == "__main__":
    # debug mode.
    app.run(debug=True)