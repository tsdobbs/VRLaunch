from fastapi import FastAPI, Response
import subprocess


#Assume that user is already logged in - app launches in user context of whoever is active
#Steam app code for SteamVR is 250820
launch_string = f'"C:\Program Files (x86)\Steam\Steam.exe" -applaunch 250820'

app = FastAPI()

@app.get("/awake")
def are_you_awake():
    return "OK"

@app.get("/launchVR")
def launchVR(response: Response):
    result = subprocess.run(launch_string, capture_output=True, text=True, shell=True)
    if result.returncode == 0:
        response.status_code = 200
        return "Launch Successful"
    elif result.returncode==1326:
        response.status_code = 403
        return "Username / Password For Launching Steam Didn't Work"
    else:
        response.status_code = 500
        return result.stdout, result.stderr, result.returncode