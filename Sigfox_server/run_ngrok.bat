echo off
echo flask server create
start python server.py
echo wait flask server started ...
timeout 3
echo ngrok server created
ngrok.exe http 5000
echo server shutdown...!
pause