clear all
close all

% packagelength = 200;


opts = detectImportOptions("gateway.csv");
opts.Delimiter = {',',';'};
data_single = readtable("gateway.csv",opts);

data = table2array(data_single(1,1:end));

waveform_amplitude = mean(abs(data));

Const_Tx_real = data;
Const_Tx_imag = zeros(length(data),1);

fontsize = 20;

%plotting constellation points
figure;
plot(waveform_amplitude*cos(0:0.01:2*pi),waveform_amplitude*sin(0:0.01:2*pi),'--','Color','black'); hold on;
plot(Const_Tx_real,Const_Tx_imag,'o','Color','#0072BD','LineWidth',2); grid on;
title('Received amplitude for a tissue thickness of 1.4 mm','Interpreter','latex');
set(gca, 'XTick', [-waveform_amplitude 0 waveform_amplitude]);
set(gca, 'XTickLabel', {'$-A_{Rx}$','$0$','$A_{Rx}$'}, 'TickLabelInterpreter', 'latex');
set(gca, 'YTick', [-waveform_amplitude 0 waveform_amplitude]);
set(gca, 'YTickLabel', {'$-A_{Rx}$','$0$','$A_{Rx}$'}, 'TickLabelInterpreter', 'latex');
xlabel('In Phase','Interpreter','latex');
ylabel('Quadrature','Interpreter','latex');
axis([-1.1*max(Const_Tx_real) 1.1*max(Const_Tx_real) -1.1*max(Const_Tx_real) 1.1*max(Const_Tx_real)]);
axis square
set(gca,'FontSize',fontsize);

% figure;
% plot(abs(Const_Rx_real(1))*cos(0:0.01:2*pi),abs(Const_Rx_real(1))*sin(0:0.01:2*pi),'--','Color','black'); hold on;
% plot(Const_Rx_real(1:bits_end),Const_Rx_imag(1:bits_end),'o','Color','#0072BD','LineWidth',2); grid on;
% title('Reception','Interpreter','latex');
% set(gca, 'XTick', [-abs(Const_Rx_real(1)) 0 abs(Const_Rx_real(1))]);
% set(gca, 'XTickLabel', {'$-A_{Rx}$','$0$','$A_{Rx}$'}, 'TickLabelInterpreter', 'latex');
% set(gca, 'YTick', [-abs(Const_Rx_real(1)) 0 abs(Const_Rx_real(1))]);
% set(gca, 'YTickLabel', {'$-A_{Rx}$','$0$','$A_{Rx}$'}, 'TickLabelInterpreter', 'latex');
% xlabel('In Phase','Interpreter','latex');
% ylabel('Quadrature','Interpreter','latex');
% axis([-1.1*max(abs(Const_Rx_real(1:bits_end))) 1.1*max(abs(Const_Rx_real(1:bits_end))) -1.1*max(abs(Const_Rx_real(1:bits_end))) 1.1*max(abs(Const_Rx_real(1:bits_end)))]);
% axis square
% set(gca,'FontSize',fontsize);