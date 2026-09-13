#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <iomanip>
#include <stdexcept>
#include <vector>
#include <limits>
#include <sstream> // Biến một chuỗi bình thường thành một "luồng dữ liệu" (giống như luồng nhập từ bàn phím)
#include <ctime>   // Thư viện để lấy thời gian thực
using namespace std;

// Cấu trúc Đồ Uống (Menu)
struct DoUong
{
    int id;      // Mã món
    string name; // Tên món
    int price;
    char size;        // Size: 'S', 'M', 'L'
    int status = 100; // Trạng thái: Số lượng tồn kho (Nếu = 0 là hết hàng)
};

// Cấu trúc Bàn
struct Ban
{
    int tableNum; // Số bàn
    string pos;   // Vị trí: "Trong nhà", "Ngoài trời", "Tầng 2"
    int seatNum;  // Số lượng ghế
    int status;   // Trạng thái: 1 = Trống, 0 = Có khách, -1 = Đang dọn
};

// Cấu trúc Món Đã Đặt (Dùng để bỏ vào Hóa Đơn)
struct MonDaDat
{
    int id;
    string name;
    int soLuong;
    int price;
    long long thanhTien; // Dùng long long tránh tràn số nếu số lượng lớn
    string note;
};

// Cấu trúc Hóa Đơn
struct HoaDon
{
    string id;                 // Mã hóa đơn (Ví dụ: HD001)
    int tableNum;              // Số bàn
    vector<MonDaDat> foodList; // Danh sách các món khách đã gọi (Giỏ hàng)
    long long total = 0;       // Tổng tiền các món (chưa thuế)
    const float VAT = 1.08;    // Hệ số VAT (Nhân trực tiếp để ra tiền sau thuế)
    float discount = 0.0;      // Giảm giá VIP (10% = 0.1, 20% = 0.2)
    long long finals = 0;      // Thành tiền (Sẽ được tính qua hàm)
    string time;               // Thời gian xuất hóa đơn

    // Hàm tính toán thành tiền cuối cùng
    void tinhThanhTien()
    {
        // Thành tiền = (Tổng tiền * 1.08) - (Tổng tiền * Giảm giá)
        // Lưu ý: Ép kiểu (long long) để cắt bỏ số thập phân thừa khi tính toán tiền VND
        finals = (long long)(total * VAT) - (long long)(total * discount);
    }
};

// dùng để thống kê món trong hàm tìm best seller
struct ThongKeMon
{
    int id;
    string name;
    int tongSoLuong;
};

void inMenu(const DoUong *menu, const int &soLuongMenu);
void inSoDo(Ban **maTran);
int timViTriMonTheoID(const DoUong *menu, const int &soLuongMenu, const int &idMon);
void inGioHangHienTai(const vector<MonDaDat> &gioHang);
void xoaMonKhoiGio(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu);
void chinhSuaMonTrongGio(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu);
void quanLyGioHang(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu);
vector<MonDaDat> nhapChonDoUong(DoUong *menu, const int &soLuongMenu);
int xuLyChonBan(Ban **maTran);
string layThoiGianHienTai();
double xuLyKhachVIP(const string *danhSachVIP, const int &soLuongVIP);
void luuLichSuGiaoDich(const HoaDon &bill);                                                         // ghi vào file
void xemBaoCaoBestSeller(const string &tieuChi, const DoUong *menuQuan, const int &soLuongMonMenu); // đọc file và in ra

