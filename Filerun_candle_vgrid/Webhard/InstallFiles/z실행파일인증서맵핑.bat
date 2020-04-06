signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 다운로더" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"  ../Bin/FilerunDown.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 업로더" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/FilerunUp.exe

signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/CreateShortCut.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/UpdateWindow.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 Active-X" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/FilerunWebControl.dll
