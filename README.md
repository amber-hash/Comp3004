README: HintonMarket Management System
Project Overview
Course: COMP 3004 – Winter 2026

Project: Deliverable 1 - HintonMarket Vertical Prototype
Team: #60
Date: March 12, 2026

1. Team Members
Harleen Kaur: System Decomposition & Design Documentation
Olivia Petherick: Frontend UI Development & Integration
Amber Hashwani: Backend Logic & Controller Development, Testing & Debugging

2. Project Structure
The source code is organized into four subsystems to ensure high cohesion and low coupling:

src/controller/: Centralized logic (e.g., MarketManager).
src/data/: In-memory collections and data seeding (DataStore).
src/models/: Domain entities (Users, Vendors, Bookings, etc.).
src/ui/: Qt-based boundary classes and dashboards.

3. Build & Execution
   
Designed for the Official Course VM (Ubuntu Linux):
Extract team_60_D1.zip.
Open HintonMarket.pro in Qt Creator.
Configure with the Default Desktop Kit.
Build and Run (Ctrl + R).
Note: Maintain the directory structure to ensure relative path integrity.

4. Test Accounts (Case-Sensitive)
   
Food Vendors usernames: freshharvest, sunrisebakery, greenvalley, maplesyrup (non-compliant, missing Food Handler Cert)
Artiasn Vendors usernames: claycreations, woodcraft, silkthread, jewelrybox
Market Staff usernames: operator, admin

6. Functional Scope (D1)
   
Identification: Vendor verification via DataStore.
Market Browser: 4-week rolling view of stall availability.
Booking/Cancellation: Real-time updates to schedules and dashboards.
Waitlist Logic: FIFO queue management with availability notifications.
Dashboard: Displays business info, compliance status, and active bookings.

6. Documentation
   
System Decomposition: Located in the root directory as team_60_D1.pdf.
Design Rationale: Detailed architectural breakdown (UI, Application, Entity, Data) is included in the PDF.




