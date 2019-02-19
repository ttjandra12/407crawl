%This MATLAB code calculates the amount of torque needed by the servo motor
%driven lead screw to lift a mass of weight M. 

%References:
%   https://www.engineeringtoolbox.com/friction-coefficients-d_778.htmlhttps://www.amesweb.info/Screws/Tables/FrictionCoefficientsPowerLeadScrews.aspx
%   http://www.ce.siue.edu/examples/Worked_examples_Internet_text-only/Data_files-Worked_Exs-Word_&_pdf/Friction_screws.pdf

%Component Specifications:
%   - Carbon Steel ACME Lead Screw, Right Hand, 1/4"-16 Thread Size
%   - Acetal 1/4"-16 Thread Flange Nut

%variables
W = 1; %mass to lift, kg
r = 0.00555752; %outer radius of lead screw, m
L = 0.16193; %lead of screw, m
ro = 0.14; %friction coefficient of lead screw for carbon steel 
alpha = 30/180*pi;

theta = atan(L/(2*pi*r));
phi = atan(ro); %angle of friction

M = (W/(2*sin(alpha)))*r*tan(theta-phi); %torque needed, kg-m
torque_needed = M*10 %torque needed converted to kg-cm

