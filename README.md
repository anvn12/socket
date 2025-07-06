# LƯU Ý: Chương trình và hướng dẫn trên hệ điều hành Windows
# Hướng dẫn khởi động chương trình
1. Đảm bảo đã cài đặt ClamAV (nếu chưa cài thì kéo đến phần cuối tệp tin này), đã host FTP server (đảm bảo biết các thông tin cần cho kết nối như địa chỉ IP, tên user, password...) và đã cài đặt Microsoft Visual Studio 2022.
2. Vào thư mục "socket-client", khởi động tệp tin "socket.sln" bằng Microsoft Visual Studio 2022.
3. Khởi động project "clamav-agent" (ở máy host server) và "socket-client" (ở máy chạy FTP client).
4. Sau khi khởi động "clamav-agent" (ở máy host server), nhập đường dẫn đến thư mục chứa ClamAV (đường dẫn có thể khác sau tùy thiết lập cài đặt). Nếu đường dẫn hợp lệ, chương trình sẽ hiện:
	`===ClamAV Agent===`
	`Waiting for file...`
5. Ở chương trình "socket-client" (ở máy chạy FTP client), lúc này có thể tiến hành nhập các câu lệnh để kết nối server (có thể gõ "help" hoặc "?" để hiện hướng dẫn các câu lệnh)

- Bước 2 và 3 có thể thay thế bằng: vào thư mục "socket-client", sau đó vào thư mục "x64", vào thư mục "Debug", chạy "clamav-agent.exe" (ở máy host server) và chạy "clamav-client.exe" (ở máy chạy FTP client).
# Câu lệnh mẫu và kết quả
- Nếu chế độ passive đang tắt (off) thì sẽ hiện dòng `200 PORT command successful.` (nếu có). Ngược lại, nếu chế độ passive đang bật (on) thì sẽ hiện dòng `227 Entering Passive Mode (192,168,157,1,78,130)` thay cho `200 PORT command successful.` (nếu có).
- Nếu nhập câu lệnh không hợp lệ sẽ hiện `Invalid command.`.
Sau đây là các lệnh và kết quả trả về mẫu (giả sử server IP là "192.168.157.1", passive off):
## File and Directory Operations
### ls
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `ls`
	- Nếu passive off:
	```
	200 PORT command successful.
	150 Starting data transfer.
	<danh sách thư mục, tệp tin>
	
	226 Operation successful
	```
	- Nếu passive on:
	```
	227 Entering Passive Mode (192,168,157,1,78,130)
	150 Starting data transfer.
	<danh sách thư mục, tệp tin>
	
	226 Operation successful
	```
### cd `<dir>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `cd "abc"`
	- Nếu thư mục đã tồn tại:
	`250 CWD command successful`
	- Nếu thư mục chưa tồn tại:
	`550 Unknown error`
### pwd
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `pwd`
	`257 "/" is current directory.`
### mkdir `<dir>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `mkdir "abc"`
	- Nếu thư mục đã tồn tại:
	`550 File or directory already exists`
	- Nếu thư mục chưa tồn tại:
	`257 "/abc" created successfully.`
### rmdir `<dir>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `mkdir "abc"`
	- Nếu thư mục đã tồn tại:
	`250 Directory deleted successfully.`
	- Nếu thư mục chưa tồn tại:
	`550 Couldn't open the directory`
### delete `<file>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `delete "a.txt"`
	- Nếu tệp tin đã tồn tại:
	`250 File deleted successfully.`
	- Nếu thư mục chưa tồn tại:
	`550 Couldn't open the file`
### rename `<old>` `<new>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `rename "a" "b"`
	- Nếu tệp tin (thư mục) đã tồn tại:
	```
	350 File (hoặc Directory) exists, ready for destination name.
	250 File or directory renamed successfully.
	```
	- Nếu tệp tin (thư mục) chưa tồn tại:
	```
	550 Unknown error
	503 Use RNFR first.
	```
