# Donutgram 2000 🍩🌀

**Donutgram 2000** is a beautifully spinning ASCII-rendered torus built with modern C++ for the Windows console. It's interactive, educational, and cleanly engineered — perfect for anyone curious about 3D math, projection, and animation in pure C++.

---

## 🎬 Preview

```text
==========================================
         WELCOME TO DONUTGRAM 2000
     ASCII Torus Renderer & Spinner XT
==========================================

         Press any key to begin...
(Then the donut spins in glorious ASCII.)

🎮 Controls
Key	Action
w	Tilt donut upward
s	Tilt donut downward
a	Spin donut left faster
d	Spin donut right faster
q	Quit the program

🛠️ How to Build
📦 Requirements
Windows

C++17 compiler (MinGW, MSVC, or Clang on Windows)

VS Code or any text editor

🔧 Compile using g++ (MinGW)
bash
Copy
Edit
g++ -std=c++17 -Wall -o donutgram2000.exe donutgram2000.cpp
▶️ Run
bash
Copy
Edit
./donutgram2000.exe
💡 How It Works
Donutgram 2000 uses:

3D parametric equations to define the shape of a torus

Rotation math for two axes using sinf and cosf

Perspective projection via 1/z depth mapping

Lighting via dot product between surface normals and a light vector

Real-time rendering using Windows Console APIs

Each frame calculates depth and brightness per pixel, then prints to the screen.

📂 Files
donutgram2000.cpp — main source file with fully commented code

LICENSE — MIT license (free to use)

README.md — you’re reading it!

🔓 License
This project is licensed under the MIT License.
You are free to use, modify, and share it — just give credit.

Made with π, sine waves, and an unreasonable love of ASCII art.
By Bearmeadow92 🧑‍💻
