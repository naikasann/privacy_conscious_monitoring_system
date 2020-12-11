import requests

def main():
    send_string = "おじいちゃん転倒検知！\n"\
                  "https://www.google.com/maps?q=36.530187,136.627849 \n"\
                  "半径これくらいです。確認して!{} "
    s = send_string.format("aaa")
    send_line_notify(s)

def send_line_notify(notification_message):
    line_notify_token = 'dGeM6thZCz1B5a6DKTA9jh77nY5grwPTgylQ5ZPERwf'
    line_notify_api = 'https://notify-api.line.me/api/notify'
    headers = {'Authorization': f'Bearer {line_notify_token}'}
    data = {'message': f'message: {notification_message}'}
    requests.post(line_notify_api, headers = headers, data = data)

if __name__ == "__main__":
    main()