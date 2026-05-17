@echo off
setlocal

echo OK: starting ESP32 serial CSV logger
echo OK: this window will stay open while logging runs

set "ROOT=%~dp0.."
set "PYTHON=%ROOT%\.venv\Scripts\python.exe"

if not exist "%PYTHON%" (
  echo ERROR: Python not found at "%PYTHON%"
  pause
  exit /b 1
)

"%PYTHON%" "%~dp0serial_csv_logger.py" --port COM11 --output "%ROOT%\logs\air_quality.csv"

echo.
echo OK: logger stopped
pause