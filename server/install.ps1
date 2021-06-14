mkdir "C:\ProgramData\VRLaunch"
Copy-Item api.py C:\ProgramData\VRLaunch\api.py
Copy-Item .\requirements.txt C:\ProgramData\VRLaunch\requirements.txt
Copy-Item .\start_server.bat C:\ProgramData\VRLaunch\start_server.bat
Set-Location "C:\ProgramData\VRLaunch"
virtualenv venv
.\venv\Scripts\activate.ps1
pip install -r requirements.txt
Remove-Item requirements.txt