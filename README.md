# Airplane-Management-And-Warranty-System

---

# 🧾 Development Setup & Package Installation

## **Step 1: Clone Repo**

**WARNING: Make sure that the file path where the repo was cloned to does not contain any "&" or other special characters.**

Clone the repo using the folllowing address: ``https://github.com/Osezuah/Airplane-Management-And-Warranty-System.git``
Which is also found when clicking the green "<> Code" button above.

*git bash commands:*
```bash
git clone "https://github.com/Osezuah/Airplane-Management-And-Warranty-System.git" "<Folder Path>"
```

## **Step 2: VCPKG**

Open the Solution in Visual Studio. Then, Open the Terminal under View > Terminal or pres CTRL+\`
Run the following command:
```bash
vcpkg integrate install
```

## **Step 3: Ensure Manifest Usage is Enabled**

Now, ensure that the projects use vcpkg manifest by right clicking on both the Client and Server Projects > Properties > Configuration Properties > vcpkg. Make sure that "Use vcpkg Manifest" is set to Yes (as should be the default).

## **Step 4: Database**

Make sure to have Postgresql version 17 and Docker installed.
*Docker should be runningbefore the next step, and while using the program.*

Run the following command on your Developer Command Prompt in Visual Studio or your terminal to build the database:
```bash
docker-compose up -d
```

## **Step 4: Build**

Build the Solution. This should take a long time as the required dependencies are downloaded through vcpkg.