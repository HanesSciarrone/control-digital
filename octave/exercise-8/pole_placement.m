pkg load signal
pkg load control

clc
clear all

R1 = 2e3
C1 = 1e-6
R2 = 1e3
C2 = 1e-6
h = 0.001

A = [ (-1/C1)*(1/R1 + 1/R2) 1/(R2*C1);1/(C2*R2) -1/(R2*C2)]
B = [1/(R1*C1) ; 0]
C = [0 1]
D = 0

%Obtener la función transferencia en tiempo continuo a partir del modelo de estados
[num,den] = ss2tf(A,B,C,D);
Hs = tf(num,den)

% Obtener la función transfenrecia en tiempo discreto
Hz = c2d(Hs, h, 'zoh')
[numz, denz] = tfdata(Hz, 'v')

Phi = expm(A*h)
Gamma = (expm(A*h) - expm(A*0))/A*B
C_z = C
D_z = D

%%
% 2) Obtener la respuesta al escalón en tiempo continuo y discreto y el diagrama de polos y ceros
%%

figure;
step(Hs, Hz)

poles_LA =pole(Hz)

zplane(numz, denz)

% Calculo del K necesario para mejorar la respuesta del sistema sin el seguimiento de la referencia.

poles = [0.5; 0.01]
K = place(Phi, Gamma, poles)

Phi_LC = Phi - Gamma*K
K0 = (C * (eye(2) - Phi_LC)^(-1) * Gamma)^(-1)
Gamma_LC = Gamma * K0

t = h:h:40;
r = [ zeros(length(t)/2, 1); ones(length(t)/2, 1) ];

y_open = filter(numz, denz, r);
[numz_pole_placement, denz_pole_placement] = ss2tf(Phi_LC, Gamma_LC, C_z, D_z);
y_pole_placement = filter(numz_pole_placement, denz_pole_placement, r);

figure;
hold on;
stairs(t, r)
stairs(t, y_open, "LineWidth", 3)
stairs(t, y_pole_placement, "LineWidth", 3)
legend("Entrada", "Respuesta natural", "Salida controlada")
