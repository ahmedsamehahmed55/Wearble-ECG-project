% Auther: Ahmed Ahmed
% Date: 8/23/2025   
close all;
clear all;
clc
v_diff =0.3:0.01:2.9;    %% choose the range   
R_top = 2000000/ 2;        %% voltage divider resistance value    

m = v_diff ./ R_top;
R_unknown = (3.3 ./ m) - R_top;

figure;
plot(v_diff, R_unknown / 1000, 'LineWidth', 1.5);
xlabel('Voltage difference on the node (V)');
ylabel('Unknown Resistance (k\Omega)');
title('Unknown Resistance vs Node Voltage');
grid on;
