signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 다운로더" /du "http://filerun.co.kr/" /n "Eye of Imagination Inc"  ../Bin/FilerunDown.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 업로더" /du "http://filerun.co.kr/" /n "Eye of Imagination Inc"  ../Bin/FilerunUp.exe

signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런" /du "http://filerun.co.kr/" /n "Eye of Imagination Inc"  ../Bin/CreateShortCut.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런" /du "http://filerun.co.kr/" /n "Eye of Imagination Inc"  ../Bin/UpdateWindow.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 Active-X" /du "http://filerun.co.kr/" /n "Eye of Imagination Inc"  ../Bin/FilerunWebControl.dll
