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
users = {
    "admin": "admin",
}

def send_line_notify(notification_message):
    line_notify_token = 'dGeM6thZCz1B5a6DKTA9jh77nY5grwPTgylQ5ZPERwf'
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
def top_page():
    return render_template("/debug_page.html")

@app.route("/dataform", methods = ["POST"])
def data_form():
    Tumble_msg = "おじいちゃんの転倒検知を検知しました！\n"\
                  "https://www.google.com/maps?q={},{} \n"\
                  "誤差半径は{}mです。 "\
                  "位置情報検出方法は{}で検出を行いました。"

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
            # Extracting location information from json. If have GPS => Location information acquisition()
            if request_data["computedLocation"]["status"] == 1:
                lat, lng = (int(request_data["computedLocation"]["lat"]), int(request_data["computedLocation"]["lng"]))
                # Radius of error (meters)
                radius = int(request_data["computedLocation"]["radius"])
            else:
                # donthave => Register Null Island(joke :DDDDDDDDDDDDDDD)
                lat, lng = (0, 0)
                radius = 0
        else:
            # If the POST communication was from a different agent.This time you reply with a json and do nothing.
            return jsonify({'status':'It is sent by an unregistered agent. We are unable to receive it.'})

    return jsonify({'status':'OK'})

@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                               'favicon.ico', mimetype='image/vnd.microsoft.icon')

@app.errorhandler(404)
def page_not_found(error):
    return render_template('/error/404.html')

if __name__ == "__main__":
    # debug mode.
    app.run(debug=True, port=3000)
    # test server create
    #app.run(debug=False, host='0.0.0.0', port=80)