int main()
{
    // 1. Khởi tạo danh sách ID VIP của quán
    int soLuongVIP = 3;
    string *danhSachVIP = new string[soLuongVIP]{"VIP001", "TLU2024", "KH_VIP99"};

    // dat ban
    // Cấp phát mảng động 2 chiều cho maTran
    int rows = 6;
    int cols = 3;
    Ban **maTran = new Ban *[rows];
    for (int i = 0; i < rows; i++)
    {
        maTran[i] = new Ban[cols];
    }
    int dem = 1;

    // Khởi tạo dữ liệu mẫu cho ma trận
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            maTran[i][j].tableNum = dem++;
            maTran[i][j].status = 1;  // Khởi tạo tất cả đều trống
            maTran[i][j].seatNum = 4; // Giả sử mặc định bàn 4 ghế

            // Phân bổ vị trí ngẫu nhiên để test
            if (i == 0 || i == 3)
                maTran[i][j].pos = "Ngoai troi";
            else if (i == 1 || i == 2 || i == 4)
                maTran[i][j].pos = "Trong nha";
            else if (i == 5)
            {
                maTran[i][j].seatNum = 8; // phòng họp có nhiều ghế hơn bàn thường
                maTran[i][j].pos = "Phong hop";
            }
        }
    }

    // Cấp phát mảng động 19 phần tử kiểu DoUong
    int soLuongMenu = 19;
    DoUong *menu = new DoUong[soLuongMenu]{
        // 1xx - Cafe
        {101, "Ca phe den", 20000},
        {102, "Ca phe sua", 25000},
        {103, "Bac xiu", 29000},
        {104, "Ca phe muoi", 35000},

        // 2xx - Trà
        {201, "Tra dao cam sa", 40000},
        {202, "Tra vai", 35000},
        {203, "Tra sen vang", 45000},
        {204, "Tra nhai", 30000},

        // 3xx - Sinh tố
        {301, "Sinh to bo", 45000},
        {302, "Sinh to xoai", 40000},
        {303, "Sinh to dau", 40000},
        {304, "Sinh to mang cau", 42000},

        // 4xx - Đá xay
        {401, "Matcha da xay", 55000},
        {402, "Cookies Oreo da xay", 50000},
        {403, "Chocolate da xay", 55000},
        {404, "Ca phe da xay", 45000},

        // 5xx - Còn lại
        {501, "Matcha Latte", 50000},
        {502, "Cacao Chocolate", 45000},
        {503, "Cam que mat ong", 45000}};

    char tiepTuc;
    int billCount = 0; // đếm số hóa đơn để in ra id hóa đơn mỗi lần tăng lên 1 đơn vị;
    do
    {
        // Gọi hàm chọn bàn
        int soBanDuocChon = xuLyChonBan(maTran);

        inMenu(menu, soLuongMenu);

        vector<MonDaDat> gioHang = nhapChonDoUong(menu, soLuongMenu);

        if (!gioHang.empty())
        {
            quanLyGioHang(gioHang, menu, soLuongMenu);
        }

        if (gioHang.empty())
        {
            cout << "KHACH HANG CHUA ORDER MON GI.\n";
        }
        else
        {
            billCount++;
            // 2. Format mã hóa đơn (Sinh ra chuỗi HD_001, HD_002...)
            stringstream ss;
            ss << "HD_" << setfill('0') << setw(3) << billCount;

            // KHỞI TẠO HÓA ĐƠN VÀ NẠP DỮ LIỆU
            HoaDon billCuoiCung;
            billCuoiCung.id = ss.str();
            billCuoiCung.tableNum = soBanDuocChon; // Giả sử khách ngồi bàn số 5
            billCuoiCung.foodList = gioHang;
            billCuoiCung.discount = xuLyKhachVIP(danhSachVIP, soLuongVIP); // Gọi hàm xử lý VIP và gán thẳng kết quả vào biến discount
            billCuoiCung.time = layThoiGianHienTai();

            // 2. TÍNH TỔNG TIỀN MÓN (Chưa thuế)
            for (const auto &mon : billCuoiCung.foodList)
            {
                billCuoiCung.total += mon.thanhTien;
            }

            // GỌI HÀM TÍNH THÀNH TIỀN TỰ ĐỘNG BÊN TRONG STRUCT
            billCuoiCung.tinhThanhTien();

            // 4. IN RA KẾT QUẢ HÓA ĐƠN

            cout << "\n===================================================\n";
            cout << "               HOA DON THANH TOAN                  \n";
            cout << "===================================================\n";
            cout << "Ma HD: " << billCuoiCung.id;

            if (billCuoiCung.tableNum == -1)
            {
                cout << " | Mang di\n";
            }
            else
            {
                cout << " | Ban so: " << billCuoiCung.tableNum << "\n";
            }

            cout << "Thoi gian: " << billCuoiCung.time << "\n";
            cout << "---------------------------------------------------\n";

            for (const auto &mon : billCuoiCung.foodList)
            {
                // In thông tin món chính
                cout << mon.soLuong << " x " << left << setw(20) << mon.name
                     << right << setw(15) << mon.thanhTien << " VND\n";

                // KIỂM TRA VÀ IN GHI CHÚ NẾU CÓ
                if (!mon.note.empty())
                {
                    cout << "      => Ghi chu: " << mon.note << "\n";
                }
            }

            cout << "---------------------------------------------------\n";

            // In ra các thông số để khách hàng đối chiếu
            cout << left << setw(30) << "TONG TIEN MON (Chua VAT):"
                 << right << setw(15) << billCuoiCung.total << " VND\n";

            // Tiền giảm giá = total * discount
            cout << left << setw(30) << "GIAM GIA VIP (" + to_string((int)(billCuoiCung.discount * 100)) + "%):"
                 << right << setw(15) << -(long long)(billCuoiCung.total * billCuoiCung.discount) << " VND\n";

            cout << "===================================================\n";

            // In ra kết quả cuối cùng đã được tính toán từ hàm tinhThanhTien()
            cout << left << setw(30) << "THANH TIEN (Da bao gom 8% VAT):"
                 << right << setw(15) << billCuoiCung.finals << " VND\n";

            cout << "===================================================\n";
            cout << "             XIN CAM ON QUY KHACH!                 \n";

            luuLichSuGiaoDich(billCuoiCung); // lưu bill vào file
        }

        cout << "\nBan co muon don khach tiep theo khong? (Y/N): ";
        cin >> tiepTuc;
    } while (tiepTuc == 'y' || tiepTuc == 'Y');

    cout << "ket thuc ca lam";

    char xemBaoCao;
    while (true)
    {
        cout << "\n=> Quan ly co muon xem Bao Cao Best Seller khong? (Y/N): ";
        cin >> xemBaoCao;

        // Trường hợp 1: Nhập Y (Đồng ý xem)
        if (xemBaoCao == 'y' || xemBaoCao == 'Y')
        {
            string tieuChi;
            cout << ">> Nhap thoi gian muon thong ke (VD: 25/04/2026 hoac 04/2026 hoac 2026): ";
            cin >> tieuChi;

            // Gọi hàm thống kê
            xemBaoCaoBestSeller(tieuChi, menu, soLuongMenu);

            // Sau khi in báo cáo xong, code sẽ chạy xuống cuối và tự động quay lại đầu vòng lặp while(true)
        }
        // Trường hợp 2: Nhập N (Không muốn xem nữa / Thoát)
        else if (xemBaoCao == 'n' || xemBaoCao == 'N')
        {
            cout << ">> Da thoat tinh nang xem bao cao.\n";
            break; // Chỉ khi nhập N mới thoát vòng lặp để kết thúc chương trình
        }
        // Trường hợp 3: Nhập sai phím (Bắt lỗi)
        else
        {
            cout << "[-] Lua chon khong hop le! Vui long chi nhap 'Y' hoac 'N'.\n";
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        delete[] maTran[i];
    }
    delete[] maTran;
    delete[] danhSachVIP;
    delete[] menu;

    maTran = nullptr;
    danhSachVIP = nullptr;
    menu = nullptr;

    cout << "\nCHUC BAN MOT NGAY TOT LANH! TAM BIET.\n";
    return 0;
}

