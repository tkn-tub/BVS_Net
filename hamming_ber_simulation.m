function hamming_ber_simulation()
    % Parameters for Hamming (12,8)
    n = 12;  % Codeword length
    k = 8;   % Information bit length
    packet_size = 32; % Number of bits per packet
    num_packets = 200; % Number of packets to simulate
    EbNo_dB = 0:0.1:10;  % Range of Eb/No in dB
    results = zeros(length(EbNo_dB), 5); % To store results: [Eb/No, p_b, BER_receiver, PER_no_FEC, PER_with_FEC]
    Rb = 10e6; % Bit Rate in bps (same as before)
    
    % Convert Eb/No to SNR
    SNR_dB = EbNo_dB + 10 * log10(Rb); % Convert Eb/No to SNR

    % Loop over Eb/No values
    for i = 1:length(EbNo_dB)
        % Calculate BER (p_b) for AWGN channel
        p_b = calculate_bipolar_ber(EbNo_dB(i));
        
        % Calculate uncorrected error probability
        P_uncorrected = calculate_uncorrected_error_probability(n, p_b);
        
        % Calculate receiver BER
        BER_receiver = (P_uncorrected * n) / k;
        
        % Calculate Packet Error Rate (PER) without FEC
        PER_no_FEC = 1 - (1 - p_b)^packet_size;
        
        % Calculate PER with Hamming FEC
        PER_with_FEC = 1 - (1 - BER_receiver)^packet_size;

        % Store results
        results(i, :) = [EbNo_dB(i), p_b, BER_receiver, PER_no_FEC, PER_with_FEC];
    end

    % Display results
    fprintf('SNR (dB)\tChannel BER (p_b)\tReceiver BER\tPER (no FEC)\tPER (with FEC)\n');
    for i = 1:length(SNR_dB)
        fprintf('%.4f\t\t%.6e\t\t%.6e\t\t%.6e\t\t%.6e\n', SNR_dB(i), results(i, 2), results(i, 3), results(i, 4), results(i, 5));
    end

    % Plot results BER
    figure(1);
    semilogy(SNR_dB, results(:, 2), 'LineWidth', 1.5, 'DisplayName', 'BER vs SNR without FEC module');
    hold on;
    semilogy(SNR_dB, results(:, 3), 'LineWidth', 1.5, 'DisplayName', 'BER vs SNR with FEC module');
    grid on;
    %xlim([38 48]);
    xlabel('SNR (dB)');
    ylabel('BER');
    legend('Location', 'Southwest');
    title('Hamming (12,8) BER Performance');

    % Plot results PER
    figure(2);
    semilogy(SNR_dB, results(:, 4), 'LineWidth', 1.5, 'DisplayName', 'PER vs SNR without FEC module');
    hold on;
    semilogy(SNR_dB, results(:, 5), 'LineWidth', 1.5, 'DisplayName', 'PER vs SNR with FEC module');
    grid on;
    %xlim([38 48]);
    xlabel('SNR (dB)');
    ylabel('PER');
    legend('Location', 'Southwest');
    title('Hamming (12,8) PER Performance');
end

% Function to calculate BER for bipolar OOK modulation
function p_b = calculate_bipolar_ber(EbNo_dB)
    EbNo_linear = 10^(EbNo_dB / 10);  
    p_b = qfunc(sqrt(2 * EbNo_linear));  % Bipolar OOK uses Q-function
end

% Function to calculate uncorrected error probability for Hamming (12,8)
function P_uncorrected = calculate_uncorrected_error_probability(n, p_b)
    P_uncorrected = 0;
    for j = 2:n  % Start from 2 errors since 0 and 1 errors are correctable
        P_uncorrected = P_uncorrected + nchoosek(n, j) * (p_b^j) * ((1 - p_b)^(n - j));
    end
end
