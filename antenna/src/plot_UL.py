import numpy as np  # use fft
import sys
import matplotlib.pyplot as plt
import argparse  # support parse command

def read_data_noise(file_path_noise):
    with open(file_path_noise, "r") as file:
        lines = file.read().split()
    noise_data_number = np.array(lines, dtype=np.float64)
    return noise_data_number

def read_data_filtered(file_path_filtered):
    with open(file_path_filtered, "r") as file:
        lines = file.read().split()
    filtered_data_number = np.array(lines, dtype=np.float64)
    return filtered_data_number


def process_fft_data(data_number, fs):
    fft_result = np.fft.fft(data_number)
    fft_freqs = np.fft.fftfreq(fft_result.size, d=1/fs)
    return fft_result, fft_freqs

def process_data_noise(noise_data_number, fs):
    noise_wavepoint = noise_data_number
    timeDomain = np.linspace(0,(1/fs)* np.size(noise_data_number), np.size(noise_data_number))
    return noise_wavepoint, timeDomain

def process_data_filtered(filtered_data_number, fs):
    filtered_wavepoint = filtered_data_number
    timeDomain = np.linspace(0,(1/fs)* np.size(filtered_data_number), np.size(filtered_data_number))
    return filtered_wavepoint, timeDomain

def max_freq_with_higher_amplitude_than_threshold(fft_result, fft_freqs):
    res = 0
    threshold = 5e-3
    for i in range(len(fft_result) - 1, -1, -1):
        if fft_result[i] >= threshold:
            res = fft_freqs[i]
            break
    return res

def plot_domain(timeDomain, noise_ampDomain, filtered_ampDomain, fft_result, fft_freqs, name_pic):
    plt.figure(figsize=(14,6))
    N = fft_result.size
    amp = np.abs(fft_result[:N // 2]) * 2 / N
    freq = fft_freqs[:N // 2]
    max_freq = max_freq_with_higher_amplitude_than_threshold(amp, freq)
    fig, axs = plt.subplots(3)
    plt.subplots_adjust(left=0.1, bottom=0.1, top=0.9, wspace=0.4,hspace=0.6)
    axs[0].plot(timeDomain, noise_ampDomain)
    axs[0].set_xlabel("Time (s)")
    axs[0].set_ylabel("Amplitude")
    axs[1].plot(timeDomain, filtered_ampDomain)
    axs[1].set_xlabel("Time (s)")
    axs[1].set_ylabel("Amplitude")
    axs[2]= plt.gca()
    axs[2].set_ylim([0, np.max(amp)*1.2])
    axs[2].set_xlim([0, max_freq])
    axs[2].plot(freq, amp)
    axs[2].set_xlabel("Frequency (Hz)")
    axs[2].set_ylabel("Amplitude")
    plt.show()
    plt.savefig(name_pic)
    plt.close()


def parse_command(parser):
    parser.add_argument("--output", help="an output file")
    parser.add_argument("--inputNoise", help="an input data")
    parser.add_argument("--inputFiltered", help="an input data")
    parser.add_argument("--fs", type=int, help="frequency sample")
    args = parser.parse_args()
    return args

np.set_printoptions(threshold=sys.maxsize)
parser = argparse.ArgumentParser()
args = parse_command(parser)
noiseData = read_data_noise(args.inputNoise)
filteredData = read_data_filtered(args.inputFiltered)
noise_wavepoint, timeDomain = process_data_noise(noiseData, args.fs)
filtered_wavepoint, timeDomain = process_data_filtered(filteredData,args.fs)
fft_result, fft_freqs = process_fft_data(filteredData, args.fs)
plot_domain(timeDomain, noise_wavepoint, filtered_wavepoint, fft_result, fft_freqs, args.output)