void inMenu(const DoUong *menu, const int &soLuongMenu)
{
    cout << "\n===================================================\n";
    cout << "                  MENU QUAN CAFE                   \n";
    cout << "===================================================\n";

    cout << " [*] LUU Y GIA KICH CO (SIZE):\n";
    cout << "  - Gia niem yet tren Menu la gia mac dinh (Size M).\n";
    cout << "  - Chon Size S : Tru di 5.000 VND\n";
    cout << "  - Chon Size L : Cong them 5.000 VND\n";
    cout << "===================================================\n";

    int currentCategory = 0; // Biến dùng để theo dõi nhóm đồ uống hiện tại

    // Vòng lặp duyệt qua toàn bộ các món trong vector
    for (int i = 0; i < soLuongMenu; i++)
    {

        // Lấy chữ số hàng trăm để xác định danh mục (VD: 201 / 100 = 2)
        int category = menu[i].id / 100;

        // Nếu mã category thay đổi -> Đã chuyển sang một nhóm đồ uống mới
        if (category != currentCategory)
        {

            // In tên danh mục tương ứng
            cout << "\n---------------------------------------------------\n";
            if (category == 1)
                cout << "                    CA PHE                         \n";
            else if (category == 2)
                cout << "                      TRA                          \n";
            else if (category == 3)
                cout << "                    SINH TO                        \n";
            else if (category == 4)
                cout << "                    DA XAY                         \n";
            else if (category == 5)
                cout << "                  THUC UONG KHAC                   \n";
            cout << "---------------------------------------------------\n";

            // In thanh tiêu đề cột cho mỗi nhóm
            cout << left << setw(10) << "ID"
                 << left << setw(25) << "TEN MON"
                 << right << setw(14) << "GIA (VND)" << "\n";
            cout << "---------------------------------------------------\n";

            // Cập nhật lại biến cờ hiệu
            currentCategory = category;
        }

        // Tiến hành in thông tin của từng món ăn
        cout << left << setw(10) << menu[i].id
             << left << setw(25) << menu[i].name
             << right << setw(14) << menu[i].price << "\n";
    }

    cout << "===================================================\n";
}

int timViTriMonTheoID(const DoUong *menu, const int &soLuongMenu, const int &idMon)
{
    for (int i = 0; i < soLuongMenu; i++)
    {
        if (menu[i].id == idMon)
        {
            return i;
        }
    }
    return -1;
}

