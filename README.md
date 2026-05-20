# 1. Waveform coding using PCM

```
clc
clear
close all

fm = 5;      % message signal frequency
A = 5;       % amplitude
fs = 50;     % sampling frequency
bits = 4;    % number of bits for quantization

% Continuous time axis for original signal
t = 0:1/(100*fm):1;

% Sampling time axis
ts = 0:1/fs:1;

% Original analog signal
x = A*cos(2*pi*fm*t);

% Sampled signal
xs = A*cos(2*pi*fm*ts);

% -------- Sampling Plot --------
figure(1)
plot(t,x,'linewidth',2)
title('Sampling')
ylabel('Amplitude')
xlabel('Time (sec)')
hold on
stem(ts,xs,'r','linewidth',2)
legend('Original Signal','Sampled Signal')

% -------- Normalization for Quantization --------
x_norm = xs + A;        % shift signal to positive
x_norm = x_norm/(2*A);  % normalize between 0 and 1

% Number of quantization levels
L = (-1 + 2^bits);

% Scale to quantization levels
x_level = L * x_norm;

% Quantized signal
x_quant = round(x_level);

% -------- Encoding --------
bit_stream = [];

for i = 1:length(x_quant)
    
    % Convert level to binary
    b = de2bi(x_quant(i),bits);
    
    % Store bits
    bit_stream = [bit_stream b-48];
    
end

display('Encoded Bit Stream')
display(bit_stream)

% -------- Quantization Plot --------
figure(2)
stem(ts,x_level,'linewidth',2)
title('Quantization')
ylabel('Levels')
hold on
stairs(ts,x_quant,'r','linewidth',2)
legend('Sampled Signal','Quantized Signal')

% -------- Encoding Plot --------
figure(3)
stairs([bit_stream bit_stream(length(bit_stream))],'linewidth',2)
title('Encoding')
ylabel('Binary Signal')
xlabel('Bits')
axis([0 length(bit_stream)-1 0 2])
grid on
```

# 2. Matched filter

```
clc
clear
close all

% Oversampling factor
L = 8;

% Roll-off factor
alpha = 0.4;

% Generate one input bit
bits = [1];

% Raised cosine filter
h = rcosfir(alpha, [-3 3], 10, 1, 'normal');

% Upsample the bit sequence
bits_up = upsample(bits, L);

% Pulse shaping at transmitter
tx_signal = conv(bits_up, h);

% Plot transmitter filter output
figure(1)
stem(tx_signal)
title('Transmitter Filter Output')
xlabel('Sample')
ylabel('Amplitude')

% Pass signal through AWGN channel
rx_signal = awgn(tx_signal, 30);

% Matched filtering at receiver
mf_output = conv(rx_signal, h);

% Plot matched filter output
figure(2)
stem(mf_output)
title('Matched Filter Output at Receiver')
xlabel('Sample')
ylabel('Amplitude')

% Calculate middle sample position
mid = length(-4:1/L:4);

% Remove filter delay
y_trunc = mf_output(mid-1:end);

% Downsample to symbol rate
y_down = downsample(y_trunc, L, 1);

% Plot downsampled signal
figure(3)
stem(y_down)
title('Downsampled Output (Decision Samples)')
xlabel('Sample')
ylabel('Amplitude')
```

# 3. Eye diagram

```
clc
clear
close all

fs = 20;
fd = 1;
pd = 500;

x = randi([0 1], pd, 1);

figure(1)
h1 = rcosfir(0.2, [-3 3], 10, 1, 'normal');
stem(h1)

figure(2)
h2 = rcosfir(0.8, [-3 3], 10, 1, 'normal');
stem(h2)

xp = upsample(x,10);

rev1 = conv(xp, h1, 'valid');
rev2 = conv(xp, h2, 'valid');

n = fs/fd/2;

figure
eyediagram(rev1, n)

figure
eyediagram(rev2, n)
```

# 4. BPSK generation

```
clc
clear
close all

N = 10;              % number of bits
Tb = 1;              % bit duration
fc = 2;              % carrier frequency
fs = 100;            % sampling frequency

% Generate random binary data
data = randi([0 1],1,N);

% NRZ encoding (0 -> -1 , 1 -> +1)
nrz = 2*data - 1;

% Time vector
t = 0:1/fs:N*Tb-1/fs;

% Expand NRZ signal
m = repelem(nrz, fs*Tb);

% Carrier signal
carrier = sqrt(2/Tb) * sin(2*pi*fc*t);

% BPSK signal
bpsk = m .* carrier;

% Plot signals
figure

subplot(4,1,1)
stem(data)
title('Binary Data')

subplot(4,1,2)
plot(m)
title('NRZ Message Signal')

subplot(4,1,3)
plot(carrier)
title('Carrier Signal')

subplot(4,1,4)
plot(bpsk)
title('BPSK Signal')

```

