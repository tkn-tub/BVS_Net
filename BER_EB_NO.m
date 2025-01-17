%function ASK_FSK_PSK(msglen)
n=20000;
b=randi(1,n);
f1=1;
f2=2;
t=0:1/30:1-1/30;
%ASK
sa1=sin(2*pi*f1*t);
E1=sum(sa1.^2);
sa1=sa1/sqrt(E1); %unit energy 
sa0=0*sin(2*pi*f1*t);
%MODULATION
ask=[];
for i=1:n
    if b(i)==1
        ask=[ask sa1];
    else
        ask=[ask sa0];
    end
end

%figure(1)
%subplot(411)
%stairs(0:10,[b(1:10) b(10)],'linewidth',1.5)
%axis([0 10 -0.5 1.5])
%title('Message Bits');grid on
%subplot(412)
%tb=0:1/30:10-1/30;
%plot(tb, ask(1:10*30),'b','linewidth',1.5)
%title('ASK Modulation');grid on
%xlabel('Time');ylabel('Amplitude')

%AWGN
for snr=0:20
    askn=awgn(ask,snr);
    %DETECTION
    A=[];
    for i=1:n
        %ASK Detection
        if sum(sa1.*askn(1+30*(i-1):30*i))>0.5
            A=[A 1];
        else
            A=[A 0];
        end
    end
    %BER
    errA=0;
    for i=1:n
        if A(i)==b(i)
            errA=errA;
        else
            errA=errA+1;
        end
        end
    BER_A(snr+1)=errA/n;
end

%figure(2)
%subplot(411)
%stairs(0:10,[b(1:10) b(10)],'linewidth',1.5)
%axis([0 10 -0.5 1.5]);grid on
%title('Received signal after AWGN Channel')
%subplot(412)
%tb=0:1/30:10-1/30;
%plot(tb, askn(1:10*30),'b','linewidth',1.5)
%title('Received ASK signal');grid on

%figure(3)
%semilogy(0:20,BER_A, 'b','linewidth',2)
%ylim([10^(-4) 1])
%title('BER vs SNR')
%grid on;
%xlabel('Eo/No(dB)')
%ylabel('BER')
%legend('ASK');

EbN0_dB = 0:0.5:20;
BER = 0.5 * erfc(sqrt(10.^(EbN0_dB/10)));
figure(4)
semilogy(EbN0_dB, BER, 'b')
xlabel('Eb/N0 (dB)')
ylabel('Bit Error Rate (BER)')
%xlim([0 20])
%ylim([10^(-4) 1])
title('BER vs SNR')
grid on;
