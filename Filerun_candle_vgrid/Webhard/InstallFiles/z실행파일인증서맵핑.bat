signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "���Ϸ� �ٿ�δ�" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"  ../Bin/FilerunDown.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "���Ϸ� ���δ�" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/FilerunUp.exe

signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "���Ϸ�" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/CreateShortCut.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "���Ϸ�" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/UpdateWindow.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "���Ϸ� Active-X" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!" ../Bin/FilerunWebControl.dll
