# Airplane-Management-And-Warranty-System

# 🧾 Team Setup – Airplane Management Server

## **Step 1: Install vcpkg (one-time per machine)**

Open **Command Prompt** or **PowerShell** and run:

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install

✅ This installs vcpkg and tells Visual Studio to automatically detect vcpkg libraries.

---

## **Step 2: Install required libraries via vcpkg**

Run the following commands **in the same terminal**:

vcpkg install libpq:x64-windows@17.4
vcpkg install crow:x64-windows
vcpkg install asio:x64-windows

* `libpq` → PostgreSQL client library v17.4
* `crow` → Crow C++ web framework
* `asio` → Asio networking library

These libraries will now be available to Visual Studio.

---

## **Step 3: Clone the project repository**

Navigate to the folder where you want the project and run:

git clone <your-repo-url>
cd <your-repo-folder>

---

## **Step 4: Open the solution**

1. Open `YourSolution.sln` in Visual Studio.
2. Ensure the configuration is **Debug** and platform is **x64**.

> Your solution is already configured to find Crow, Asio, and PostgreSQL headers/libs from vcpkg.

---

## **Step 5: Post-build DLL copy for server project**

The server project requires runtime DLLs (`libpq.dll`, etc.). This is handled automatically with a post-build step:

1. Right-click **Server Project → Properties → Build Events → Post-Build Event**
2. Ensure the following command exists:

xcopy /Y /D "%VCPKG_ROOT%\installed\x64-windows\bin\*.dll" "$(OutDir)"

✅ This copies all required DLLs into the server’s `x64/Debug` folder after building.

---

## **Step 6: Build the server project**

1. In Visual Studio, right-click the **Server Project → Build**
2. The executable will appear in `ServerProject\x64\Debug`

> Only the **server project** requires these dependencies. The client project does not.

---

## **Step 7: Run the server**

Navigate to `ServerProject\x64\Debug` and run the executable.

* The server will connect to PostgreSQL via the vcpkg-provided `libpq`
* Crow and Asio headers/libs are automatically resolved
* All DLLs required for runtime are in place

