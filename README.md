# LƯU Ý: Chương trình và hướng dẫn trên hệ điều hành Windows
# Hướng dẫn khởi động chương trình
1. Đảm bảo đã cài đặt ClamAV (nếu chưa cài thì kéo đến phần cuối tệp tin này), đã host FTP server (đảm bảo biết các thông tin cần cho kết nối như địa chỉ IP, tên user, password...) và đã cài đặt Microsoft Visual Studio 2022.
2. Vào thư mục "socket-client", khởi động tệp tin "socket.sln" bằng Microsoft Visual Studio 2022.
3. Khởi động lần lượt 2 project "clamav-agent" và "socket-client".
4. Sau khi khởi động "clamav-agent", nhập đường dẫn đến thư mục chứa ClamAV (đường dẫn có thể khác sau tùy thiết lập cài đặt). Nếu đường dẫn hợp lệ, chương trình sẽ hiện:
	`===ClamAV Agent===`
	`Waiting for file...`
5. Ở chương trình "socket-client", lúc này có thể tiến hành nhập các câu lệnh để kết nối server (có thể gõ "help" hoặc "?" để hiện hướng dẫn các câu lệnh)
# Câu lệnh mẫu và kết quả

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