void inGioHangHienTai(const vector<MonDaDat> &gioHang)
{
    cout << "\n-------------------------------------------------------------\n";
    cout << "                    GIO HANG HIEN TAI                        \n";
    cout << "-------------------------------------------------------------\n";
    cout << left << setw(5) << "STT"
         << left << setw(30) << "TEN MON (SIZE)"
         << right << setw(6) << "SL"
         << right << setw(15) << "THANH TIEN" << "\n";
    cout << "-------------------------------------------------------------\n";

    for (size_t i = 0; i < gioHang.size(); i++)
    {
        cout << left << setw(5) << (i + 1)
             << left << setw(30) << gioHang[i].name
             << right << setw(6) << gioHang[i].soLuong
             << right << setw(11) << gioHang[i].thanhTien << " VND\n";
        if (!gioHang[i].note.empty())
        {
            cout << "      => Ghi chu: " << gioHang[i].note << "\n";
        }
    }
    cout << "-------------------------------------------------------------\n";
}

void xoaMonKhoiGio(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu)
{
    if (gioHang.empty())
    {
        cout << "[-] Gio hang hien dang trong, khong the xoa!\n";
        return;
    }

    inGioHangHienTai(gioHang);

    int sttChon;
    cout << ">> Nhap STT mon muon XOA (Nhap 0 de huy): ";
    if (!(cin >> sttChon) || sttChon < 0 || sttChon > (int)gioHang.size())
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "[-] STT khong hop le!\n";
        return;
    }

    if (sttChon == 0)
    {
        cout << "[!] Da huy thao tac xoa.\n";
        return;
    }

    int idx = sttChon - 1;
    // Hoàn trả số lượng món về lại kho menu
    int viTriKho = timViTriMonTheoID(menu, soLuongMenu, gioHang[idx].id);
    if (viTriKho != -1)
    {
        menu[viTriKho].status += gioHang[idx].soLuong;
    }

    cout << "=> [Thanh cong] Da xoa mon: " << gioHang[idx].name << " khoi gio hang.\n";
    gioHang.erase(gioHang.begin() + idx);
}

void chinhSuaMonTrongGio(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu)
{
    if (gioHang.empty())
    {
        cout << "[-] Gio hang hien dang trong, khong the sua!\n";
        return;
    }

    inGioHangHienTai(gioHang);

    int sttChon;
    cout << ">> Nhap STT mon muon CHINH SUA (Nhap 0 de huy): ";
    if (!(cin >> sttChon) || sttChon < 0 || sttChon > (int)gioHang.size())
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "[-] STT khong hop le!\n";
        return;
    }

    if (sttChon == 0)
    {
        cout << "[!] Da huy thao tac sua.\n";
        return;
    }

    int idx = sttChon - 1;
    int viTriKho = timViTriMonTheoID(menu, soLuongMenu, gioHang[idx].id);

    cout << "\nBan dang chon sua mon: [" << gioHang[idx].name << "]\n";
    cout << "1. Thay doi so luong\n";
    cout << "2. Thay doi ghi chu\n";
    cout << ">> Chon thao tac: ";
    int opt;
    if (!(cin >> opt))
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "[-] Lua chon khong hop le!\n";
        return;
    }

    if (opt == 1)
    {
        int slMoi;
        cout << ">> So luong hien tai la: " << gioHang[idx].soLuong << "\n";
        cout << ">> Nhap so luong moi (Nhap 0 se tu dong xoa mon): ";
        if (!(cin >> slMoi) || slMoi < 0)
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "[-] So luong khong hop le!\n";
            return;
        }

        if (slMoi == 0)
        {
            if (viTriKho != -1)
                menu[viTriKho].status += gioHang[idx].soLuong;
            gioHang.erase(gioHang.begin() + idx);
            cout << "=> [Thanh cong] Da dua so luong ve 0, mon da bi xoa khoi gio.\n";
            return;
        }

        // Tính toán chênh lệch kho: delta > 0 tức là mua thêm, delta < 0 là giảm bớt
        int chenhLech = slMoi - gioHang[idx].soLuong;
        if (viTriKho != -1)
        {
            if (chenhLech > 0 && menu[viTriKho].status < chenhLech)
            {
                cout << "[-] Trong kho chi con them " << menu[viTriKho].status << " ly, khong du so luong!\n";
                return;
            }
            menu[viTriKho].status -= chenhLech; // Tự động cộng/trừ kho
        }

        gioHang[idx].soLuong = slMoi;
        gioHang[idx].thanhTien = (long long)slMoi * gioHang[idx].price;
        cout << "=> [Thanh cong] Da cap nhat so luong thanh: " << slMoi << "\n";
    }
    else if (opt == 2)
    {
        cin.ignore(10000, '\n');
        cout << ">> Nhap ghi chu moi: ";
        string noteMoi;
        getline(cin, noteMoi);
        gioHang[idx].note = noteMoi;
        cout << "=> [Thanh cong] Da cap nhat ghi chu moi!\n";
    }
    else
    {
        cout << "[-] Lua chon khong hop le!\n";
    }
}

