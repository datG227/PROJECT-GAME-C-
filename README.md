# 🎮 Sokoban – C++ / raylib

Trò chơi **Sokoban**(Xếp thùng) cổ điển được xây dựng bằng **C++** và thư viện đồ họa **raylib**, chạy trên Windows 64-bit.

---

## ⚙️ Ký hiệu bản đồ (map format)

| Ký tự | Ý nghĩa |
|-------|---------|
| `#` | Tường (wall) |
| `R` | Đá (rock – không thể đẩy) |
| `@` | Vị trí ban đầu của người chơi |
| `$` | Hộp (box) |
| `.` | Đích (target) |
| `*` | Hộp đang ở trên đích |
| `+` | Người chơi đang ở trên đích |

---

## 📋 Mô tả

---

## 🕹️ Cách chơi

| Phím | Hành động |
|------|-----------|
| `↑` / `W` | Di chuyển lên |
| `↓` / `S` | Di chuyển xuống |
| `←` / `A` | Di chuyển sang trái |
| `→` / `D` | Di chuyển sang phải |
| `R` | Khởi động lại màn hiện tại |
| `N` / `Enter` | Qua màn tiếp theo (khi thắng) |
| `Escape` | Thoát game (ở màn cuối) |

**Mục tiêu:** Đẩy tất cả hộp vào các ô đích để thắng màn.  
Chú ý: hộp chỉ đẩy được về phía trước, không kéo lại được!

---

## 📦 Cấu trúc dự án

```
DOAN C++/
├── main.cpp          # Vòng lặp game chính, khởi tạo cửa sổ
├── game.cpp / .h     # Logic game: di chuyển, va chạm, kiểm tra thắng
├── levels.cpp / .h   # Dữ liệu 6 màn chơi và hàm LoadLevel
├── render.cpp / .h   # Vẽ board, HUD, màn hình thắng
├── textures.cpp / .h # Load/Unload sprite từ file ảnh
├── types.h           # Cấu trúc dữ liệu: GameState, Textures, hằng số
├── tiles/            # Sprite ảnh (PNG)
│   ├── player.png
│   ├── box.png
│   ├── box_on_target.png
│   ├── target.png
│   ├── wall.png
│   ├── rock.png
│   ├── ground1.png
│   ├── ground2.png
│   ├── flower1.png
│   └── flower2.png
├── app.ico           # Icon ứng dụng
├── app.rc            # Resource file (icon)
└── DOAN C++.vcxproj  # File dự án Visual Studio
```

---

## 🗺️ Các màn chơi

| # | Tên | Kích thước | Số hộp |
|---|-----|-----------|--------|
| 1 | First Steps | 20 × 9 | 2 |
| 2 | Side Step | 22 × 10 | 3 |
| 3 | The Island | 24 × 10 | 3 |
| 4 | Zigzag | 28 × 11 | 4 |
| 5 | Fortress | 30 × 12 | 5 |
| 6 | The Labyrinth | 34 × 13 | 6 |

## 🔧 Yêu cầu & Build

### Yêu cầu
- **Visual Studio 2022** (hoặc mới hơn) với workload *Desktop development with C++*
- **raylib** (đã được cấu hình trong `.vcxproj`)
- Windows 10/11 x64

### Build
1. Mở file `DOAN C++.sln` bằng Visual Studio.
2. Chọn cấu hình **Debug** hoặc **Release**, nền tảng **x64**.
3. Nhấn `Ctrl + Shift + B` để build.
4. File thực thi xuất hiện tại `x64/Debug/DOAN C++.exe`.

> **Lưu ý:** Thư mục `tiles/` phải nằm cùng cấp với file `.exe` khi chạy để game load được sprite.

---

## 🛠️ Chi tiết kỹ thuật

- **Ngôn ngữ:** C++17
- **Thư viện đồ họa:** [raylib](https://www.raylib.com/)
- **Độ phân giải:** 1280 × 800
- **Kích thước tile:** 56 × 56 px (tự động scale nhỏ hơn nếu màn quá lớn)
- **FPS:** 60 (VSync bật)
- **Hoạt ảnh di chuyển:** 0.14 giây mỗi bước (smooth interpolation)
- **Build target:** Windows x64

---

## 📐 Kiến trúc code

```
main.cpp
  └── Vòng lặp chính (raylib window loop)
       ├── Input → StartMove()       [game.cpp]
       ├── Update → UpdateMove()     [game.cpp]
       └── Draw
            ├── DrawFrame()          [render.cpp]  – vẽ tiles & sprite
            ├── DrawHUD()            [render.cpp]  – hiển thị số bước & tên màn
            └── DrawWinOverlay()     [render.cpp]  – thông báo thắng
```

`GameState` (trong `types.h`) lưu toàn bộ trạng thái game:  
- Ba lớp bản đồ riêng biệt: `terrain`, `targets`, `boxes` (dạng vector of string)  
- Trạng thái hoạt ảnh: `moving`, `moveT`, `fromX/Y`, `toX/Y`  
- Thông tin người chơi: `playerX/Y`, `facing`, `moves`, `won`

---

## 👤 Tác giả

Đồ án môn học – C++ 
Năm học: 2025 – 2026
