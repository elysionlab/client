signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 SETUP" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"   ./Filerun.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 SETUP" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"   ./Filerun_setup.exe
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 Active-X" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"   ./FilerunWebControl.CAB
signtool sign /t http://timestamp.verisign.com/scripts/timestamp.dll /d "파일런 코덱" /du "http://filerun.co.kr/" /f "www.filerun.co.kr.pfx" -p "dpffl0902!"   ./K-Lite_Codec_Pack_640_Basic.exe
