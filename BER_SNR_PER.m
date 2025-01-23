clc; clear; close all;

%% PARAMETERS
SNR_dB = linspace(0, 20, 100); % SNR (0 ~ 20 dB)
SNR_linear = 10.^(SNR_dB / 10); % SNR in linear
P_transmit = 1; % Transmit power (W)
P1 = 0.5; % Probability of 1 from OOK modulation
P_avg = P_transmit * P1; % average transmit power

% Path Loss
d = 0.005; % distance (5 mm: blood vessel + skin thickness)
f = 1e12; % frequency (1 THz)
c = 3e8; % lightspeed (m/s)
L_path = (4 * pi * d * f / c)^2; % Path Loss

% Noise Power
kB = 1.38e-23; % (J/K)
T = 310; % (K)
B = 10e6; % (Hz)
P_noise = kB * T * B; % Noise power
P_noise_observed = P_noise * L_path; % Noise power with path loss

%% THEORETICAL BER CALCULATION
% FEC 미적용 BER 계산 (OOK)
BER_no_FEC = 0.5 * exp(-SNR_linear / 2);

% FEC 적용 후 BER 계산 (Hamming (12,8))
P_error_bit = BER_no_FEC; % 비트 오류 확률
P_error_Hamming = 3 * P_error_bit.^2 - 2 * P_error_bit.^3; % Hamming (12,8) 후 BER

%% PLOT BER vs SNR
figure;

% FEC 미적용 그래프
semilogy(SNR_dB, BER_no_FEC, 'b-', 'LineWidth', 1.5);
hold on;

% FEC 적용 그래프
semilogy(SNR_dB, P_error_Hamming, 'r--', 'LineWidth', 1.5);

% 그래프 설정
grid on;
xlabel('SNR (dB)', 'FontSize', 12);
ylabel('BER', 'FontSize', 12);
title('BER vs SNR', 'FontSize', 14);
legend('Without FEC', 'With FEC', 'FontSize', 12);