## Upload and Download
### get, recv `<file>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `get "<file>"`
	```
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Downloading (ASCII): <file>
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
### mget `<files>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `mget "<file 1>" "<file 2>"`
	- Nếu prompt đang bật:
	```
	Get "<file 1>"? (Press 'y' to download): <nhập y để tải>
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Downloading (ASCII): <file 1>
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	
	Get "<file 2>"? (Press 'y' to download): <nhập y để tải>
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: i>
	200 Type set to I
	Downloading (Binary): <file 2>
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
	- Nếu prompt tắt:
	```
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Downloading (ASCII): <file 1>
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: i>
	200 Type set to I
	Downloading (Binary): <file 2>
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
### put `<file>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `put "<file>"`
	```
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Upload: "<file>"
	Scanning: "<file>"
	Scanning result: OK
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
- Dòng thông số dòng "Scanning result:"
	- "OK"
	- "WARNING: Clamscan found threats!"
	- "ERROR: Scan failed!"
	Nếu thông số khác "OK" thì không hiện 3 dòng cuối.
### mput `<files>`
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: `mput "<file 1>" "<file 2>"`
	- Nếu prompt đang bật:
	```
	Upload: "<file 1>"? (Press 'y' to upload): <nhập y để tải: y>
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Scanning: "<file 1>"
	Scanning result: OK
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	
	Upload: "<file 2>"? (Press 'y' to upload): <nhập y để tải: y>
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: i>
	200 Type set to I
	Scanning: "<file 2>" (999 Bytes)
	Scanning result: OK
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
	- Nếu prompt tắt:
	```
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: a>
	200 Type set to A
	Scanning: "<file 1>"
	Scanning result: OK
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	
	Select transfer mode: [A] ASCII or [I] Binary: <nhập chế độ: i>
	200 Type set to I
	Scanning: "<file 2>" (999 Bytes)
	Scanning result: OK
	200 PORT command successful.
	150 Starting data transfer.
	226 Operation successful
	```
- Dòng thông số dòng "Scanning result:"
	- "OK"
	- "WARNING: Clamscan found threats!"
	- "ERROR: Scan failed!"
	Nếu thông số khác "OK" thì không hiện 3 dòng cuối.
### prompt
`Passive mode Off.` hoặc `Passive mode On.`
## Session Management
### ascii
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: 
	`200 Type set to A`
### binary
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: 
	`200 Type set to I`
### status
- Nếu chưa kết nối vào server: 
	```
	Not connected.
	Type: ASCII; Mode: Active;
	Prompting: On.
	```
- Nếu đã kết nối vào server (giả sử IP server là "192.168.157.1"): 
	```
	Connected to 192.168.157.1.
	Type: Binary; Mode: Active;
	Prompting: On.
	```
- Các thông số cụ thể:
	- "Not connected." hoặc "Connected to `<host>`."
	- Type: "ASCII" hoặc "Binary"
	- Mode: "Active" hoặc "Passive"
	- Prompting: "On" hoặc "Off"
### passive
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: 
	`Passive mode Off.` hoặc `Passive mode On.`
### open `<host>`
- Nếu chưa kết nối vào server: 
	```
	Connected to 192.168.157.1
	220-FileZilla Server 1.10.3
	220 Please visit https://filezilla-project.org/
	202 UTF8 mode is always enabled. No need to send this command
	Username: <nhập username>
	331 Please, specify the password.
	Password: <nhập password>
	230 Login successful.
	```
- Nếu đã kết nối vào server: 
	`Already connected to 192.168.157.1, disconnect first.`
### close
- Nếu chưa kết nối vào server: 
	`Not connected.`
- Nếu đã kết nối vào server: 
	`221 Goodbye.`
### quit, bye
- Nếu chưa kết nối vào server: 
	```
	D:\socket\socket-client\x64\Debug\socket-client.exe (process 18972) exited with code 0 (0x0).
	Press any key to close this window . . .
	```
- Nếu đã kết nối vào server: 
	```
	221 Goodbye.
	
	D:\socket\socket-client\x64\Debug\socket-client.exe (process 18584) exited with code 0 (0x0).
	Press any key to close this window . . .
	```
