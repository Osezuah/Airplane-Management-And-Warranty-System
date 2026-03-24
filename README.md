# Airplane-Management-And-Warranty-System

---

# 🧾 Team Setup – Airplane Management Server

## **Step 1: Install vcpkg**

Open **Command Prompt** or **PowerShell** and run:

Clone the repo: git clone https://github.com/microsoft/vcpkg.git

Run the bootstrap script: .\vcpkg\bootstrap-vcpkg.bat

Crucial Step: Run .\vcpkg\vcpkg integrate install from an Administrator terminal.

✅ This installs vcpkg and tells Visual Studio to automatically detect vcpkg libraries.

---

## **Step 2: Clone the project repository**

Clone this repository to your local machine.

Open the .sln (Solution) file in Visual Studio 2022.

---

## **Step 3: Automatic Dependency Install (Manifest Mode)**

This project uses vcpkg Manifest Mode. You do not need to manually install any libraries.

When you open the project, Visual Studio will detect the vcpkg.json file.

---

## **Step 4: Troubleshooting**
Missing Headers: If Visual Studio shows red squiggles initially, go to Server > Project Properties > Configuration Properties > vcpkg and ensure Use Vcpkg Manifest is set to Yes.

---

## **Step 5: Build the server project**

1. In Visual Studio, right-click the **Server Project → Build**

---