void quanLyGioHang(vector<MonDaDat> &gioHang, DoUong *menu, const int &soLuongMenu)
{
    while (true)
    {
        // Kiểm tra nếu giỏ hàng trống (ví dụ sau khi người dùng xóa hết món)
        if (gioHang.empty())
        {
            cout << "\n[!] Gio hang hien dang trong!\n";
            return;
        }

        // Chỉ in giỏ hàng và menu khi bắt đầu hoặc sau khi hoàn thành 1 thao tác hợp lệ
        inGioHangHienTai(gioHang);

        cout << "\n=========================================\n";
        cout << "           XAC NHAN & DIEU CHINH         \n";
        cout << "=========================================\n";
        cout << "1. Goi them mon moi (Them mon)           \n";
        cout << "2. Chinh sua mon (Doi so luong / Ghi chu)\n";
        cout << "3. Xoa mon khoi gio                      \n";
        cout << "4. Xac nhan gio hang & Tien hanh hoa don \n";
        cout << "-----------------------------------------\n";

        int choice;
        // Vòng lặp bắt lỗi nhập: chỉ in báo lỗi và dòng nhập lại, không in lại menu
        while (true)
        {
            cout << ">> Lua chon cua ban (1-4): ";
            if (!(cin >> choice))
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[-] Loi: Vui long chi nhap so nguyen!\n";
                continue;
            }

            if (choice < 1 || choice > 4)
            {
                cout << "[-] Lua chon khong hop le! Vui long chon tu 1 den 4.\n";
                continue;
            }

            // Nhập đúng từ 1 đến 4 thì thoát vòng lặp phụ
            break;
        }

        // Xử lý các lựa chọn hợp lệ
        switch (choice)
        {
        case 1:
        {
            inMenu(menu, soLuongMenu);
            vector<MonDaDat> themMoi = nhapChonDoUong(menu, soLuongMenu);
            for (const auto &item : themMoi)
            {
                bool daCo = false;
                for (auto &monCu : gioHang)
                {
                    if (monCu.name == item.name && monCu.note == item.note)
                    {
                        monCu.soLuong += item.soLuong;
                        monCu.thanhTien = (long long)monCu.soLuong * monCu.price;
                        daCo = true;
                        break;
                    }
                }
                if (!daCo)
                {
                    gioHang.push_back(item);
                }
            }
            break;
        }
        case 2:
            chinhSuaMonTrongGio(gioHang, menu, soLuongMenu);
            break;
        case 3:
            xoaMonKhoiGio(gioHang, menu, soLuongMenu);
            break;
        case 4:
            cout << ">> Da xac nhan gio hang. Chuyen sang buoc xu ly hoa don...\n";
            return;
        }
    }
}

vector<MonDaDat> nhapChonDoUong(DoUong *menu, const int &soLuongMenu)
{
    vector<MonDaDat> gioHang;
    int idChon;
    bool continueOrder = true;

    cout << "\n=========================================\n";
    cout << "          BAT DAU ORDER DO UONG          \n";
    cout << "=========================================\n";

    do
    {
        bool nhapThanhCong = false;

        do
        {
            try
            {
                // 1. NHẬP ID MÓN
                cout << "\n>> Nhap ID do uong (Nhap '0' de ket thuc): ";
                if (!(cin >> idChon))
                {
                    throw runtime_error("Ban phai nhap mot so nguyen (VD: 101, 203)!");
                }

                if (idChon == 0)
                {
                    continueOrder = false;
                    break;
                }

                int viTriMon = timViTriMonTheoID(menu, soLuongMenu, idChon);

                if (viTriMon == -1)
                {
                    throw runtime_error("ID khong ton tai trong Menu!");
                }

                // 2. NHẬP SIZE ĐỒ UỐNG
                char sizeChon;
                cout << ">> Nhap Size (S/M/L) cho mon [" << menu[viTriMon].name << "]: ";
                cin >> sizeChon;

                // Đưa về chữ thường để dễ xét điều kiện
                sizeChon = tolower(sizeChon);

                if (sizeChon != 's' && sizeChon != 'm' && sizeChon != 'l')
                {
                    throw runtime_error("Size khong hop le! Chi chap nhan S, M hoac L.");
                }

                // 3. NHẬP SỐ LƯỢNG
                int soLuong;
                cout << ">> Nhap so luong: ";
                if (!(cin >> soLuong))
                {
                    throw runtime_error("So luong phai la mot con so!");
                }

                if (soLuong <= 0)
                {
                    throw runtime_error("So luong phai lon hon 0!");
                }

                if (menu[viTriMon].status < soLuong)
                {
                    throw runtime_error("Trong kho chi con " + to_string(menu[viTriMon].status) + " ly!");
                }

                // 4. NHẬP GHI CHÚ
                cin.ignore(10000, '\n'); // Dọn dẹp bộ đệm
                string ghiChu;
                cout << ">> Nhap ghi chu (VD: it da, nhieu ngot...) hoac nhan Enter de bo qua: ";
                getline(cin, ghiChu);

                // 5. TÍNH TOÁN GIÁ TIỀN DỰA TRÊN SIZE
                int giaTheoSize = menu[viTriMon].price;
                if (sizeChon == 's')
                {
                    giaTheoSize -= 5000;
                }
                else if (sizeChon == 'l')
                {
                    giaTheoSize += 5000;
                }

                // Chuyển size về lại chữ IN HOA để in ra hóa đơn cho đẹp (Size S, Size L)
                char sizeInHoa = toupper(sizeChon);
                string tenMonKemSize = menu[viTriMon].name + " (Size " + string(1, sizeInHoa) + ")";

                // NẾU KHÔNG CÓ LỖI, BẮT ĐẦU CẬP NHẬT DỮ LIỆU
                nhapThanhCong = true;
                menu[viTriMon].status -= soLuong; // Trừ kho 1 lần duy nhất

                // 6. GỘP MÓN VÀO GIỎ HÀNG
                bool daCoTrongGio = false;
                for (auto &mon : gioHang)
                {
                    // So sánh trùng tên (đã kèm size) và trùng cả ghi chú
                    if (mon.name == tenMonKemSize && mon.note == ghiChu)
                    {
                        mon.soLuong += soLuong;
                        mon.thanhTien = (long long)mon.soLuong * mon.price;
                        daCoTrongGio = true;
                        break;
                    }
                }

                if (!daCoTrongGio)
                {
                    gioHang.push_back({idChon, tenMonKemSize, soLuong, giaTheoSize, (long long)soLuong * giaTheoSize, ghiChu});
                }

                cout << "=> [Thanh cong] Da them " << soLuong << " x " << tenMonKemSize << " vao hoa don.\n";
            }
            catch (const exception &e)
            {
                cout << "[LOI] " << e.what() << " Vui long nhap lai.\n";
                cin.clear();
                cin.ignore(10000, '\n');
            }

        } while (!nhapThanhCong && continueOrder);

    } while (continueOrder);

    return gioHang;
}