Phần đường dẫn ở cuối kết quả có thể khác nhau.
### help, ?
```
================== FTP Client Help Menu ==================

1. File and Directory Operations
----------------------------------------------------------
  ls                      List files and folders on the FTP server
  cd      <dir>           Change current directory on the FTP server
  pwd                     Show current directory on the FTP server
  mkdir   <dir>           Create a new directory on the FTP server
  rmdir   <dir>           Remove a directory from the FTP server
  delete  <file>          Delete a file on the FTP server
  rename  <old>   <new>   Rename a file on the FTP server

2. Upload and Download
----------------------------------------------------------
  get     <file>          Download a file from the FTP server
  recv    <file>          Download a file from the FTP server (alias for "get")
  mget    <files>         Download multiple files
  put     <file>          Upload a file
  mput    <files>         Upload multiple files
  prompt                  Toggle confirmation for "mget/mput" operations

3. Session Management
----------------------------------------------------------
  ascii                   Set transfer mode to ASCII (text files)
  binary                  Set transfer mode to Binary (non-text files)
  status                  Show current session status
  passive                 Toggle passive FTP mode on/off
  open    <host>          Connect to FTP server
  close                   Disconnect from FTP server
  quit                    Exit the FTP client
  bye                     Exit the FTP client (alias for "quit")

4. Misc
----------------------------------------------------------
  help                    Show this help menu
  ?                       Show this help menu (alias for "help")

NOTE: If an argument includes spaces, wrap it in double quotes, e.g., "My File.txt".

==========================================================
```
# Phần mềm FTP Server đã sử dụng và cách thiết lập
Nhóm sinh viên đã dùng phần mềm FTP Server của FileZilla trong suốt quá trình thực hiện đồ án.
## Cách thiết lập FTP Server bằng FileZilla Server
1. Tải ứng dụng từ [FileZilla](https://filezilla-project.org/)
2. Chọn phiên bản và tiến hành cài đặt như ứng dụng bình thường.
3. Khi mở ứng dụng, chọn nút "Connect to Server...", và tiến hành nhập các thông tin của thiết bị Host. Chọn "OK".
4. Chọn "Server", chọn "Configure..."
5. Ở phần "Server listeners", chọn "Add" và thêm các thông tin của server (Address, Port, Protocol).
6. Ở phần "Users", có thể thêm và tùy chỉnh các user theo ý muốn.
7. Có thể tùy chỉnh các chức năng thêm nếu muốn.
8. Cuối cùng chọn "Apply", chọn "OK".

Nhóm sinh viên đã tham khảo cách thiết lập ở [Youtube FileZilla](https://youtu.be/tCftckKivkc?si=dEFg4bjRODUxOdLG). Có thể xem nếu cần thêm mô phỏng bằng hình ảnh.
# Tải và thiết lập ClamAV
1. Truy cập [ClamAV Download](https://www.clamav.net/downloads)
2. Chọn phiên bản và hệ điều hành (Windows)
3. Chọn tải phiên bản phù hợp (arm, win32, x64) và tải file .zip
4. Giải nén file .zip. Ghi lại đường dẫn của thư mục này để dùng khi khởi động ClamAV Agent (chẳng hạn "C:\Users\admin\Downloads\clamav-1.4.3.win.x64") 
5. Truy cập thư mục "conf_examples"
6. Di chuyển 2 tệp tin "clamd.conf.sample" và "freshclam.conf.sample" ra thư mục trước đó (thư mục sau khi giải nén có được, chẳng hạn "clamav-1.4.3.win.x64", nơi chứa tất cả các thư mục và tệp tin của ClamAV)
7. Đổi "clamd.conf.sample" và "freshclam.conf.sample" thành "clamd.conf", "freshclam.conf" (xóa phần đuôi .sample đi)
8. Truy cập lần lượt vào 2 tệp tin "clamd.conf" và "freshclam.conf"; xóa đi dòng chữ "Example"; sau đó lưu lại.
9. Mở cmd (còn gọi là "Command Prompt"), sau đó chuyển đến đường dẫn đã ghi lại ở bước 4 bằng lệnh "cd". Chẳng hạn: "cd "C:\Users\admin\Downloads\clamav-1.4.3.win.x64"". Nếu đường dẫn ở bước 4 nằm khác phân vùng ổ cứng, có thể thực hiện "cd /d `<đường dẫn>`".
10. Tiến hành gõ "freshclam.exe" ở cmd và đợi đến khi tải xong.
11. Lúc này ClamAV đã được tải, để kiểm tra có thể tiến hành chạy thử "clamscan `<file>`". Nếu có thể quét thì cài đặt thành công.

Nhóm sinh viên đã tham khảo cách cài đặt ở [Youtube ClamAV](https://youtu.be/9gQXBUJbSHE?si=R2lJ_mZxtO99K0_h). Có thể xem nếu cần thêm mô phỏng bằng hình ảnh.
