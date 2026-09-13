# ỨNG DỤNG QUẢN LÝ QUÁN CAFE MINI (Console-Based POS)

> Đồ án môn học **Lập trình cơ sở 2 (LTCS2)** – Trường Đại học Thăng Long.  
> Chương trình quản lý nghiệp vụ bán hàng, điều phối bàn, kiểm soát tồn kho và phân tích doanh thu được xây dựng bằng C++.

---

## 1. Tổng quan đề tài (Overview)

Dự án mô phỏng hệ thống quản trị quy trình phục vụ tại một quán cafe quy mô nhỏ. Chương trình xử lý toàn bộ chu trình nghiệp vụ thực tế từ khâu tiếp đón khách, quản lý trạng thái không gian bàn (dùng tại chỗ hoặc mang đi), xử lý giỏ hàng theo chu trình **CRUD** linh hoạt, đến áp dụng chính sách chiết khấu khách hàng VIP, tính thuế VAT tự động và lưu trữ dữ liệu giao dịch phục vụ phân tích thống kê[cite: 2, 3].

---

## 2. Tính năng cốt lõi (Key Features)

* **Điều phối không gian & Phân tầng sơ đồ quán:** Quản lý 18 bàn trên ma trận 2 tầng (Trong nhà, Ngoài trời, Phòng họp) với 3 trạng thái thời gian thực: `Trống`, `Có khách`, `Đang dọn`[cite: 2, 3]. Hỗ trợ phân luồng nhanh cho khách mua mang về (`Take-away`)[cite: 2, 3].
* **Thực đơn thông minh & Kiểm soát tồn kho:** Tự động phân loại danh mục (Cà phê, Trà, Sinh tố...) dựa trên mã ID[cite: 2, 3]. Tự động tính phụ thu/giảm giá theo kích cỡ (Size S/M/L) và chặn đặt hàng khi vượt ngưỡng tồn kho[cite: 2, 3].
* **Quản lý giỏ hàng đầy đủ chu trình CRUD:**
  * **Create:** Đặt món mới hoặc gọi bổ sung món vào đơn hiện tại[cite: 2].
  * **Read:** Trực quan hóa danh sách món đã chọn theo dạng bảng chi tiết[cite: 2].
  * **Update:** Điều chỉnh linh hoạt số lượng hoặc ghi chú của từng món, tự động bù/trừ số lượng tồn kho theo thời gian thực[cite: 2].
  * **Delete:** Hủy món khỏi đơn và tự động hoàn trả số lượng về kho thực đơn tổng[cite: 2].
* **Thanh toán & Chăm sóc khách hàng VIP:** Xác thực mã thành viên VIP để chiết khấu 10%[cite: 2, 3], tự động tính thuế VAT 8%[cite: 2, 3], tạo mã hóa đơn tự tăng và xuất biên lai với mốc thời gian thực của hệ thống[cite: 2, 3].
* **Lưu trữ tệp & Phân tích Best Seller:** Lưu vết toàn bộ lịch sử giao dịch vào tệp `sales_history.txt`[cite: 2, 3]. Hỗ trợ phân tích, trích lọc dữ liệu và áp dụng thuật toán sắp xếp để xếp hạng Top 3 sản phẩm bán chạy nhất theo ngày, tháng hoặc năm[cite: 2, 3].

---

## 3. Kiến trúc kỹ thuật & Cấu trúc dữ liệu

* **Phương pháp phát triển:** Lập trình hướng thủ tục (Procedural Programming) kết hợp Lập trình hướng cấu trúc.
* **Cấu trúc dữ liệu (Data Structures):**
  * `struct`: Đóng gói các thực thể dữ liệu nghiệp vụ (`DoUong`, `Ban`, `MonDaDat`, `HoaDon`, `ThongKeMon`)[cite: 2, 3].
  * **Mảng động 2 chiều (`Ban** maTran`):** Cấp phát trên vùng nhớ Heap bằng toán tử `new`/`delete` để mô phỏng không gian sơ đồ bàn[cite: 2, 3].
  * **Mảng động (`DoUong* menu`):** Quản lý thực đơn với kích thước tùy biến theo thời gian chạy[cite: 2, 3].
  * **STL `std::vector`:** Lưu trữ giỏ hàng linh hoạt, tối ưu thao tác thêm/xóa phần tử theo nhu cầu đơn hàng[cite: 2, 3].
* **Kỹ thuật xử lý an toàn:**
  * Bắt lỗi ngoại lệ (`try-catch` kết hợp `throw runtime_error`)[cite: 2, 3].
  * Xóa cờ lỗi luồng nhập (`cin.clear()` & `cin.ignore()`) ngăn chặn hoàn toàn hiện tượng trôi lệnh và tràn bộ đệm khi nhập sai kiểu dữ liệu[cite: 2, 3].
  * Tối ưu hiệu năng bộ nhớ bằng con trỏ hằng và hằng tham chiếu (`const &`)[cite: 2, 3].

---

## 4. Cấu trúc thư mục (Project Structure)

```text
├── BAITAPLON_LTCS2_Nhom5.cpp   # Mã nguồn chính của chương trình C++
├── bao_cao BTL LTCS2 Nhom5.docx# Báo cáo chi tiết đề tài
├── sales_history.txt           # Tệp cơ sở dữ liệu lưu vết lịch sử đơn hàng (tự động tạo khi chạy)
└── README.md                   # Tài liệu giới thiệu và hướng dẫn thực thi