// 1. Hàm in sơ đồ quán với 3 trạng thái
void inSoDo(Ban **maTran)
{
    cout << "\n============= SO DO QUAN CAFE =============\n\n";
    for (int i = 0; i < 6; i++)
    {
        if (i == 0)
        {
            cout << "                 TANG 1           " << endl;
            cout << "-------------------------------------------" << endl;
        }
        else if (i == 3)
        {
            cout << "===========================================" << endl;
            cout << "                 TANG 2           " << endl;
            cout << "===========================================" << endl;
        }

        // --- Chú thích Khu vực (In ở đầu dòng) ---
        if (i == 0 || i == 3)
        {
            cout << left << setw(15) << "(Ngoai troi)";
        }
        else if (i == 1 || i == 2 || i == 4)
        {
            cout << left << setw(15) << "(Trong nha)";
        }
        else if (i == 5)
        {
            cout << left << setw(15) << "(Phong hop)";
        }

        cout << "\t";
        for (int j = 0; j < 3; j++)
        {
            // Kiểm tra biến status để in màu tương ứng
            if (maTran[i][j].status == 1)
            {
                // Trống -> Xanh
                cout << "[ " << maTran[i][j].tableNum << " ]   ";
            }
            else if (maTran[i][j].status == 0)
            {
                // Có khách -> Đỏ
                cout << "[ X ]   ";
            }
            else if (maTran[i][j].status == -1)
            {
                // Đang dọn -> Vàng
                cout << "[ D ]   ";
            }
        }
        cout << "\n\n";
    }
    cout << "===========================================\n";
    cout << "[ So ]" << ": TRONG  |  "
         << "[ X ]" << ": CO KHACH  |  "
         << "[ D ]" << ": DANG DON\n";
}

