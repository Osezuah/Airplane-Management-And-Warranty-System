INSERT INTO Technician (Name) 
Values ('Client');

INSERT INTO Airplane (Name, Manufacturer, Engines, RangeKm, CargoCapacityKg, FuelCapacityL, AirplaneLengthM, WingSpanM)
Values 
('777-200LR', 'Boeing', 'GE90-115B', 17446, 15300, 202287, 64, 65),
('777-300ER', 'Boeing', 'GE90-115B', 14594, 23024, 181280, 74, 65),
('A330-300', 'Airbus', 'Rolls Royce Trent 772B', 10500, 13900, 97530, 64,60),
('CRJ900', 'Mitsubishi', '2 GE CF34-8C5 Turbofans', 2778, 2134, 10977, 36, 25),
('Dash 8-400', 'De Havilland', '2 Pratt and Whitney PW150A', 1878, 2136, 6526, 33, 28);

INSERT INTO Warranty (AirplaneID_FK, Status)
Values 
(10, 'valid'),
(11, 'valid'),
(12, 'valid'),
(13, 'valid'),
(14, 'valid');