What is Replem?

# 5. Error performance of QPSK

```
l=1e6;
EbNodb= 1:2:10;
EbNo= 10.^(EbNodb/10);
N=10;
j=1i;

for n=1:length(EbNodb)
	data_inphase = randi([0 1], 1, 10);
	data_quadrature = randi([0 1], 1, 10);
	
	si= 2*data_inphase -1;
	sq= 2*data_quadrature -1;
	
	s = si + j*sq;
	
	SNR = 10;
	w = 1*awgn(randn(1,N), SNR)
	
	r = s+w;
	
	si_ = sign(real(r));
	sq_ = sign(img(r));
	
	ber1(n)= (1-sum(si==si_)/1);
	ber2(n)= (1-sum(sq==sq_)/1);
end

ber_theo = (1/2) * erfc(sqrt(EbNo))

semilogy(EbNodb, ber_theo, 'r*-');

title("Ber performance of QPSK modulation");
xlabel("EbNoDb")
ylabel("Ber")
grid on
```

# Error performance of BPSK

```
clc;  
clear all;  
close all;  
numBits = 10^5;  
bits = randi([0, 1], numBits, 1);  
bpsk_signal = 2 * bits - 1;  
EbNodB = 0:10;  
EbNo = 10.^(EbNodB/10);  
errors = zeros(size(EbNo));  
  
for i = 1:length(EbNo);  
No = 1 / EbNo(i);  
noise = sqrt(No/2) * randn(size(bpsk_signal));  
received_signal = bpsk_signal + noise;  
received_bits = received_signal > 0;  
errors(i) = sum(bits ~= received_bits);  
end  
  
Ber = errors / numBits;  
theoreticalBer = 0.5 * erfc(sqrt(EbNo));  
semilogy (EbNodB, Ber, 'bo-', 'LineWidth', 1.5);  
hold on;  
semilogy (EbNodB, theoreticalBer, '-r', 'linewidth', 1.5);  
hold off;  
grid on;  
legend ('simulation', 'theoretical');



```
clc;  
clear all;  
close all;  

% CHANGED: Increased from 10^5 to 10^6. At 9-10 dB, errors are so rare that 
% 10^5 bits often yield exactly 0 errors, causing log-plot warnings.
numBits = 10^6;  

bits = randi([0, 1], numBits, 1);  
bpsk_signal = 2 * bits - 1;  
EbNodB = 0:10;  
EbNo = 10.^(EbNodB/10);  
errors = zeros(size(EbNo));  
  
for i = 1:length(EbNo)  % CHANGED: Swapped the typo semicolon (;) with a regular loop structure
    No = 1 / EbNo(i);  
    noise = sqrt(No/2) * randn(size(bpsk_signal));  
    received_signal = bpsk_signal + noise;  
    received_bits = received_signal > 0;  
    errors(i) = sum(bits ~= received_bits);  
end  
  
Ber = errors / numBits;  

% CHANGED: Replaced any absolute 0 values with NaN. 
% semilogy cannot calculate log10(0). Converting 0 to NaN makes the function 
% skip the point cleanly instead of throwing an "omitting non-positive data" warning.
Ber(Ber == 0) = NaN;  

theoreticalBer = 0.5 * erfc(sqrt(EbNo));  

figure(1); % CHANGED: Explicitly declared figure handle for cleaner rendering
semilogy (EbNodB, Ber, 'bo-', 'LineWidth', 1.5);  
hold on;  
semilogy (EbNodB, theoreticalBer, '-r', 'linewidth', 1.5);  
hold off;  

% CHANGED: Cleaned up duplicate 'grid on' commands and added explicit Y-limits 
% to keep the graph looking uniform down to the lowest simulated values.
ylim([10^-6 1]); 
grid on;  
legend ('Simulation', 'Theoretical');  
xlabel ('Eb/No (dB)');  
ylabel ('Bit Error Rate (BER)');
```
xlabel ('EbNO(dB)');  
ylabel ('BER');  
grid on;
```