// 2. Hàm xử lý chọn bàn (Trả về số bàn hoặc 0 nếu hủy)
int xuLyChonBan(Ban **maTran)
{
    int loaiPhucVu;
    while (true)
    {
        cout << "\n=> Khach hang muon: 1. Dung tai quan  |  2. Mang di: ";
        cin >> loaiPhucVu;

        if (cin.fail())
        {
            cin.clear(); // Xóa cờ báo lỗi của cin
            cin.ignore(1000, '\n');
            cout << "[-] Vui long chi nhap so, khong nhap chu!\n";
            continue;
        }

        // XỬ LÝ KHI NHẬP ĐÚNG SỐ
        if (loaiPhucVu == 2)
        {
            cout << ">> Da chon [MANG DI]. Bo qua chon ban, chuyen den menu goi mon...\n";
            return -1; // Trả về -1 cho hóa đơn mang đi
        }
        else if (loaiPhucVu == 1)
        {
            cout << ">> Da chon [DUNG TAI QUAN]. Tien hanh chon ban...\n";
            break; // Hợp lệ -> Thoát vòng lặp hỏi để đi tiếp xuống phần in sơ đồ bàn
        }
        else
        {
            // BẮT LỖI 2: Người dùng nhập số nhưng không phải 1 hay 2 (VD: nhập 3, 4, 0...)
            cout << "[-] Lua chon khong hop le. Vui long chi nhap 1 hoac 2!\n";
        }
    }

    int chonBan;

    while (true)
    {
        // CHÚ Ý: Chuyển hàm inSoDo vào trong vòng lặp để sơ đồ luôn được làm mới
        inSoDo(maTran);

        while (true)
        {
            cout << "=> Nhap so ban can thao tac (1-18) hoac '0' de huy: ";

            // Thêm mới: Bắt lỗi nếu người dùng vô tình nhập chữ cái
            if (!(cin >> chonBan))
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "[-] Loi: Nhap sai dinh dang! Vui long chi nhap so.\n";
                continue;
            }

            if (chonBan == 0)
            {
                cout << "Da huy thao tac.\n";
                return 0;
            }

            if (chonBan < 1 || chonBan > 18)
            {
                cout << "[-] Loi: So ban khong hop le. Vui long chon lai!\n";
                continue;
            }

            // Thoát vòng lặp nhỏ nếu nhập 0
            break;
        }

        // Tìm bàn và xử lý logic dựa trên trạng thái hiện tại
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (maTran[i][j].tableNum == chonBan)
                {
                    // Trường hợp 1: Bàn trống -> Mở bàn cho khách
                    if (maTran[i][j].status == 1)
                    {
                        maTran[i][j].status = 0; // Chuyển trạng thái sang Có khách
                        cout << ">> Chon ban so " << chonBan << " (" << maTran[i][j].seatNum << " ghe, " << maTran[i][j].pos << ") thanh cong!\n";

                        return chonBan; // Trả về số bàn để làm hóa đơn
                    }

                    // Trường hợp 2: Bàn đang có khách -> Gợi ý dọn bàn
                    else if (maTran[i][j].status == 0)
                    {
                        char xacNhan;
                        cout << "Xin loi, ban so " << chonBan << " dang co khach!\n";
                        cout << ">> Khach da roi di? Ban co muon doi sang trang thai DANG DON? (y/n): ";
                        cin >> xacNhan;

                        if (xacNhan == 'y' || xacNhan == 'Y')
                        {
                            maTran[i][j].status = -1; // Cập nhật sang Đang dọn
                            cout << ">> Da chuyen ban " << chonBan << " sang trang thai DANG DON.\n";
                        }
                    }

                    // Trường hợp 3: Bàn đang dọn -> Gợi ý mở lại bàn
                    else if (maTran[i][j].status == -1)
                    {
                        char xacNhan;
                        cout << "Ban so " << chonBan << " dang duoc don dep.\n";
                        cout << ">> Da don xong? Chuyen ban ve trang thai TRONG? (y/n): ";
                        cin >> xacNhan;

                        if (xacNhan == 'y' || xacNhan == 'Y')
                        {
                            maTran[i][j].status = 1; // Cập nhật sang Trống
                            cout << ">> Ban " << chonBan << " da san sang don khach moi!\n";
                        }
                    }
                }
            }
        }
    }
}

string layThoiGianHienTai()
{
    // 1. Lấy thời gian hiện tại của hệ thống (tính bằng giây từ 1970)
    time_t now = time(0);

    // 2. Chuyển đổi sang cấu trúc thời gian local (múi giờ máy tính của bạn)
    tm *ltm = localtime(&now);

    // 3. Tạo một mảng ký tự để chứa chuỗi thời gian đã format
    char buffer[80];

    // 4. Format thời gian theo chuẩn "Ngày/Tháng/Năm Giờ:Phút:Giây"
    // %d: Ngày, %m: Tháng, %Y: Năm (4 số), %H: Giờ (24h), %M: Phút, %S: Giây
    strftime(buffer, sizeof(buffer), "%H:%M:%S %d/%m/%Y", ltm);

    // Trả về dạng string để dễ lưu vào Struct
    return string(buffer);
}

// Hàm kiểm tra và xử lý thẻ VIP
// Trả về mức giảm giá (0.1 nếu là VIP, 0.0 nếu không phải)
// Thêm dấu & vào biến int để truyền bằng const tham chiếu
double xuLyKhachVIP(const string *danhSachVIP, const int &soLuongVIP)
{
    char laVIP;
    cout << "\n>> Khach hang co the VIP khong? (y/n): ";
    cin >> laVIP;

    if (laVIP == 'y' || laVIP == 'Y')
    {
        string idNhap;

        // Vòng lặp yêu cầu nhập đến khi đúng hoặc chủ động hủy
        while (true)
        {
            cout << ">> Nhap ID Khach VIP (hoac nhap '0' de huy): ";
            cin >> idNhap;

            // Xử lý lối thoát (Tránh kẹt vòng lặp nếu khách quên mã)
            if (idNhap == "0")
            {
                cout << "[!] Da huy nhap ID. Tiep tuc thanh toan vo hoa don thuong.\n";
                return 0.0;
            }

            bool timThay = false;

            // Đối chiếu ID khách nhập với danh sách VIP
            for (int i = 0; i < soLuongVIP; i++)
            {
                if (idNhap == danhSachVIP[i])
                {
                    timThay = true;
                    break;
                }
            }

            // Xử lý kết quả kiểm tra
            if (timThay)
            {
                cout << "[+] Xac nhan thanh cong! Khach hang duoc giam gia 10% (0.1).\n";
                return 0.1;
            }
            else
            {
                // Nhập sai thì báo lỗi, vòng lặp while(true) sẽ tự quay lại bắt nhập tiếp
                cout << "[-] ID khong chinh xac hoac khong ton tai. Vui long thu lai!\n";
            }
        }
    }
    else
    {
        cout << "ban da huy nhap the VIP.\n";
    }

    return 0.0;
}

void luuLichSuGiaoDich(const HoaDon &bill)
{
    ofstream outFile("sales_history.txt", ios::app);
    if (outFile.is_open())
    {
        // Lặp qua danh sách các món khách đã đặt trong hóa đơn
        for (const auto &mon : bill.foodList)
        {
            // Ghi: ThoiGian,ID,SoLuong (Chúng ta bỏ phần chữ ngày tháng đi vì biến time của bill đã có sẵn)
            outFile << bill.time << "," << mon.id << "," << mon.soLuong << "\n";
        }
        outFile.close();
    }
    else
    {
        cout << "[LOI] Khong the ghi vao file sales_history.txt\n";
    }
}

// 4. HÀM LỌC, CỘNG DỒN VÀ IN BÁO CÁO
void xemBaoCaoBestSeller(const string &tieuChi, const DoUong *menuQuan, const int &soLuongMonMenu)
{
    ifstream inFile("sales_history.txt");
    if (!inFile.is_open())
    {
        cout << "\n[-] Chua co du lieu ban hang!\n";
        return;
    }

    vector<ThongKeMon> dsThongKe;
    string line;

    while (getline(inFile, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);
        string ngayBan, idStr, soLuongStr;
        int idMon, soLuong;

        // Tách chuỗi
        getline(ss, ngayBan, ',');
        getline(ss, idStr, ',');
        getline(ss, soLuongStr, ',');

        // Bộ lọc chuỗi (Ngày/Tháng/Năm)
        if (ngayBan.find(tieuChi) != string::npos)
        {

            try
            {
                idMon = stoi(idStr);
                soLuong = stoi(soLuongStr);
            }
            catch (...)
            {
                continue;
            }

            // TRUY XUẤT TÊN TỪ MENU GỐC DỰA VÀO "id"
            string tenMonTraCuu = "[Mon Da Xoa Hoac Khong Ton Tai]";
            for (int i = 0; i < soLuongMonMenu; i++)
            {
                if (menuQuan[i].id == idMon)
                {
                    tenMonTraCuu = menuQuan[i].name; // Truy xuất name từ struct DoUong
                    break;
                }
            }

            // CỘNG DỒN
            bool daCo = false;
            for (int i = 0; i < dsThongKe.size(); i++)
            {
                if (dsThongKe[i].id == idMon)
                {
                    dsThongKe[i].tongSoLuong += soLuong;
                    daCo = true;
                    break;
                }
            }

            if (!daCo)
            {
                dsThongKe.push_back({idMon, tenMonTraCuu, soLuong});
            }
        }
    }
    inFile.close();

    if (dsThongKe.empty())
    {
        cout << "\n[-] Khong co doanh thu trong thoi gian: " << tieuChi << "\n";
        return;
    }

    // SẮP XẾP VÀ IN KẾT QUẢ
    for (int i = 0; i < dsThongKe.size() - 1; i++)
    {
        for (int j = i + 1; j < dsThongKe.size(); j++)
        {
            if (dsThongKe[j].tongSoLuong > dsThongKe[i].tongSoLuong)
            {
                swap(dsThongKe[i], dsThongKe[j]);
            }
        }
    }

    cout << "\n=================================================\n";
    cout << "   BAO CAO BEST SELLER (" << tieuChi << ")\n";
    cout << "=================================================\n";
    cout << left << setw(10) << "HANG" << setw(25) << "TEN MON" << "DA BAN\n";
    cout << "-------------------------------------------------\n";
    for (int i = 0; i < dsThongKe.size() && i < 3; i++)
    {
        cout << left << "TOP " << setw(6) << i + 1
             << setw(25) << dsThongKe[i].name
             << dsThongKe[i].tongSoLuong << " ly\n";
    }
    cout << "=================================================\n";
}